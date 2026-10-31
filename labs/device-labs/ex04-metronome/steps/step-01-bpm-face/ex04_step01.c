/*
 * Lab 04 / Step 01 — BPM face
 *
 * WHAT THIS CODE DOES
 *   Success: steady visual pulse at ~120 BPM.
 *
 * HOW IT WORKS
 *   Architecture
 *   One task owns the metronome face; each period is flash, short hold,
 *   idle, then sleep the remainder.
 *   Repeat that chain each BPM period.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Need a clear metronome face before audio/timing lessons.
 *   - Need paced redraw + yield (vTaskDelay).
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: steady visual pulse at ~120 BPM.
 *
 * WHAT YOU LEARN HERE
 *   Here delay is used as a crude period generator. That is fine for a
 *   visual demo. Later steps put work before a relative delay and the
 *   real period grows — that is the jitter lesson.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex04_step01.h"

#include "bsp.h"
#include "lab_metro.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define BPM 120u
#define FLASH_MS 80u

static unsigned period_ms(unsigned bpm)
{
    if (bpm < 40u) {
        bpm = 40u;
    }
    return 60000u / bpm;
}

void ex04_step01_start(void)
{
    unsigned period = period_ms(BPM);
    metro_init();
    metro_start();
    metro_set_bpm(BPM);
    metro_set_hint("visual only");

    for (;;) {
        metro_flash_beat();
        vTaskDelay(pdMS_TO_TICKS(FLASH_MS));
        metro_set_beat_idle();
        if (period > FLASH_MS) {
            vTaskDelay(pdMS_TO_TICKS(period - FLASH_MS));
        }
    }
}
