/*
 * Lab 05 / Step 05 — Drop stats
 *
 * WHAT THIS CODE DOES
 *   Success: status like in~100Hz drop N; chart still usable.
 *
 * HOW IT WORKS
 *   Architecture
 *   Non-blocking Send exposes backlog pressure as counters.
 *   Sequence
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Silent Send(..., 0) failures hide backlog pressure.
 *   - Need a simple way to see produce rate vs loss.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: status like in~100Hz drop N; chart still usable.
 *
 * WHAT YOU LEARN HERE
 *   Drops are not “FreeRTOS failing” — they are backpressure. Fix by
 *   faster UI, larger queue, or accepting loss for live scopes.
 *   Lab 05 feature recap
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex05_step05.h"

#include "bsp.h"
#include "imu_sim.h"
#include "lab_scope.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/queue.h"
#include "freertos/task.h"

#define SENSOR_MS 10u
#define SENSOR_DT_S 0.01f
#define UI_MS 33u
#define QUEUE_LEN 32u /* shorter than step 04 — easier to see drops */

typedef struct {
    volatile unsigned drops;
    volatile unsigned pushed;
} scope_stats_t;

static QueueHandle_t g_samples;
static scope_stats_t g_stats;

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
        if (xQueueSend(g_samples, &ax, 0) != pdTRUE) {
            g_stats.drops++;
        } else {
            g_stats.pushed++;
        }
        vTaskDelay(pdMS_TO_TICKS(SENSOR_MS));
    }
}

static void ui_task(void *arg)
{
    float ax;
    unsigned n;
    unsigned last_pushed = 0;
    unsigned last_drops = 0;
    TickType_t last_stat = xTaskGetTickCount();
    unsigned pushed;
    unsigned drops;
    unsigned hz;

    (void)arg;

    scope_init();
    scope_start();
    scope_set_status("drops 0");

    for (;;) {
        n = 0;
        while (xQueueReceive(g_samples, &ax, 0) == pdTRUE) {
            scope_push(ax);
            n++;
        }
        if (n == 0u) {
            if (xQueueReceive(g_samples, &ax, pdMS_TO_TICKS(UI_MS)) == pdTRUE) {
                scope_push(ax);
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(UI_MS));
        }

        /* ~1 Hz status refresh: approx produce rate + cumulative drops. */
        if ((xTaskGetTickCount() - last_stat) >= pdMS_TO_TICKS(1000)) {
            pushed = g_stats.pushed;
            drops = g_stats.drops;
            hz = pushed - last_pushed;
            last_pushed = pushed;
            last_drops = drops;
            last_stat = xTaskGetTickCount();
            /* Chart title max is short — keep status compact. */
            scope_set_status("%uHz d%u", hz, last_drops);
            (void)last_drops;
        }
    }
}

void ex05_step05_start(void)
{
g_stats.drops = 0;
    g_stats.pushed = 0;

    g_samples = xQueueCreate(QUEUE_LEN, sizeof(float));
    if (g_samples == NULL) {
        return;
    }

    (void)xTaskCreate(sensor_task, "sensor", 4096, NULL, 6, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
