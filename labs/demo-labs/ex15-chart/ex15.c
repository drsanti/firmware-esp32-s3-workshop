/*
 * Demo 15 — chart
 *
 * WHAT THIS CODE DOES
 *   Multi-pane rb_chart — SCROLL + HISTORY
 *
 * HOW IT WORKS
 *   main/main.c calls ex15_start() after bsp_init().
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
#include "ex15.h"

#include "bsp.h"
#include "imu_sim.h"
#include "rb_chart.h"
#include "rb_screen.h"

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

/* Prefer >= plot pixel width so HISTORY can fill the pane edge-to-edge. */
#define HIST_CAP 320
#define SAMPLE_DT_S 0.033f

static rb_display_t g_disp;
static rb_screen_t g_screen;
static rb_layout_t g_root;
static rb_chart_t g_acc;
static rb_chart_t g_gyro;
static float g_gyro_hist[HIST_CAP * 3];

static void feed_task(void *arg)
{
    float acc[3];
    float gyro[3];
    int real;
    int was_real = -1;
    float t = 0.0f;
    uint32_t rng = 1u;

    (void)arg;
    for (;;) {
        real = (bsp_imu_read(acc, gyro) == 0);
        if (!real) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += SAMPLE_DT_S;
        }
        if (real != was_real) {
            was_real = real;
            rb_chart_set_title(&g_acc,
                               real ? "Accel SCROLL" : "Accel SCROLL (sim)");
            rb_chart_set_title(&g_gyro,
                               real ? "Gyro HISTORY" : "Gyro HISTORY (sim)");
        }
        rb_chart_push_frame(&g_acc, acc, 3);
        rb_chart_push_vec3(&g_gyro, gyro[0], gyro[1], gyro[2]);
        vTaskDelay(pdMS_TO_TICKS(33)); /* ~30 Hz */
    }
}

void ex15_start(void)
{
    static const char *const acc_l[3] = {"ax", "ay", "az"};
    static const char *const gyr_l[3] = {"gx", "gy", "gz"};
    int i;
    (void)rb_display_open(&g_disp);
    (void)rb_screen_init(&g_screen, &g_disp);
    (void)rb_layout_init(&g_root, &g_screen, RB_LAYOUT_ROWS, 2, 1, 6);

    rb_chart_init(&g_acc);
    rb_chart_set_mode(&g_acc, RB_CHART_MODE_SCROLL);
    rb_chart_set_title(&g_acc, "Accel SCROLL");
    rb_chart_set_channels(&g_acc, 3);
    /* m/s^2 — covers ±2 g tip + gravity on az when flat */
    rb_chart_set_range(&g_acc, -20.0f, 20.0f);
    rb_chart_set_show_grid(&g_acc, 1);
    rb_chart_set_show_zero(&g_acc, 1);
    for (i = 0; i < 3; i++) {
        rb_chart_set_channel_label(&g_acc, (uint8_t)i, acc_l[i]);
    }

    rb_chart_init(&g_gyro);
    rb_chart_set_channels(&g_gyro, 3);
    rb_chart_set_history(&g_gyro, g_gyro_hist, HIST_CAP);
    rb_chart_set_mode(&g_gyro, RB_CHART_MODE_HISTORY);
    rb_chart_set_title(&g_gyro, "Gyro HISTORY");
    /* rad/s — quiet desk; raise if you need faster spins */
    rb_chart_set_range(&g_gyro, -5.0f, 5.0f);
    rb_chart_set_show_grid(&g_gyro, 1);
    rb_chart_set_show_zero(&g_gyro, 1);
    for (i = 0; i < 3; i++) {
        rb_chart_set_channel_label(&g_gyro, (uint8_t)i, gyr_l[i]);
    }

    (void)rb_layout_add(&g_root, &g_acc.base);
    (void)rb_layout_add(&g_root, &g_gyro.base);
    rb_screen_set_root(&g_screen, &g_root.base);
    rb_screen_reflow(&g_screen);

    (void)xTaskCreate(feed_task, "imu", 4096, 0, 5, 0);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
