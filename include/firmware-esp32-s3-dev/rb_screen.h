/*
 * rb_screen.h — display device wrapper + UI tree (screen → layout → …).
 *
 * rb_display_t owns the panel framebuffer (not a widget).
 * rb_screen_t is the UI root; rb_layout_t is a container; leaves
 * (e.g. rb_term_t) embed rb_widget_t as their first member.
 *
 * Static allocation only — caller owns all structs.
 *
 * Note: middleware/rb_ui.h is the Feature shim for rb_ui_* (uicmd) —
 * a different module. Do not confuse the two.
 */
#ifndef ROBOTOS_RB_SCREEN_H
#define ROBOTOS_RB_SCREEN_H

#include <stdint.h>

#include "ui/gfx.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_LAYOUT_MAX_CHILDREN 16

typedef struct {
    int16_t  x, y;
    uint16_t w, h;
} rb_rect_t;

typedef struct rb_display {
    display_dev_t *dev;
    uint16_t *fb;
    rbgfx_t gfx;
    uint8_t open;
} rb_display_t;

typedef enum {
    RB_WIDGET_SCREEN = 1,
    RB_WIDGET_LAYOUT,
    RB_WIDGET_TERM,
    RB_WIDGET_LABEL,
    RB_WIDGET_LED,
    RB_WIDGET_SWITCH,
    RB_WIDGET_BUTTON,
    RB_WIDGET_SLIDER,
    RB_WIDGET_CHECKBOX,
    RB_WIDGET_RADIO,
    RB_WIDGET_GAUGE,
    RB_WIDGET_PROGRESS,
} rb_widget_kind_t;

typedef struct rb_widget {
    rb_widget_kind_t kind;
    rb_rect_t rect;           /* display pixels */
    struct rb_widget *parent;
    rb_display_t *disp;       /* cached root display */
    uint8_t visible;
} rb_widget_t;

typedef struct rb_screen {
    rb_widget_t base;
    rb_widget_t *root;        /* typically &layout.base */
} rb_screen_t;

typedef enum {
    RB_LAYOUT_ROWS = 1,
    RB_LAYOUT_COLS,
    RB_LAYOUT_GRID,
} rb_layout_kind_t;

typedef struct rb_layout {
    rb_widget_t base;
    rb_layout_kind_t kind;
    uint16_t gap;
    unsigned nrows, ncols;
    rb_widget_t *children[RB_LAYOUT_MAX_CHILDREN];
    unsigned child_count;
} rb_layout_t;

/* ---- display (device + FB) -------------------------------------- */
int      rb_display_open(rb_display_t *d);
void     rb_display_close(rb_display_t *d);
void     rb_display_flush(rb_display_t *d);
uint16_t rb_display_width(const rb_display_t *d);
uint16_t rb_display_height(const rb_display_t *d);
rb_display_t *rb_display_default(void);

/* ---- screen (UI root) ------------------------------------------- */
int  rb_screen_init(rb_screen_t *s, rb_display_t *d);
void rb_screen_set_root(rb_screen_t *s, rb_widget_t *root);
void rb_screen_reflow(rb_screen_t *s);

/* ---- layout (container) ----------------------------------------- */
int  rb_layout_init(rb_layout_t *L, rb_screen_t *screen, rb_layout_kind_t kind,
                    unsigned nrows, unsigned ncols, uint16_t gap);
int  rb_layout_add(rb_layout_t *L, rb_widget_t *child);
void rb_layout_reflow(rb_layout_t *L);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_SCREEN_H */
