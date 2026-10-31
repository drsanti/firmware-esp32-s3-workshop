/*
 * Demo 12 — param
 *
 * WHAT THIS CODE DOES
 *   rb_param table — local +/- and host ParamCmd
 *
 * HOW IT WORKS
 *   main/main.c calls ex12_start() after bsp_init().
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
#include "ex12.h"

#include "bsp.h"
#include "protocol.h"
#include "rb.h"
#include "rb_button.h"
#include "rb_cmd.h"
#include "rb_param.h"
#include "rb_screen.h"
#include "rb_serial.h"
#include "rb_style.h"
#include "rb_term.h"
#include "rb_version.h"
#include "serial_proto/serial.pb.h"
#include "serial_proto/serial_proto.h"

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static rb_display_t g_disp;
static rb_screen_t g_screen;
static rb_layout_t g_root;
static rb_layout_t g_btns;
static rb_term_t g_term;
static rb_button_t g_btn_dec;
static rb_button_t g_btn_inc;

static rb_serial_transport_t g_link;
static rb_node_t g_node;
static rb_pub_t g_param_ev_pub;
static rb_sub_t g_param_sub;
static rb_sub_t g_reboot_sub;

static int32_t g_level = 42;
static char g_name[24] = "demo12";

static void term_log(const char *fmt, ...)
{
    char buf[96];
    va_list ap;

    va_start(ap, fmt);
    (void)vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    rb_term_println(&g_term, buf);
}

static int level_get(char *buf, size_t n)
{
    return snprintf(buf, n, "%ld", (long)g_level) > 0 ? 0 : -1;
}

static int level_set(const char *value)
{
    long x = strtol(value, 0, 10);
    if (x < 0 || x > 100) {
        return -22;
    }
    g_level = (int32_t)x;
    return 0;
}

static int name_get(char *buf, size_t n)
{
    return snprintf(buf, n, "%s", g_name) > 0 ? 0 : -1;
}

static int name_set(const char *value)
{
    if (value == 0 || value[0] == '\0' || strlen(value) >= sizeof(g_name)) {
        return -22;
    }
    (void)snprintf(g_name, sizeof(g_name), "%s", value);
    return 0;
}

static rb_param_t g_params[] = {
    {"demo.level", "i64", NULL, 0, level_get, level_set, 0},
    {"demo.name", "str", NULL, 0, name_get, name_set, 0},
};

static void param_emit(const char *name, const char *value, const char *type,
                       int rc, uint32_t rev, void *user)
{
    robotos_serial_v1_ParamEvent e = robotos_serial_v1_ParamEvent_init_zero;
    uint8_t buf[160];
    size_t n;

    (void)user;
    (void)strncpy(e.name, name, sizeof(e.name) - 1);
    (void)strncpy(e.value, value, sizeof(e.value) - 1);
    (void)strncpy(e.type, type, sizeof(e.type) - 1);
    e.rc = rc;
    e.revision = rev;
    n = sp_encode(robotos_serial_v1_ParamEvent_fields, &e, buf, sizeof(buf));
    if (n) {
        (void)rb_publish(&g_param_ev_pub, buf, (uint16_t)n);
    }
    rb_log(&g_node, "param %s=%s rc=%d", name, value, rc);
    term_log("%s=%s rc=%d", name, value, rc);
}

static void on_btn(rb_button_t *btn, rb_button_event_t ev, void *user)
{
    int delta = (int)(intptr_t)user;
    char buf[16];

    (void)btn;
    if (ev != RB_BUTTON_EV_CLICK) {
        return;
    }
    g_level += delta;
    if (g_level < 0) {
        g_level = 0;
    }
    if (g_level > 100) {
        g_level = 100;
    }
    (void)snprintf(buf, sizeof(buf), "%ld", (long)g_level);
    /* Drive the same path a host SET would — bumps revision + emit. */
    (void)level_set(buf);
    g_params[0].revision++;
    param_emit(g_params[0].name, buf, g_params[0].type, 0,
               g_params[0].revision, 0);
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
            (void)rb_button_feed_touch(&g_btn_dec, &smp);
            (void)rb_button_feed_touch(&g_btn_inc, &smp);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void ex12_start(void)
{
    rb_node_cfg_t cfg;
    (void)rb_display_open(&g_disp);
    (void)rb_screen_init(&g_screen, &g_disp);
    (void)rb_layout_init(&g_root, &g_screen, RB_LAYOUT_ROWS, 2, 1, 4);
    (void)rb_layout_init(&g_btns, &g_screen, RB_LAYOUT_COLS, 1, 2, 6);

    rb_term_init(&g_term);
    (void)rb_term_set_scale(&g_term, 1);
    rb_term_set_color(&g_term, RB_TERM_CYAN, RB_TERM_BLACK);

    rb_button_init(&g_btn_dec);
    rb_button_set_style(&g_btn_dec, RB_STYLE_SECONDARY);
    rb_button_set_label(&g_btn_dec, "-");
    rb_button_set_on_event(&g_btn_dec, on_btn, (void *)(intptr_t)-5);

    rb_button_init(&g_btn_inc);
    rb_button_set_style(&g_btn_inc, RB_STYLE_PRIMARY);
    rb_button_set_label(&g_btn_inc, "+");
    rb_button_set_on_event(&g_btn_inc, on_btn, (void *)(intptr_t)5);

    (void)rb_layout_add(&g_btns, &g_btn_dec.base);
    (void)rb_layout_add(&g_btns, &g_btn_inc.base);
    (void)rb_layout_add(&g_root, &g_term.base);
    (void)rb_layout_add(&g_root, &g_btns.base);
    rb_screen_set_root(&g_screen, &g_root.base);
    rb_screen_reflow(&g_screen);

    rb_transport_serial_init(&g_link);
    cfg.transport = &g_link.base;
    cfg.name = "demo12";
    cfg.heartbeat_ms = 1000;
    (void)rb_node_init(&g_node, &cfg);
    rb_cmd_bind_log(&g_node);
    (void)rb_pub_init(&g_node, &g_param_ev_pub, RSP_MSG_PARAM_EVENT);

    rb_param_init(g_params, 2, param_emit, 0);
    (void)rb_sub_init(&g_node, &g_param_sub, RSP_MSG_CMD_PARAM, rb_param_on_cmd,
                      0);
    (void)rb_sub_init(&g_node, &g_reboot_sub, RSP_MSG_CMD_REBOOT, rb_cmd_reboot,
                      0);
    (void)rb_param_restore();

    rb_log(&g_node, "robotos-fw %s param demo", rb_fw_version());
    term_log("robotos-fw %s", rb_fw_version());
    term_log("params: demo.level demo.name");
    term_log("tap + / - or host ParamCmd");

    (void)xTaskCreate(spin_task, "spin", 4096, 0, 5, 0);
    (void)xTaskCreate(touch_task, "touch", 3072, 0, 5, 0);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
