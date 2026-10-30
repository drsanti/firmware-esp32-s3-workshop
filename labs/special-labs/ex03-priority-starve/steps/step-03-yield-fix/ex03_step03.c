/*
 * S3 / step-03 - hog yields (and runs below UI priority).
 *
 * Details: README.md
 */

#include "ex03_step03.h"
#include "bsp.h"
#include "lab_lcd.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

static volatile unsigned g_work;

static void hog_task(void *arg)
{
    unsigned n;

    (void)arg;
    for (;;) {
        /* Burst of work, then yield so UI can run. */
        for (n = 0; n < 10000u; n++) {
            g_work++;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

static void ui_task(void *arg)
{
    unsigned ticks = 0;

    (void)arg;
    (void)lcd_begin(3);
    lcd_printf(0, "S3 yield OK");
    lcd_printf(1, "ui 0");
    lcd_printf(2, "work 0");

    for (;;) {
        ticks++;
        lcd_printf(1, "ui %u", ticks);
        lcd_printf(2, "work %u", g_work);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void ex03_step03_start(void)
{
    /* UI higher than hog — Delay still required when hog is CPU-bound. */
    (void)xTaskCreate(hog_task, "hog", 2048, NULL, 3, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
