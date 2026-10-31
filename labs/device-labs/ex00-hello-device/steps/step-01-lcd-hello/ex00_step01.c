/*
 * Lab 00 / Step 01 — LCD hello
 *
 * WHAT THIS CODE DOES
 *   Success: LCD shows a title, a subtitle, and a yellow counter that
 *   increments about once per second.
 *
 * HOW IT WORKS
 *   Not yet: second task, notify, speaker, IMU.
 *   LCD helper (labs/common/lab_lcd.h)
 *   Rows are 0-based. ASCII only. Scale is integer 1–3.
 *   Architecture
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Need a proven display path before sound or sensors.
 *   - A tight empty loop would burn CPU and fight the scheduler.
 *   - Students must activate ESP-IDF in the shell before idf.py works.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: LCD shows a title, a subtitle, and a yellow counter that
 *   increments about once per second.
 *
 * WHAT YOU LEARN HERE
 *   Why a delay
 *   Ticks
 *   Prefer pdMS_TO_TICKS(ms) over raw tick counts.
 *   vTaskDelay vs vTaskDelayUntil
 *   Mental model: pace + yield, not busy-spin.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex00_step01.h"

#include "bsp.h"
#include "lab_lcd.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void ex00_step01_start(void)
{
    int cnt = 0;

    /* Three rows: 0 title, 1 subtitle, 2 live counter (ASCII only). */
    (void)lcd_begin(3);
    lcd_printf(0, "Hello Device");
    lcd_printf(1, "step 01 - LCD");

    lcd_set_color(2, RB_LABEL_YELLOW); /* RB_LABEL_* palette */
    lcd_set_scale(2, 2);               /* scale 1..3 */

    for (;;) {
        lcd_printf(2, "%d", cnt++);
        /* ~1 Hz update AND yield — never spin with an empty loop. */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
