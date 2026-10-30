/*
 * Lab 05 / Step 03 — UI @100 Hz (**broken**)
 *
 * WHAT THIS CODE DOES
 *   Success: you can name “drawing every sample is too much.”
 *
 * HOW IT WORKS
 *   Architecture
 *   Sample and full chart draw share one loop at 10 ms — UI work
 *   dominates.
 *   Timing
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Chart updates are heavier than a label poke.
 *   - 100 draws/s in the sense task starves responsiveness.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: you can name “drawing every sample is too much.”
 *
 * WHAT YOU LEARN HERE
 *   One loop forcing both rates to 100 Hz is the architecture smell — not
 *   a kernel bug.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex05_step03.h"

#include "bsp.h"
#include "imu_sim.h"
#include "lab_scope.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define SAMPLE_MS 10u /* ~100 Hz — intentional stress */
#define SAMPLE_DT_S 0.01f

void ex05_step03_start(void)
{
    float acc[3];
    float gyro[3];
    float t = 0.0f;
    uint32_t rng = 1u;
    int real;
    int was_real = -1;
    scope_init();
    scope_start();
    scope_set_status("100 Hz draw BAD");

    for (;;) {
        real = (bsp_imu_read(acc, gyro) == 0);
        if (!real) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += SAMPLE_DT_S;
        }
        if (real != was_real) {
            was_real = real;
            scope_set_status(real ? "100Hz live BAD" : "100Hz sim BAD");
        }

        /*
         * INTENTIONAL BUG: draw on every 100 Hz sample in this task.
         * scope_push redraws the HISTORY chart — too much UI work per second.
         */
        scope_push(acc[0]);
        vTaskDelay(pdMS_TO_TICKS(SAMPLE_MS));
    }
}
