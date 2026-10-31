/*
 * Lab 01 / Step 04 — Split tasks + overwrite queue
 *
 * WHAT THIS CODE DOES
 *   Success: tilt freely; the bubble keeps up. You can explain *why* a
 *   length-1 overwrite queue fits “latest tilt.”
 *
 * HOW IT WORKS
 *   Architecture
 *   Sensor and UI decouple; length-1 overwrite always holds the latest
 *   tilt.
 *   Sequence
 *   UI blocks on Receive; sensor never waits on a full queue.
 *   Rejected for continuous tilt:
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Step 03: sense + draw + long work in one loop → UI dies while
 *   blocked.
 *   - Sensor and UI often want different rates (e.g. sample ~50 Hz, draw
 *   ~30 Hz).
 *   - UI needs the newest tilt, not a backlog of old samples.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: tilt freely; the bubble keeps up. You can explain *why* a
 *   length-1 overwrite queue fits “latest tilt.”
 *
 * WHAT YOU LEARN HERE
 *   xQueueCreate(1, sizeof(tilt_sample_t))
 *   The queue copies the struct (by value). No shared pointer lifetime
 *   issues for this small sample.
 *   xQueueOverwrite vs xQueueSend
 *   Use overwrite when new data makes old data useless (live
 *   instruments).
 *   xQueueReceive(..., portMAX_DELAY)
 *   UI sleeps with almost no CPU until the sensor posts. That is better
 *   than polling a shared variable in a tight loop.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex01_step04.h"

#include "bsp.h"
#include "imu_sim.h"
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
        /* Len-1 queue: always keep the newest tilt; never block on full. */
        (void)xQueueOverwrite(g_tilt, &sample);

        vTaskDelay(pdMS_TO_TICKS(SENSOR_MS));
    }
}

static void ui_task(void *arg)
{
    tilt_sample_t sample;

    (void)arg;

    spirit_init();
    spirit_start();
    spirit_set_live(1);

    for (;;) {
        /* Block until a sample arrives; draw the latest tilt. */
        if (xQueueReceive(g_tilt, &sample, portMAX_DELAY) == pdTRUE) {
            spirit_set_accel_mps2(sample.ax, sample.ay);
        }
    }
}

void ex01_step04_start(void)
{
g_tilt = xQueueCreate(1, sizeof(tilt_sample_t));
    if (g_tilt == NULL) {
        return;
    }

    (void)xTaskCreate(sensor_task, "sensor", 4096, NULL, 5, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
