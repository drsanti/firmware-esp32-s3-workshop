/*
 * S4 / step-03 - counting semaphore pool (max 3).
 *
 * Details: README.md
 */

#include "ex04_step03.h"
#include "bsp.h"
#include "lab_beep.h"
#include "lab_lcd.h"

#include <stdint.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/semphr.h"
#include "freertos/task.h"

#define SLOTS 3u

static SemaphoreHandle_t g_slots;
static volatile unsigned g_in_use;

static void refresh(void)
{
    UBaseType_t free_n = uxSemaphoreGetCount(g_slots);
    lcd_printf(1, "in use %u/%u", g_in_use, SLOTS);
    lcd_printf(2, "free %u", (unsigned)free_n);
}

static void worker(void *arg)
{
    uint16_t hz = (uint16_t)(uintptr_t)arg;

    for (;;) {
        if (xSemaphoreTake(g_slots, portMAX_DELAY) == pdTRUE) {
            g_in_use++;
            refresh();
            (void)beep_play(BEEP_TONE, &(beep_params_t){.hz = hz, .ms = 150, .gain = 45});
            g_in_use--;
            (void)xSemaphoreGive(g_slots);
            refresh();
        }
        vTaskDelay(pdMS_TO_TICKS(80));
    }
}

void ex04_step03_start(void)
{
    static const uint16_t hz[] = {440, 550, 660, 770, 880};
    unsigned i;

    g_slots = xSemaphoreCreateCounting(SLOTS, SLOTS);
    if (g_slots == NULL) {
        return;
    }

    (void)lcd_begin(3);
    lcd_printf(0, "S4 sem pool");
    refresh();

    for (i = 0; i < 5u; i++) {
        (void)xTaskCreate(worker, "w", 4096, (void *)(uintptr_t)hz[i], 4, NULL);
    }
}
