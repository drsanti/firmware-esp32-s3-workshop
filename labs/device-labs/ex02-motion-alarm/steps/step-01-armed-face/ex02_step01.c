/*
 * Lab 02 / Step 01 — Armed face
 *
 * WHAT THIS CODE DOES
 *   Success: green ARMED banner; counter ticks.
 *
 * HOW IT WORKS
 *   Architecture
 *   State machine
 *   This step holds ARMED only; the detail line pulses on each loop.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Need a clear alarm face, not raw numbers.
 *   - Need paced redraw + yield (same delay lesson as Lab 00/01).
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: green ARMED banner; counter ticks.
 *
 * WHAT YOU LEARN HERE
 *   Same idea as Lab 00/01: delay sets the pulse rate and blocks this
 *   task so the scheduler can run idle (and later peers). A tight for
 *   (;;) with no delay would burn CPU for no UI gain.
 *   Not yet: tasks, queues. Step 02 adds IMU.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex02_step01.h"

#include "bsp.h"
#include "lab_alarm.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define PULSE_MS 500u

void ex02_step01_start(void)
{
    unsigned pulse = 0;
    alarm_init();
    alarm_start();
    alarm_set_state(ALARM_ST_ARMED);
    alarm_set_live(1);

    for (;;) {
        pulse++;
        alarm_set_detail("watching... %u", pulse);

        /* Pace the banner; yield so FreeRTOS idle can run. */
        vTaskDelay(pdMS_TO_TICKS(PULSE_MS));
    }
}
