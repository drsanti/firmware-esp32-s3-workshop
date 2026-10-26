/*
 * Demo 05 — buttons with DOWN / UP / CLICK events.
 * Normal: CLICK bumps a press counter in the label.
 * Toggle: CLICK mirrors ON state to its peer.
 */
#include "bsp.h"
#include "rb_button.h"
#include "rb_screen.h"
#include "rb_style.h"

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

#include <stdio.h>

#define DEMO_ROWS 2
#define DEMO_COLS 4
#define DEMO_N (DEMO_ROWS * DEMO_COLS)

static rb_display_t g_disp;
static rb_screen_t g_screen;
static rb_layout_t g_layout;
static rb_button_t g_btn[DEMO_N];
static unsigned g_presses[4];

static const rb_style_t k_styles[DEMO_N] = {
    RB_STYLE_PRIMARY, RB_STYLE_SECONDARY, RB_STYLE_SUCCESS, RB_STYLE_WARNING,
    RB_STYLE_DANGER,  RB_STYLE_INFO,      RB_STYLE_DARK,    RB_STYLE_LIGHT,
};

static const char *const k_labels[DEMO_N] = {
    "PRI", "SEC", "OK", "WARN", "ERR", "INFO", "DARK", "LIT",
};

static void on_normal_event(rb_button_t *btn, rb_button_event_t ev, void *user)
{
    unsigned *n = (unsigned *)user;
    char buf[RB_BUTTON_LABEL_MAX];

    if (ev == RB_BUTTON_EV_DOWN) {
        rb_button_set_border_width(btn, 3);
        return;
    }
    if (ev == RB_BUTTON_EV_UP) {
        rb_button_set_border_width(btn, 1);
        return;
    }
    if (ev != RB_BUTTON_EV_CLICK || n == 0) {
        return;
    }
    (*n)++;
    (void)snprintf(buf, sizeof(buf), "%u", *n);
    rb_button_set_label(btn, buf);
}

static void on_toggle_event(rb_button_t *btn, rb_button_event_t ev, void *user)
{
    rb_button_t *peer = (rb_button_t *)user;

    if (ev != RB_BUTTON_EV_CLICK || peer == 0) {
        return;
    }
    rb_button_set_on(peer, rb_button_is_on(btn));
}

static void touch_task(void *arg)
{
    (void)arg;

    for (;;) {
        bsp_touch_sample_t smp;
        int i;

        if (bsp_touch_read(&smp) == 0) {
            for (i = 0; i < DEMO_N; i++) {
                (void)rb_button_feed_touch(&g_btn[i], &smp);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void app_main(void)
{
    int i;

    bsp_init();

    (void)rb_display_open(&g_disp);
    (void)rb_screen_init(&g_screen, &g_disp);
    (void)rb_layout_init(&g_layout, &g_screen, RB_LAYOUT_GRID, DEMO_ROWS,
                         DEMO_COLS, 4);

    for (i = 0; i < DEMO_N; i++) {
        rb_button_init(&g_btn[i]);
        if (i < 4) {
            rb_button_set_mode(&g_btn[i], RB_BUTTON_MODE_NORMAL);
            rb_button_set_label(&g_btn[i], k_labels[i]);
            g_presses[i] = 0;
            rb_button_set_on_event(&g_btn[i], on_normal_event, &g_presses[i]);
        } else {
            rb_button_set_mode(&g_btn[i], RB_BUTTON_MODE_TOGGLE);
            rb_button_set_labels(&g_btn[i], "ON", "OFF");
            rb_button_set_indicator_size(&g_btn[i], 80, 2);
            rb_button_set_on(&g_btn[i], (i & 1) == 0);
        }
        rb_button_set_style(&g_btn[i], k_styles[i]);
        (void)rb_layout_add(&g_layout, &g_btn[i].base);
    }
    rb_button_set_on_event(&g_btn[4], on_toggle_event, &g_btn[5]);
    rb_button_set_on_event(&g_btn[5], on_toggle_event, &g_btn[4]);
    rb_button_set_on_event(&g_btn[6], on_toggle_event, &g_btn[7]);
    rb_button_set_on_event(&g_btn[7], on_toggle_event, &g_btn[6]);

    rb_screen_set_root(&g_screen, &g_layout.base);
    rb_screen_reflow(&g_screen);

    (void)xTaskCreate(touch_task, "touch", 3072, NULL, 5, NULL);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
