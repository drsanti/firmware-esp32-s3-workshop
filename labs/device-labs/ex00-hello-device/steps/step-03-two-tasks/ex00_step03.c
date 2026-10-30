/*
 * Lab 00 / Step 03 — Two tasks + notify
 *
 * WHAT THIS CODE DOES
 *   Success: tap Beep → you hear a tone from audio_task, not from the
 *   click callback.
 *
 * HOW IT WORKS
 *   Architecture
 *   Two tasks share one notify bridge; the pad click path never calls
 *   beep_*.
 *   Sequence
 *   One Beep tap: UI returns immediately; audio blocks on the speaker.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   Playing audio inside the touch/click path blocks that path for the
 *   whole tone. Notify wakes another task that is allowed to block on
 *   bsp_audio_write.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: tap Beep → you hear a tone from audio_task, not from the
 *   click callback.
 *
 * WHAT YOU LEARN HERE
 *   Why two tasks?
 *   xTaskCreate starts a new concurrent loop. Priority 5 matches UI here
 *   — scheduling fairness is enough; we are not tuning real-time ranks
 *   yet.
 *   Create audio first and keep g_audio: Give needs a valid TaskHandle_t.
 *   If UI ran first and somehow notified before create finished, Give
 *   would be skipped (g_audio == NULL).
 *   Why notify (not a queue) for Beep?
 *   Beep is an event, not a stream of samples. Notify is the lightest
 *   “wake that task” tool FreeRTOS offers for this product moment.
 *   xTaskNotifyGive / ulTaskNotifyTake arguments
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex00_step03.h"

#include "bsp.h"
#include "lab_beep.h"
#include "lab_sound_pad.h"

#include <stdint.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

static TaskHandle_t g_audio;
static uint8_t g_gain = 40; /* updated by Gain slider */

/* UI side: only wake audio — do not play sound here. */
static void on_beep_click(void *user)
{
    (void)user;
    if (g_audio != NULL) {
        (void)xTaskNotifyGive(g_audio);
    }
}

/* Store loudness for audio_task (do not call beep_* here either). */
static void on_gain(uint8_t percent, void *user)
{
    (void)user;
    g_gain = percent;
}

static void audio_task(void *arg)
{
    (void)arg;
    for (;;) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        (void)beep_play(BEEP_TONE, &(beep_params_t){
                                       .hz = 880u,
                                       .ms = 150u,
                                       .gain = g_gain,
                                   });
    }
}

static void ui_task(void *arg)
{
    (void)arg;

    sound_pad_init();
    (void)sound_pad_add_gain_slider(on_gain, NULL);
    (void)sound_pad_add_echo_slider(NULL, NULL);    /* helper → beep_set_echo */
    (void)sound_pad_add_release_slider(NULL, NULL); /* helper → beep_set_release */
    (void)sound_pad_add_button("Beep", RB_STYLE_SUCCESS, on_beep_click, NULL);
    sound_pad_start();

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void ex00_step03_start(void)
{
/* Create audio first so g_audio is valid before any tap. */
    (void)xTaskCreate(audio_task, "audio", 4096, NULL, 5, &g_audio);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
