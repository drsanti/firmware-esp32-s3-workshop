/*
 * Demo 10 — first wire/runtime: node + log + topic pub + cmd sub.
 * LCD term mirrors rb_log lines for students without a host.
 */
#include "bsp.h"
#include "protocol.h"
#include "rb.h"
#include "rb_cmd.h"
#include "rb_screen.h"
#include "rb_serial.h"
#include "rb_term.h"
#include "rb_version.h"

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

static rb_display_t g_disp;
static rb_screen_t g_screen;
static rb_layout_t g_layout;
static rb_term_t g_term;

static rb_serial_transport_t g_link;
static rb_node_t g_node;
static rb_pub_t g_tick_pub;
static rb_sub_t g_led_sub;
static rb_sub_t g_reboot_sub;
static rb_sub_t g_time_sub;
static uint32_t g_tick;

static void term_log(const char *fmt, ...)
{
    char buf[96];
    va_list ap;

    va_start(ap, fmt);
    (void)vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    rb_term_println(&g_term, buf);
}

static int on_led_cmd(rb_sub_t *sub, const uint8_t *payload, uint16_t len,
                      void *ctx)
{
    unsigned first = (payload != 0 && len > 0) ? payload[0] : 0u;

    (void)sub;
    (void)ctx;
    rb_log(&g_node, "LedCmd first=0x%02x len=%u", first, (unsigned)len);
    term_log("cmd LED 0x%02x %uB", first, (unsigned)len);
    return 0;
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

void app_main(void)
{
    rb_node_cfg_t cfg;

    bsp_init();

    (void)rb_display_open(&g_disp);
    (void)rb_screen_init(&g_screen, &g_disp);
    (void)rb_layout_init(&g_layout, &g_screen, RB_LAYOUT_ROWS, 1, 1, 0);
    rb_term_init(&g_term);
    (void)rb_term_set_scale(&g_term, 1);
    rb_term_set_color(&g_term, RB_TERM_CYAN, RB_TERM_BLACK);
    (void)rb_layout_add(&g_layout, &g_term.base);
    rb_screen_set_root(&g_screen, &g_layout.base);
    rb_screen_reflow(&g_screen);

    rb_transport_serial_init(&g_link);
    cfg.transport = &g_link.base;
    cfg.name = "demo10";
    cfg.heartbeat_ms = 1000;
    (void)rb_node_init(&g_node, &cfg);
    rb_cmd_bind_log(&g_node);

    (void)rb_pub_init(&g_node, &g_tick_pub, RSP_MSG_IMU);
    (void)rb_sub_init(&g_node, &g_led_sub, RSP_MSG_CMD_LED, on_led_cmd, 0);
    (void)rb_sub_init(&g_node, &g_reboot_sub, RSP_MSG_CMD_REBOOT,
                      rb_cmd_reboot, 0);
    (void)rb_sub_init(&g_node, &g_time_sub, RSP_MSG_CMD_TIME,
                      rb_cmd_timesync, 0);

    rb_log(&g_node, "robotos-fw %s up", rb_fw_version());
    term_log("robotos-fw %s", rb_fw_version());
    term_log("node=demo10 wire ready");

    (void)xTaskCreate(spin_task, "spin", 4096, 0, 5, 0);

    for (;;) {
        g_tick++;
        (void)rb_publish(&g_tick_pub, &g_tick, (uint16_t)sizeof(g_tick));
        rb_log(&g_node, "tick %" PRIu32, g_tick);
        term_log("tick %" PRIu32, g_tick);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
