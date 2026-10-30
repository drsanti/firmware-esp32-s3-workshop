/*
 * Lab 02 / Step 02 — Shake detect
 *
 * WHAT THIS CODE DOES
 *   Success: shake (or wait for sim) → red ALARM → back to ARMED; numbers
 *   move.
 *
 * HOW IT WORKS
 *   Architecture
 *   State machine
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Need a shake definition (threshold + edge, not “always high”).
 *   - Must not put long work in this loop (contract for step 03).
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: shake (or wait for sim) → red ALARM → back to ARMED; numbers
 *   move.
 *
 * WHAT YOU LEARN HERE
 *   Still only vTaskDelay for pacing. The design lesson is the contract:
 *   > Sense + draw may share one loop only if every pass stays short.
 *   Step 03 breaks the contract on purpose so you feel the freeze.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex02_step02.h"

#include "bsp.h"
#include "imu_sim.h"
#include "lab_alarm.h"

#include <math.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define SAMPLE_MS 40u
#define SAMPLE_DT_S 0.04f
#define SHAKE_THRESH_MPS2 2.5f
#define ALARM_HOLD_MS 800u

void ex02_step02_start(void)
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

        /* Rising edge → show ALARM for a short hold (visual only). */
        if (shake && !was_shake) {
            alarm_set_state(ALARM_ST_ALARM);
            alarm_until = xTaskGetTickCount() + pdMS_TO_TICKS(ALARM_HOLD_MS);
        }
        was_shake = shake;

        if (alarm_until != 0 && xTaskGetTickCount() >= alarm_until) {
            alarm_until = 0;
            alarm_set_state(ALARM_ST_ARMED);
        }

        alarm_set_detail("horiz %.1f", (double)horiz);

        /*
         * Contract: this loop stays short. Do not call beep_play (or other
         * long work) here — that is the intentional break in step 03.
         */
        vTaskDelay(pdMS_TO_TICKS(SAMPLE_MS));
    }
}
