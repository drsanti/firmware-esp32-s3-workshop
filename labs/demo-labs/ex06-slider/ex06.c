/*
 * Demo 06 — slider
 *
 * WHAT THIS CODE DOES
 *   Nested layout rb_slider — V inputs drive H progress
 *
 * HOW IT WORKS
 *   main/main.c calls ex06_start() after bsp_init().
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
#include "ex06.h"

#include "bsp.h"
#include "rb_screen.h"
#include "rb_slider.h"
#include "rb_style.h"

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

#define DEMO_N 4

static rb_display_t g_disp;
static rb_screen_t g_screen;
static rb_layout_t g_root;
static rb_layout_t g_left;
static rb_layout_t g_right;
static rb_slider_t g_sl[DEMO_N];

static void on_slider_change(rb_slider_t *s, int32_t value, void *user)
{
    rb_slider_t *peer = (rb_slider_t *)user;

    (void)s;
    if (peer) {
        rb_slider_set_value(peer, value);
    }
}

static void touch_task(void *arg)
{
    (void)arg;
    for (;;) {
        bsp_touch_sample_t smp;
        int i;
        if (bsp_touch_read(&smp) == 0) {
            for (i = 0; i < DEMO_N; i++) {
                (void)rb_slider_feed_touch(&g_sl[i], &smp);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void ex06_start(void)
{
    int i;
    (void)rb_display_open(&g_disp);
    (void)rb_screen_init(&g_screen, &g_disp);

    (void)rb_layout_init(&g_root, &g_screen, RB_LAYOUT_COLS, 1, 2, 6);
    (void)rb_layout_init(&g_left, &g_screen, RB_LAYOUT_COLS, 1, 2, 4);
    (void)rb_layout_init(&g_right, &g_screen, RB_LAYOUT_ROWS, 2, 1, 4);

    for (i = 0; i < DEMO_N; i++) {
        rb_slider_init(&g_sl[i]);
        rb_slider_set_range(&g_sl[i], 0, 100);
        rb_slider_set_show_value(&g_sl[i], 1);
        rb_slider_set_show_fill(&g_sl[i], 0);
        rb_slider_set_label(&g_sl[i], "Label");
    }

    /* Left — vertical INPUT */
    rb_slider_set_orient(&g_sl[0], RB_SLIDER_ORIENT_VERTICAL);
    rb_slider_set_mode(&g_sl[0], RB_SLIDER_MODE_INPUT);
    rb_slider_set_style(&g_sl[0], RB_STYLE_PRIMARY);
    rb_slider_set_value(&g_sl[0], 35);
    rb_slider_set_on_change(&g_sl[0], on_slider_change, &g_sl[2]);

    rb_slider_set_orient(&g_sl[1], RB_SLIDER_ORIENT_VERTICAL);
    rb_slider_set_mode(&g_sl[1], RB_SLIDER_MODE_INPUT);
    rb_slider_set_style(&g_sl[1], RB_STYLE_DANGER);
    rb_slider_set_thumb_rgb(&g_sl[1], 232, 62, 140);
    rb_slider_set_value(&g_sl[1], 70);
    rb_slider_set_on_change(&g_sl[1], on_slider_change, &g_sl[3]);

    /* Right — horizontal PROGRESS (driven by left callbacks) */
    rb_slider_set_orient(&g_sl[2], RB_SLIDER_ORIENT_HORIZONTAL);
    rb_slider_set_mode(&g_sl[2], RB_SLIDER_MODE_PROGRESS);
    rb_slider_set_style(&g_sl[2], RB_STYLE_SUCCESS);
    rb_slider_set_value(&g_sl[2], 35);

    rb_slider_set_orient(&g_sl[3], RB_SLIDER_ORIENT_HORIZONTAL);
    rb_slider_set_mode(&g_sl[3], RB_SLIDER_MODE_PROGRESS);
    rb_slider_set_style(&g_sl[3], RB_STYLE_WARNING);
    rb_slider_set_value(&g_sl[3], 70);

    (void)rb_layout_add(&g_left, &g_sl[0].base);
    (void)rb_layout_add(&g_left, &g_sl[1].base);
    (void)rb_layout_add(&g_right, &g_sl[2].base);
    (void)rb_layout_add(&g_right, &g_sl[3].base);
    (void)rb_layout_add(&g_root, &g_left.base);
    (void)rb_layout_add(&g_root, &g_right.base);

    rb_screen_set_root(&g_screen, &g_root.base);
    rb_screen_reflow(&g_screen);

    (void)xTaskCreate(touch_task, "touch", 3072, 0, 5, 0);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
