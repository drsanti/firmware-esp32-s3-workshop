/*
 * rb_term.h — LCD text terminal pane (leaf under screen/layout).
 *
 * Tree: rb_display_t → rb_screen_t → rb_layout_t → rb_term_t
 * Every draw takes bsp_display_lock() (recursive mutex on ESP-IDF).
 *
 *   rb_display_open(&disp);
 *   rb_screen_init(&screen, &disp);
 *   rb_layout_init(&layout, &screen, RB_LAYOUT_GRID, 2, 2, 4);
 *   rb_term_init(&panes[i]);
 *   rb_layout_add(&layout, &panes[i].base);
 *   rb_screen_set_root(&screen, &layout.base);
 *   rb_screen_reflow(&screen);
 *   rb_term_log_printf_ex(&panes[0], RB_TERM_CYAN, RB_TERM_BLACK, "A %lu", n);
 */
#ifndef ROBOTOS_RB_TERM_H
#define ROBOTOS_RB_TERM_H

#include <stdarg.h>
#include <stdint.h>

#include "rb_screen.h"
#include "rb_style.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_TERM_RGB(r, g, b) \
    ((uint16_t)((((r)&0xF8u) << 8) | (((g)&0xFCu) << 3) | ((b) >> 3)))
#define RB_TERM_BLACK   0x0000u
#define RB_TERM_WHITE   0xFFFFu
#define RB_TERM_RED     0xF800u
#define RB_TERM_GREEN   0x07E0u
#define RB_TERM_BLUE    0x001Fu
#define RB_TERM_YELLOW  0xFFE0u
#define RB_TERM_CYAN    0x07FFu
#define RB_TERM_MAGENTA 0xF81Fu
#define RB_TERM_GRAY    0x7BEFu

/* Storage limits for the line-log ring (per pane). */
#define RB_TERM_LOG_MAX_ROWS 24
#define RB_TERM_LOG_MAX_COLS 48

typedef struct {
    char     text[RB_TERM_LOG_MAX_COLS];
    uint16_t fg, bg;
} rb_term_log_line_t;

typedef struct rb_term {
    rb_widget_t base; /* must be first — layout casts widget* → term* */
    uint16_t cols, rows;
    uint16_t col, row;
    uint16_t fg, bg;
    uint16_t pad_l, pad_t, pad_r, pad_b;
    uint8_t scale;
    uint8_t ok;
    rb_term_log_line_t log[RB_TERM_LOG_MAX_ROWS];
    uint16_t log_count;
} rb_term_t;

/* ---- term lifecycle --------------------------------------------- */
void rb_term_init(rb_term_t *t);
/* Apply base.disp + base.rect (after layout reflow or manual rect). */
int  rb_term_apply_geometry(rb_term_t *t);

/* Convenience: bind to a rect without a layout object. */
int rb_term_init_rect(rb_term_t *t, rb_display_t *d, const rb_rect_t *r);
int rb_term_init_full(rb_term_t *t, rb_display_t *d);

/* Singleton full-screen term (optional shortcut). */
int           rb_term_init_default(void);
int           rb_term_ready(void);
rb_term_t    *rb_term_default(void);

/* ---- style / geometry ------------------------------------------- */
void     rb_term_set_style(rb_term_t *t, rb_style_t style);
void     rb_term_set_color(rb_term_t *t, uint16_t fg, uint16_t bg);
void     rb_term_set_fg_rgb(rb_term_t *t, uint8_t r, uint8_t g, uint8_t b);
void     rb_term_set_bg_rgb(rb_term_t *t, uint8_t r, uint8_t g, uint8_t b);
uint8_t  rb_term_set_scale(rb_term_t *t, uint8_t scale);
uint8_t  rb_term_scale(const rb_term_t *t);
void     rb_term_set_padding(rb_term_t *t, uint16_t left, uint16_t top,
                             uint16_t right, uint16_t bottom);
uint16_t rb_term_cols(const rb_term_t *t);
uint16_t rb_term_rows(const rb_term_t *t);
void     rb_term_goto(rb_term_t *t, uint16_t col, uint16_t row);
void     rb_term_home(rb_term_t *t);
void     rb_term_get_cursor(const rb_term_t *t, uint16_t *col, uint16_t *row);
void     rb_term_clear(rb_term_t *t);
void     rb_term_clear_line(rb_term_t *t, uint16_t row);

/* ---- console cursor writes (pixel scroll) ----------------------- */
void rb_term_print(rb_term_t *t, const char *s);
void rb_term_println(rb_term_t *t, const char *s);
void rb_term_printf(rb_term_t *t, const char *fmt, ...);
void rb_term_putchar(rb_term_t *t, char ch);
void rb_term_set_line(rb_term_t *t, uint16_t row, const char *s);
void rb_term_print_at(rb_term_t *t, uint16_t col, uint16_t row,
                      const char *s);
void rb_term_printf_at(rb_term_t *t, uint16_t col, uint16_t row,
                       const char *fmt, ...);
void rb_term_set_line_ex(rb_term_t *t, uint16_t row, uint16_t fg,
                         uint16_t bg, const char *s);
void rb_term_print_at_ex(rb_term_t *t, uint16_t col, uint16_t row,
                         uint16_t fg, uint16_t bg, const char *s);
void rb_term_printf_at_ex(rb_term_t *t, uint16_t col, uint16_t row,
                          uint16_t fg, uint16_t bg, const char *fmt, ...);

/* ---- line log (index 0 = top / oldest; push drops first when full) */
uint16_t rb_term_log_capacity(const rb_term_t *t);
uint16_t rb_term_log_count(const rb_term_t *t);
void     rb_term_log_clear(rb_term_t *t);
void     rb_term_log_redraw(rb_term_t *t);
void     rb_term_log_remove_first(rb_term_t *t);
void     rb_term_log_push(rb_term_t *t, const char *s);
void     rb_term_log_push_ex(rb_term_t *t, uint16_t fg, uint16_t bg,
                             const char *s);
void     rb_term_log_printf(rb_term_t *t, const char *fmt, ...);
void     rb_term_log_printf_ex(rb_term_t *t, uint16_t fg, uint16_t bg,
                               const char *fmt, ...);
void     rb_term_log_set(rb_term_t *t, uint16_t index, const char *s);
void     rb_term_log_set_ex(rb_term_t *t, uint16_t index, uint16_t fg,
                            uint16_t bg, const char *s);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_TERM_H */
