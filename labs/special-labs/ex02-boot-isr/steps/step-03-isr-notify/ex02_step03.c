/*
 * S2 / step-03 - BOOT ISR only notifies; task does LCD + beep.
 *
 * Details: README.md
 */

#include "ex02_step03.h"
#include "bsp.h"
#include "lab_beep.h"
#include "lab_lcd.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define BOOT_GPIO 0

static TaskHandle_t g_ui;
static TaskHandle_t g_audio;

static void IRAM_ATTR boot_isr(void *arg)
{
    BaseType_t hp = pdFALSE;

    (void)arg;
    if (g_ui != NULL) {
        vTaskNotifyGiveFromISR(g_ui, &hp);
        if (hp == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    }
}

static void audio_task(void *arg)
{
    (void)arg;
    for (;;) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        (void)beep_play(BEEP_CLICK, NULL);
    }
}

static void ui_task(void *arg)
{
    unsigned presses = 0;
    TickType_t last = 0;
    TickType_t now;

    (void)arg;
    (void)lcd_begin(3);
    lcd_printf(0, "S2 ISR notify");
    lcd_printf(1, "presses 0");
    lcd_printf(2, "debounce task");

    for (;;) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        now = xTaskGetTickCount();
        /* Simple debounce in task (not in ISR). */
        if ((now - last) < pdMS_TO_TICKS(200)) {
            continue;
        }
        last = now;
        presses++;
        lcd_printf(1, "presses %u", presses);
        if (g_audio != NULL) {
            (void)xTaskNotifyGive(g_audio);
        }
    }
}

void ex02_step03_start(void)
{

    (void)xTaskCreate(audio_task, "audio", 4096, NULL, 5, &g_audio);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 4, &g_ui);

    (void)gpio_install_isr_service(0);
    (void)gpio_set_intr_type(BOOT_GPIO, GPIO_INTR_NEGEDGE);
    (void)gpio_isr_handler_add(BOOT_GPIO, boot_isr, NULL);
}
