/*
 * S1 / step-02 - same two voices, aggressive overlap (broken).
 *
 *   Both call beep_play with no lock → garbled / stepped-on audio.
 *
 * Details: README.md
 */

#include "ex01_step02.h"
#include "bsp.h"
#include "lab_beep.h"
#include "lab_lcd.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

static void hi_task(void *arg)
{
    (void)arg;
    for (;;) {
        lcd_printf(1, "hi PLAY");
        (void)beep_play(BEEP_TONE, &(beep_params_t){.hz = 880, .ms = 180, .gain = 50});
        lcd_printf(1, "hi idle");
        vTaskDelay(pdMS_TO_TICKS(120));
    }
}

static void lo_task(void *arg)
{
    (void)arg;
    for (;;) {
        lcd_printf(2, "lo PLAY");
        (void)beep_play(BEEP_TONE, &(beep_params_t){.hz = 440, .ms = 180, .gain = 50});
        lcd_printf(2, "lo idle");
        vTaskDelay(pdMS_TO_TICKS(120));
    }
}

void ex01_step02_start(void)
{
    (void)lcd_begin(3);
    lcd_printf(0, "S1 OVERLAP");
    lcd_printf(1, "hi idle");
    lcd_printf(2, "lo idle");

    (void)xTaskCreate(hi_task, "hi", 4096, NULL, 4, NULL);
    (void)xTaskCreate(lo_task, "lo", 4096, NULL, 4, NULL);
}
