/*
 * rb_gauge.h — horseshoe gauge (270° CW from bottom-left).
 *
 *   rb_gauge_init(&g);
 *   rb_gauge_set_range(&g, 0, 100);
 *   rb_gauge_set_unit(&g, "mV");
 *   rb_gauge_set_label(&g, "Batt");
 *   rb_gauge_set_bands(&g, 70, 90);           // warn / danger thresholds
 *   rb_gauge_set_mode(&g, RB_GAUGE_MODE_INPUT);
 *   rb_gauge_set_on_change(&g, on_change, user);
 *   // DISPLAY: set_value from code; INPUT: feed_touch drags the fill
 */
#ifndef ROBOTOS_RB_GAUGE_H
#define ROBOTOS_RB_GAUGE_H

#include <stdint.h>

#include "bsp.h"
#include "rb_screen.h"
#include "rb_style.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_GAUGE_LABEL_MAX 12
#define RB_GAUGE_UNIT_MAX  6

typedef enum {
    RB_GAUGE_MODE_DISPLAY = 0,
    RB_GAUGE_MODE_INPUT,
} rb_gauge_mode_t;

typedef struct rb_gauge rb_gauge_t;
/* Fires from feed_touch when value changes (not from set_value). */
typedef void (*rb_gauge_on_change_fn)(rb_gauge_t *g, int32_t value,
                                      void *user);

struct rb_gauge {
    rb_widget_t base; /* must be first */
    uint8_t mode;
    uint8_t show_value;
    uint8_t pressed;
    uint8_t ok;
    uint8_t thickness_px; /* 0 = auto ~20% of outer radius */
    int16_t sweep_deg;    /* magnitude; default 270; drawn CW */
    int16_t start_deg;    /* default -135 (bottom-left tip) */
    int32_t min_v, max_v, value;
    int32_t warn_at;   /* value >= warn_at → warn fill; > max = off */
    int32_t danger_at; /* value >= danger_at → danger fill; > max = off */
    uint8_t tr_r, tr_g, tr_b;
    uint8_t fl_r, fl_g, fl_b;
    uint8_t wn_r, wn_g, wn_b;
    uint8_t dg_r, dg_g, dg_b;
    uint8_t tx_r, tx_g, tx_b;
    char unit[RB_GAUGE_UNIT_MAX];
    char label[RB_GAUGE_LABEL_MAX];
    rb_gauge_on_change_fn on_change;
    void *user;
};

void rb_gauge_init(rb_gauge_t *g);
int  rb_gauge_apply_geometry(rb_gauge_t *g);

void rb_gauge_set_mode(rb_gauge_t *g, rb_gauge_mode_t mode);
void rb_gauge_set_range(rb_gauge_t *g, int32_t min_v, int32_t max_v);
void rb_gauge_set_value(rb_gauge_t *g, int32_t value);
int32_t rb_gauge_get_value(const rb_gauge_t *g);
void rb_gauge_set_style(rb_gauge_t *g, rb_style_t style);
void rb_gauge_set_unit(rb_gauge_t *g, const char *text);  /* NULL/"" hide */
void rb_gauge_set_label(rb_gauge_t *g, const char *text); /* NULL/"" hide */
void rb_gauge_set_show_value(rb_gauge_t *g, int on);
void rb_gauge_set_thickness(rb_gauge_t *g, uint8_t thickness_px);
void rb_gauge_set_sweep_deg(rb_gauge_t *g, int16_t sweep_deg);
void rb_gauge_set_start_deg(rb_gauge_t *g, int16_t start_deg);
/* Thresholds in value space. Pass warn/danger > max_v to disable. */
void rb_gauge_set_bands(rb_gauge_t *g, int32_t warn_at, int32_t danger_at);
void rb_gauge_set_track_rgb(rb_gauge_t *g, uint8_t r, uint8_t gg, uint8_t b);
void rb_gauge_set_fill_rgb(rb_gauge_t *g, uint8_t r, uint8_t gg, uint8_t b);
void rb_gauge_set_warn_rgb(rb_gauge_t *g, uint8_t r, uint8_t gg, uint8_t b);
void rb_gauge_set_danger_rgb(rb_gauge_t *g, uint8_t r, uint8_t gg, uint8_t b);
void rb_gauge_set_text_rgb(rb_gauge_t *g, uint8_t r, uint8_t gg, uint8_t b);
/* fn may be NULL to clear. Invoked outside the display lock. */
void rb_gauge_set_on_change(rb_gauge_t *g, rb_gauge_on_change_fn fn,
                            void *user);

void rb_gauge_redraw(rb_gauge_t *g);
int  rb_gauge_contains(const rb_gauge_t *g, int16_t x, int16_t y);
/* INPUT mode only: drag maps angle → value. Returns 1 if changed. */
int  rb_gauge_feed_touch(rb_gauge_t *g, const bsp_touch_sample_t *s);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_GAUGE_H */
