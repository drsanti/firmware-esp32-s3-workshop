/*
 * rb_style.h — shared semantic UI styles (Bootstrap-like roles).
 *
 * One palette for all leaves: button / switch / led / label / term.
 * Widgets map tokens to their paint roles via rb_*_set_style().
 *
 *   rb_button_set_style(&btn, RB_STYLE_PRIMARY);
 *   rb_led_set_style(&led, RB_STYLE_DANGER);
 */
#ifndef ROBOTOS_RB_STYLE_H
#define ROBOTOS_RB_STYLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RB_STYLE_PRIMARY = 0,
    RB_STYLE_SECONDARY,
    RB_STYLE_SUCCESS,
    RB_STYLE_WARNING,
    RB_STYLE_DANGER,
    RB_STYLE_INFO,
    RB_STYLE_DARK,
    RB_STYLE_LIGHT,
    RB_STYLE_COUNT
} rb_style_t;

typedef struct rb_style_colors {
    uint8_t face_r, face_g, face_b;
    uint8_t border_r, border_g, border_b;
    uint8_t text_r, text_g, text_b;
    uint8_t accent_r, accent_g, accent_b; /* LED / switch ON / indicator */
} rb_style_colors_t;

/* Returns pointer into a static table; never NULL (clamps unknown). */
const rb_style_colors_t *rb_style_colors(rb_style_t style);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_STYLE_H */
