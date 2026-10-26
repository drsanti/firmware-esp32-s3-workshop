/*
 * rb_switch.h — flat slide switch leaf (pill or round-rect).
 *
 * Colored track, state label, white knob + grip lines.
 * Apps may set state with rb_switch_set_on(), customize labels
 * (default "ON"/"OFF"), and/or feed touch samples to toggle.
 *
 *   rb_switch_init(&sw);
 *   rb_switch_set_labels(&sw, "YES", "NO");
 *   rb_switch_set_on(&sw, 1);
 *   // poll: bsp_touch_read(&smp); rb_switch_feed_touch(&sw, &smp);
 */
#ifndef ROBOTOS_RB_SWITCH_H
#define ROBOTOS_RB_SWITCH_H

#include <stdint.h>

#include "bsp.h"
#include "rb_screen.h"
#include "rb_style.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_SWITCH_LABEL_MAX 8 /* incl. NUL — e.g. "YES", "OFF" */

typedef enum {
    RB_SWITCH_SHAPE_PILL = 0,   /* capsule */
    RB_SWITCH_SHAPE_ROUND_RECT, /* small corner radius */
} rb_switch_shape_t;

typedef struct rb_switch rb_switch_t;
/* Fires from feed_touch when toggled (not from set_on). */
typedef void (*rb_switch_on_change_fn)(rb_switch_t *sw, int on, void *user);

struct rb_switch {
    rb_widget_t base; /* must be first */
    uint8_t on;       /* 0/1 */
    uint8_t shape;
    uint8_t r_on, g_on, b_on;
    uint8_t r_off, g_off, b_off;
    uint8_t kr, kg, kb; /* knob fill */
    uint8_t pressed; /* touch edge latch */
    uint8_t ok;
    char label_on[RB_SWITCH_LABEL_MAX];
    char label_off[RB_SWITCH_LABEL_MAX];
    rb_switch_on_change_fn on_change;
    void *user;
};

void rb_switch_init(rb_switch_t *sw);
int  rb_switch_apply_geometry(rb_switch_t *sw);

void rb_switch_set_on(rb_switch_t *sw, int on);
int  rb_switch_is_on(const rb_switch_t *sw);
void rb_switch_set_shape(rb_switch_t *sw, rb_switch_shape_t shape);
void rb_switch_set_style(rb_switch_t *sw, rb_style_t style);
void rb_switch_set_on_rgb(rb_switch_t *sw, uint8_t r, uint8_t g, uint8_t b);
void rb_switch_set_off_rgb(rb_switch_t *sw, uint8_t r, uint8_t g, uint8_t b);
void rb_switch_set_knob_rgb(rb_switch_t *sw, uint8_t r, uint8_t g, uint8_t b);
/* Copy labels (truncated to RB_SWITCH_LABEL_MAX-1). NULL keeps current. */
void rb_switch_set_labels(rb_switch_t *sw, const char *on_text,
                          const char *off_text);
/* fn may be NULL to clear. Invoked outside the display lock. */
void rb_switch_set_on_change(rb_switch_t *sw, rb_switch_on_change_fn fn,
                             void *user);
void rb_switch_redraw(rb_switch_t *sw);

/* Hit-test against the layout cell (full base.rect — easy tap target). */
int rb_switch_contains(const rb_switch_t *sw, int16_t x, int16_t y);

/* Rising-edge press inside the cell toggles. Returns 1 if toggled. */
int rb_switch_feed_touch(rb_switch_t *sw, const bsp_touch_sample_t *s);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_SWITCH_H */
