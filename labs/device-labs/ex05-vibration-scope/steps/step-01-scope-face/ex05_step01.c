/*
 * Lab 05 / Step 01 — Scope face
 *
 * WHAT THIS CODE DOES
 *   Success: a wave builds across the plot, then slides with newest on
 *   the right.
 *
 * HOW IT WORKS
 *   Architecture
 *   Fake sine feeds the HISTORY ring; delay sets frame rate.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Need a scope face before sensors.
 *   - Need paced push + yield.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: a wave builds across the plot, then slides with newest on
 *   the right.
 *
 * WHAT YOU LEARN HERE
 *   Same pace + yield lesson. Later steps stress what happens when you
 *   push/draw much faster than the panel can comfortably update.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex05_step01.h"

#include "bsp.h"
#include "imu_sim.h"
#include "lab_scope.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define FRAME_MS 33u
#define FRAME_DT_S 0.033f

void ex05_step01_start(void)
{
    float t = 0.0f;
    float ax;
    scope_init();
    scope_start();
    scope_set_status("fake wave");

    for (;;) {
        /* Soft sine via imu_sim helper (no libm). */
        ax = 8.0f * imu_sim_sin(2.0f * IMU_SIM_PI * 0.7f * t);
        scope_push(ax);
        t += FRAME_DT_S;
        vTaskDelay(pdMS_TO_TICKS(FRAME_MS));
    }
}
