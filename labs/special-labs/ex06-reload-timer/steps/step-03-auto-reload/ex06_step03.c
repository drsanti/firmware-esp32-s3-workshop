/*
 * S6 / step-03 - auto-reload software timer drives the beat.
 *
 * Details: README.md
 */

#include "ex06_step03.h"
#include "bsp.h"
#include "lab_lcd.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"
#include "freertos/timers.h"

#define BOOT_BTN_MASK 2u
#define BEAT_MS 500u

static TaskHandle_t g_ui;

static int boot_edge(uint32_t *was)
{
    uint32_t now = bsp_buttons() & BOOT_BTN_MASK;
    int edge = (now != 0u && *was == 0u) ? 1 : 0;
    *was = now;
    return edge;
}

static void beat_cb(TimerHandle_t t)
{
    (void)t;
    if (g_ui != NULL) {
        (void)xTaskNotifyGive(g_ui);
    }
}

static void ui_task(void *arg)
{
    uint32_t was = 0;
    unsigned boots = 0;
    unsigned beats = 0;

    (void)arg;
    (void)lcd_begin(3);
    lcd_printf(0, "S6 auto-reload");
    lcd_printf(1, "beat idle");
    lcd_printf(2, "boots 0");

    for (;;) {
        if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(20)) > 0u) {
            beats++;
            lcd_printf(1, "* BEAT %u *", beats);
            vTaskDelay(pdMS_TO_TICKS(60));
            lcd_printf(1, "beat idle");
        }
        if (boot_edge(&was)) {
            boots++;
            lcd_printf(2, "boots %u", boots);
        }
    }
}

void ex06_step03_start(void)
{
    TimerHandle_t beat;

    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, &g_ui);

    beat = xTimerCreate("beat", pdMS_TO_TICKS(BEAT_MS), pdTRUE, NULL, beat_cb);
    if (beat != NULL) {
        (void)xTimerStart(beat, 0);
    }
}
