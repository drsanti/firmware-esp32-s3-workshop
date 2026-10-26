/*
 * rb_progress.h — DISPLAY-only progress bar (H/V) with bands.
 *
 * HORIZONTAL: row1 = label (left) + value (right); row2 = round-rect bar.
 * VERTICAL:   value above bar, label below; fill grows bottom→top.
 *
 *   rb_progress_init(&p);
 *   rb_progress_set_orient(&p, RB_PROGRESS_ORIENT_HORIZONTAL);
 *   rb_progress_set_range(&p, 0, 100);
 *   rb_progress_set_style(&p, RB_STYLE_SUCCESS);
 *   rb_progress_set_bands(&p, 50, 90);   // warn / danger fill
 *   rb_progress_set_label(&p, "CPU");
 *   rb_progress_set_value(&p, 20);
 */
#ifndef ROBOTOS_RB_PROGRESS_H
#define ROBOTOS_RB_PROGRESS_H

#include <stdint.h>

#include "bsp.h"
#include "rb_screen.h"
#include "rb_style.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_PROGRESS_LABEL_MAX 12

typedef enum {
    RB_PROGRESS_ORIENT_HORIZONTAL = 0,
    RB_PROGRESS_ORIENT_VERTICAL,
} rb_progress_orient_t;

typedef struct rb_progress {
    rb_widget_t base; /* must be first */
    uint8_t orient;
    uint8_t show_value;
    uint8_t ok;
    uint8_t thickness_px; /* 0 = default ~16; bar short-side */
    uint8_t caption_gap;  /* px between caption and bar (default 4) */
    int32_t min_v, max_v, value;
    int32_t warn_at;   /* value >= warn_at → warn fill; > max = off */
    int32_t danger_at; /* value >= danger_at → danger fill; > max = off */
    uint8_t tr_r, tr_g, tr_b;
    uint8_t fl_r, fl_g, fl_b;
    uint8_t wn_r, wn_g, wn_b;
    uint8_t dg_r, dg_g, dg_b;
    uint8_t tx_r, tx_g, tx_b;
    char label[RB_PROGRESS_LABEL_MAX];
} rb_progress_t;

void rb_progress_init(rb_progress_t *p);
int  rb_progress_apply_geometry(rb_progress_t *p);

void rb_progress_set_orient(rb_progress_t *p, rb_progress_orient_t orient);
void rb_progress_set_range(rb_progress_t *p, int32_t min_v, int32_t max_v);
void rb_progress_set_value(rb_progress_t *p, int32_t value);
int32_t rb_progress_get_value(const rb_progress_t *p);
void rb_progress_set_style(rb_progress_t *p, rb_style_t style);
void rb_progress_set_label(rb_progress_t *p, const char *text); /* NULL/"" hide */
void rb_progress_set_show_value(rb_progress_t *p, int on);
void rb_progress_set_track_thickness(rb_progress_t *p, uint8_t thickness_px);
/* Thresholds in value space. Pass warn/danger > max_v to disable. */
void rb_progress_set_bands(rb_progress_t *p, int32_t warn_at, int32_t danger_at);
void rb_progress_set_track_rgb(rb_progress_t *p, uint8_t r, uint8_t g, uint8_t b);
void rb_progress_set_fill_rgb(rb_progress_t *p, uint8_t r, uint8_t g, uint8_t b);
void rb_progress_set_warn_rgb(rb_progress_t *p, uint8_t r, uint8_t g, uint8_t b);
void rb_progress_set_danger_rgb(rb_progress_t *p, uint8_t r, uint8_t g,
                                uint8_t b);
void rb_progress_set_text_rgb(rb_progress_t *p, uint8_t r, uint8_t g, uint8_t b);

void rb_progress_redraw(rb_progress_t *p);
int  rb_progress_contains(const rb_progress_t *p, int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_PROGRESS_H */
