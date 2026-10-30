/*
 * S6 / step-02 - heartbeat via vTaskDelay in the UI loop (awkward).
 *
 *   BOOT count only polled between long delays → feels stuck.
 *
 * Details: README.md
 */

#include "ex06_step02.h"
#include "bsp.h"
#include "lab_lcd.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define BOOT_BTN_MASK 2u
#define BEAT_MS 500u

static int boot_edge(uint32_t *was)
{
    uint32_t now = bsp_buttons() & BOOT_BTN_MASK;
    int edge = (now != 0u && *was == 0u) ? 1 : 0;
    *was = now;
    return edge;
}

void ex06_step02_start(void)
{
    uint32_t was = 0;
    unsigned boots = 0;
    unsigned beats = 0;

    (void)lcd_begin(3);
    lcd_printf(0, "S6 Delay beat");
    lcd_printf(1, "beat idle");
    lcd_printf(2, "boots 0");

    for (;;) {
        /* BROKEN for responsiveness: long delay owns the UI task. */
        beats++;
        lcd_printf(1, "* BEAT %u *", beats);
        vTaskDelay(pdMS_TO_TICKS(80));
        lcd_printf(1, "beat idle");

        if (boot_edge(&was)) {
            boots++;
            lcd_printf(2, "boots %u", boots);
        }

        vTaskDelay(pdMS_TO_TICKS(BEAT_MS));
    }
}
