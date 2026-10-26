/*
 * Demo 03 — six FreeRTOS tasks, each logging to its own rb_term.
 * Tree: display → screen → layout(3 cols × 2 rows) → rb_term panes.
 */
#include "bsp.h"
#include "rb_screen.h"
#include "rb_term.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

typedef TickType_t demo_tick_t;

#define DEMO_ROWS 2
#define DEMO_COLS 3
#define DEMO_N (DEMO_ROWS * DEMO_COLS)

static rb_display_t g_disp;
static rb_screen_t g_screen;
static rb_layout_t g_layout;
static rb_term_t g_terms[DEMO_N];

typedef struct {
    rb_term_t *term;
    uint8_t id;
    uint16_t fg;
    uint32_t period_ms;
} term_cfg_t;

static const uint16_t k_fg[DEMO_N] = {
    RB_TERM_CYAN,    RB_TERM_YELLOW,  RB_TERM_GREEN,
    RB_TERM_MAGENTA, RB_TERM_RED,     RB_TERM_WHITE,
};

static void term_task(void *arg)
{
    term_cfg_t *cfg = (term_cfg_t *)arg;
    uint32_t n = 0;

    rb_term_set_color(cfg->term, cfg->fg, RB_TERM_BLACK);
    rb_term_log_printf_ex(cfg->term, cfg->fg, RB_TERM_BLACK,
                          "term %u ready", (unsigned)cfg->id);

    for (;;) {
        rb_term_log_printf_ex(cfg->term, cfg->fg, RB_TERM_BLACK,
                              "t%u #%" PRIu32, (unsigned)cfg->id, n);
        n++;
        {
            demo_tick_t delay = pdMS_TO_TICKS(cfg->period_ms);
            vTaskDelay(delay);
        }
    }
}

void app_main(void)
{
    static term_cfg_t cfgs[DEMO_N];
    static char task_names[DEMO_N][8];
    int i;

    bsp_init();

    (void)rb_display_open(&g_disp);
    (void)rb_screen_init(&g_screen, &g_disp);
    (void)rb_layout_init(&g_layout, &g_screen, RB_LAYOUT_GRID, DEMO_ROWS,
                         DEMO_COLS, 2);
    for (i = 0; i < DEMO_N; i++) {
        rb_term_init(&g_terms[i]);
        (void)rb_term_set_scale(&g_terms[i], 1);
        (void)rb_layout_add(&g_layout, &g_terms[i].base);
    }
    rb_screen_set_root(&g_screen, &g_layout.base);
    rb_screen_reflow(&g_screen);

    for (i = 0; i < DEMO_N; i++) {
        cfgs[i].term = &g_terms[i];
        cfgs[i].id = (uint8_t)i;
        cfgs[i].fg = k_fg[i];
        cfgs[i].period_ms = 250u + (uint32_t)(i * 50);
        (void)snprintf(task_names[i], sizeof(task_names[i]), "tm%02d", i);
        (void)xTaskCreate(term_task, task_names[i], 3072, &cfgs[i], 5, NULL);
    }

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
