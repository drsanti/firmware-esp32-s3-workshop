/*
 * Lab 03 / Step 04 — Timer GO
 *
 * WHAT THIS CODE DOES
 *   Success: watch live count up during WAIT, then GO appears.
 *
 * HOW IT WORKS
 *   Architecture
 *   The ui task keeps pulsing react_* hints on the LCD during WAIT.
 *   Sequence
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Need “fire GO later” without blocking the UI task.
 *   - Timer callbacks run in the timer service task — must stay short.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: watch live count up during WAIT, then GO appears.
 *
 * WHAT YOU LEARN HERE
 *   Why a timer (not delay in UI)
 *   Key APIs
 *   Callback rules
 *   Allowed: notify, give semaphore, set a flag carefully. Not allowed
 *   here: beep_play, long loops, complex UI.
 *   Mental model
 *   > Delay = this task sleeps. > Timer = ask the system to wake someone
 *   later.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex03_step04.h"

#include "bsp.h"
#include "lab_react.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"
#include "freertos/timers.h"

#define BOOT_BTN_MASK 2u
#define POLL_MS 20u
#define WAIT_MIN_MS 1000u
#define WAIT_SPAN_MS 2000u
#define RESULT_HOLD_MS 2000u

static uint32_t g_rng = 1u;
static TaskHandle_t g_ui;
static TimerHandle_t g_go_timer;
static TickType_t g_go_tick;

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

/* Timer service task context: keep SHORT — notify only. */
static void go_timer_cb(TimerHandle_t t)
{
    (void)t;
    g_go_tick = xTaskGetTickCount();
    if (g_ui != NULL) {
        (void)xTaskNotifyGive(g_ui);
    }
}

static void ui_task(void *arg)
{
    uint32_t boot_was = 0;
    uint32_t wait_ms;
    uint32_t ms;
    unsigned pulse = 0;

    (void)arg;

    react_init();
    react_start();

    for (;;) {
        react_set_state(REACT_ST_READY);
        react_set_detail("BOOT to start");
        react_set_hint("timer wait next");

        while (!boot_pressed(&boot_was)) {
            vTaskDelay(pdMS_TO_TICKS(POLL_MS));
        }

        wait_ms = WAIT_MIN_MS + (rnd_u32() % WAIT_SPAN_MS);
        react_set_state(REACT_ST_WAIT);
        react_set_detail("timer %u ms", (unsigned)wait_ms);
        (void)xTimerChangePeriod(g_go_timer, pdMS_TO_TICKS(wait_ms), 0);
        (void)xTimerStart(g_go_timer, 0);

        /* Stay responsive during WAIT — pulse detail; watch for GO notify. */
        (void)ulTaskNotifyTake(pdTRUE, 0); /* clear stale */
        for (;;) {
            if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(POLL_MS)) > 0u) {
                break; /* timer fired */
            }
            pulse++;
            react_set_hint("live %u", pulse);
            (void)boot_pressed(&boot_was); /* drain edges; false-start in step 05 */
        }

        react_set_state(REACT_ST_GO);
        react_set_detail("BOOT now!");
        react_set_hint(" ");

        while (!boot_pressed(&boot_was)) {
            vTaskDelay(pdMS_TO_TICKS(POLL_MS));
        }

        ms = (uint32_t)((xTaskGetTickCount() - g_go_tick) * portTICK_PERIOD_MS);
        react_set_state(REACT_ST_RESULT);
        react_set_detail("%u ms", (unsigned)ms);
        react_set_hint("BOOT = next");

        vTaskDelay(pdMS_TO_TICKS(RESULT_HOLD_MS));
        (void)boot_pressed(&boot_was);
    }
}

void ex03_step04_start(void)
{
g_go_timer = xTimerCreate("go", pdMS_TO_TICKS(1000), pdFALSE, NULL, go_timer_cb);
    if (g_go_timer == NULL) {
        return;
    }

    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, &g_ui);
}
