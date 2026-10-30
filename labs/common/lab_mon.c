/*
 * lab_mon.c - Task Monitor banner via lab_lcd.
 *
 * Layout (4 lines):
 *   0 title (scale 2)
 *   1..3 task rows (scale 1) — hint reuses row 3 when set
 */
#include "lab_mon.h"

#include "lab_lcd.h"

#include <stdarg.h>
#include <stdio.h>

void mon_init(void)
{
}

void mon_start(void)
{
    (void)lcd_begin(4);

    lcd_set_scale(0, 2);
    lcd_set_color(0, RB_LABEL_CYAN);
    lcd_printf(0, "Task Mon");

    lcd_set_scale(1, 1);
    lcd_set_color(1, RB_LABEL_GRAY);
    lcd_printf(1, "-");

    lcd_set_scale(2, 1);
    lcd_set_color(2, RB_LABEL_GRAY);
    lcd_printf(2, "-");

    lcd_set_scale(3, 1);
    lcd_set_color(3, RB_LABEL_GRAY);
    lcd_printf(3, "-");
}

void mon_set_title(const char *fmt, ...)
{
    char buf[RB_LABEL_TEXT_MAX];
    va_list ap;

    if (fmt == 0) {
        return;
    }
    va_start(ap, fmt);
    (void)vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    lcd_set_color(0, RB_LABEL_CYAN);
    lcd_printf(0, "%s", buf);
}

void mon_set_row(unsigned index, const char *fmt, ...)
{
    char buf[RB_LABEL_TEXT_MAX];
    va_list ap;
    unsigned row;

    if (index >= MON_ROWS || fmt == 0) {
        return;
    }
    row = index + 1u;
    va_start(ap, fmt);
    (void)vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    lcd_set_color(row, RB_LABEL_GREEN);
    lcd_printf(row, "%s", buf);
}

void mon_set_hint(const char *fmt, ...)
{
    char buf[RB_LABEL_TEXT_MAX];
    va_list ap;

    if (fmt == 0) {
        return;
    }
    va_start(ap, fmt);
    (void)vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    lcd_set_color(3, RB_LABEL_GRAY);
    lcd_printf(3, "%s", buf);
}

void mon_clear_rows(void)
{
    unsigned i;

    for (i = 0; i < MON_ROWS; i++) {
        mon_set_row(i, "-");
    }
}
