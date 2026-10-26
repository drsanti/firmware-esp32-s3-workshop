/*
 * Demo 14 — H/V pill progress bars with warn/danger bands.
 * Left: 3× horizontal (snap values 20/50/100). Right: 2× vertical breathe.
 */
#include "bsp.h"
#include "rb_progress.h"
#include "rb_screen.h"
#include "rb_style.h"

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

static rb_display_t g_disp;
static rb_screen_t g_screen;
static rb_layout_t g_root;
static rb_layout_t g_hcol;
static rb_layout_t g_vcol;
static rb_progress_t g_h[3];
static rb_progress_t g_v[2];

static const int32_t k_h_vals[3] = {20, 50, 100};
static const char *const k_h_labels[3] = {"Load", "Temp", "Fan"};
static const char *const k_v_labels[2] = {"Bat", "CPU"};

static void breathe_task(void *arg)
{
    int32_t t = 0;
    int i;

    (void)arg;
    for (;;) {
        for (i = 0; i < 2; i++) {
            int32_t p = (t + i * 50) % 200;
            int32_t v = (p < 100) ? p : (200 - p);
            rb_progress_set_value(&g_v[i], v);
        }
        t++;
        vTaskDelay(pdMS_TO_TICKS(40));
    }
}

void app_main(void)
{
    int i;

    bsp_init();
    (void)rb_display_open(&g_disp);
    (void)rb_screen_init(&g_screen, &g_disp);
    (void)rb_layout_init(&g_root, &g_screen, RB_LAYOUT_COLS, 1, 2, 8);
    (void)rb_layout_init(&g_hcol, &g_screen, RB_LAYOUT_ROWS, 3, 1, 8);
    (void)rb_layout_init(&g_vcol, &g_screen, RB_LAYOUT_COLS, 1, 2, 8);

    for (i = 0; i < 3; i++) {
        rb_progress_init(&g_h[i]);
        rb_progress_set_orient(&g_h[i], RB_PROGRESS_ORIENT_HORIZONTAL);
        rb_progress_set_style(&g_h[i], RB_STYLE_SUCCESS);
        rb_progress_set_range(&g_h[i], 0, 100);
        rb_progress_set_bands(&g_h[i], 50, 90);
        rb_progress_set_label(&g_h[i], k_h_labels[i]);
        rb_progress_set_show_value(&g_h[i], 1);
        rb_progress_set_value(&g_h[i], k_h_vals[i]);
        (void)rb_layout_add(&g_hcol, &g_h[i].base);
    }

    for (i = 0; i < 2; i++) {
        rb_progress_init(&g_v[i]);
        rb_progress_set_orient(&g_v[i], RB_PROGRESS_ORIENT_VERTICAL);
        rb_progress_set_style(&g_v[i],
                              i == 0 ? RB_STYLE_INFO : RB_STYLE_PRIMARY);
        rb_progress_set_range(&g_v[i], 0, 100);
        rb_progress_set_bands(&g_v[i], 70, 90);
        rb_progress_set_label(&g_v[i], k_v_labels[i]);
        rb_progress_set_show_value(&g_v[i], 1);
        rb_progress_set_value(&g_v[i], 30 + i * 20);
        (void)rb_layout_add(&g_vcol, &g_v[i].base);
    }

    (void)rb_layout_add(&g_root, &g_hcol.base);
    (void)rb_layout_add(&g_root, &g_vcol.base);
    rb_screen_set_root(&g_screen, &g_root.base);
    rb_screen_reflow(&g_screen);

    (void)xTaskCreate(breathe_task, "breathe", 3072, 0, 5, 0);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
