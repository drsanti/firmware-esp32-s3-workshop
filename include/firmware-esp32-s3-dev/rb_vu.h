/*
 * rb_vu.h — DISPLAY segmented VU / bargraph indicator (multi-instance).
 *
 * A row/column of colored boxes. Not mic-specific: feed any scalar
 * (audio level, CPU %, battery, RSSI, …) the same way you feed a gauge.
 *
 * Two scales:
 *   LINEAR — set_range + set_value / set_peak  (generic parameters)
 *   DBFS   — set_level(rms, peak) with amp 0..32767 → floor_db..0 dBFS
 *
 *   rb_vu_t a, b;
 *   rb_vu_init(&a);
 *   rb_vu_set_segments(&a, 20);
 *   rb_vu_set_zones(&a, 12, 16);
 *   rb_vu_set_label(&a, "CPU");
 *   rb_vu_set_value(&a, 72);          // LINEAR default 0..100
 *
 *   rb_vu_init(&b);
 *   rb_vu_set_scale(&b, RB_VU_SCALE_DBFS);
 *   rb_vu_set_level(&b, rms, peak);   // mic / audio
 *
 * Peak-hold marker decays over time (set_hold_ms / set_decay_ms);
 * call rb_vu_tick() when idle so hold falls without new samples.
 *
 * Preferred include: rb_vu.h
 */
#ifndef ROBOTOS_RB_VU_H
#define ROBOTOS_RB_VU_H

#include <stdint.h>

#include "bsp.h"
#include "rb_screen.h"
#include "rb_style.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_VU_LABEL_MAX 12
#define RB_VU_SEGS_MIN  4
#define RB_VU_SEGS_MAX  40
#define RB_VU_SEGS_DEFAULT 20

typedef enum {
    RB_VU_ORIENT_HORIZONTAL = 0,
    RB_VU_ORIENT_VERTICAL,
} rb_vu_orient_t;

typedef enum {
    RB_VU_SCALE_LINEAR = 0, /* min_v..max_v via set_value / set_peak */
    RB_VU_SCALE_DBFS,       /* amp 0..32767 via set_level             */
} rb_vu_scale_t;

typedef struct rb_vu {
    rb_widget_t base; /* must be first — multi-instance, no globals */
    uint8_t orient;
    uint8_t scale;
    uint8_t n_segs;
    uint8_t warn_from;   /* first warn segment index */
    uint8_t danger_from; /* first danger segment index */
    uint8_t show_value;
    uint8_t show_peak;
    uint8_t clipped;
    uint8_t ok;
    uint8_t gap_px;
    uint8_t thickness_px; /* 0 = fill cell short side */
    uint8_t radius;
    uint8_t caption_gap;
    int16_t floor_db; /* DBFS only; span is floor_db..0 */
    int32_t min_v, max_v, value, peak;
    int16_t hold_seg;
    uint16_t hold_ms;
    uint16_t decay_ms;
    uint32_t hold_raised_ms;
    uint32_t last_decay_ms;
    uint8_t ok_r, ok_g, ok_b;
    uint8_t wn_r, wn_g, wn_b;
    uint8_t dg_r, dg_g, dg_b;
    uint8_t tr_r, tr_g, tr_b;
    uint8_t pk_r, pk_g, pk_b;
    uint8_t tx_r, tx_g, tx_b;
    char label[RB_VU_LABEL_MAX];
} rb_vu_t;

void rb_vu_init(rb_vu_t *vu);
int  rb_vu_apply_geometry(rb_vu_t *vu);

void rb_vu_set_orient(rb_vu_t *vu, rb_vu_orient_t orient);
void rb_vu_set_scale(rb_vu_t *vu, rb_vu_scale_t scale);
void rb_vu_set_segments(rb_vu_t *vu, uint8_t n);
/* Color splits by segment index: i < warn → ok; i < danger → warn; else danger. */
void rb_vu_set_zones(rb_vu_t *vu, uint8_t warn_from, uint8_t danger_from);
void rb_vu_set_gap_px(rb_vu_t *vu, uint8_t gap);
void rb_vu_set_thickness_px(rb_vu_t *vu, uint8_t px);
void rb_vu_set_radius(rb_vu_t *vu, uint8_t r);
void rb_vu_set_db_range(rb_vu_t *vu, int16_t floor_db); /* DBFS; top is 0 */
void rb_vu_set_hold_ms(rb_vu_t *vu, uint16_t ms);
void rb_vu_set_decay_ms(rb_vu_t *vu, uint16_t ms);

void rb_vu_set_range(rb_vu_t *vu, int32_t min_v, int32_t max_v); /* LINEAR */
void rb_vu_set_value(rb_vu_t *vu, int32_t value);                /* LINEAR */
void rb_vu_set_peak(rb_vu_t *vu, int32_t peak);                  /* LINEAR */
int32_t rb_vu_get_value(const rb_vu_t *vu);
int32_t rb_vu_get_peak(const rb_vu_t *vu);

/* DBFS: amplitude 0..32767 (same units as rb_mic last_rms / last_peak). */
void rb_vu_set_level(rb_vu_t *vu, uint16_t rms, uint16_t peak);
void rb_vu_set_clipped(rb_vu_t *vu, int clipped);

void rb_vu_tick(rb_vu_t *vu); /* idle peak-hold decay */

void rb_vu_set_style(rb_vu_t *vu, rb_style_t style);
void rb_vu_set_label(rb_vu_t *vu, const char *text); /* NULL/"" hide */
void rb_vu_set_show_value(rb_vu_t *vu, int on);
void rb_vu_set_show_peak(rb_vu_t *vu, int on);

void rb_vu_set_ok_rgb(rb_vu_t *vu, uint8_t r, uint8_t g, uint8_t b);
void rb_vu_set_warn_rgb(rb_vu_t *vu, uint8_t r, uint8_t g, uint8_t b);
void rb_vu_set_danger_rgb(rb_vu_t *vu, uint8_t r, uint8_t g, uint8_t b);
void rb_vu_set_track_rgb(rb_vu_t *vu, uint8_t r, uint8_t g, uint8_t b);
void rb_vu_set_peak_rgb(rb_vu_t *vu, uint8_t r, uint8_t g, uint8_t b);
void rb_vu_set_text_rgb(rb_vu_t *vu, uint8_t r, uint8_t g, uint8_t b);

void rb_vu_redraw(rb_vu_t *vu);
int  rb_vu_contains(const rb_vu_t *vu, int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_VU_H */
