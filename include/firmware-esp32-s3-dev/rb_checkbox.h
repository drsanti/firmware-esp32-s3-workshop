/*
 * rb_checkbox.h — flat checkbox leaf (box + optional label).
 *
 *   rb_checkbox_init(&cb);
 *   rb_checkbox_set_label(&cb, "Mute");
 *   rb_checkbox_set_style(&cb, RB_STYLE_PRIMARY);
 *   rb_checkbox_set_on_change(&cb, on_change, user);
 *   // poll: bsp_touch_read(&smp); rb_checkbox_feed_touch(&cb, &smp);
 */
#ifndef ROBOTOS_RB_CHECKBOX_H
#define ROBOTOS_RB_CHECKBOX_H

#include <stdint.h>

#include "bsp.h"
#include "rb_screen.h"
#include "rb_style.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_CHECKBOX_LABEL_MAX 16

typedef struct rb_checkbox rb_checkbox_t;
/* Fires from feed_touch when toggled (not from set_on). */
typedef void (*rb_checkbox_on_change_fn)(rb_checkbox_t *cb, int on,
                                         void *user);

struct rb_checkbox {
    rb_widget_t base; /* must be first */
    uint8_t on;
    uint8_t pressed;
    uint8_t ok;
    uint8_t box_r, box_g, box_b; /* border / unchecked face */
    uint8_t fill_r, fill_g, fill_b; /* checked fill */
    uint8_t mark_r, mark_g, mark_b; /* check mark */
    uint8_t tx_r, tx_g, tx_b;       /* label */
    char label[RB_CHECKBOX_LABEL_MAX];
    rb_checkbox_on_change_fn on_change;
    void *user;
};

void rb_checkbox_init(rb_checkbox_t *cb);
int  rb_checkbox_apply_geometry(rb_checkbox_t *cb);

void rb_checkbox_set_on(rb_checkbox_t *cb, int on);
int  rb_checkbox_is_on(const rb_checkbox_t *cb);
void rb_checkbox_set_label(rb_checkbox_t *cb, const char *text); /* NULL/"" */
void rb_checkbox_set_style(rb_checkbox_t *cb, rb_style_t style);
void rb_checkbox_set_box_rgb(rb_checkbox_t *cb, uint8_t r, uint8_t g,
                             uint8_t b);
void rb_checkbox_set_fill_rgb(rb_checkbox_t *cb, uint8_t r, uint8_t g,
                              uint8_t b);
void rb_checkbox_set_mark_rgb(rb_checkbox_t *cb, uint8_t r, uint8_t g,
                              uint8_t b);
void rb_checkbox_set_text_rgb(rb_checkbox_t *cb, uint8_t r, uint8_t g,
                              uint8_t b);
/* fn may be NULL to clear. Invoked outside the display lock. */
void rb_checkbox_set_on_change(rb_checkbox_t *cb, rb_checkbox_on_change_fn fn,
                               void *user);
void rb_checkbox_redraw(rb_checkbox_t *cb);

int rb_checkbox_contains(const rb_checkbox_t *cb, int16_t x, int16_t y);
/* Rising-edge press inside the cell toggles. Returns 1 if toggled. */
int rb_checkbox_feed_touch(rb_checkbox_t *cb, const bsp_touch_sample_t *s);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_CHECKBOX_H */
