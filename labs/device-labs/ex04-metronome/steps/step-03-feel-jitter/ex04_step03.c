/*
 * Lab 04 / Step 03 — Feel jitter
 *
 * WHAT THIS CODE DOES
 *   Success: jitter numbers are clearly above ~0 (often tens of ms from
 *   the click).
 *
 * HOW IT WORKS
 *   Architecture
 *   Measure at loop start; then do work and a relative delay — the slip
 *   shows up in the numbers.
 *   Timing
 *   Each beat adds beep time on top of the nominal period.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Need proof that relative delay after work slips.
 *   - Students should see numbers, not only “it feels off.”
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: jitter numbers are clearly above ~0 (often tens of ms from
 *   the click).
 *
 * WHAT YOU LEARN HERE
 *   Resolution is one tick (often 1 or 10 ms). That is enough to see
 *   click-induced slip.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex04_step03.h"

#include "bsp.h"
#include "lab_beep.h"
#include "lab_metro.h"

#include <stdlib.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define BPM 120u

static unsigned period_ms(unsigned bpm)
{
    if (bpm < 40u) {
        bpm = 40u;
    }
    return 60000u / bpm;
}

void ex04_step03_start(void)
{
    unsigned period = period_ms(BPM);
    TickType_t prev = 0;
    TickType_t now;
    int first = 1;
    int last_j = 0;
    int max_j = 0;
    int actual_ms;
    int err;
    metro_init();
    metro_start();
    metro_set_bpm(BPM);
    metro_set_jitter(0, 0);

    for (;;) {
        now = xTaskGetTickCount();
        if (!first) {
            actual_ms = (int)((now - prev) * portTICK_PERIOD_MS);
            err = actual_ms - (int)period;
            last_j = abs(err);
            if (last_j > max_j) {
                max_j = last_j;
            }
            metro_set_jitter(last_j, max_j);
        }
        first = 0;
        prev = now;

        metro_flash_beat();
        (void)beep_play(BEEP_CLICK, &(beep_params_t){
                                        .gain = 16u,
                                        .ms = 25u,
                                        .echo = 5u,
                                        .release = 10u,
                                    });
        metro_set_beat_idle();
        vTaskDelay(pdMS_TO_TICKS(period));
    }
}
