/*
 * Lab 01 / Step 02 — IMU → bubble
 *
 * WHAT THIS CODE DOES
 *   Success: tilt the board (or watch sim motion); the bubble follows;
 *   LEVEL appears when centered. Both boards count as success (live or
 *   sim).
 *
 * HOW IT WORKS
 *   Architecture
 *   Same single loop as step 01; IMU or sim feeds the bubble each frame.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Fake tilt was only a demo — a real instrument needs sensor data.
 *   - Not every workshop board has a working IMU → need a sim fallback
 *   (same idea as Lab 00 step 04).
 *   - One loop is still fine only if each pass stays short. Blocking
 *   audio here would freeze the bubble (that failure is step 03 on
 *   purpose).
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: tilt the board (or watch sim motion); the bubble follows;
 *   LEVEL appears when centered. Both boards count as success (live or
 *   sim).
 *
 * WHAT YOU LEARN HERE
 *   What is new vs step 01?
 *   FreeRTOS API is still mainly vTaskDelay + pdMS_TO_TICKS. The design
 *   lesson is the contract: one loop is allowed only if it does not block
 *   for long.
 *   Why ~33 ms again?
 *   - ~30 Hz is enough for a bubble the eye can follow.
 *   - Yields every frame so the scheduler stays healthy.
 *   - Matches sim SAMPLE_DT_S so synthetic motion does not jump oddly.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex01_step02.h"

#include "bsp.h"
#include "imu_sim.h"
#include "lab_spirit.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define FRAME_MS 33u
#define SAMPLE_DT_S 0.033f /* keep sim time in step with ~30 Hz */

void ex01_step02_start(void)
{
    float acc[3];
    float gyro[3];
    float t = 0.0f;
    uint32_t rng = 1u;
    int real;
    int was_real = -1;
    spirit_init();
    spirit_start();

    for (;;) {
        /* Prefer hardware IMU; fall back to sim when read fails. */
        real = (bsp_imu_read(acc, gyro) == 0);
        if (!real) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += SAMPLE_DT_S;
        }
        if (real != was_real) {
            was_real = real;
            /* Bright core for both live and sim so the bubble stays clear. */
            spirit_set_live(1);
        }

        spirit_set_accel_mps2(acc[0], acc[1]);

        /*
         * Contract: this loop stays short. Do not call beep_play (or other
         * long blocking work) here — that is the broken design in step 03.
         */
        vTaskDelay(pdMS_TO_TICKS(FRAME_MS));
    }
}
