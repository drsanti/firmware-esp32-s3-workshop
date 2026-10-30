/*
 * ex01 — task affinity (pin to core 0 / core 1).
 *
 * Teaching focus:
 *   - xTaskCreatePinnedToCore → affinity (which CPU may run this task)
 *   - xPortGetCoreID()        → prove where we actually run
 *   - dual-core ESP32-S3       → two schedulers, shared heap / objects
 *
 * LCD: common/ex_terms — one pane per core so affinity is visible.
 */
#include "ex01.h"

#include "ex_terms.h"

#include <stdint.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

typedef struct {
    rb_term_t *term;
    const char *name;
    uint16_t fg;
    uint32_t period_ms;
    BaseType_t pin_core; /* 0 or 1 — what we asked for at create */
} pane_cfg_t;

static void pane_task(void *arg)
{
    pane_cfg_t *cfg = (pane_cfg_t *)arg;
    uint32_t n = 0;

    ex_terms_log(cfg->term, cfg->fg, "%s pin=%d on=%d", cfg->name,
                 (int)cfg->pin_core, (int)xPortGetCoreID());

    for (;;) {
        /* Log counter + live core id (should match pin_core every time). */
        ex_terms_log(cfg->term, cfg->fg, "%s #%lu c%d", cfg->name,
                     (unsigned long)n, (int)xPortGetCoreID());
        n++;
        vTaskDelay(pdMS_TO_TICKS(cfg->period_ms));
    }
}

void ex01_start(void)
{
    static pane_cfg_t core0;
    static pane_cfg_t core1;

    (void)ex_terms_begin(1, 2);

    core0.term = ex_terms_get(0);
    core0.name = "core0";
    core0.fg = RB_TERM_CYAN;
    core0.period_ms = 500u;
    core0.pin_core = 0;

    core1.term = ex_terms_get(1);
    core1.name = "core1";
    core1.fg = RB_TERM_YELLOW;
    core1.period_ms = 500u;
    core1.pin_core = 1;

    /*
     *   fn, name, stack, arg, prio, handle, coreID
     * Same period on purpose — the lesson is affinity, not rate.
     */
    (void)xTaskCreatePinnedToCore(pane_task, "core0", 3072, &core0, 4, NULL,
                                  0);
    (void)xTaskCreatePinnedToCore(pane_task, "core1", 3072, &core1, 4, NULL,
                                  1);
}
