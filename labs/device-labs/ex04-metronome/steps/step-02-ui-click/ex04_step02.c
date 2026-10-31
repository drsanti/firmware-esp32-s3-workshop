/*
 * Lab 04 / Step 02 — UI click
 *
 * WHAT THIS CODE DOES
 *   Success: you hear a click roughly at 120 BPM (exact timing comes
 *   next).
 *
 * HOW IT WORKS
 *   Architecture
 *   The same UI loop now plays audio; blocking work sits before the
 *   relative delay.
 *   Timing
 *   Beat-to-beat spacing grows because the delay starts after the click
 *   finishes.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - beep_play blocks for the click duration.
 *   - Relative delay starts *after* that work → real period = work +
 *   delay.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: you hear a click roughly at 120 BPM (exact timing comes
 *   next).
 *
 * WHAT YOU LEARN HERE
 *   So:
 *   not N alone. That is acceptable for a first click, wrong for a
 *   metronome.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex04_step02.h"

#include "bsp.h"
#include "lab_beep.h"
#include "lab_metro.h"

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

void ex04_step02_start(void)
{
    unsigned period = period_ms(BPM);
    metro_init();
    metro_start();
    metro_set_bpm(BPM);
    metro_set_hint("click in UI loop");

    for (;;) {
        metro_flash_beat();
        /* Quiet tick; still blocks — that is the relative-delay lesson. */
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
