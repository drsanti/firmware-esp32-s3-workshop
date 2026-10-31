/*
 * Lab 01 / Step 05 — LEVEL cue (notify + audio)
 *
 * WHAT THIS CODE DOES
 *   Success: tip into LEVEL → hear the chime → bubble keeps moving. You
 *   can name which FreeRTOS tool carries the stream and which carries the
 *   event.
 *
 * HOW IT WORKS
 *   Architecture
 *   Tilt still flows on the queue; LEVEL is an event on notify only.
 *   Sequence
 *   Entering LEVEL: UI keeps receiving tilt while audio plays the chime.
 *   Rejected:
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Step 03 put the chime in the sense/draw loop → freeze.
 *   - Step 04 fixed the data path but had no audio.
 *   - LEVEL is a discrete edge (“just became level”), not a continuous
 *   sample.
 *   - Audio still blocks for hundreds of ms — it must not share the UI
 *   task.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: tip into LEVEL → hear the chime → bubble keeps moving. You
 *   can name which FreeRTOS tool carries the stream and which carries the
 *   event.
 *
 * WHAT YOU LEARN HERE
 *   Two tools, two jobs
 *   Hold this model:
 *   > Stream of measurements → queue → UI. > Discrete events → notify →
 *   audio.
 *   Mixing them (notify for tilt, or a long queue of “play chime”
 *   messages) either loses data ownership or adds ceremony you do not
 *   need here.
 *   Rising edge, not level-high
 *   Leave LEVEL, tip back in → edge again → second chime. That matches a
 *   spirit level “click” when you find true.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex01_step05.h"

#include "bsp.h"
#include "imu_sim.h"
#include "lab_beep.h"
#include "lab_spirit.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/queue.h"
#include "freertos/task.h"

#define SENSOR_MS 20u /* ~50 Hz */
#define SAMPLE_DT_S 0.02f

typedef struct {
    float ax;
    float ay;
} tilt_sample_t;

static QueueHandle_t g_tilt;
static TaskHandle_t g_audio;

static void audio_task(void *arg)
{
    (void)arg;
    for (;;) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        (void)beep_play(BEEP_CHIME, NULL);
    }
}

static void sensor_task(void *arg)
{
    float acc[3];
    float gyro[3];
    float t = 0.0f;
    uint32_t rng = 1u;
    tilt_sample_t sample;

    (void)arg;

    for (;;) {
        if (bsp_imu_read(acc, gyro) != 0) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += SAMPLE_DT_S;
        }

        sample.ax = acc[0];
        sample.ay = acc[1];
        (void)xQueueOverwrite(g_tilt, &sample);

        vTaskDelay(pdMS_TO_TICKS(SENSOR_MS));
    }
}

static void ui_task(void *arg)
{
    tilt_sample_t sample;
    int was_level = 0;
    int level;

    (void)arg;

    spirit_init();
    spirit_start();
    spirit_set_live(1);

    for (;;) {
        if (xQueueReceive(g_tilt, &sample, portMAX_DELAY) == pdTRUE) {
            spirit_set_accel_mps2(sample.ax, sample.ay);
            level = spirit_is_level();

            /* Rising edge only — notify audio; never call beep_play here. */
            if (level && !was_level && g_audio != NULL) {
                (void)xTaskNotifyGive(g_audio);
            }
            was_level = level;
        }
    }
}

void ex01_step05_start(void)
{
g_tilt = xQueueCreate(1, sizeof(tilt_sample_t));
    if (g_tilt == NULL) {
        return;
    }

    /* Audio first so g_audio is valid before UI can NotifyGive. */
    (void)xTaskCreate(audio_task, "audio", 4096, NULL, 5, &g_audio);
    (void)xTaskCreate(sensor_task, "sensor", 4096, NULL, 5, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
