/*
 * Lab 05 / Step 04 — Split + sample queue
 *
 * WHAT THIS CODE DOES
 *   Success: live strip without the step-03 slog.
 *
 * HOW IT WORKS
 *   Architecture
 *   Sequence
 *   Rejected: len-1 overwrite (Lab 01) — UI would see only the newest
 *   sample per frame and throw away vibration detail between draws.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Need different rates for produce vs draw.
 *   - Need a short backlog (not Lab 01’s “latest only”).
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: live strip without the step-03 slog.
 *
 * WHAT YOU LEARN HERE
 *   Drain pattern
 *   Timeout 0 = non-blocking. Empty → stop draining. That batches work
 *   for one UI frame.
 *   Mental model
 *   > Fast producer → queue → slower consumer. > Different rates, one
 *   buffer.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex05_step04.h"

#include "bsp.h"
#include "imu_sim.h"
#include "lab_scope.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/queue.h"
#include "freertos/task.h"

#define SENSOR_MS 10u /* ~100 Hz */
#define SENSOR_DT_S 0.01f
#define UI_MS 33u /* ~30 Hz */
#define QUEUE_LEN 64u

static QueueHandle_t g_samples;

static void sensor_task(void *arg)
{
    float acc[3];
    float gyro[3];
    float t = 0.0f;
    uint32_t rng = 1u;
    float ax;

    (void)arg;

    for (;;) {
        if (bsp_imu_read(acc, gyro) != 0) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += SENSOR_DT_S;
        }
        ax = acc[0];
        /* Non-blocking: drop if UI is behind (step 05 counts drops). */
        (void)xQueueSend(g_samples, &ax, 0);
        vTaskDelay(pdMS_TO_TICKS(SENSOR_MS));
    }
}

static void ui_task(void *arg)
{
    float ax;
    unsigned n;

    (void)arg;

    scope_init();
    scope_start();
    scope_set_status("100->Q->30");

    for (;;) {
        n = 0;
        /* Drain whatever arrived since last frame; draw each sample. */
        while (xQueueReceive(g_samples, &ax, 0) == pdTRUE) {
            scope_push(ax);
            n++;
        }
        if (n == 0u) {
            /* Idle wait for at least one sample if quiet. */
            if (xQueueReceive(g_samples, &ax, pdMS_TO_TICKS(UI_MS)) == pdTRUE) {
                scope_push(ax);
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(UI_MS));
        }
    }
}

void ex05_step04_start(void)
{
g_samples = xQueueCreate(QUEUE_LEN, sizeof(float));
    if (g_samples == NULL) {
        return;
    }

    (void)xTaskCreate(sensor_task, "sensor", 4096, NULL, 6, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
