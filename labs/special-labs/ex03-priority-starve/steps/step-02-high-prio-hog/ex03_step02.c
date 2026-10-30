/*
 * S3 / step-02 - high-priority busy hog (broken).
 *
 *   hog prio 6, ui prio 4 — hog never blocks → UI freezes.
 *
 * Details: README.md
 */

#include "ex03_step02.h"
#include "bsp.h"
#include "lab_lcd.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

static volatile unsigned g_work;

static void hog_task(void *arg)
{
    (void)arg;
    for (;;) {
        g_work++;
        /* BROKEN: no yield — higher prio than UI steals the core forever. */
    }
}

static void ui_task(void *arg)
{
    unsigned ticks = 0;

    (void)arg;
    (void)lcd_begin(3);
    lcd_printf(0, "S3 HOG BAD");
    lcd_printf(1, "ui 0");
    lcd_printf(2, "work 0");

    for (;;) {
        ticks++;
        lcd_printf(1, "ui %u", ticks);
        lcd_printf(2, "work %u", g_work);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void ex03_step02_start(void)
{
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 4, NULL);
    (void)xTaskCreate(hog_task, "hog", 2048, NULL, 6, NULL);
}
