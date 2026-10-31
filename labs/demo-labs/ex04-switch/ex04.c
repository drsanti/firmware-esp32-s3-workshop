/*
 * Demo 04 — switch
 *
 * WHAT THIS CODE DOES
 *   2x2 rb_switch collage (pill/rect x ON/OFF)
 *
 * HOW IT WORKS
 *   main/main.c calls ex04_start() after bsp_init().
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
#include "ex04.h"

#include "bsp.h"
#include "rb_screen.h"
#include "rb_switch.h"

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

#define DEMO_ROWS 2
#define DEMO_COLS 2
#define DEMO_N (DEMO_ROWS * DEMO_COLS)

static rb_display_t g_disp;
static rb_screen_t g_screen;
static rb_layout_t g_layout;
static rb_switch_t g_sw[DEMO_N];

static void on_switch_change(rb_switch_t *sw, int on, void *user)
{
    rb_switch_t *peer = (rb_switch_t *)user;

    (void)sw;
    if (peer) {
        rb_switch_set_on(peer, on);
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
                (void)rb_switch_feed_touch(&g_sw[i], &smp);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void ex04_start(void)
{
    int i;
    (void)rb_display_open(&g_disp);
    (void)rb_screen_init(&g_screen, &g_disp);
    (void)rb_layout_init(&g_layout, &g_screen, RB_LAYOUT_GRID, DEMO_ROWS,
                         DEMO_COLS, 4);

    /* Collage: pill YES/NO, rect ON/OFF (both shapes) */
    rb_switch_init(&g_sw[0]);
    rb_switch_set_shape(&g_sw[0], RB_SWITCH_SHAPE_PILL);
    rb_switch_set_labels(&g_sw[0], "YES", "NO");
    rb_switch_set_on(&g_sw[0], 1);

    rb_switch_init(&g_sw[1]);
    rb_switch_set_shape(&g_sw[1], RB_SWITCH_SHAPE_ROUND_RECT);
    rb_switch_set_labels(&g_sw[1], "YES", "NO");
    rb_switch_set_on(&g_sw[1], 1);

    rb_switch_init(&g_sw[2]);
    rb_switch_set_shape(&g_sw[2], RB_SWITCH_SHAPE_PILL);
    rb_switch_set_on(&g_sw[2], 1);

    rb_switch_init(&g_sw[3]);
    rb_switch_set_shape(&g_sw[3], RB_SWITCH_SHAPE_ROUND_RECT);
    rb_switch_set_on(&g_sw[3], 1);

    /* Top → bottom column peers (user touch only; set_on does not re-fire). */
    rb_switch_set_on_change(&g_sw[0], on_switch_change, &g_sw[2]);
    rb_switch_set_on_change(&g_sw[1], on_switch_change, &g_sw[3]);
    rb_switch_set_on_change(&g_sw[2], on_switch_change, &g_sw[0]);
    rb_switch_set_on_change(&g_sw[3], on_switch_change, &g_sw[1]);

    for (i = 0; i < DEMO_N; i++) {
        (void)rb_layout_add(&g_layout, &g_sw[i].base);
    }
    rb_screen_set_root(&g_screen, &g_layout.base);
    rb_screen_reflow(&g_screen);

    (void)xTaskCreate(touch_task, "touch", 3072, NULL, 5, NULL);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
