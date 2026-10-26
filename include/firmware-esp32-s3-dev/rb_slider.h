/*
 * rb_slider.h — flat slider / progress (H/V) with label + value.
 *
 * Layout (from design snap):
 *   VERTICAL:   value above rail, label below
 *   HORIZONTAL: row1 = label (left) + value (right);
 *               row2 = rail + thumb
 *
 *   rb_slider_set_label(&sl, "VOL");
 *   rb_slider_set_show_value(&sl, 1);
 *   rb_slider_set_style(&sl, RB_STYLE_PRIMARY);
 */
#ifndef ROBOTOS_RB_SLIDER_H
#define ROBOTOS_RB_SLIDER_H

#include <stdint.h>

#include "bsp.h"
#include "rb_screen.h"
#include "rb_style.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_SLIDER_LABEL_MAX 12

typedef enum {
    RB_SLIDER_ORIENT_HORIZONTAL = 0,
    RB_SLIDER_ORIENT_VERTICAL,
} rb_slider_orient_t;

typedef enum {
    RB_SLIDER_MODE_INPUT = 0,
    RB_SLIDER_MODE_PROGRESS,
} rb_slider_mode_t;

typedef struct rb_slider rb_slider_t;
/* Fires from feed_touch when value changes (not from set_value). */
typedef void (*rb_slider_on_change_fn)(rb_slider_t *s, int32_t value,
                                       void *user);

struct rb_slider {
    rb_widget_t base; /* must be first */
    uint8_t orient;
    uint8_t mode;
    uint8_t show_value;
    uint8_t show_fill; /* 1 = colored fill along track (off in snap) */
    uint8_t caption_gap; /* px between caption row and rail (default 4) */
    uint8_t pressed;
    uint8_t ok;
    int32_t min_v, max_v, value;
    uint8_t step;
    uint8_t thumb_r_px; /* 0 = auto */
    uint8_t track_t_px;
    uint8_t thumb_border_on;
    uint8_t thumb_border_w;
    uint8_t tr_r, tr_g, tr_b;
    uint8_t fl_r, fl_g, fl_b;
    uint8_t th_r, th_g, th_b;
    uint8_t tb_r, tb_g, tb_b;
    uint8_t tx_r, tx_g, tx_b;
    char label[RB_SLIDER_LABEL_MAX];
    rb_slider_on_change_fn on_change;
    void *user;
};

void rb_slider_init(rb_slider_t *s);
int  rb_slider_apply_geometry(rb_slider_t *s);

void rb_slider_set_orient(rb_slider_t *s, rb_slider_orient_t orient);
void rb_slider_set_mode(rb_slider_t *s, rb_slider_mode_t mode);
void rb_slider_set_style(rb_slider_t *s, rb_style_t style);
void rb_slider_set_range(rb_slider_t *s, int32_t min_v, int32_t max_v);
void rb_slider_set_value(rb_slider_t *s, int32_t value);
int32_t rb_slider_get_value(const rb_slider_t *s);
void rb_slider_set_step(rb_slider_t *s, uint8_t step);
void rb_slider_set_label(rb_slider_t *s, const char *text); /* NULL/"" hide */
void rb_slider_set_show_value(rb_slider_t *s, int on);
void rb_slider_set_show_fill(rb_slider_t *s, int on);
/* Gap (px) between caption text and rail. Default 4. */
void rb_slider_set_caption_gap(rb_slider_t *s, uint8_t gap_px);
/* fn may be NULL to clear. Invoked outside the display lock. */
void rb_slider_set_on_change(rb_slider_t *s, rb_slider_on_change_fn fn,
                             void *user);

void rb_slider_set_thumb_radius(rb_slider_t *s, uint8_t radius_px);
void rb_slider_set_track_thickness(rb_slider_t *s, uint8_t thickness_px);
void rb_slider_set_track_rgb(rb_slider_t *s, uint8_t r, uint8_t g, uint8_t b);
void rb_slider_set_fill_rgb(rb_slider_t *s, uint8_t r, uint8_t g, uint8_t b);
void rb_slider_set_thumb_rgb(rb_slider_t *s, uint8_t r, uint8_t g, uint8_t b);
void rb_slider_set_text_rgb(rb_slider_t *s, uint8_t r, uint8_t g, uint8_t b);
void rb_slider_set_thumb_border(rb_slider_t *s, int visible);
void rb_slider_set_thumb_border_width(rb_slider_t *s, uint8_t width_px);
void rb_slider_set_thumb_border_rgb(rb_slider_t *s, uint8_t r, uint8_t g,
                                    uint8_t b);

void rb_slider_redraw(rb_slider_t *s);
int  rb_slider_contains(const rb_slider_t *s, int16_t x, int16_t y);
int  rb_slider_feed_touch(rb_slider_t *s, const bsp_touch_sample_t *smp);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_SLIDER_H */
