/*
 * lab_react.c - Reaction Game banner via lab_lcd.
 */
#include "lab_react.h"

#include "lab_lcd.h"

#include <stdarg.h>
#include <stdio.h>

void react_init(void)
{
}

void react_start(void)
{
    (void)lcd_begin(4);
    lcd_set_scale(0, 2);
    lcd_set_color(0, RB_LABEL_CYAN);
    lcd_printf(0, "Reaction");

    lcd_set_scale(1, 3);
    lcd_set_color(1, RB_LABEL_GREEN);
    lcd_printf(1, "READY");

    lcd_set_scale(2, 1);
    lcd_set_color(2, RB_LABEL_GRAY);
    lcd_printf(2, " ");

    lcd_set_scale(3, 1);
    lcd_set_color(3, RB_LABEL_GRAY);
    lcd_printf(3, " ");
}

void react_set_state(react_st_t st)
{
    switch (st) {
    case REACT_ST_READY:
        lcd_set_color(1, RB_LABEL_GREEN);
        lcd_printf(1, "READY");
        break;
    case REACT_ST_WAIT:
        lcd_set_color(1, RB_LABEL_YELLOW);
        lcd_printf(1, "WAIT");
        break;
    case REACT_ST_GO:
        lcd_set_color(1, RB_LABEL_RED);
        lcd_printf(1, "GO!");
        break;
    case REACT_ST_RESULT:
        lcd_set_color(1, RB_LABEL_CYAN);
        lcd_printf(1, "RESULT");
        break;
    default:
        lcd_set_color(1, RB_LABEL_GRAY);
        lcd_printf(1, "?");
        break;
    }
}

void react_set_detail(const char *fmt, ...)
{
    char buf[RB_LABEL_TEXT_MAX];
    va_list ap;

    if (fmt == 0) {
        return;
    }
    va_start(ap, fmt);
    (void)vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    lcd_set_color(2, RB_LABEL_GRAY);
    lcd_printf(2, "%s", buf);
}

void react_set_hint(const char *fmt, ...)
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
