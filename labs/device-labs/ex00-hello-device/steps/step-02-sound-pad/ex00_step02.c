/*
 * Lab 00 / Step 02 — Sound pad (buttons, callbacks, tones)
 *
 * WHAT THIS CODE DOES
 *   Success: tap Lo / Mid / Hi → three different pitches; drag Gain /
 *   Echo / Release to refine the tone.
 *
 * HOW IT WORKS
 *   ex00_step02_start() runs after bsp_init() when selected in the lab
 *   switcher.
 *   Follow the code below; architecture detail is in README.md.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   Without this step, the next feature has no solid base on the board.
 *   A wrong timing/blocking pattern usually shows up as a stuck UI or
 *   silent speaker.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: tap Lo / Mid / Hi → three different pitches; drag Gain /
 *   Echo / Release to refine the tone.
 *
 * WHAT YOU LEARN HERE
 *   This step’s FreeRTOS surface is small on purpose. The big design
 *   lesson is where sound runs.
 *   vTaskDelay in app_main
 *   After sound_pad_start(), app_main only does:
 *   Why keep the task alive with a delay instead of returning?
 *   - app_main is a FreeRTOS task. Returning ends that task; the system
 *   keeps
 *   running other tasks (including the pad’s touch poller), but an idle
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex00_step02.h"

#include "bsp.h"
#include "lab_beep.h"
#include "lab_sound_pad.h"

#include <stdint.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

/* Shared click handler: user pointer holds the tone frequency (Hz). */
static void on_tone_click(void *user)
{
    unsigned hz = (unsigned)(uintptr_t)user;

    (void)beep_play(BEEP_TONE, &(beep_params_t){
                                   .hz = (uint16_t)hz,
                                   .ms = 150u,
                               });
}

void ex00_step02_start(void)
{
sound_pad_init();
    /* NULL on_change → helper applies beep_set_* for you */
    (void)sound_pad_add_gain_slider(NULL, NULL);
    (void)sound_pad_add_echo_slider(NULL, NULL);
    (void)sound_pad_add_release_slider(NULL, NULL);

    /* TODO (students): three buttons, each with a callback */
    (void)sound_pad_add_button("Lo", RB_STYLE_INFO, on_tone_click,
                               (void *)(uintptr_t)440u);

    (void)sound_pad_add_button("Mid", RB_STYLE_SUCCESS, on_tone_click,
                               (void *)(uintptr_t)880u);

    (void)sound_pad_add_button("Hi", RB_STYLE_WARNING, on_tone_click,
                               (void *)(uintptr_t)1760u);

    sound_pad_start(); /* reflow UI + start touch task */

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
