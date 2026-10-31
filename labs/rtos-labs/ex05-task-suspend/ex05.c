/*
 * ex05 — task suspend / resume.
 *
 * Teaching focus:
 *   - vTaskSuspend removes a task from scheduling (not deleted)
 *   - vTaskResume puts it back; local state (counter) is preserved
 *   - each toggle button controls one worker independently
 *
 * LCD 2x2 grid: term A | term B / button A | button B
 * Workers on core 0; UI (touch + status) on core 1.
 */
#include "ex05.h"

#include "bsp.h"
#include "rb_button.h"
#include "rb_screen.h"
#include "rb_style.h"
#include "rb_term.h"

#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define WORKER_MS 20u
#define UI_MS 20u
#define N_WORKERS 2u

enum { IDX_A = 0, IDX_B = 1 };

static rb_display_t s_disp;
static rb_screen_t s_screen;
static rb_layout_t s_layout;
static rb_term_t s_term[N_WORKERS];
static rb_button_t s_btn[N_WORKERS];

static TaskHandle_t s_handle[N_WORKERS];
static volatile uint32_t s_count[N_WORKERS];
/* 1 = running (resumed), 0 = suspended — mirrored from button state. */
static volatile uint8_t s_running[N_WORKERS];

static void worker_task(void *arg)
{
    unsigned idx = (unsigned)(uintptr_t)arg;

    for (;;) {
        s_count[idx]++;
        vTaskDelay(pdMS_TO_TICKS(WORKER_MS));
    }
}

static void on_toggle(rb_button_t *btn, rb_button_event_t ev, void *user)
{
    unsigned idx = (unsigned)(uintptr_t)user;
    TaskHandle_t h;

    if (ev != RB_BUTTON_EV_CLICK || idx >= N_WORKERS) {
        return;
    }
    h = s_handle[idx];
    if (h == NULL) {
        return;
    }

    if (rb_button_is_on(btn)) {
        s_running[idx] = 1;
        vTaskResume(h);
    } else {
        s_running[idx] = 0;
        vTaskSuspend(h);
    }
}

static void ui_task(void *arg)
{
    static const char *names[N_WORKERS] = {"A", "B"};
    static const uint16_t fgs[N_WORKERS] = {RB_TERM_CYAN, RB_TERM_YELLOW};
    char line[RB_TERM_LOG_MAX_COLS];
    unsigned i;

    (void)arg;

    for (i = 0; i < N_WORKERS; i++) {
        rb_term_set_padding(&s_term[i], 4, 4, 4, 4);
        (void)rb_term_apply_geometry(&s_term[i]);
    }

    for (;;) {
        bsp_touch_sample_t smp;

        if (bsp_touch_read(&smp) == 0) {
            for (i = 0; i < N_WORKERS; i++) {
                (void)rb_button_feed_touch(&s_btn[i], &smp);
            }
        }

        for (i = 0; i < N_WORKERS; i++) {
            const char *st = s_running[i] ? "RUN" : "SUSP";

            (void)snprintf(line, sizeof(line), "%s  #%lu", names[i],
                           (unsigned long)s_count[i]);
            rb_term_set_line_ex(&s_term[i], 0, fgs[i], RB_TERM_BLACK, line);
            (void)snprintf(line, sizeof(line), "state %s", st);
            rb_term_set_line_ex(&s_term[i], 1, fgs[i], RB_TERM_BLACK, line);
        }

        vTaskDelay(pdMS_TO_TICKS(UI_MS));
    }
}

void ex05_start(void)
{
    unsigned i;
    static const rb_style_t styles[N_WORKERS] = {RB_STYLE_INFO, RB_STYLE_WARNING};

    s_count[IDX_A] = 0;
    s_count[IDX_B] = 0;
    s_running[IDX_A] = 1;
    s_running[IDX_B] = 1;
    s_handle[IDX_A] = NULL;
    s_handle[IDX_B] = NULL;

    if (rb_display_open(&s_disp) != 0) {
        return;
    }
    if (rb_screen_init(&s_screen, &s_disp) != 0) {
        return;
    }
    /* rows: terms on top, buttons below — cols: A | B */
    if (rb_layout_init(&s_layout, &s_screen, RB_LAYOUT_GRID, 2, 2, 4) != 0) {
        return;
    }

    for (i = 0; i < N_WORKERS; i++) {
        rb_term_init(&s_term[i]);
        (void)rb_term_set_scale(&s_term[i], 1);
        (void)rb_layout_add(&s_layout, &s_term[i].base);
    }
    for (i = 0; i < N_WORKERS; i++) {
        rb_button_init(&s_btn[i]);
        rb_button_set_mode(&s_btn[i], RB_BUTTON_MODE_TOGGLE);
        rb_button_set_labels(&s_btn[i], "RUN", "SUSP");
        rb_button_set_style(&s_btn[i], styles[i]);
        rb_button_set_indicator_size(&s_btn[i], 80, 2);
        rb_button_set_on(&s_btn[i], 1); /* start running */
        rb_button_set_on_event(&s_btn[i], on_toggle, (void *)(uintptr_t)i);
        (void)rb_layout_add(&s_layout, &s_btn[i].base);
    }

    rb_screen_set_root(&s_screen, &s_layout.base);
    rb_screen_reflow(&s_screen);

    (void)xTaskCreatePinnedToCore(worker_task, "wkA", 2048,
                                  (void *)(uintptr_t)IDX_A, 5,
                                  &s_handle[IDX_A], 0);
    (void)xTaskCreatePinnedToCore(worker_task, "wkB", 2048,
                                  (void *)(uintptr_t)IDX_B, 5,
                                  &s_handle[IDX_B], 0);
    (void)xTaskCreatePinnedToCore(ui_task, "ui", 4096, NULL, 4, NULL, 1);
}
