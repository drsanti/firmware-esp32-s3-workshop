/*
 * ex02 — OS tick + tick hook.
 *
 * Teaching focus (kept here):
 *   - FreeRTOS timebase: configTICK_RATE_HZ / portTICK_PERIOD_MS
 *   - xTaskGetTickCount advances on every tick interrupt
 *   - Tick hook runs in ISR context — keep it tiny (no LCD / delay)
 *   - BUSY mode: extra work in the hook → UI frames stall (visible cost)
 *
 * ESP-IDF: esp_register_freertos_tick_hook_for_cpu (per-core callbacks).
 * Vanilla FreeRTOS would use vApplicationTickHook + configUSE_TICK_HOOK.
 *
 * LCD / touch live in ex02_ui.*.
 */
#include "ex02.h"

#include "ex02_ui.h"

#include <stdint.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#include "esp_attr.h"
#include "esp_freertos_hooks.h"

/* Busy-loop iterations in the tick hook (ISR). Short enough for IWDT. */
#define BUSY_ITERS 12000u

static volatile uint8_t s_busy;
static volatile uint8_t s_go;
static volatile uint32_t s_hook[2];
static volatile uint32_t s_ui_frames;

static void on_mode_toggle(int busy, void *user)
{
    (void)user;
    s_busy = busy ? 1u : 0u;
}

/*
 * Tick callback — ISR context on the core that just ticked.
 * Only touch volatiles / spin; never call rb_* or vTaskDelay.
 */
static void IRAM_ATTR tick_hook(void)
{
    int core = (int)xPortGetCoreID();

    if (core >= 0 && core < 2) {
        s_hook[core]++;
    }

    if (s_busy) {
        volatile uint32_t n = BUSY_ITERS;
        while (n > 0u) {
            n--;
        }
    }
}

static void ui_task(void *arg)
{
    (void)arg;

    while (!s_go) {
        vTaskDelay(1);
    }

    for (;;) {
        s_ui_frames++;
        ex02_ui_tick((int)s_busy, (uint32_t)xTaskGetTickCount(), s_hook[0],
                     s_hook[1], s_ui_frames, (unsigned)configTICK_RATE_HZ,
                     (unsigned)portTICK_PERIOD_MS);
    }
}

void ex02_start(void)
{
    s_busy = 0;
    s_go = 0;
    s_hook[0] = 0;
    s_hook[1] = 0;
    s_ui_frames = 0;

    /* Register on both cores — each core's SysTick invokes its own list. */
    if (esp_register_freertos_tick_hook_for_cpu(tick_hook, 0) != ESP_OK) {
        return;
    }
    if (esp_register_freertos_tick_hook_for_cpu(tick_hook, 1) != ESP_OK) {
        return;
    }

    if (ex02_ui_begin(on_mode_toggle, NULL) != 0) {
        return;
    }

    (void)xTaskCreatePinnedToCore(ui_task, "ui", 4096, NULL, 4, NULL, 1);

    s_go = 1;
}
