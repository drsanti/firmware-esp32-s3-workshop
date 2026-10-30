/*
 * Demo 11 — gauge
 *
 * WHAT THIS CODE DOES
 *   2x3 rb_gauge — styles + bands + breathe
 *
 * HOW IT WORKS
 *   main/main.c calls ex11_start() after bsp_init().
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
#include "ex11.h"

#include "bsp.h"
#include "rb_gauge.h"
#include "rb_screen.h"
#include "rb_style.h"

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

#define DEMO_ROWS 2
#define DEMO_COLS 3
#define DEMO_N (DEMO_ROWS * DEMO_COLS)

static rb_display_t g_disp;
static rb_screen_t g_screen;
static rb_layout_t g_layout;
static rb_gauge_t g_gauge[DEMO_N];

static const rb_style_t k_styles[DEMO_N] = {
    RB_STYLE_PRIMARY, RB_STYLE_SUCCESS, RB_STYLE_WARNING,
    RB_STYLE_DANGER,  RB_STYLE_INFO,    RB_STYLE_SECONDARY,
};

static const char *const k_units[DEMO_N] = {
    "mV", "mm", "Hz", "%", "C", "rpm",
};

static const char *const k_labels[DEMO_N] = {
    "Batt", "Lift", "Fan", "CPU", "Temp", "Motor",
};

static const int32_t k_phase[DEMO_N] = {0, 20, 40, 60, 80, 100};

static void breathe_task(void *arg)
{
    int32_t t = 0;
    int i;

    (void)arg;
    for (;;) {
        for (i = 0; i < DEMO_N; i++) {
            int32_t p = (t + k_phase[i]) % 200;
            int32_t v = (p < 100) ? p : (200 - p);
            rb_gauge_set_value(&g_gauge[i], v);
        }
        t++;
        vTaskDelay(pdMS_TO_TICKS(40));
    }
}

void ex11_start(void)
{
    int i;
    (void)rb_display_open(&g_disp);
    (void)rb_screen_init(&g_screen, &g_disp);
    (void)rb_layout_init(&g_layout, &g_screen, RB_LAYOUT_GRID, DEMO_ROWS,
                         DEMO_COLS, 6);

    for (i = 0; i < DEMO_N; i++) {
        rb_gauge_init(&g_gauge[i]);
        rb_gauge_set_style(&g_gauge[i], k_styles[i]);
        rb_gauge_set_range(&g_gauge[i], 0, 100);
        rb_gauge_set_show_value(&g_gauge[i], 1);
        rb_gauge_set_unit(&g_gauge[i], k_units[i]);
        rb_gauge_set_label(&g_gauge[i], k_labels[i]);
        rb_gauge_set_bands(&g_gauge[i], 70, 90); /* warn / danger fills */
        rb_gauge_set_value(&g_gauge[i], k_phase[i] % 100);
        (void)rb_layout_add(&g_layout, &g_gauge[i].base);
    }

    rb_screen_set_root(&g_screen, &g_layout.base);
    rb_screen_reflow(&g_screen);

    (void)xTaskCreate(breathe_task, "breathe", 3072, 0, 5, 0);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
