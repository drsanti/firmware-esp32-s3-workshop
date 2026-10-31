/*
 * Demo 08 — radio
 *
 * WHAT THIS CODE DOES
 *   Two exclusive rb_radio groups (Mode / Speed)
 *
 * HOW IT WORKS
 *   main/main.c calls ex08_start() after bsp_init().
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
#include "ex08.h"

#include "bsp.h"
#include "rb_radio.h"
#include "rb_screen.h"
#include "rb_style.h"

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

#define DEMO_ROWS 4
#define DEMO_COLS 2
#define DEMO_N (DEMO_ROWS * DEMO_COLS)

static rb_display_t g_disp;
static rb_screen_t g_screen;
static rb_layout_t g_root;
static rb_layout_t g_left;
static rb_layout_t g_right;
static rb_radio_t g_rad[DEMO_N];
static rb_radio_group_t g_mode;
static rb_radio_group_t g_speed;

static const rb_style_t k_styles[DEMO_ROWS] = {
    RB_STYLE_PRIMARY, RB_STYLE_SUCCESS, RB_STYLE_WARNING, RB_STYLE_DANGER,
};

static const char *const k_mode[DEMO_ROWS] = {
    "Auto", "Manual", "Eco", "Sport",
};

static const char *const k_speed[DEMO_ROWS] = {
    "Slow", "Med", "Fast", "Max",
};

static void on_group_change(rb_radio_group_t *grp, rb_radio_t *sel,
                            void *user)
{
    (void)grp;
    (void)sel;
    (void)user;
    /* Selection exclusivity is visible on-screen; no peer mirror. */
}

static void touch_task(void *arg)
{
    (void)arg;
    for (;;) {
        bsp_touch_sample_t smp;
        int i;
        if (bsp_touch_read(&smp) == 0) {
            for (i = 0; i < DEMO_N; i++) {
                (void)rb_radio_feed_touch(&g_rad[i], &smp);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void ex08_start(void)
{
    int i;
    (void)rb_display_open(&g_disp);
    (void)rb_screen_init(&g_screen, &g_disp);

    (void)rb_layout_init(&g_root, &g_screen, RB_LAYOUT_COLS, 1, 2, 6);
    (void)rb_layout_init(&g_left, &g_screen, RB_LAYOUT_ROWS, DEMO_ROWS, 1,
                         4);
    (void)rb_layout_init(&g_right, &g_screen, RB_LAYOUT_ROWS, DEMO_ROWS, 1,
                         4);

    rb_radio_group_init(&g_mode);
    rb_radio_group_init(&g_speed);
    rb_radio_group_set_on_change(&g_mode, on_group_change, 0);
    rb_radio_group_set_on_change(&g_speed, on_group_change, 0);

    for (i = 0; i < DEMO_ROWS; i++) {
        rb_radio_init(&g_rad[i]);
        rb_radio_set_style(&g_rad[i], k_styles[i]);
        rb_radio_set_label(&g_rad[i], k_mode[i]);
        (void)rb_radio_group_add(&g_mode, &g_rad[i]);
        (void)rb_layout_add(&g_left, &g_rad[i].base);

        rb_radio_init(&g_rad[i + DEMO_ROWS]);
        rb_radio_set_style(&g_rad[i + DEMO_ROWS], k_styles[i]);
        rb_radio_set_label(&g_rad[i + DEMO_ROWS], k_speed[i]);
        (void)rb_radio_group_add(&g_speed, &g_rad[i + DEMO_ROWS]);
        (void)rb_layout_add(&g_right, &g_rad[i + DEMO_ROWS].base);
    }

    (void)rb_layout_add(&g_root, &g_left.base);
    (void)rb_layout_add(&g_root, &g_right.base);

    rb_screen_set_root(&g_screen, &g_root.base);
    rb_screen_reflow(&g_screen);

    (void)rb_radio_group_select(&g_mode, &g_rad[0]);
    (void)rb_radio_group_select(&g_speed, &g_rad[DEMO_ROWS + 1]);

    (void)xTaskCreate(touch_task, "touch", 3072, 0, 5, 0);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
