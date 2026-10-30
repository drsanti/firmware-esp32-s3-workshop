/*
 * S2 / step-02 - BOOT GPIO ISR does a long busy-wait (broken).
 *
 *   Holding / tapping BOOT stalls the whole system while the ISR spins.
 *
 * Details: README.md
 */

#include "ex02_step02.h"
#include "bsp.h"
#include "lab_lcd.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define BOOT_GPIO 0

static volatile unsigned g_isr_hits;

static void IRAM_ATTR boot_isr(void *arg)
{
    volatile int i;

    (void)arg;
    g_isr_hits++;
    /* BROKEN: burn time in ISR — blocks scheduling / other ISRs. */
    for (i = 0; i < 800000; i++) {
    }
}

static void ui_task(void *arg)
{
    unsigned last = 0;
    unsigned n = 0;

    (void)arg;
    (void)lcd_begin(3);
    lcd_printf(0, "S2 ISR BUSY");
    lcd_printf(1, "hits 0");
    lcd_printf(2, "ui ticks 0");

    for (;;) {
        n++;
        lcd_printf(2, "ui ticks %u", n);
        if (g_isr_hits != last) {
            last = g_isr_hits;
            lcd_printf(1, "hits %u", last);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void ex02_step02_start(void)
{

    (void)gpio_install_isr_service(0);
    (void)gpio_set_intr_type(BOOT_GPIO, GPIO_INTR_NEGEDGE);
    (void)gpio_isr_handler_add(BOOT_GPIO, boot_isr, NULL);

    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 4, NULL);
}
