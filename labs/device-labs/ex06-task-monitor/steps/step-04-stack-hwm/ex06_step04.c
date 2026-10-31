/*
 * Lab 06 / Step 04 — Stack HWM
 *
 * WHAT THIS CODE DOES
 *   Success: HWM numbers are visible and make sense vs create stack
 *   depth.
 *
 * HOW IT WORKS
 *   Architecture
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Stack overflow is silent until crash — need a margin metric.
 *   - Students confuse HWM with “stack used right now.”
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: HWM numbers are visible and make sense vs create stack
 *   depth.
 *
 * WHAT YOU LEARN HERE
 *   Units: words (on Xtensa, typically 4 bytes/word — confirm in docs if
 *   you convert to bytes).
 *   Mental model: how close did this task ever get to the cliff?
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex06_step04.h"

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
    /* Extra locals burn a little stack so HWM differs across tasks. */
    volatile uint32_t pad[32];
    unsigned i;

    (void)arg;
    for (i = 0; i < 32u; i++) {
        pad[i] = i;
    }

    for (;;) {
        (void)pad[0];
        vTaskDelay(pdMS_TO_TICKS(400));
    }
}

static void refresh(void)
{
    TaskStatus_t snap[SNAP_MAX];
    UBaseType_t n;
    UBaseType_t i;
    unsigned row = 0;
    UBaseType_t hwm;

    n = uxTaskGetSystemState(snap, SNAP_MAX, NULL);
    mon_clear_rows();
    for (i = 0; i < n && row < MON_ROWS; i++) {
        /* Prefer snapshot field; single-task API is equivalent: */
        hwm = snap[i].usStackHighWaterMark;
        if (snap[i].xHandle != NULL) {
            hwm = uxTaskGetStackHighWaterMark(snap[i].xHandle);
        }
        mon_set_row(row, "%.5s %c%u %u", snap[i].pcTaskName,
                    state_ch(snap[i].eCurrentState),
                    (unsigned)snap[i].uxCurrentPriority, (unsigned)hwm);
        row++;
    }
    mon_set_title("HWM %u", (unsigned)n);
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

void ex06_step04_start(void)
{
(void)xTaskCreate(load_task, "load_a", 2048, NULL, 3, NULL);
    (void)xTaskCreate(load_task, "load_b", 3072, NULL, 2, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
