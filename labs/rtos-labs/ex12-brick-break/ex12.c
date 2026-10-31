/*
 * ex12 — IMU Breakout (brick breaker).
 *
 *   imu_task  → overwrite tilt queue
 *   game_task → paddle / ball / bricks + gfx
 *   sfx_task  → beep_play
 *   touch/BOOT → start / launch / retry
 */
#include "ex12.h"

#include "ex12_game.h"
#include "ex12_gfx.h"

#include "bsp.h"
#include "imu_sim.h"
#include "lab_beep.h"

#include <stdint.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/queue.h"
#include "freertos/task.h"

#define SAMPLE_MS 20u
#define FRAME_MS 33u
#define BOOT_BTN_MASK 2u

typedef struct {
    float ax;
    uint8_t live;
} tilt_sample_t;

static QueueHandle_t s_q_tilt;
static QueueHandle_t s_q_sfx;
static volatile uint8_t s_start_req;
static volatile uint8_t s_go;

static int boot_edge(uint32_t *was)
{
    uint32_t now = bsp_buttons() & BOOT_BTN_MASK;
    int edge = (now != 0u && *was == 0u) ? 1 : 0;

    *was = now;
    return edge;
}

static void imu_task(void *arg)
{
    float acc[3];
    float gyro[3];
    float t = 0.0f;
    uint32_t rng = 1u;
    tilt_sample_t sample;

    (void)arg;

    while (!s_go) {
        vTaskDelay(1);
    }

    for (;;) {
        sample.live = (bsp_imu_read(acc, gyro) == 0) ? 1u : 0u;
        if (!sample.live) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += 0.02f;
        }
        sample.ax = acc[0];
        (void)xQueueOverwrite(s_q_tilt, &sample);
        vTaskDelay(pdMS_TO_TICKS(SAMPLE_MS));
    }
}

static void sfx_task(void *arg)
{
    ex12_sfx_t kind;

    (void)arg;

    while (!s_go) {
        vTaskDelay(1);
    }

    for (;;) {
        if (xQueueReceive(s_q_sfx, &kind, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        switch (kind) {
        case EX12_SFX_START:
        case EX12_SFX_CLEAR:
            (void)beep_play(BEEP_CHIME, NULL);
            break;
        case EX12_SFX_CLICK:
            (void)beep_play(BEEP_CLICK, NULL);
            break;
        case EX12_SFX_LOSE:
            (void)beep_play(BEEP_ALARM, NULL);
            break;
        default:
            break;
        }
    }
}

static void push_sfx(ex12_sfx_t kind)
{
    if (kind == EX12_SFX_NONE || s_q_sfx == NULL) {
        return;
    }
    (void)xQueueSend(s_q_sfx, &kind, 0);
}

static void game_task(void *arg)
{
    ex12_game_t game;
    tilt_sample_t tilt;
    uint32_t boot_was = 0;
    uint8_t touch_was = 0;
    float ax = 0.0f;
    int live = 0;

    (void)arg;

    while (!s_go) {
        vTaskDelay(1);
    }

    ex12_game_init(&game, ex12_gfx_width(), ex12_gfx_height());
    tilt.ax = 0.0f;
    tilt.live = 0;

    for (;;) {
        bsp_touch_sample_t smp;
        uint8_t touching = 0;

        if (xQueueReceive(s_q_tilt, &tilt, 0) == pdTRUE) {
            ax = tilt.ax;
            live = (int)tilt.live;
        }

        if (bsp_touch_read(&smp) == 0) {
            touching = (smp.n > 0u || smp.gesture == BOARD_GEST_CLICK) ? 1u
                                                                       : 0u;
        }
        if (touching && !touch_was) {
            s_start_req = 1;
        }
        touch_was = touching;
        if (boot_edge(&boot_was)) {
            s_start_req = 1;
        }

        if (s_start_req) {
            s_start_req = 0;
            push_sfx(ex12_game_on_start(&game));
        }

        push_sfx(ex12_game_tick(&game, ax, live));
        ex12_gfx_paint(&game);
        vTaskDelay(pdMS_TO_TICKS(FRAME_MS));
    }
}

void ex12_start(void)
{
    s_start_req = 0;
    s_go = 0;

    s_q_tilt = xQueueCreate(1, sizeof(tilt_sample_t));
    s_q_sfx = xQueueCreate(4, sizeof(ex12_sfx_t));
    if (s_q_tilt == NULL || s_q_sfx == NULL) {
        return;
    }

    if (ex12_gfx_begin() != 0) {
        return;
    }

    (void)xTaskCreatePinnedToCore(imu_task, "imu", 3072, NULL, 5, NULL, 0);
    (void)xTaskCreatePinnedToCore(sfx_task, "sfx", 4096, NULL, 4, NULL, 1);
    (void)xTaskCreatePinnedToCore(game_task, "game", 4096, NULL, 5, NULL, 1);

    s_go = 1;
}
