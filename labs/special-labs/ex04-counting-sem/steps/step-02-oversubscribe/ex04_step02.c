/*
 * S4 / step-02 - five workers, no pool limit (broken).
 *
 * Details: README.md
 */

#include "ex04_step02.h"
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
        (void)beep_play(BEEP_TONE, &(beep_params_t){.hz = hz, .ms = 150, .gain = 45});
        g_in_use--;
        vTaskDelay(pdMS_TO_TICKS(80));
    }
}

void ex04_step02_start(void)
{
    static const uint16_t hz[] = {440, 550, 660, 770, 880};
    unsigned i;

    (void)lcd_begin(3);
    lcd_printf(0, "S4 OVERSUB");
    lcd_printf(1, "in use 0/%u", SLOTS);
    lcd_printf(2, "5 workers");

    for (i = 0; i < 5u; i++) {
        (void)xTaskCreate(worker, "w", 4096, (void *)(uintptr_t)hz[i], 4, NULL);
    }
}
