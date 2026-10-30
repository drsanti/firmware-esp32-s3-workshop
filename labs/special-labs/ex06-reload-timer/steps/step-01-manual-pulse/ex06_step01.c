/*
 * S6 / step-01 - BOOT manually pulses a beat on the LCD.
 *
 * Details: README.md
 */

#include "ex06_step01.h"
#include "bsp.h"
#include "lab_lcd.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define BOOT_BTN_MASK 2u

static int boot_edge(uint32_t *was)
{
    uint32_t now = bsp_buttons() & BOOT_BTN_MASK;
    int edge = (now != 0u && *was == 0u) ? 1 : 0;
    *was = now;
    return edge;
}

void ex06_step01_start(void)
{
    uint32_t was = 0;
    unsigned n = 0;

    (void)lcd_begin(3);
    lcd_printf(0, "S6 manual");
    lcd_printf(1, "beat idle");
    lcd_printf(2, "press BOOT");

    for (;;) {
        if (boot_edge(&was)) {
            n++;
            lcd_printf(1, "* BEAT *");
            lcd_printf(2, "pulses %u", n);
            vTaskDelay(pdMS_TO_TICKS(80));
            lcd_printf(1, "beat idle");
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
