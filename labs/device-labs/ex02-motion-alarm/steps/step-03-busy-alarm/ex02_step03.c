/*
 * Lab 02 / Step 03 — Busy alarm (**broken**)
 *
 * WHAT THIS CODE DOES
 *   Success: you can feel/name the architecture bug (not a FreeRTOS
 *   failure).
 *
 * HOW IT WORKS
 *   Architecture
 *   State machine
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - beep_play blocks until the two-tone finishes.
 *   - Sense + draw + audio in one task → UI dies during sound.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: you can feel/name the architecture bug (not a FreeRTOS
 *   failure).
 *
 * WHAT YOU LEARN HERE
 *   FreeRTOS is still scheduling correctly. Your task chose to block on
 *   speaker I/O. Other tasks (if any) could run — but you have only this
 *   loop.
 *   Step 04 moves audio to another task and uses a queue for the event.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex02_step03.h"

#include "bsp.h"
#include "imu_sim.h"
#include "lab_alarm.h"
#include "lab_beep.h"

#include <math.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define SAMPLE_MS 40u
#define SAMPLE_DT_S 0.04f
#define SHAKE_THRESH_MPS2 2.5f

void ex02_step03_start(void)
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
            /*
             * INTENTIONAL BUG: play sound in the detect/draw loop.
             * beep_play blocks for the whole alarm — banner stops updating.
             * Do not copy this into a real product.
             */
            (void)beep_play(BEEP_ALARM, NULL);
            alarm_set_state(ALARM_ST_ARMED);
        }
        was_shake = shake;

        alarm_set_detail("horiz %.1f", (double)horiz);

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_MS));
    }
}
