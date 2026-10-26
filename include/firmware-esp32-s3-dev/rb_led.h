/*
 * rb_led.h — framed indicator LED leaf (circle or round-rect).
 *
 * Display-only: color + intensity drive lens + soft glow. Does not
 * call bsp_led_* (bind hardware in the app if needed).
 *
 *   rb_led_init(&led);
 *   rb_led_set_shape(&led, RB_LED_SHAPE_CIRCLE);
 *   rb_led_set_rgb(&led, 0, 200, 255);
 *   rb_led_set_intensity(&led, 255);   // 0..255
 */
#ifndef ROBOTOS_RB_LED_H
#define ROBOTOS_RB_LED_H

#include <stdint.h>

#include "rb_screen.h"
#include "rb_style.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RB_LED_SHAPE_CIRCLE = 0,
    RB_LED_SHAPE_ROUND_RECT,
} rb_led_shape_t;

typedef struct rb_led {
    rb_widget_t base; /* must be first */
    uint8_t r, g, b;
    uint8_t fr, fg, fb; /* frame ring */
    uint8_t br, bg, bb; /* body (behind lens) */
    uint8_t intensity; /* 0 = off look, 255 = full */
    uint8_t shape;
    uint8_t ok;
} rb_led_t;

void rb_led_init(rb_led_t *led);
int  rb_led_apply_geometry(rb_led_t *led);

void rb_led_set_style(rb_led_t *led, rb_style_t style);
void rb_led_set_rgb(rb_led_t *led, uint8_t r, uint8_t g, uint8_t b);
void rb_led_set_frame_rgb(rb_led_t *led, uint8_t r, uint8_t g, uint8_t b);
void rb_led_set_body_rgb(rb_led_t *led, uint8_t r, uint8_t g, uint8_t b);
void rb_led_set_intensity(rb_led_t *led, uint8_t intensity);
void rb_led_set_shape(rb_led_t *led, rb_led_shape_t shape);
void rb_led_redraw(rb_led_t *led);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_LED_H */
