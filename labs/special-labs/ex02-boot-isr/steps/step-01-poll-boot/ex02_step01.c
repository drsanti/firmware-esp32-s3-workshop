/*
 * S2 / step-01 - poll BOOT (baseline, no GPIO ISR).
 *
 * Details: README.md
 */

#include "ex02_step01.h"
#include "bsp.h"
#include "lab_beep.h"
#include "lab_lcd.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define BOOT_BTN_MASK 2u

static TaskHandle_t g_audio;
static unsigned g_presses;

static int boot_edge(uint32_t *was)
{
    uint32_t now = bsp_buttons() & BOOT_BTN_MASK;
    int edge = (now != 0u && *was == 0u) ? 1 : 0;
    *was = now;
    return edge;
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
    uint32_t was = 0;

    (void)arg;
    (void)lcd_begin(3);
    lcd_printf(0, "S2 poll BOOT");
    lcd_printf(1, "presses 0");
    lcd_printf(2, "poll path");

    for (;;) {
        if (boot_edge(&was)) {
            g_presses++;
            lcd_printf(1, "presses %u", g_presses);
            if (g_audio != NULL) {
                (void)xTaskNotifyGive(g_audio);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void ex02_step01_start(void)
{
    (void)xTaskCreate(audio_task, "audio", 4096, NULL, 5, &g_audio);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 4, NULL);
}
