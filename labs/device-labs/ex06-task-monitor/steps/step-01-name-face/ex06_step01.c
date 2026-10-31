/*
 * Lab 06 / Step 01 — Name face
 *
 * WHAT THIS CODE DOES
 *   Success: you see a compact three-row list layout.
 *
 * HOW IT WORKS
 *   Architecture
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Need a readable monitor face before learning introspection APIs.
 *   - LCD has only four lines — title + three rows.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: you see a compact three-row list layout.
 *
 * WHAT YOU LEARN HERE
 *   Mental model: layout before kernel truth.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex06_step01.h"

#include "bsp.h"
#include "lab_mon.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

void ex06_step01_start(void)
{
mon_init();
    mon_start();
    mon_set_title("Task Mon");
    mon_set_row(0, "ui");
    mon_set_row(1, "load_a");
    mon_set_row(2, "load_b");

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
