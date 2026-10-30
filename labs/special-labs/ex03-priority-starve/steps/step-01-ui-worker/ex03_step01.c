/*
 * S3 / step-01 - UI counter + polite worker (both stay alive).
 *
 * Details: README.md
 */

#include "ex03_step01.h"
#include "bsp.h"
#include "lab_lcd.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

static volatile unsigned g_work;

static void worker_task(void *arg)
{
    (void)arg;
    for (;;) {
        g_work++;
        vTaskDelay(pdMS_TO_TICKS(50)); /* polite */
    }
}

static void ui_task(void *arg)
{
    unsigned ticks = 0;

    (void)arg;
    (void)lcd_begin(3);
    lcd_printf(0, "S3 UI+worker");
    lcd_printf(1, "ui 0");
    lcd_printf(2, "work 0");

    for (;;) {
        ticks++;
        lcd_printf(1, "ui %u", ticks);
        lcd_printf(2, "work %u", g_work);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void ex03_step01_start(void)
{
    (void)xTaskCreate(worker_task, "worker", 2048, NULL, 3, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 4, NULL);
}
