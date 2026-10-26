/*
 * Demo 09 — composed form: radio + switch + checkbox + slider + button.
 */
#include "bsp.h"
#include "rb_button.h"
#include "rb_checkbox.h"
#include "rb_label.h"
#include "rb_radio.h"
#include "rb_screen.h"
#include "rb_slider.h"
#include "rb_style.h"
#include "rb_switch.h"

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

#include <stdio.h>

#define MODE_N 3

static rb_display_t g_disp;
static rb_screen_t g_screen;
static rb_layout_t g_root;
static rb_layout_t g_body;
static rb_layout_t g_left;
static rb_layout_t g_right;
static rb_layout_t g_foot;

static rb_label_t g_title;
static rb_label_t g_status;
static rb_radio_t g_mode[MODE_N];
static rb_radio_group_t g_mode_grp;
static rb_switch_t g_pwr;
static rb_checkbox_t g_mute;
static rb_slider_t g_level;
static rb_button_t g_apply;

static const char *const k_modes[MODE_N] = {"Auto", "Day", "Night"};

static void refresh_status(void)
{
    rb_radio_t *sel = rb_radio_group_selected(&g_mode_grp);
    const char *mode = (sel && sel->label[0]) ? sel->label : "?";
    char buf[RB_LABEL_TEXT_MAX];

    (void)snprintf(buf, sizeof(buf), "%s P%d M%d L%ld", mode,
                   rb_switch_is_on(&g_pwr), rb_checkbox_is_on(&g_mute),
                   (long)rb_slider_get_value(&g_level));
    rb_label_set_text(&g_status, buf);
}

static void on_apply(rb_button_t *btn, rb_button_event_t ev, void *user)
{
    (void)btn;
    (void)user;
    if (ev == RB_BUTTON_EV_CLICK) {
        refresh_status();
    }
}

static void touch_task(void *arg)
{
    (void)arg;
    for (;;) {
        bsp_touch_sample_t smp;
        int i;
        if (bsp_touch_read(&smp) == 0) {
            for (i = 0; i < MODE_N; i++) {
                (void)rb_radio_feed_touch(&g_mode[i], &smp);
            }
            (void)rb_switch_feed_touch(&g_pwr, &smp);
            (void)rb_checkbox_feed_touch(&g_mute, &smp);
            (void)rb_slider_feed_touch(&g_level, &smp);
            (void)rb_button_feed_touch(&g_apply, &smp);
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

    (void)rb_layout_init(&g_root, &g_screen, RB_LAYOUT_ROWS, 4, 1, 4);
    (void)rb_layout_init(&g_body, &g_screen, RB_LAYOUT_COLS, 1, 2, 6);
    (void)rb_layout_init(&g_left, &g_screen, RB_LAYOUT_ROWS, MODE_N, 1, 2);
    (void)rb_layout_init(&g_right, &g_screen, RB_LAYOUT_ROWS, 2, 1, 4);
    (void)rb_layout_init(&g_foot, &g_screen, RB_LAYOUT_COLS, 1, 2, 4);

    rb_label_init(&g_title);
    rb_label_set_style(&g_title, RB_STYLE_INFO);
    rb_label_set_align(&g_title, RB_LABEL_ALIGN_CENTER);
    rb_label_set_text(&g_title, "Settings");

    rb_label_init(&g_status);
    rb_label_set_style(&g_status, RB_STYLE_SECONDARY);
    rb_label_set_align(&g_status, RB_LABEL_ALIGN_CENTER);
    rb_label_set_text(&g_status, "tap Apply");

    rb_radio_group_init(&g_mode_grp);
    for (i = 0; i < MODE_N; i++) {
        rb_radio_init(&g_mode[i]);
        rb_radio_set_style(&g_mode[i], RB_STYLE_PRIMARY);
        rb_radio_set_label(&g_mode[i], k_modes[i]);
        (void)rb_radio_group_add(&g_mode_grp, &g_mode[i]);
        (void)rb_layout_add(&g_left, &g_mode[i].base);
    }

    rb_switch_init(&g_pwr);
    rb_switch_set_style(&g_pwr, RB_STYLE_SUCCESS);
    rb_switch_set_labels(&g_pwr, "ON", "OFF");
    rb_switch_set_on(&g_pwr, 1);

    rb_checkbox_init(&g_mute);
    rb_checkbox_set_style(&g_mute, RB_STYLE_WARNING);
    rb_checkbox_set_label(&g_mute, "Mute");

    rb_slider_init(&g_level);
    rb_slider_set_orient(&g_level, RB_SLIDER_ORIENT_HORIZONTAL);
    rb_slider_set_mode(&g_level, RB_SLIDER_MODE_INPUT);
    rb_slider_set_style(&g_level, RB_STYLE_PRIMARY);
    rb_slider_set_range(&g_level, 0, 100);
    rb_slider_set_value(&g_level, 40);
    rb_slider_set_label(&g_level, "Level");
    rb_slider_set_show_value(&g_level, 1);

    rb_button_init(&g_apply);
    rb_button_set_style(&g_apply, RB_STYLE_PRIMARY);
    rb_button_set_label(&g_apply, "Apply");
    rb_button_set_on_event(&g_apply, on_apply, 0);

    (void)rb_layout_add(&g_right, &g_pwr.base);
    (void)rb_layout_add(&g_right, &g_mute.base);
    (void)rb_layout_add(&g_body, &g_left.base);
    (void)rb_layout_add(&g_body, &g_right.base);
    (void)rb_layout_add(&g_foot, &g_level.base);
    (void)rb_layout_add(&g_foot, &g_apply.base);

    (void)rb_layout_add(&g_root, &g_title.base);
    (void)rb_layout_add(&g_root, &g_body.base);
    (void)rb_layout_add(&g_root, &g_foot.base);
    (void)rb_layout_add(&g_root, &g_status.base);

    rb_screen_set_root(&g_screen, &g_root.base);
    rb_screen_reflow(&g_screen);
    (void)rb_radio_group_select(&g_mode_grp, &g_mode[0]);
    refresh_status();

    (void)xTaskCreate(touch_task, "touch", 4096, 0, 5, 0);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
