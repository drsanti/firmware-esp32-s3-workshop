/*
 * Lab 06 / Step 02 — Live names
 *
 * WHAT THIS CODE DOES
 *   Success: names change from fake strings to real kernel names
 *   (including IDLE / ui).
 *
 * HOW IT WORKS
 *   Architecture
 *   Sequence
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Hard-coded names lie when you add tasks.
 *   - Need a snapshot API, not a continuous trace.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: names change from fake strings to real kernel names
 *   (including IDLE / ui).
 *
 * WHAT YOU LEARN HERE
 *   Rejected here: parsing vTaskList text — awkward for LCD columns.
 *   Prefer the structured array.
 *   Mental model: photo of the task list, not a video of the scheduler.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex06_step02.h"

#include "bsp.h"
#include "lab_mon.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define SNAP_MAX 16u
#define UI_MS 500u

static void load_task(void *arg)
{
    (void)arg;
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(400));
    }
}

static void refresh_names(void)
{
    TaskStatus_t snap[SNAP_MAX];
    UBaseType_t n;
    UBaseType_t i;
    unsigned row = 0;

    n = uxTaskGetSystemState(snap, SNAP_MAX, NULL);
    mon_clear_rows();
    for (i = 0; i < n && row < MON_ROWS; i++) {
        mon_set_row(row, "%.8s", snap[i].pcTaskName);
        row++;
    }
    mon_set_title("names %u", (unsigned)n);
}

static void ui_task(void *arg)
{
    (void)arg;
    mon_init();
    mon_start();

    for (;;) {
        refresh_names();
        vTaskDelay(pdMS_TO_TICKS(UI_MS));
    }
}

void ex06_step02_start(void)
{
(void)xTaskCreate(load_task, "load_a", 2048, NULL, 3, NULL);
    (void)xTaskCreate(load_task, "load_b", 2048, NULL, 3, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
