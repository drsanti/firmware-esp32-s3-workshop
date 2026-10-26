/*
 * rb_label.h — single-line text leaf under screen/layout.
 *
 *   rb_label_init(&lab);
 *   rb_layout_add(&layout, &lab.base);
 *   rb_screen_reflow(&screen);
 *   rb_label_printf(&lab, "A %lu", n);
 */
#ifndef ROBOTOS_RB_LABEL_H
#define ROBOTOS_RB_LABEL_H

#include <stdarg.h>
#include <stdint.h>

#include "rb_screen.h"
#include "rb_style.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_LABEL_RGB(r, g, b) \
    ((uint16_t)((((r)&0xF8u) << 8) | (((g)&0xFCu) << 3) | ((b) >> 3)))
#define RB_LABEL_BLACK   0x0000u
#define RB_LABEL_WHITE   0xFFFFu
#define RB_LABEL_RED     0xF800u
#define RB_LABEL_GREEN   0x07E0u
#define RB_LABEL_BLUE    0x001Fu
#define RB_LABEL_YELLOW  0xFFE0u
#define RB_LABEL_CYAN    0x07FFu
#define RB_LABEL_MAGENTA 0xF81Fu
#define RB_LABEL_GRAY    0x7BEFu

#define RB_LABEL_TEXT_MAX 64

typedef enum {
    RB_LABEL_ALIGN_LEFT = 0,
    RB_LABEL_ALIGN_CENTER,
    RB_LABEL_ALIGN_RIGHT,
} rb_label_align_t;

typedef struct rb_label {
    rb_widget_t base; /* must be first */
    char text[RB_LABEL_TEXT_MAX];
    uint16_t fg, bg;
    uint16_t pad_l, pad_t, pad_r, pad_b;
    uint8_t scale;
    uint8_t align;
    uint8_t ok;
} rb_label_t;

void rb_label_init(rb_label_t *l);
int  rb_label_apply_geometry(rb_label_t *l);

void     rb_label_set_style(rb_label_t *l, rb_style_t style);
void     rb_label_set_color(rb_label_t *l, uint16_t fg, uint16_t bg);
void     rb_label_set_fg_rgb(rb_label_t *l, uint8_t r, uint8_t g, uint8_t b);
void     rb_label_set_bg_rgb(rb_label_t *l, uint8_t r, uint8_t g, uint8_t b);
void     rb_label_set_padding(rb_label_t *l, uint16_t left, uint16_t top,
                              uint16_t right, uint16_t bottom);
uint8_t  rb_label_set_scale(rb_label_t *l, uint8_t scale);
void     rb_label_set_align(rb_label_t *l, rb_label_align_t align);
void     rb_label_set_text(rb_label_t *l, const char *s);
void     rb_label_printf(rb_label_t *l, const char *fmt, ...);
void     rb_label_redraw(rb_label_t *l);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_LABEL_H */
