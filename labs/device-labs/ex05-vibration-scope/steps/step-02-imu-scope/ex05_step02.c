/*
 * Lab 05 / Step 02 — IMU scope
 *
 * WHAT THIS CODE DOES
 *   Success: shake the board (or watch sim) and the strip follows.
 *
 * HOW IT WORKS
 *   Architecture
 *   One short loop: read IMU or sim, push one sample, yield ~50 Hz.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Wire sensor → chart without overloading the UI.
 *   - Document the single-loop contract before breaking it.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: shake the board (or watch sim) and the strip follows.
 *
 * WHAT YOU LEARN HERE
 *   > Sense + draw may share one loop only if every pass stays short.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex05_step02.h"

#include "bsp.h"
#include "imu_sim.h"
#include "lab_scope.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define SAMPLE_MS 20u
#define SAMPLE_DT_S 0.02f

void ex05_step02_start(void)
{
    float acc[3];
    float gyro[3];
    float t = 0.0f;
    uint32_t rng = 1u;
    int real;
    int was_real = -1;
    scope_init();
    scope_start();

    for (;;) {
        real = (bsp_imu_read(acc, gyro) == 0);
        if (!real) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += SAMPLE_DT_S;
        }
        if (real != was_real) {
            was_real = real;
            scope_set_live(real);
        }

        scope_push(acc[0]);
        /*
         * Contract: keep this loop short. Pushing+drawing every 10 ms (100 Hz)
         * is the intentional break in step 03.
         */
        vTaskDelay(pdMS_TO_TICKS(SAMPLE_MS));
    }
}
