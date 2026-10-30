/*
 * S1 / step-03 - mutex guards beep_play (fix).
 *
 * Details: README.md
 */

#include "ex01_step03.h"
#include "bsp.h"
#include "lab_beep.h"
#include "lab_lcd.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/semphr.h"
#include "freertos/task.h"

static SemaphoreHandle_t g_beep_mu;

static void play_locked(unsigned row, const char *tag, uint16_t hz)
{
    if (xSemaphoreTake(g_beep_mu, portMAX_DELAY) == pdTRUE) {
        lcd_printf(row, "%s PLAY", tag);
        (void)beep_play(BEEP_TONE, &(beep_params_t){.hz = hz, .ms = 180, .gain = 50});
        lcd_printf(row, "%s idle", tag);
        (void)xSemaphoreGive(g_beep_mu);
    }
}

static void hi_task(void *arg)
{
    (void)arg;
    for (;;) {
        play_locked(1, "hi", 880);
        vTaskDelay(pdMS_TO_TICKS(120));
    }
}

static void lo_task(void *arg)
{
    (void)arg;
    for (;;) {
        play_locked(2, "lo", 440);
        vTaskDelay(pdMS_TO_TICKS(120));
    }
}

void ex01_step03_start(void)
{
    g_beep_mu = xSemaphoreCreateMutex();
    if (g_beep_mu == NULL) {
        return;
    }

    (void)lcd_begin(3);
    lcd_printf(0, "S1 mutex OK");
    lcd_printf(1, "hi idle");
    lcd_printf(2, "lo idle");

    (void)xTaskCreate(hi_task, "hi", 4096, NULL, 4, NULL);
    (void)xTaskCreate(lo_task, "lo", 4096, NULL, 4, NULL);
}
