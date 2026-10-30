/*
 * S4 / step-01 - three-slot face, two polite workers (under the limit).
 *
 * Details: README.md
 */

#include "ex04_step01.h"
#include "bsp.h"
#include "lab_beep.h"
#include "lab_lcd.h"

#include <stdint.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define SLOTS 3u

static volatile unsigned g_in_use;

static void worker(void *arg)
{
    uint16_t hz = (uint16_t)(uintptr_t)arg;

    for (;;) {
        g_in_use++;
        lcd_printf(1, "in use %u/%u", g_in_use, SLOTS);
        (void)beep_play(BEEP_TONE, &(beep_params_t){.hz = hz, .ms = 100, .gain = 35});
        g_in_use--;
        lcd_printf(1, "in use %u/%u", g_in_use, SLOTS);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void ex04_step01_start(void)
{
    (void)lcd_begin(3);
    lcd_printf(0, "S4 3 slots");
    lcd_printf(1, "in use 0/%u", SLOTS);
    lcd_printf(2, "2 workers");

    (void)xTaskCreate(worker, "w0", 4096, (void *)(uintptr_t)660, 4, NULL);
    (void)xTaskCreate(worker, "w1", 4096, (void *)(uintptr_t)880, 4, NULL);
}
