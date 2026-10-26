/*
 * rb_button.h — flat button leaf (normal or toggle + indicator).
 *
 *   rb_button_set_on_event(&btn, on_event, user);
 *   // poll: bsp_touch_read(&smp); rb_button_feed_touch(&btn, &smp);
 *
 * Events (user touch only; not from set_on):
 *   DOWN  — finger entered / pressed inside
 *   UP    — finger left or lifted (after a DOWN)
 *   CLICK — lift while over the button (DOWN→lift); toggle flips here
 */
#ifndef ROBOTOS_RB_BUTTON_H
#define ROBOTOS_RB_BUTTON_H

#include <stdint.h>

#include "bsp.h"
#include "rb_screen.h"
#include "rb_style.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_BUTTON_LABEL_MAX 8

typedef enum {
    RB_BUTTON_MODE_NORMAL = 0,
    RB_BUTTON_MODE_TOGGLE,
} rb_button_mode_t;

typedef enum {
    RB_BUTTON_EV_DOWN = 0,
    RB_BUTTON_EV_UP,
    RB_BUTTON_EV_CLICK,
} rb_button_event_t;

typedef struct rb_button rb_button_t;
typedef void (*rb_button_on_event_fn)(rb_button_t *btn, rb_button_event_t ev,
                                      void *user);

struct rb_button {
    rb_widget_t base; /* must be first */
    uint8_t mode;
    uint8_t on;      /* toggle state */
    uint8_t pressed; /* touch latch */
    uint8_t r, g, b; /* face fill */
    uint8_t br, bg, bb;
    uint8_t tr, tg, tb; /* label text */
    uint8_t border_on;
    uint8_t border_w;
    uint8_t ind_on_r, ind_on_g, ind_on_b;
    uint8_t ind_off_r, ind_off_g, ind_off_b;
    uint8_t ind_w_pct;
    uint8_t ind_h_px;
    uint8_t ok;
    char label[RB_BUTTON_LABEL_MAX];
    char label_on[RB_BUTTON_LABEL_MAX];
    char label_off[RB_BUTTON_LABEL_MAX];
    rb_button_on_event_fn on_event;
    void *user;
};

void rb_button_init(rb_button_t *btn);
int  rb_button_apply_geometry(rb_button_t *btn);

void rb_button_set_mode(rb_button_t *btn, rb_button_mode_t mode);
void rb_button_set_label(rb_button_t *btn, const char *text);
void rb_button_set_labels(rb_button_t *btn, const char *on_text,
                          const char *off_text);
void rb_button_set_style(rb_button_t *btn, rb_style_t style);
void rb_button_set_rgb(rb_button_t *btn, uint8_t r, uint8_t g, uint8_t b);
void rb_button_set_text_rgb(rb_button_t *btn, uint8_t r, uint8_t g, uint8_t b);
void rb_button_set_border(rb_button_t *btn, int visible);
void rb_button_set_border_width(rb_button_t *btn, uint8_t width_px);
void rb_button_set_border_rgb(rb_button_t *btn, uint8_t r, uint8_t g,
                              uint8_t b);
void rb_button_set_indicator_rgb(rb_button_t *btn, uint8_t on_r, uint8_t on_g,
                                 uint8_t on_b, uint8_t off_r, uint8_t off_g,
                                 uint8_t off_b);
void rb_button_set_indicator_size(rb_button_t *btn, uint8_t width_pct,
                                  uint8_t thickness_px);
void rb_button_set_on(rb_button_t *btn, int on);
int  rb_button_is_on(const rb_button_t *btn);
/* fn may be NULL to clear. Invoked outside the display lock. */
void rb_button_set_on_event(rb_button_t *btn, rb_button_on_event_fn fn,
                            void *user);
void rb_button_redraw(rb_button_t *btn);

int rb_button_contains(const rb_button_t *btn, int16_t x, int16_t y);
/* Returns 1 if a CLICK occurred (toggle may have flipped). */
int rb_button_feed_touch(rb_button_t *btn, const bsp_touch_sample_t *s);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_BUTTON_H */
