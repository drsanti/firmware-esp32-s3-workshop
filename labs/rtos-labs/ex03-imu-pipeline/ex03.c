/*
 * ex03 — cross-core IMU pipeline.
 *
 * Teaching focus:
 *   - q_raw / q_out (len 1 + overwrite) → latest sample, no backlog
 *   - imu_task @ core 0 → bsp_imu_read / imu_sim only
 *   - compute_task @ core 1 → |a| only
 *   - ui_task → widgets only (ex_imu_panel)
 *
 * Affinity is placement; the queue is the contract between cores.
 */
#include "ex03.h"

#include "bsp.h"
#include "ex_imu_panel.h"
#include "imu_sim.h"

#include <math.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/queue.h"
#include "freertos/task.h"

#define SAMPLE_MS 20u /* ~50 Hz */
#define SAMPLE_DT_S 0.02f

typedef struct {
    float acc[3];
    float gyro[3];
    uint8_t live;
    int8_t core_sample;
} raw_sample_t;

typedef struct {
    float acc[3];
    float gyro[3];
    float mag;
    uint8_t live;
    int8_t core_sample;
    int8_t core_compute;
} out_sample_t;

static QueueHandle_t s_q_raw;
static QueueHandle_t s_q_out;

static void imu_task(void *arg)
{
    float acc[3];
    float gyro[3];
    float t = 0.0f;
    uint32_t rng = 1u;
    raw_sample_t raw;

    (void)arg;

    for (;;) {
        raw.live = (bsp_imu_read(acc, gyro) == 0) ? 1u : 0u;
        if (!raw.live) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += SAMPLE_DT_S;
        }
        raw.acc[0] = acc[0];
        raw.acc[1] = acc[1];
        raw.acc[2] = acc[2];
        raw.gyro[0] = gyro[0];
        raw.gyro[1] = gyro[1];
        raw.gyro[2] = gyro[2];
        raw.core_sample = (int8_t)xPortGetCoreID();

        (void)xQueueOverwrite(s_q_raw, &raw);
        vTaskDelay(pdMS_TO_TICKS(SAMPLE_MS));
    }
}

static void compute_task(void *arg)
{
    raw_sample_t raw;
    out_sample_t out;

    (void)arg;

    for (;;) {
        if (xQueueReceive(s_q_raw, &raw, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        out.acc[0] = raw.acc[0];
        out.acc[1] = raw.acc[1];
        out.acc[2] = raw.acc[2];
        out.gyro[0] = raw.gyro[0];
        out.gyro[1] = raw.gyro[1];
        out.gyro[2] = raw.gyro[2];
        out.live = raw.live;
        out.core_sample = raw.core_sample;
        out.core_compute = (int8_t)xPortGetCoreID();
        out.mag = sqrtf(raw.acc[0] * raw.acc[0] + raw.acc[1] * raw.acc[1] +
                        raw.acc[2] * raw.acc[2]);

        (void)xQueueOverwrite(s_q_out, &out);
    }
}

static void ui_task(void *arg)
{
    out_sample_t out;

    (void)arg;

    if (ex_imu_panel_begin() != 0) {
        for (;;) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    for (;;) {
        if (xQueueReceive(s_q_out, &out, portMAX_DELAY) == pdTRUE) {
            ex_imu_panel_set(out.acc, out.gyro, out.mag, (int)out.live,
                             (int)out.core_sample, (int)out.core_compute);
        }
    }
}

void ex03_start(void)
{
    s_q_raw = xQueueCreate(1, sizeof(raw_sample_t));
    s_q_out = xQueueCreate(1, sizeof(out_sample_t));
    if (s_q_raw == NULL || s_q_out == NULL) {
        return;
    }

    (void)xTaskCreatePinnedToCore(imu_task, "imu", 4096, NULL, 5, NULL, 0);
    (void)xTaskCreatePinnedToCore(compute_task, "compute", 4096, NULL, 5, NULL,
                                  1);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 4, NULL);
}
