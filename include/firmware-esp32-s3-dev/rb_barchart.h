/*
 * rb_barchart.h — DISPLAY multi-bar chart (multi-instance).
 *
 * N values → N bars. FFT bins, histograms, EQ bands, category scores —
 * not a time strip (rb_chart) and not a single-level meter (rb_vu).
 *
 * Orient:
 *   VERTICAL   (default) — bars side-by-side, grow bottom→top (FFT)
 *   HORIZONTAL — bars stacked, grow left→right
 *
 *   static rb_barchart_t fft, loads;
 *   float mags[32];
 *   rb_barchart_init(&fft);
 *   rb_barchart_set_bins(&fft, 32);
 *   rb_barchart_set_range(&fft, 0.f, 1.f);
 *   rb_barchart_set_values(&fft, mags, 32);
 *
 *   rb_barchart_init(&loads);
 *   rb_barchart_set_orient(&loads, RB_BARCHART_ORIENT_HORIZONTAL);
 *   rb_barchart_set_bins(&loads, 4);
 *   rb_barchart_set_value(&loads, 0, 40.f);
 *
 * Preferred include: rb_barchart.h
 */
#ifndef ROBOTOS_RB_BARCHART_H
#define ROBOTOS_RB_BARCHART_H

#include <stdint.h>

#include "bsp.h"
#include "rb_screen.h"
#include "rb_style.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_BARCHART_BINS_MIN     1
#define RB_BARCHART_BINS_MAX     64
#define RB_BARCHART_BINS_DEFAULT 16
#define RB_BARCHART_TITLE_MAX    16
#define RB_BARCHART_LABEL_MAX    8

typedef enum {
    RB_BARCHART_ORIENT_VERTICAL = 0, /* bars grow up; classic FFT */
    RB_BARCHART_ORIENT_HORIZONTAL,   /* bars grow right           */
} rb_barchart_orient_t;

typedef struct rb_barchart {
    rb_widget_t base; /* must be first — multi-instance, no globals */
    uint8_t orient;
    uint8_t n_bins;
    uint8_t show_title;
    uint8_t show_baseline;
    uint8_t autoscale;
    uint8_t ok;
    uint8_t gap_px;
    uint8_t radius;
    uint8_t pad_px;
    float y_min, y_max;
    float values[RB_BARCHART_BINS_MAX];
    uint8_t bin_rgb_on[RB_BARCHART_BINS_MAX]; /* 1 = use per-bin RGB */
    uint8_t bin_r[RB_BARCHART_BINS_MAX];
    uint8_t bin_g[RB_BARCHART_BINS_MAX];
    uint8_t bin_b[RB_BARCHART_BINS_MAX];
    char bin_label[RB_BARCHART_BINS_MAX][RB_BARCHART_LABEL_MAX];
    uint8_t show_bin_labels; /* H only practical for few bins */
    /* Shared bar / track / text / baseline */
    uint8_t br_r, br_g, br_b;
    uint8_t tr_r, tr_g, tr_b;
    uint8_t tx_r, tx_g, tx_b;
    uint8_t bl_r, bl_g, bl_b; /* baseline */
    /* Optional height bands (value space); > y_max disables */
    float warn_at;
    float danger_at;
    uint8_t wn_r, wn_g, wn_b;
    uint8_t dg_r, dg_g, dg_b;
    char title[RB_BARCHART_TITLE_MAX];
} rb_barchart_t;

void rb_barchart_init(rb_barchart_t *bc);
int  rb_barchart_apply_geometry(rb_barchart_t *bc);

void rb_barchart_set_orient(rb_barchart_t *bc, rb_barchart_orient_t orient);
void rb_barchart_set_bins(rb_barchart_t *bc, uint8_t n); /* 1..BINS_MAX */
uint8_t rb_barchart_get_bins(const rb_barchart_t *bc);

void rb_barchart_set_range(rb_barchart_t *bc, float y_min, float y_max);
void rb_barchart_set_autoscale(rb_barchart_t *bc, int on);
void rb_barchart_set_gap_px(rb_barchart_t *bc, uint8_t gap);
void rb_barchart_set_radius(rb_barchart_t *bc, uint8_t r);
void rb_barchart_set_pad_px(rb_barchart_t *bc, uint8_t pad);

void rb_barchart_set_title(rb_barchart_t *bc, const char *text); /* NULL/"" hide */
void rb_barchart_set_show_title(rb_barchart_t *bc, int on);
void rb_barchart_set_show_baseline(rb_barchart_t *bc, int on);
void rb_barchart_set_show_bin_labels(rb_barchart_t *bc, int on);
void rb_barchart_set_bin_label(rb_barchart_t *bc, uint8_t i, const char *text);

/* Height bands in value space. Pass warn/danger > y_max to disable. */
void rb_barchart_set_bands(rb_barchart_t *bc, float warn_at, float danger_at);

void rb_barchart_set_style(rb_barchart_t *bc, rb_style_t style);
void rb_barchart_set_bar_rgb(rb_barchart_t *bc, uint8_t r, uint8_t g, uint8_t b);
void rb_barchart_set_track_rgb(rb_barchart_t *bc, uint8_t r, uint8_t g,
                               uint8_t b);
void rb_barchart_set_text_rgb(rb_barchart_t *bc, uint8_t r, uint8_t g,
                              uint8_t b);
void rb_barchart_set_baseline_rgb(rb_barchart_t *bc, uint8_t r, uint8_t g,
                                  uint8_t b);
void rb_barchart_set_warn_rgb(rb_barchart_t *bc, uint8_t r, uint8_t g,
                              uint8_t b);
void rb_barchart_set_danger_rgb(rb_barchart_t *bc, uint8_t r, uint8_t g,
                                uint8_t b);
/* Per-bin color override; clears back to shared bar RGB when off. */
void rb_barchart_set_bin_rgb(rb_barchart_t *bc, uint8_t i, uint8_t r, uint8_t g,
                             uint8_t b);
void rb_barchart_clear_bin_rgb(rb_barchart_t *bc, uint8_t i);

void rb_barchart_set_value(rb_barchart_t *bc, uint8_t i, float v);
float rb_barchart_get_value(const rb_barchart_t *bc, uint8_t i);
/* Copy up to n values (clamped to n_bins); remaining bins unchanged if n < n_bins. */
void rb_barchart_set_values(rb_barchart_t *bc, const float *values, uint8_t n);
void rb_barchart_clear(rb_barchart_t *bc);

void rb_barchart_redraw(rb_barchart_t *bc);
int  rb_barchart_contains(const rb_barchart_t *bc, int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_BARCHART_H */
