/*
 * lab_scope.c - single-channel HISTORY accel chart (status via chart title).
 */
#include "lab_scope.h"

#include "rb_chart.h"
#include "rb_screen.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* Prefer >= plot pixel width so HISTORY can fill the pane edge-to-edge. */
#define SCOPE_HIST_CAP 320u

static rb_display_t s_disp;
static rb_screen_t s_screen;
static rb_layout_t s_root;
static rb_chart_t s_chart;
static float s_hist[SCOPE_HIST_CAP]; /* 1 channel */
static char s_title_base[RB_CHART_TITLE_MAX];
static char s_status[RB_CHART_TITLE_MAX];
static int s_inited;
static int s_started;

static void refresh_title(void)
{
    if (s_status[0] != '\0') {
        rb_chart_set_title(&s_chart, s_status);
    } else {
        rb_chart_set_title(&s_chart, s_title_base);
    }
}

void scope_init(void)
{
    if (s_inited) {
        return;
    }

    (void)rb_display_open(&s_disp);
    (void)rb_screen_init(&s_screen, &s_disp);

    (void)snprintf(s_title_base, sizeof(s_title_base), "Vibe ax");
    s_status[0] = '\0';

    rb_chart_init(&s_chart);
    rb_chart_set_channels(&s_chart, 1);
    (void)rb_chart_set_history(&s_chart, s_hist, (uint16_t)SCOPE_HIST_CAP);
    rb_chart_set_mode(&s_chart, RB_CHART_MODE_HISTORY);
    rb_chart_set_title(&s_chart, s_title_base);
    rb_chart_set_channel_label(&s_chart, 0, "ax");
    rb_chart_set_range(&s_chart, -15.0f, 15.0f);
    rb_chart_set_show_grid(&s_chart, 1);
    rb_chart_set_show_zero(&s_chart, 1);

    s_inited = 1;
    s_started = 0;
}

void scope_start(void)
{
    if (!s_inited || s_started) {
        return;
    }

    (void)rb_layout_init(&s_root, &s_screen, RB_LAYOUT_ROWS, 1, 1, 0);
    (void)rb_layout_add(&s_root, &s_chart.base);
    rb_screen_set_root(&s_screen, &s_root.base);
    rb_screen_reflow(&s_screen);
    (void)rb_chart_apply_geometry(&s_chart);
    s_started = 1;
}

void scope_set_title(const char *title)
{
    if (title == 0 || title[0] == '\0') {
        (void)snprintf(s_title_base, sizeof(s_title_base), "Vibe ax");
    } else {
        (void)snprintf(s_title_base, sizeof(s_title_base), "%s", title);
    }
    if (s_status[0] == '\0') {
        refresh_title();
    }
}

void scope_set_status(const char *fmt, ...)
{
    va_list ap;

    if (fmt == 0) {
        return;
    }
    va_start(ap, fmt);
    (void)vsnprintf(s_status, sizeof(s_status), fmt, ap);
    va_end(ap);
    refresh_title();
}

void scope_push(float ax_mps2)
{
    float v = ax_mps2;
    rb_chart_push_frame(&s_chart, &v, 1);
}

void scope_set_live(int on)
{
    scope_set_status(on ? "IMU live" : "IMU sim");
}
