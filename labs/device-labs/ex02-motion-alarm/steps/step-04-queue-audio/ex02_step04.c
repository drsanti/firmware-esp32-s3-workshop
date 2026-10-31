/*
 * Lab 02 / Step 04 — Queue + audio
 *
 * WHAT THIS CODE DOES
 *   Success: trip alarm → hear sound → numbers keep moving.
 *
 * HOW IT WORKS
 *   Architecture
 *   Sequence
 *   Rejected here:
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Step 03: audio in detect loop → freeze.
 *   - Alarm is a discrete event (not a continuous tilt stream like Lab
 *   01).
 *   - Audio may backlog if shakes are fast — need a short queue policy.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: trip alarm → hear sound → numbers keep moving.
 *
 * WHAT YOU LEARN HERE
 *   xQueueCreate / Send / Receive
 *   Why not notify for this step?
 *   Notify is enough for a bare “wake and chirp.” A queue makes the event
 *   path explicit and ready for a payload (kind / pattern). Lab 00/01
 *   already taught notify — here you practice Send/Receive.
 *   Mental model
 *   > Stream of measurements → overwrite queue (Lab 01). > Discrete cues
 *   → event queue (this step) or notify (Lab 00/01).
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex02_step04.h"

#include "bsp.h"
#include "imu_sim.h"
#include "lab_alarm.h"
#include "lab_beep.h"

#include <math.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/queue.h"
#include "freertos/task.h"

#define SAMPLE_MS 40u
#define SAMPLE_DT_S 0.04f
#define SHAKE_THRESH_MPS2 2.5f
#define ALARM_HOLD_MS 800u
#define EVT_QUEUE_LEN 4u

typedef struct {
    uint8_t kind; /* 0 = alarm chirp (room for more patterns later) */
} alarm_evt_t;

static QueueHandle_t g_evt;

static void audio_task(void *arg)
{
    alarm_evt_t evt;

    (void)arg;
    for (;;) {
        if (xQueueReceive(g_evt, &evt, portMAX_DELAY) == pdTRUE) {
            (void)evt;
            (void)beep_play(BEEP_ALARM, NULL);
        }
    }
}

static void detect_task(void *arg)
{
    float acc[3];
    float gyro[3];
    float t = 0.0f;
    uint32_t rng = 1u;
    int real;
    int was_real = -1;
    int was_shake = 0;
    int shake;
    float horiz;
    TickType_t alarm_until = 0;
    alarm_evt_t evt;

    (void)arg;

    alarm_init();
    alarm_start();
    alarm_set_state(ALARM_ST_ARMED);

    for (;;) {
        real = (bsp_imu_read(acc, gyro) == 0);
        if (!real) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += SAMPLE_DT_S;
        }
        if (real != was_real) {
            was_real = real;
            alarm_set_live(real);
        }

        horiz = sqrtf(acc[0] * acc[0] + acc[1] * acc[1]);
        shake = (horiz >= SHAKE_THRESH_MPS2);

        if (shake && !was_shake) {
            alarm_set_state(ALARM_ST_ALARM);
            alarm_until = xTaskGetTickCount() + pdMS_TO_TICKS(ALARM_HOLD_MS);
            evt.kind = 0;
            /* Non-blocking: drop if audio is backed up (better than freeze). */
            (void)xQueueSend(g_evt, &evt, 0);
        }
        was_shake = shake;

        if (alarm_until != 0 && xTaskGetTickCount() >= alarm_until) {
            alarm_until = 0;
            alarm_set_state(ALARM_ST_ARMED);
        }

        alarm_set_detail("horiz %.1f", (double)horiz);
        vTaskDelay(pdMS_TO_TICKS(SAMPLE_MS));
    }
}

void ex02_step04_start(void)
{
g_evt = xQueueCreate(EVT_QUEUE_LEN, sizeof(alarm_evt_t));
    if (g_evt == NULL) {
        return;
    }

    (void)xTaskCreate(audio_task, "audio", 4096, NULL, 5, NULL);
    (void)xTaskCreate(detect_task, "detect", 4096, NULL, 5, NULL);
}
