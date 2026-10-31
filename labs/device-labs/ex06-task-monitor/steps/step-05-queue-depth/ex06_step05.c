/*
 * Lab 06 / Step 05 — Queue depth + page
 *
 * WHAT THIS CODE DOES
 *   Success: queue count moves; BOOT flips pages of the task list.
 *
 * HOW IT WORKS
 *   Architecture
 *   Sequence
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Only three LCD rows — need paging for longer lists.
 *   - Queue fill is invisible without MessagesWaiting / spaces.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: queue count moves; BOOT flips pages of the task list.
 *
 * WHAT YOU LEARN HERE
 *   Contrast Lab 05: there the queue carried samples. Here the queue
 *   exists so you can see depth on the monitor.
 *   Mental model: depth is a gauge; the task list is a paged table.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex06_step05.h"

#include "bsp.h"
#include "lab_mon.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/queue.h"
#include "freertos/task.h"

#define SNAP_MAX 16u
#define UI_MS 500u
#define QUEUE_LEN 8u
#define BOOT_BTN_MASK 2u

static QueueHandle_t g_demo_q;
static TaskStatus_t g_snap[SNAP_MAX];
static UBaseType_t g_snap_n;
static unsigned g_page;

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

static int boot_pressed(uint32_t *was)
{
    uint32_t now = bsp_buttons() & BOOT_BTN_MASK;
    int edge = (now != 0u && *was == 0u) ? 1 : 0;
    *was = now;
    return edge;
}

static void load_task(void *arg)
{
    (void)arg;
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(400));
    }
}

static void fill_task(void *arg)
{
    uint8_t token = 1;

    (void)arg;
    for (;;) {
        (void)xQueueSend(g_demo_q, &token, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void take_snapshot(void)
{
    g_snap_n = uxTaskGetSystemState(g_snap, SNAP_MAX, NULL);
}

static void draw_page(void)
{
    unsigned pages;
    unsigned start;
    unsigned row;
    unsigned i;
    UBaseType_t waiting;
    UBaseType_t spaces;
    UBaseType_t hwm;
    uint8_t token;

    pages = ((unsigned)g_snap_n + MON_ROWS - 1u) / MON_ROWS;
    if (pages == 0u) {
        pages = 1u;
    }
    if (g_page >= pages) {
        g_page = 0;
    }

    /* Drain one queued item so depth moves over time. */
    (void)xQueueReceive(g_demo_q, &token, 0);

    waiting = uxQueueMessagesWaiting(g_demo_q);
    spaces = uxQueueSpacesAvailable(g_demo_q);
    mon_set_title("Q%u/%u p%u", (unsigned)waiting,
                  (unsigned)(waiting + spaces), g_page + 1u);

    start = g_page * MON_ROWS;
    mon_clear_rows();
    for (row = 0; row < MON_ROWS; row++) {
        i = start + row;
        if (i >= (unsigned)g_snap_n) {
            break;
        }
        hwm = uxTaskGetStackHighWaterMark(g_snap[i].xHandle);
        mon_set_row(row, "%.5s %c%u %u", g_snap[i].pcTaskName,
                    state_ch(g_snap[i].eCurrentState),
                    (unsigned)g_snap[i].uxCurrentPriority, (unsigned)hwm);
    }
}

static void ui_task(void *arg)
{
    uint32_t boot_was = 0;

    (void)arg;
    mon_init();
    mon_start();
    take_snapshot();

    for (;;) {
        if (boot_pressed(&boot_was)) {
            g_page++;
        }
        take_snapshot();
        draw_page();
        vTaskDelay(pdMS_TO_TICKS(UI_MS));
    }
}

void ex06_step05_start(void)
{
g_demo_q = xQueueCreate(QUEUE_LEN, sizeof(uint8_t));
    if (g_demo_q == NULL) {
        return;
    }
    g_page = 0;

    (void)xTaskCreate(load_task, "load_a", 2048, NULL, 3, NULL);
    (void)xTaskCreate(load_task, "load_b", 2048, NULL, 2, NULL);
    (void)xTaskCreate(fill_task, "fill", 2048, NULL, 4, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
