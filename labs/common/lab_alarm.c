/*
 * lab_alarm.c - simple ARMED / IDLE / ALARM banner via lab_lcd.
 */
#include "lab_alarm.h"

#include "lab_lcd.h"

#include <stdarg.h>
#include <stdio.h>

void alarm_init(void)
{
    /* lcd_begin owns open; nothing else yet. */
}

void alarm_start(void)
{
    (void)lcd_begin(4);
    lcd_set_scale(0, 2);
    lcd_set_color(0, RB_LABEL_CYAN);
    lcd_printf(0, "Motion Alarm");

    lcd_set_scale(1, 3);
    lcd_set_color(1, RB_LABEL_GREEN);
    lcd_printf(1, "ARMED");

    lcd_set_scale(2, 1);
    lcd_set_color(2, RB_LABEL_GRAY);
    lcd_printf(2, " ");

    lcd_set_scale(3, 1);
    lcd_set_color(3, RB_LABEL_GRAY);
    lcd_printf(3, " ");
}

void alarm_set_state(alarm_st_t st)
{
    switch (st) {
    case ALARM_ST_IDLE:
        lcd_set_color(1, RB_LABEL_GRAY);
        lcd_printf(1, "IDLE");
        break;
    case ALARM_ST_ARMED:
        lcd_set_color(1, RB_LABEL_GREEN);
        lcd_printf(1, "ARMED");
        break;
    case ALARM_ST_ALARM:
        lcd_set_color(1, RB_LABEL_RED);
        lcd_printf(1, "ALARM");
        break;
    default:
        lcd_set_color(1, RB_LABEL_YELLOW);
        lcd_printf(1, "?");
        break;
    }
}

void alarm_set_detail(const char *fmt, ...)
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

void alarm_set_live(int on)
{
    lcd_set_color(3, on ? RB_LABEL_CYAN : RB_LABEL_YELLOW);
    lcd_printf(3, on ? "IMU live" : "IMU sim");
}
