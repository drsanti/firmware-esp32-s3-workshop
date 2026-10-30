/*
 * rb_level.h — DISPLAY spirit-level bubble (tilt from accel x/y).
 *
 * Ball position = ax/ay in g, clamped to ±1. Radius follows the
 * layout cell (min(w,h)/2 − pad). Multi-instance like other leaves.
 *
 *   rb_level_init(&lv);
 *   rb_level_set_accel_mps2(&lv, ax, ay);
 *   rb_level_set_live(&lv, 1);   // white core when sensor ok
 */
#ifndef ROBOTOS_RB_LEVEL_H
#define ROBOTOS_RB_LEVEL_H

#include <stdint.h>

#include "bsp.h"
#include "rb_screen.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rb_level {
    rb_widget_t base; /* must be first */
    float ax_g, ay_g; /* tilt in g, clamped on set */
    uint8_t live;     /* 1 = sensor ok (white core) */
    uint8_t show_cross;
    uint8_t ok;
    uint8_t pad_px; /* inset from cell; 0 = default 6 */
    uint8_t ball_r; /* 0 = default ~1/8 of field R */
    uint8_t ring_r, ring_g, ring_b;
    uint8_t ball_r8, ball_g, ball_b;
    uint8_t cross_r, cross_g, cross_b;
} rb_level_t;

void rb_level_init(rb_level_t *lv);
int  rb_level_apply_geometry(rb_level_t *lv);

/* ax/ay in m/s^2 (bsp_imu_read). Converted to g and clamped ±1. */
void rb_level_set_accel_mps2(rb_level_t *lv, float ax, float ay);
/* Direct g units (already /9.81). */
void rb_level_set_accel_g(rb_level_t *lv, float ax_g, float ay_g);
void rb_level_set_live(rb_level_t *lv, int on);
void rb_level_set_show_cross(rb_level_t *lv, int on);
void rb_level_set_ring_rgb(rb_level_t *lv, uint8_t r, uint8_t g, uint8_t b);
void rb_level_set_ball_rgb(rb_level_t *lv, uint8_t r, uint8_t g, uint8_t b);
void rb_level_set_cross_rgb(rb_level_t *lv, uint8_t r, uint8_t g, uint8_t b);

void rb_level_redraw(rb_level_t *lv);
int  rb_level_contains(const rb_level_t *lv, int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_LEVEL_H */
