/*
 * Demo 01 — label
 *
 * WHAT THIS CODE DOES
 *   4x4 rb_label grid via rb_screen / rb_layout
 *
 * HOW IT WORKS
 *   main/main.c calls ex01_start() after bsp_init().
 *   This file builds the rb_* widget tree and starts any demo tasks.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   Seventeen separate IDF apps make flash/reconfigure slow for a widget catalog.
 *
 * PROBLEM THIS FILE SOLVES
 *   One flashable demo-labs binary; switch demos in main/main.c.
 *
 * WHAT YOU LEARN HERE
 *   - rb_screen / rb_layout composition for this widget
 *   - See README.md in this folder for the full labsheet
 *
 * Labsheet: README.md in this folder.
 */
#include "ex01.h"

#include "bsp.h"
#include "rb_label.h"
#include "rb_screen.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

/* Touch a FreeRTOS.h type so include-cleaner keeps the required header. */
typedef TickType_t lab_tick_t;

#define LAB_ROWS 4
#define LAB_COLS 4
#define LAB_N (LAB_ROWS * LAB_COLS)

static rb_display_t g_disp;
static rb_screen_t g_screen;
static rb_layout_t g_layout;
static rb_label_t g_labels[LAB_N];

typedef struct {
    rb_label_t *label;
    uint8_t id;
    uint16_t color;
    uint32_t period_ms;
} counter_cfg_t;

static const uint16_t k_colors[LAB_N] = {
    RB_LABEL_CYAN,    RB_LABEL_YELLOW,  RB_LABEL_GREEN,   RB_LABEL_MAGENTA,
    RB_LABEL_RED,     RB_LABEL_BLUE,    RB_LABEL_WHITE,   RB_LABEL_GRAY,
    RB_LABEL_CYAN,    RB_LABEL_YELLOW,  RB_LABEL_GREEN,   RB_LABEL_MAGENTA,
    RB_LABEL_RED,     RB_LABEL_BLUE,    RB_LABEL_WHITE,   RB_LABEL_GRAY,
};

static void counter_task(void *arg)
{
    counter_cfg_t *cfg = (counter_cfg_t *)arg;
    uint32_t n = 0;

    for (;;) {
        rb_label_set_color(cfg->label, cfg->color, RB_LABEL_BLACK);
        rb_label_printf(cfg->label, "%u %" PRIu32, (unsigned)cfg->id, n);
        n++;
        {
            lab_tick_t delay = pdMS_TO_TICKS(cfg->period_ms);
            vTaskDelay(delay);
        }
    }
}

void ex01_start(void)
{
    static counter_cfg_t cfgs[LAB_N];
    static char task_names[LAB_N][8];
    int i;
    (void)rb_display_open(&g_disp);
    (void)rb_screen_init(&g_screen, &g_disp);
    (void)rb_layout_init(&g_layout, &g_screen, RB_LAYOUT_GRID, LAB_ROWS,
                         LAB_COLS, 2);
    for (i = 0; i < LAB_N; i++) {
        rb_label_init(&g_labels[i]);
        (void)rb_label_set_scale(&g_labels[i], 1);
        (void)rb_layout_add(&g_layout, &g_labels[i].base);
    }
    rb_screen_set_root(&g_screen, &g_layout.base);
    rb_screen_reflow(&g_screen);

    for (i = 0; i < LAB_N; i++) {
        cfgs[i].label = &g_labels[i];
        cfgs[i].id = (uint8_t)i;
        cfgs[i].color = k_colors[i];
        cfgs[i].period_ms = 200u + (uint32_t)(i * 40);
        (void)snprintf(task_names[i], sizeof(task_names[i]), "c%02d", i);
        (void)xTaskCreate(counter_task, task_names[i], 3072, &cfgs[i], 5,
                          NULL);
    }

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
