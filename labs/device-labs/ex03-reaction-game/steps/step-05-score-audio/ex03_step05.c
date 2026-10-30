/*
 * Lab 03 / Step 05 — Score + audio
 *
 * WHAT THIS CODE DOES
 *   Success: early press → FALSE START; good round → ms + best; hear
 *   click on GO.
 *
 * HOW IT WORKS
 *   Architecture
 *   Sequence
 *   State machine
 *   False start (BOOT during WAIT):
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Product rules on top of the timer path.
 *   - Audio must not block UI (Lab 00/01 pattern).
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: early press → FALSE START; good round → ms + best; hear
 *   click on GO.
 *
 * WHAT YOU LEARN HERE
 *   No new primitive beyond step 04’s timer — step 05 is composition and
 *   game rules. Queue still unused on purpose.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex03_step05.h"

#include "bsp.h"
#include "lab_beep.h"
#include "lab_react.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"
#include "freertos/timers.h"

#define BOOT_BTN_MASK 2u
#define POLL_MS 20u
#define WAIT_MIN_MS 1000u
#define WAIT_SPAN_MS 2000u
#define RESULT_HOLD_MS 2500u

static uint32_t g_rng = 1u;
static TaskHandle_t g_ui;
static TaskHandle_t g_audio;
static TimerHandle_t g_go_timer;
static TickType_t g_go_tick;
static uint32_t g_best_ms = 0xffffffffu;

static uint32_t rnd_u32(void)
{
    g_rng = g_rng * 1664525u + 1013904223u;
    return g_rng;
}

static int boot_pressed(uint32_t *was)
{
    uint32_t now = bsp_buttons() & BOOT_BTN_MASK;
    int edge = (now != 0u && *was == 0u) ? 1 : 0;
    *was = now;
    return edge;
}

static void go_timer_cb(TimerHandle_t t)
{
    (void)t;
    g_go_tick = xTaskGetTickCount();
    if (g_ui != NULL) {
        (void)xTaskNotifyGive(g_ui);
    }
}

static void audio_task(void *arg)
{
    (void)arg;
    for (;;) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        (void)beep_play(BEEP_CLICK, NULL);
    }
}

static void ui_task(void *arg)
{
    uint32_t boot_was = 0;
    uint32_t wait_ms;
    uint32_t ms;
    unsigned pulse = 0;
    int false_start;

    (void)arg;

    react_init();
    react_start();

    for (;;) {
        react_set_state(REACT_ST_READY);
        if (g_best_ms == 0xffffffffu) {
            react_set_detail("BOOT to start");
            react_set_hint("best: --");
        } else {
            react_set_detail("BOOT to start");
            react_set_hint("best: %u ms", (unsigned)g_best_ms);
        }

        while (!boot_pressed(&boot_was)) {
            vTaskDelay(pdMS_TO_TICKS(POLL_MS));
        }

        wait_ms = WAIT_MIN_MS + (rnd_u32() % WAIT_SPAN_MS);
        react_set_state(REACT_ST_WAIT);
        react_set_detail("wait...");
        (void)xTimerChangePeriod(g_go_timer, pdMS_TO_TICKS(wait_ms), 0);
        (void)xTimerStart(g_go_timer, 0);

        (void)ulTaskNotifyTake(pdTRUE, 0);
        false_start = 0;
        for (;;) {
            if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(POLL_MS)) > 0u) {
                break;
            }
            if (boot_pressed(&boot_was)) {
                false_start = 1;
                (void)xTimerStop(g_go_timer, 0);
                break;
            }
            pulse++;
            react_set_hint("live %u", pulse);
        }

        if (false_start) {
            react_set_state(REACT_ST_RESULT);
            react_set_detail("FALSE START");
            react_set_hint("too early");
            vTaskDelay(pdMS_TO_TICKS(RESULT_HOLD_MS));
            (void)boot_pressed(&boot_was);
            continue;
        }

        react_set_state(REACT_ST_GO);
        react_set_detail("BOOT now!");
        react_set_hint(" ");
        if (g_audio != NULL) {
            (void)xTaskNotifyGive(g_audio);
        }

        while (!boot_pressed(&boot_was)) {
            vTaskDelay(pdMS_TO_TICKS(POLL_MS));
        }

        ms = (uint32_t)((xTaskGetTickCount() - g_go_tick) * portTICK_PERIOD_MS);
        if (ms < g_best_ms) {
            g_best_ms = ms;
        }

        react_set_state(REACT_ST_RESULT);
        react_set_detail("%u ms", (unsigned)ms);
        react_set_hint("best: %u ms", (unsigned)g_best_ms);

        vTaskDelay(pdMS_TO_TICKS(RESULT_HOLD_MS));
        (void)boot_pressed(&boot_was);
    }
}

void ex03_step05_start(void)
{
g_go_timer = xTimerCreate("go", pdMS_TO_TICKS(1000), pdFALSE, NULL, go_timer_cb);
    if (g_go_timer == NULL) {
        return;
    }

    (void)xTaskCreate(audio_task, "audio", 4096, NULL, 5, &g_audio);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, &g_ui);
}
