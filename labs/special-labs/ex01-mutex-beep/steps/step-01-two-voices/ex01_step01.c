/*
 * S1 / step-01 - two voice tasks, staggered (no mutex yet).
 *
 * Details: README.md
 */

#include "ex01_step01.h"
#include "bsp.h"
#include "lab_beep.h"
#include "lab_lcd.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

static void hi_task(void *arg)
{
    (void)arg;
    for (;;) {
        lcd_printf(1, "hi playing");
        (void)beep_play(BEEP_TONE, &(beep_params_t){.hz = 880, .ms = 120, .gain = 40});
        lcd_printf(1, "hi idle");
        vTaskDelay(pdMS_TO_TICKS(700));
    }
}

static void lo_task(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(350)); /* stagger so tones rarely overlap */
    for (;;) {
        lcd_printf(2, "lo playing");
        (void)beep_play(BEEP_TONE, &(beep_params_t){.hz = 440, .ms = 120, .gain = 40});
        lcd_printf(2, "lo idle");
        vTaskDelay(pdMS_TO_TICKS(700));
    }
}

void ex01_step01_start(void)
{
    (void)lcd_begin(3);
    lcd_printf(0, "S1 two voices");
    lcd_printf(1, "hi idle");
    lcd_printf(2, "lo idle");

    (void)xTaskCreate(hi_task, "hi", 4096, NULL, 4, NULL);
    (void)xTaskCreate(lo_task, "lo", 4096, NULL, 4, NULL);
}
