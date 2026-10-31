/*
 * lab_lcd.h - simple multi-line text banner for labs.
 * Labs-owned helper; does not touch demos/.
 *
 *   lcd_begin(2);
 *   lcd_printf(0, "Hello Device");
 *   lcd_printf(1, "step 01 - LCD");
 *
 * Use ASCII only on the LCD (no em dash / unicode).
 */
#ifndef ROBOTOS_LAB_LCD_H
#define ROBOTOS_LAB_LCD_H

#include "rb_label.h"
#include "rb_screen.h"

#include <stdarg.h>
#include <stdio.h>

#ifndef LAB_LCD_MAX_ROWS
#define LAB_LCD_MAX_ROWS 4
#endif

#ifndef LAB_LCD_GAP
#define LAB_LCD_GAP 8
#endif

static rb_display_t s_lcd_disp;
static rb_screen_t s_lcd_screen;
static rb_layout_t s_lcd_layout;
static rb_label_t s_lcd_labels[LAB_LCD_MAX_ROWS];
static unsigned s_lcd_rows;
static int s_lcd_ok;

static inline int lcd_begin(unsigned rows)
{
    unsigned i;

    if (rows < 1u) {
        rows = 1u;
    }
    if (rows > (unsigned)LAB_LCD_MAX_ROWS) {
        rows = (unsigned)LAB_LCD_MAX_ROWS;
    }

    if (rb_display_open(&s_lcd_disp) != 0) {
        s_lcd_ok = 0;
        return -1;
    }
    if (rb_screen_init(&s_lcd_screen, &s_lcd_disp) != 0) {
        s_lcd_ok = 0;
        return -1;
    }
    if (rb_layout_init(&s_lcd_layout, &s_lcd_screen, RB_LAYOUT_ROWS, rows, 1,
                       LAB_LCD_GAP) != 0) {
        s_lcd_ok = 0;
        return -1;
    }

    for (i = 0; i < rows; i++) {
        rb_label_init(&s_lcd_labels[i]);
        if (i == 0u) {
            (void)rb_label_set_scale(&s_lcd_labels[i], 2);
            rb_label_set_color(&s_lcd_labels[i], RB_LABEL_CYAN, RB_LABEL_BLACK);
        } else {
            (void)rb_label_set_scale(&s_lcd_labels[i], 1);
            rb_label_set_color(&s_lcd_labels[i], RB_LABEL_GRAY, RB_LABEL_BLACK);
        }
        rb_label_printf(&s_lcd_labels[i], " ");
        (void)rb_layout_add(&s_lcd_layout, &s_lcd_labels[i].base);
    }

    rb_screen_set_root(&s_lcd_screen, &s_lcd_layout.base);
    rb_screen_reflow(&s_lcd_screen);
    s_lcd_rows = rows;
    s_lcd_ok = 1;
    return 0;
}

static inline void lcd_printf(unsigned row, const char *fmt, ...)
{
    char buf[RB_LABEL_TEXT_MAX];
    va_list ap;

    if (!s_lcd_ok || row >= s_lcd_rows || fmt == 0) {
        return;
    }
    va_start(ap, fmt);
    (void)vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    rb_label_printf(&s_lcd_labels[row], "%s", buf);
}

static inline void lcd_set_color(unsigned row, uint16_t fg)
{
    if (!s_lcd_ok || row >= s_lcd_rows) {
        return;
    }
    rb_label_set_color(&s_lcd_labels[row], fg, RB_LABEL_BLACK);
}

/* Font scale: typically 1..3 (clamped by rb_label_set_scale). */
static inline void lcd_set_scale(unsigned row, uint8_t scale)
{
    if (!s_lcd_ok || row >= s_lcd_rows) {
        return;
    }
    (void)rb_label_set_scale(&s_lcd_labels[row], scale);
}

#endif /* ROBOTOS_LAB_LCD_H */
