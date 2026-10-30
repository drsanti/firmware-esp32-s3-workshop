/*
 * ex04 — task priority (preemption + starvation).
 *
 * Teaching focus:
 *   - larger FreeRTOS priority → more CPU when competing
 *   - Expect counters HIGH > MID > LOW in YLD
 *   - HIGH busy-loop in HOG → MID/LOW freeze
 *
 * All workers: asymmetric burst + vTaskDelay(1) (WDT-safe, fair create).
 * HOG: HIGH skips the delay so it never yields the core.
 *
 * Workers on core 0. UI on core 1.
 */
#include "ex04.h"

#include "ex_terms.h"

#include <stdint.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define HOG_SPIN_MS 40u
#define PHASE_YIELD_MS 5000u
#define PHASE_HOG_MS 4000u
#define UI_MS 50u

/* Same pattern for all; bigger burst = more counts per timeslice. */
#define HIGH_BURST 4096u
#define MID_BURST 1024u
#define LOW_BURST 128u

enum { IDX_HI = 0, IDX_MID = 1, IDX_LO = 2, IDX_COUNT = 3 };

typedef struct {
    uint8_t is_high;
    uint8_t idx;
    uint32_t burst;
} worker_cfg_t;

/* 0 = YIELD (HIGH delays like the others), 1 = HOG (HIGH never delays). */
static volatile uint8_t s_hog;
static volatile uint8_t s_run;
static volatile uint32_t s_count[IDX_COUNT];

static void busy_spin_ms(uint32_t ms)
{
    TickType_t start = xTaskGetTickCount();
    TickType_t wait = pdMS_TO_TICKS(ms);

    if (wait < 1) {
        wait = 1;
    }
    while ((xTaskGetTickCount() - start) < wait) {
    }
}

static void worker_task(void *arg)
{
    worker_cfg_t *cfg = (worker_cfg_t *)arg;
    uint32_t i;

    while (!s_run) {
        vTaskDelay(1);
    }

    for (;;) {
        for (i = 0; i < cfg->burst; i++) {
            s_count[cfg->idx]++;
        }

        if (cfg->is_high && s_hog) {
            /* Stay Running — MID/LOW stay Ready but never get the core. */
            busy_spin_ms(HOG_SPIN_MS);
        } else {
            vTaskDelay(1);
        }
    }
}

static void phase_task(void *arg)
{
    (void)arg;

    while (!s_run) {
        vTaskDelay(1);
    }

    for (;;) {
        s_hog = 0;
        vTaskDelay(pdMS_TO_TICKS(PHASE_YIELD_MS));
        s_hog = 1;
        vTaskDelay(pdMS_TO_TICKS(PHASE_HOG_MS));
    }
}

static void ui_task(void *arg)
{
    rb_term_t *term = (rb_term_t *)arg;
    char line[RB_TERM_LOG_MAX_COLS];
    static const char *names[IDX_COUNT] = {"HIGH", "MID", "LOW"};
    static const uint16_t fgs[IDX_COUNT] = {RB_TERM_CYAN, RB_TERM_YELLOW,
                                            RB_TERM_MAGENTA};
    static const unsigned prios[IDX_COUNT] = {6u, 5u, 4u};
    unsigned i;

    rb_term_set_padding(term, 4, 4, 4, 4);
    (void)rb_term_apply_geometry(term);

    rb_term_printf_at_ex(term, 0, 0, RB_TERM_WHITE, RB_TERM_BLACK,
                         "ex04 priority  starting...");

    while (!s_run) {
        vTaskDelay(1);
    }

    for (;;) {
        const char *phase = s_hog ? "HOG" : "YLD";

        rb_term_printf_at_ex(term, 0, 0, RB_TERM_WHITE, RB_TERM_BLACK,
                             "ex04 priority  phase=%s   ", phase);

        for (i = 0; i < IDX_COUNT; i++) {
            (void)snprintf(line, sizeof(line), "%-4s p=%u  #%lu          ",
                           names[i], prios[i],
                           (unsigned long)s_count[i]);
            rb_term_set_line_ex(term, (uint16_t)(i + 2u), fgs[i],
                                RB_TERM_BLACK, line);
        }

        vTaskDelay(pdMS_TO_TICKS(UI_MS));
    }
}

void ex04_start(void)
{
    static worker_cfg_t hi;
    static worker_cfg_t mid;
    static worker_cfg_t lo;
    rb_term_t *term;

    (void)ex_terms_begin(1, 1);
    term = ex_terms_get(0);

    hi.is_high = 1;
    hi.idx = IDX_HI;
    hi.burst = HIGH_BURST;

    mid.is_high = 0;
    mid.idx = IDX_MID;
    mid.burst = MID_BURST;

    lo.is_high = 0;
    lo.idx = IDX_LO;
    lo.burst = LOW_BURST;

    s_hog = 0;
    s_run = 0;
    s_count[IDX_HI] = 0;
    s_count[IDX_MID] = 0;
    s_count[IDX_LO] = 0;

    (void)xTaskCreatePinnedToCore(ui_task, "ui", 3072, term, 3, NULL, 1);
    (void)xTaskCreatePinnedToCore(phase_task, "phase", 2048, NULL, 7, NULL, 0);
    (void)xTaskCreatePinnedToCore(worker_task, "hi", 2048, &hi, 6, NULL, 0);
    (void)xTaskCreatePinnedToCore(worker_task, "mid", 2048, &mid, 5, NULL, 0);
    (void)xTaskCreatePinnedToCore(worker_task, "lo", 2048, &lo, 4, NULL, 0);

    s_run = 1;
}
