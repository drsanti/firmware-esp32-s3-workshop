/*
 * rb_chart.h — multi-channel strip / history chart (DISPLAY leaf).
 *
 * Many independent panes (like rb_term): each rb_chart_t owns its
 * channels, cursor, and optional history buffer.
 *
 * Modes (rb_chart_set_mode):
 *   SCROLL  — stripchart: erase one column, plot, advance (tiny RAM).
 *   HISTORY — ring buffer of time steps; full redraw of the plot area.
 *             Grows left→right, then shifts (newest at right edge).
 *             Requires rb_chart_set_history(buf, capacity) first;
 *             capacity should be >= plot width for a full-width shift.
 *
 *   static rb_chart_t acc, gyro;
 *   float hist[240 * 3];
 *   rb_chart_init(&acc);
 *   rb_chart_set_mode(&acc, RB_CHART_MODE_HISTORY);
 *   rb_chart_set_history(&acc, hist, 240);
 *   rb_chart_set_channels(&acc, 3);
 *   rb_chart_set_channel_label(&acc, 0, "ax");
 *   rb_chart_set_range(&acc, -20.f, 20.f);
 *   rb_chart_push_frame(&acc, vals, 3);
 */
#ifndef ROBOTOS_RB_CHART_H
#define ROBOTOS_RB_CHART_H

#include <stdint.h>

#include "bsp.h"
#include "rb_screen.h"
#include "rb_style.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_CHART_CH_MAX     6
#define RB_CHART_LABEL_MAX  8
#define RB_CHART_TITLE_MAX  16

typedef enum {
    RB_CHART_MODE_SCROLL = 0, /* A: live strip, no history buffer     */
    RB_CHART_MODE_HISTORY,    /* B: ring buffer + full plot redraw    */
} rb_chart_mode_t;

typedef struct rb_chart {
    rb_widget_t base; /* must be first */
    uint8_t mode;
    uint8_t n_ch;
    uint8_t show_grid;
    uint8_t show_zero;
    uint8_t show_legend;
    uint8_t show_title;
    uint8_t autoscale;
    uint8_t paused;
    uint8_t ok;
    uint8_t chrome_dirty; /* 1 = redraw title/legend/grid next push */
    uint16_t plot_x;      /* SCROLL cursor (local to plot width)    */
    uint16_t hist_cap;    /* HISTORY capacity (time steps)          */
    uint16_t hist_len;    /* samples currently in ring              */
    uint16_t hist_head;   /* next write index                       */
    float *hist;          /* caller-owned: hist_cap * n_ch floats,
                           * time-major: sample s, ch c →
                           * hist[s * n_ch + c]                     */
    float y_min, y_max;   /* shared range (used when ch has no own) */
    float ch_min[RB_CHART_CH_MAX];
    float ch_max[RB_CHART_CH_MAX];
    uint8_t ch_range_on[RB_CHART_CH_MAX]; /* 1 = use ch_min/max     */
    float last[RB_CHART_CH_MAX];
    int16_t prev_y[RB_CHART_CH_MAX];
    uint8_t ch_r[RB_CHART_CH_MAX];
    uint8_t ch_g[RB_CHART_CH_MAX];
    uint8_t ch_b[RB_CHART_CH_MAX];
    char ch_label[RB_CHART_CH_MAX][RB_CHART_LABEL_MAX];
    char title[RB_CHART_TITLE_MAX];
    uint8_t tx_r, tx_g, tx_b;
    uint8_t gd_r, gd_g, gd_b; /* grid / zero line */
} rb_chart_t;

void rb_chart_init(rb_chart_t *c);
int  rb_chart_apply_geometry(rb_chart_t *c);

void rb_chart_set_mode(rb_chart_t *c, rb_chart_mode_t mode);
rb_chart_mode_t rb_chart_get_mode(const rb_chart_t *c);
/* HISTORY: caller owns buf. capacity = time steps. Size needed =
 * capacity * n_ch floats (call after set_channels, or size for max).
 * Pass buf=NULL / capacity=0 to clear. Returns 0 ok, -1 bad args. */
int  rb_chart_set_history(rb_chart_t *c, float *buf, uint16_t capacity);

void rb_chart_set_channels(rb_chart_t *c, uint8_t n); /* 1..RB_CHART_CH_MAX */
uint8_t rb_chart_get_channels(const rb_chart_t *c);
void rb_chart_set_channel_label(rb_chart_t *c, uint8_t ch, const char *text);
void rb_chart_set_channel_rgb(rb_chart_t *c, uint8_t ch, uint8_t r, uint8_t g,
                              uint8_t b);
void rb_chart_set_channel_style(rb_chart_t *c, uint8_t ch, rb_style_t style);

void rb_chart_set_range(rb_chart_t *c, float y_min, float y_max);
void rb_chart_set_channel_range(rb_chart_t *c, uint8_t ch, float y_min,
                                float y_max);
void rb_chart_clear_channel_range(rb_chart_t *c, uint8_t ch); /* use shared */

void rb_chart_set_title(rb_chart_t *c, const char *text); /* NULL/"" hide */
void rb_chart_set_show_title(rb_chart_t *c, int on);
void rb_chart_set_show_legend(rb_chart_t *c, int on);
void rb_chart_set_show_grid(rb_chart_t *c, int on);
void rb_chart_set_show_zero(rb_chart_t *c, int on);
void rb_chart_set_autoscale(rb_chart_t *c, int on);
void rb_chart_set_paused(rb_chart_t *c, int on);
void rb_chart_set_text_rgb(rb_chart_t *c, uint8_t r, uint8_t g, uint8_t b);
void rb_chart_set_grid_rgb(rb_chart_t *c, uint8_t r, uint8_t g, uint8_t b);

/* Push one sample into channel ch (advances SCROLL once all channels
 * of a frame are typically pushed via push_frame). For SCROLL, a lone
 * push plots that channel at the current column without advancing;
 * prefer push_frame for multi-ch. */
void rb_chart_push(rb_chart_t *c, uint8_t ch, float value);
/* One time step: writes n values (clamped to n_ch), then advances. */
void rb_chart_push_frame(rb_chart_t *c, const float *values, uint8_t n);
/* Convenience: push ch 0..2 (no-op if n_ch < 3). */
void rb_chart_push_vec3(rb_chart_t *c, float x, float y, float z);

void rb_chart_clear(rb_chart_t *c);
void rb_chart_redraw(rb_chart_t *c);
int  rb_chart_contains(const rb_chart_t *c, int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_CHART_H */
