/*
 * lab_metro.c - Metronome banner via lab_lcd.
 */
#include "lab_metro.h"

#include "lab_lcd.h"

#include <stdarg.h>
#include <stdio.h>

void metro_init(void)
{
}

void metro_start(void)
{
    (void)lcd_begin(4);
    lcd_set_scale(0, 2);
    lcd_set_color(0, RB_LABEL_CYAN);
    lcd_printf(0, "Metronome");

    lcd_set_scale(1, 3);
    lcd_set_color(1, RB_LABEL_GREEN);
    lcd_printf(1, "120 BPM");

    lcd_set_scale(2, 2);
    lcd_set_color(2, RB_LABEL_GRAY);
    lcd_printf(2, "beat");

    lcd_set_scale(3, 1);
    lcd_set_color(3, RB_LABEL_GRAY);
    lcd_printf(3, "jitter --");
}

void metro_set_bpm(unsigned bpm)
{
    lcd_set_color(1, RB_LABEL_GREEN);
    lcd_printf(1, "%u BPM", bpm);
}

void metro_flash_beat(void)
{
    lcd_set_color(2, RB_LABEL_YELLOW);
    lcd_printf(2, "* BEAT *");
}

void metro_set_beat_idle(void)
{
    lcd_set_color(2, RB_LABEL_GRAY);
    lcd_printf(2, "beat");
}

void metro_set_jitter(int last_ms, int max_ms)
{
    lcd_set_color(3, RB_LABEL_GRAY);
    if (max_ms < 0) {
        lcd_printf(3, "jitter %d ms", last_ms);
    } else {
        lcd_printf(3, "j %d / max %d", last_ms, max_ms);
    }
}

void metro_set_hint(const char *fmt, ...)
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
