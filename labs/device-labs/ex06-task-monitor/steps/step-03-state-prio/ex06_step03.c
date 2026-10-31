/*
 * Lab 06 / Step 03 — State + priority
 *
 * WHAT THIS CODE DOES
 *   Success: you can read who is Blocked vs Running and at which prio.
 *
 * HOW IT WORKS
 *   State letters
 *   Architecture
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Names alone do not show scheduling state.
 *   - Need compact encoding for a narrow LCD.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: you can read who is Blocked vs Running and at which prio.
 *
 * WHAT YOU LEARN HERE
 *   Mental model: columns are fields of one photo.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex06_step03.h"

#include "bsp.h"
#include "lab_mon.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define SNAP_MAX 16u
#define UI_MS 500u

static char state_ch(eTaskState st)
{
    switch (st) {
    case eRunning:
        return 'r';
    case eReady:
        return 'R';
    case eBlocked:
        return 'B';
    case eSuspended:
        return 'S';
    case eDeleted:
        return 'D';
    default:
        return '?';
    }
}

static void load_task(void *arg)
{
    (void)arg;
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(400));
    }
}

static void refresh(void)
{
    TaskStatus_t snap[SNAP_MAX];
    UBaseType_t n;
    UBaseType_t i;
    unsigned row = 0;

    n = uxTaskGetSystemState(snap, SNAP_MAX, NULL);
    mon_clear_rows();
    for (i = 0; i < n && row < MON_ROWS; i++) {
        mon_set_row(row, "%.6s %c %u", snap[i].pcTaskName,
                    state_ch(snap[i].eCurrentState),
                    (unsigned)snap[i].uxCurrentPriority);
        row++;
    }
    mon_set_title("st+prio %u", (unsigned)n);
}

static void ui_task(void *arg)
{
    (void)arg;
    mon_init();
    mon_start();

    for (;;) {
        refresh();
        vTaskDelay(pdMS_TO_TICKS(UI_MS));
    }
}

void ex06_step03_start(void)
{
(void)xTaskCreate(load_task, "load_a", 2048, NULL, 3, NULL);
    (void)xTaskCreate(load_task, "load_b", 2048, NULL, 2, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
