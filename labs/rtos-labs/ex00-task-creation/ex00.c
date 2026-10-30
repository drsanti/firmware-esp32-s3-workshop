/*
 * ex00 — create two tasks; watch the scheduler switch them.
 *
 * Teaching focus:
 *   - xTaskCreate       → how a task is born (no core pin)
 *   - two loops         → multitasking (each task has its own stack / PC)
 *   - vTaskDelay        → Blocked so the peer can Run
 *   - xPortGetCoreID()  → which core actually ran this wake (may be 0 or 1)
 *
 * LCD: common/ex_terms — one pane per task so rates + core id are visible.
 */
#include "ex00.h"

#include "ex_terms.h"

#include <stdint.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

/* Per-task config passed as the xTaskCreate "arg" (pvParameters). */
typedef struct {
    rb_term_t *term;
    const char *name;
    uint16_t fg;
    uint32_t period_ms;
} pane_cfg_t;

/*
 * Task body — same function for both tasks; arg selects pane + period.
 * Never returns: FreeRTOS tasks loop forever (or call vTaskDelete).
 */
static void pane_task(void *arg)
{
    pane_cfg_t *cfg = (pane_cfg_t *)arg;
    uint32_t n = 0;

    /* No pin on xTaskCreate — first wake may already be core 0 or 1. */
    ex_terms_log(cfg->term, cfg->fg, "%s ready c%d", cfg->name,
                 (int)xPortGetCoreID());

    for (;;) {
        /* Work: log counter + live core id, then give the CPU back. */
        ex_terms_log(cfg->term, cfg->fg, "%s #%lu c%d", cfg->name,
                     (unsigned long)n, (int)xPortGetCoreID());
        n++;

        /* Running → Blocked for period_ms; scheduler may run the other task. */
        vTaskDelay(pdMS_TO_TICKS(cfg->period_ms));
    }
}

void ex00_start(void)
{
    static pane_cfg_t blink;
    static pane_cfg_t count;

    (void)ex_terms_begin(1, 2); /* UI only — not the FreeRTOS lesson */

    blink.term = ex_terms_get(0);
    blink.name = "blink";
    blink.fg = RB_TERM_CYAN;
    blink.period_ms = 500u; /* faster pane */

    count.term = ex_terms_get(1);
    count.name = "count";
    count.fg = RB_TERM_YELLOW;
    count.period_ms = 1000u; /* slower pane */

    /*
     * Create two Ready tasks. Same priority (4): fairness comes from
     * both calling vTaskDelay, not from different priorities.
     *
     *   fn, name, stack (words), arg, priority, handle
     *
     * No core id → ESP-IDF uses tskNO_AFFINITY (may run on core 0 or 1).
     * Watch cN from xPortGetCoreID() on the LCD; ex01 pins cores explicitly.
     */
    (void)xTaskCreate(pane_task, "blink", 3072, &blink, 4, NULL);
    (void)xTaskCreate(pane_task, "count", 3072, &count, 4, NULL);

    /* ex00_start returns; blink + count keep running under the scheduler. */
}
