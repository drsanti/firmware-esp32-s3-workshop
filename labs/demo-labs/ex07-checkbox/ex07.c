/*
 * Demo 07 — checkbox
 *
 * WHAT THIS CODE DOES
 *   2x4 rb_checkbox — styles + paired on_change
 *
 * HOW IT WORKS
 *   main/main.c calls ex07_start() after bsp_init().
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
#include "ex07.h"

#include "bsp.h"
#include "rb_checkbox.h"
#include "rb_screen.h"
#include "rb_style.h"

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

#define DEMO_ROWS 2
#define DEMO_COLS 4
#define DEMO_N (DEMO_ROWS * DEMO_COLS)

static rb_display_t g_disp;
static rb_screen_t g_screen;
static rb_layout_t g_layout;
static rb_checkbox_t g_cb[DEMO_N];

static const rb_style_t k_styles[DEMO_COLS] = {
    RB_STYLE_PRIMARY, RB_STYLE_SUCCESS, RB_STYLE_WARNING, RB_STYLE_DANGER,
};

static const char *const k_labels[DEMO_COLS] = {
    "WiFi", "BLE", "Mute", "Lock",
};

static void on_checkbox_change(rb_checkbox_t *cb, int on, void *user)
{
    rb_checkbox_t *peer = (rb_checkbox_t *)user;

    (void)cb;
    if (peer) {
        rb_checkbox_set_on(peer, on);
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
                (void)rb_checkbox_feed_touch(&g_cb[i], &smp);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void ex07_start(void)
{
    int i;
    (void)rb_display_open(&g_disp);
    (void)rb_screen_init(&g_screen, &g_disp);
    (void)rb_layout_init(&g_layout, &g_screen, RB_LAYOUT_GRID, DEMO_ROWS,
                         DEMO_COLS, 4);

    for (i = 0; i < DEMO_N; i++) {
        int col = i % DEMO_COLS;
        rb_checkbox_init(&g_cb[i]);
        rb_checkbox_set_style(&g_cb[i], k_styles[col]);
        rb_checkbox_set_label(&g_cb[i], k_labels[col]);
        rb_checkbox_set_on(&g_cb[i], (i < DEMO_COLS) && (col & 1) == 0);
        (void)rb_layout_add(&g_layout, &g_cb[i].base);
    }

    /* Top → bottom peers (and reverse) per column. */
    for (i = 0; i < DEMO_COLS; i++) {
        rb_checkbox_set_on_change(&g_cb[i], on_checkbox_change,
                                  &g_cb[i + DEMO_COLS]);
        rb_checkbox_set_on_change(&g_cb[i + DEMO_COLS], on_checkbox_change,
                                  &g_cb[i]);
        rb_checkbox_set_on(&g_cb[i + DEMO_COLS], rb_checkbox_is_on(&g_cb[i]));
    }

    rb_screen_set_root(&g_screen, &g_layout.base);
    rb_screen_reflow(&g_screen);

    (void)xTaskCreate(touch_task, "touch", 3072, 0, 5, 0);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
