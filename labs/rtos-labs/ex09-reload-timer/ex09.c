/*
 * ex09 — auto-reload software timer vs vTaskDelay beat.
 *
 * Teaching focus (kept here):
 *   - DELAY mode: a task owns the period (vTaskDelay loop)
 *   - TIMER mode: xTimerCreate(..., pdTRUE) + callback bumps the beat
 *   - Callback stays light (no LCD) — timer daemon context
 *
 * LCD / touch live in ex09_ui.*.
 */
#include "ex09.h"

#include "ex09_ui.h"

#include <stdint.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"
#include "freertos/timers.h"

#define PERIOD_MS 500u
#define FLASH_MS 120u

static TimerHandle_t s_timer;
static volatile uint8_t s_use_timer;
static volatile uint8_t s_go;
static volatile uint32_t s_beats;
static volatile uint8_t s_flash;
static volatile uint32_t s_flash_until_ms;

static uint32_t now_ms(void)
{
    return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

static void beat_pulse(void)
{
    s_beats++;
    s_flash = 1;
    s_flash_until_ms = now_ms() + FLASH_MS;
}

static void timer_cb(TimerHandle_t t)
{
    (void)t;
    if (s_use_timer) {
        beat_pulse();
    }
}

static void on_mode_toggle(int use_timer, void *user)
{
    (void)user;
    s_use_timer = use_timer ? 1u : 0u;

    if (s_timer == NULL) {
        return;
    }
    if (s_use_timer) {
        (void)xTimerStart(s_timer, 0);
    } else {
        (void)xTimerStop(s_timer, 0);
    }
}

static void delay_beat_task(void *arg)
{
    (void)arg;

    while (!s_go) {
        vTaskDelay(1);
    }

    for (;;) {
        while (s_use_timer) {
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        beat_pulse();
        vTaskDelay(pdMS_TO_TICKS(PERIOD_MS));
    }
}

static void ui_task(void *arg)
{
    (void)arg;

    while (!s_go) {
        vTaskDelay(1);
    }

    for (;;) {
        if (s_flash && (int32_t)(now_ms() - s_flash_until_ms) >= 0) {
            s_flash = 0;
        }
        ex09_ui_tick((int)s_use_timer, s_beats, (int)s_flash, PERIOD_MS);
    }
}

void ex09_start(void)
{
    s_use_timer = 0;
    s_go = 0;
    s_beats = 0;
    s_flash = 0;
    s_flash_until_ms = 0;

    s_timer = xTimerCreate("beat", pdMS_TO_TICKS(PERIOD_MS), pdTRUE /* reload */,
                           NULL, timer_cb);
    if (s_timer == NULL) {
        return;
    }

    if (ex09_ui_begin(on_mode_toggle, NULL) != 0) {
        return;
    }

    (void)xTaskCreatePinnedToCore(delay_beat_task, "dly", 2048, NULL, 5, NULL,
                                  0);
    (void)xTaskCreatePinnedToCore(ui_task, "ui", 4096, NULL, 4, NULL, 1);

    s_go = 1;
}
