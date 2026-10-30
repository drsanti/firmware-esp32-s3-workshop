/*
 * Lab 04 / Step 04 — Tick task + `vTaskDelayUntil`
 *
 * WHAT THIS CODE DOES
 *   Success: clicks sound steadier; max jitter much smaller.
 *
 * HOW IT WORKS
 *   Architecture
 *   The beat timetable lives in tick_task; UI only mirrors state.
 *   Sequence
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - UI work must not define the beat schedule.
 *   - Need absolute-period pacing, not “delay from now.”
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: clicks sound steadier; max jitter much smaller.
 *
 * WHAT YOU LEARN HERE
 *   Relative vs absolute
 *   Timing
 *   Arguments
 *   Limits
 *   If work ever takes longer than one period, you cannot catch up by
 *   sleeping negative time — you will run late / skip. Keep the click
 *   short vs BPM period.
 *   Mental model
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex04_step04.h"

#include "bsp.h"
#include "lab_beep.h"
#include "lab_metro.h"

#include <stdlib.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define BPM 120u
#define UI_MS 50u

typedef struct {
    volatile unsigned beat;
    volatile int last_j;
    volatile int max_j;
} metro_stats_t;

static metro_stats_t g_stats;
static unsigned g_period_ms;

static unsigned period_ms(unsigned bpm)
{
    if (bpm < 40u) {
        bpm = 40u;
    }
    return 60000u / bpm;
}

static void tick_task(void *arg)
{
    TickType_t last_wake;
    TickType_t period_ticks;
    TickType_t prev = 0;
    TickType_t now;
    int first = 1;
    int actual_ms;
    int err;
    int last_j;
    int max_j = 0;

    (void)arg;

    period_ticks = pdMS_TO_TICKS(g_period_ms);
    if (period_ticks < 1) {
        period_ticks = 1;
    }
    last_wake = xTaskGetTickCount();

    for (;;) {
        now = xTaskGetTickCount();
        if (!first) {
            actual_ms = (int)((now - prev) * portTICK_PERIOD_MS);
            err = actual_ms - (int)g_period_ms;
            last_j = abs(err);
            if (last_j > max_j) {
                max_j = last_j;
            }
            g_stats.last_j = last_j;
            g_stats.max_j = max_j;
        }
        first = 0;
        prev = now;

        g_stats.beat++;
        (void)beep_play(BEEP_CLICK, &(beep_params_t){
                                        .gain = 16u,
                                        .ms = 25u,
                                        .echo = 5u,
                                        .release = 10u,
                                    });

        /* Absolute schedule: next wake = last_wake + period (compensates work). */
        vTaskDelayUntil(&last_wake, period_ticks);
    }
}

static void ui_task(void *arg)
{
    unsigned seen = 0;
    unsigned beat;

    (void)arg;

    metro_init();
    metro_start();
    metro_set_bpm(BPM);
    metro_set_jitter(0, 0);
    metro_set_hint("DelayUntil tick");

    for (;;) {
        beat = g_stats.beat;
        if (beat != seen) {
            seen = beat;
            metro_flash_beat();
            metro_set_jitter(g_stats.last_j, g_stats.max_j);
            vTaskDelay(pdMS_TO_TICKS(60));
            metro_set_beat_idle();
            metro_set_jitter(g_stats.last_j, g_stats.max_j);
        } else {
            vTaskDelay(pdMS_TO_TICKS(UI_MS));
        }
    }
}

void ex04_step04_start(void)
{
g_period_ms = period_ms(BPM);
    g_stats.beat = 0;
    g_stats.last_j = 0;
    g_stats.max_j = 0;

    (void)xTaskCreate(tick_task, "tick", 4096, NULL, 6, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
