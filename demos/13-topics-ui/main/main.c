/*
 * Demo 13 — slider → gauge + wire publish (topics meet UI).
 */
#include "bsp.h"
#include "protocol.h"
#include "rb.h"
#include "rb_cmd.h"
#include "rb_gauge.h"
#include "rb_screen.h"
#include "rb_serial.h"
#include "rb_slider.h"
#include "rb_style.h"
#include "rb_version.h"

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

static rb_display_t g_disp;
static rb_screen_t g_screen;
static rb_layout_t g_root;
static rb_slider_t g_slider;
static rb_gauge_t g_gauge;

static rb_serial_transport_t g_link;
static rb_node_t g_node;
static rb_pub_t g_val_pub;

static void on_slider(rb_slider_t *s, int32_t value, void *user)
{
    int32_t v = value;

    (void)s;
    (void)user;
    rb_gauge_set_value(&g_gauge, v);
    (void)rb_publish(&g_val_pub, &v, (uint16_t)sizeof(v));
    rb_log(&g_node, "pub level=%ld", (long)v);
}

static void spin_task(void *arg)
{
    (void)arg;
    for (;;) {
        rb_spin_once(&g_node);
        rb_cmd_poll();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void touch_task(void *arg)
{
    (void)arg;
    for (;;) {
        bsp_touch_sample_t smp;
        if (bsp_touch_read(&smp) == 0) {
            (void)rb_slider_feed_touch(&g_slider, &smp);
            (void)rb_gauge_feed_touch(&g_gauge, &smp);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void app_main(void)
{
    rb_node_cfg_t cfg;

    bsp_init();
    (void)rb_display_open(&g_disp);
    (void)rb_screen_init(&g_screen, &g_disp);
    (void)rb_layout_init(&g_root, &g_screen, RB_LAYOUT_COLS, 1, 2, 8);

    rb_slider_init(&g_slider);
    rb_slider_set_orient(&g_slider, RB_SLIDER_ORIENT_VERTICAL);
    rb_slider_set_mode(&g_slider, RB_SLIDER_MODE_INPUT);
    rb_slider_set_style(&g_slider, RB_STYLE_PRIMARY);
    rb_slider_set_range(&g_slider, 0, 100);
    rb_slider_set_value(&g_slider, 35);
    rb_slider_set_label(&g_slider, "Level");
    rb_slider_set_show_value(&g_slider, 1);
    rb_slider_set_on_change(&g_slider, on_slider, 0);

    rb_gauge_init(&g_gauge);
    rb_gauge_set_mode(&g_gauge, RB_GAUGE_MODE_DISPLAY);
    rb_gauge_set_style(&g_gauge, RB_STYLE_SUCCESS);
    rb_gauge_set_range(&g_gauge, 0, 100);
    rb_gauge_set_unit(&g_gauge, "%");
    rb_gauge_set_label(&g_gauge, "Out");
    rb_gauge_set_bands(&g_gauge, 70, 90);
    rb_gauge_set_show_value(&g_gauge, 1);
    rb_gauge_set_value(&g_gauge, 35);

    (void)rb_layout_add(&g_root, &g_slider.base);
    (void)rb_layout_add(&g_root, &g_gauge.base);
    rb_screen_set_root(&g_screen, &g_root.base);
    rb_screen_reflow(&g_screen);

    rb_transport_serial_init(&g_link);
    cfg.transport = &g_link.base;
    cfg.name = "demo13";
    cfg.heartbeat_ms = 1000;
    (void)rb_node_init(&g_node, &cfg);
    rb_cmd_bind_log(&g_node);
    (void)rb_pub_init(&g_node, &g_val_pub, RSP_MSG_IMU);
    rb_log(&g_node, "robotos-fw %s topics+ui", rb_fw_version());

    (void)xTaskCreate(spin_task, "spin", 4096, 0, 5, 0);
    (void)xTaskCreate(touch_task, "touch", 4096, 0, 5, 0);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
