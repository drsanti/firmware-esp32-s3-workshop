/*
 * Lab 04 / Step 05 — BPM control
 *
 * WHAT THIS CODE DOES
 *   Success: change tempo with BOOT; beat stays on DelayUntil schedule.
 *
 * HOW IT WORKS
 *   Architecture
 *   BOOT is polled in UI; tick re-reads shared BPM every beat cycle.
 *   Sequence
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Live BPM change must update period without putting timing back in
 *   the UI.
 *   - One odd interval after a change is OK; timetable continues.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: change tempo with BOOT; beat stays on DelayUntil schedule.
 *
 * WHAT YOU LEARN HERE
 *   No new API beyond step 04. The lesson is product control on top of an
 *   absolute tick:
 *   Relative vTaskDelay stays out of the beat path on purpose.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex04_step05.h"

#include "bsp.h"
#include "lab_beep.h"
#include "lab_metro.h"

#include <stdlib.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define BOOT_BTN_MASK 2u
#define UI_MS 40u
#define BPM_MIN 60u
#define BPM_MAX 180u
#define BPM_STEP 10u

typedef struct {
    volatile unsigned bpm;
    volatile unsigned beat;
    volatile int last_j;
    volatile int max_j;
} metro_stats_t;

static metro_stats_t g_stats;

static unsigned period_ms(unsigned bpm)
{
    if (bpm < 40u) {
        bpm = 40u;
    }
    return 60000u / bpm;
}

static int boot_pressed(uint32_t *was)
{
    uint32_t now = bsp_buttons() & BOOT_BTN_MASK;
    int edge = (now != 0u && *was == 0u) ? 1 : 0;
    *was = now;
    return edge;
}

static void tick_task(void *arg)
{
    TickType_t last_wake = xTaskGetTickCount();
    TickType_t period_ticks;
    TickType_t prev = 0;
    TickType_t now;
    unsigned bpm;
    unsigned period;
    int first = 1;
    int actual_ms;
    int err;
    int last_j;
    int max_j = 0;

    (void)arg;

    for (;;) {
        bpm = g_stats.bpm;
        period = period_ms(bpm);
        period_ticks = pdMS_TO_TICKS(period);
        if (period_ticks < 1) {
            period_ticks = 1;
        }

        now = xTaskGetTickCount();
        if (!first) {
            actual_ms = (int)((now - prev) * portTICK_PERIOD_MS);
            err = actual_ms - (int)period;
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
        /* Quiet short tick — repeats every beat. */
        (void)beep_play(BEEP_CLICK, &(beep_params_t){
                                        .gain = 16u,
                                        .ms = 25u,
                                        .echo = 5u,
                                        .release = 10u,
                                    });

        vTaskDelayUntil(&last_wake, period_ticks);
    }
}

static void ui_task(void *arg)
{
    uint32_t boot_was = 0;
    unsigned seen = 0;
    unsigned beat;
    unsigned bpm;
    unsigned shown_bpm = 0;

    (void)arg;

    metro_init();
    metro_start();
    bpm = g_stats.bpm;
    metro_set_bpm(bpm);
    shown_bpm = bpm;
    metro_set_jitter(0, 0);
    metro_set_hint("BOOT = +BPM");

    for (;;) {
        if (boot_pressed(&boot_was)) {
            bpm = g_stats.bpm + BPM_STEP;
            if (bpm > BPM_MAX) {
                bpm = BPM_MIN;
            }
            g_stats.bpm = bpm;
            g_stats.max_j = 0; /* reset max when tempo changes */
            metro_set_bpm(bpm);
            shown_bpm = bpm;
            metro_set_hint("BPM %u", bpm);
        }

        bpm = g_stats.bpm;
        if (bpm != shown_bpm) {
            metro_set_bpm(bpm);
            shown_bpm = bpm;
        }

        beat = g_stats.beat;
        if (beat != seen) {
            seen = beat;
            metro_flash_beat();
            metro_set_jitter(g_stats.last_j, g_stats.max_j);
            vTaskDelay(pdMS_TO_TICKS(50));
            metro_set_beat_idle();
            metro_set_jitter(g_stats.last_j, g_stats.max_j);
        } else {
            vTaskDelay(pdMS_TO_TICKS(UI_MS));
        }
    }
}

void ex04_step05_start(void)
{
g_stats.bpm = 120u;
    g_stats.beat = 0;
    g_stats.last_j = 0;
    g_stats.max_j = 0;

    (void)xTaskCreate(tick_task, "tick", 4096, NULL, 6, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
