/*
 * S5 / step-03 - WaitBits for BOOT and STILL together.
 *
 * Details: README.md
 */

#include "ex05_step03.h"
#include "bsp.h"
#include "imu_sim.h"
#include "lab_beep.h"
#include "lab_lcd.h"

#include <math.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/event_groups.h"
#include "freertos/task.h"

#define BOOT_BTN_MASK 2u
#define STILL_MPS2 1.2f
#define SAMPLE_DT_S 0.05f
#define BIT_BOOT  (1u << 0)
#define BIT_STILL (1u << 1)
#define BIT_ALL   (BIT_BOOT | BIT_STILL)

static EventGroupHandle_t g_bits;

static int boot_edge(uint32_t *was)
{
    uint32_t now = bsp_buttons() & BOOT_BTN_MASK;
    int edge = (now != 0u && *was == 0u) ? 1 : 0;
    *was = now;
    return edge;
}

static void sense_task(void *arg)
{
    float acc[3];
    float gyro[3];
    float t = 0.0f;
    uint32_t rng = 1u;
    uint32_t boot_was = 0;
    float horiz;

    (void)arg;
    for (;;) {
        if (boot_edge(&boot_was)) {
            (void)xEventGroupSetBits(g_bits, BIT_BOOT);
        }

        if (bsp_imu_read(acc, gyro) != 0) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += SAMPLE_DT_S;
        }
        horiz = sqrtf(acc[0] * acc[0] + acc[1] * acc[1]);
        if (horiz < STILL_MPS2) {
            (void)xEventGroupSetBits(g_bits, BIT_STILL);
        } else {
            (void)xEventGroupClearBits(g_bits, BIT_STILL);
        }
        vTaskDelay(pdMS_TO_TICKS(40));
    }
}

static void ui_task(void *arg)
{
    EventBits_t got;

    (void)arg;
    (void)lcd_begin(3);
    lcd_printf(0, "S5 WaitBits");
    lcd_printf(1, "need BOOT+STILL");
    lcd_printf(2, "WAITING");

    for (;;) {
        lcd_printf(2, "WAITING");
        got = xEventGroupWaitBits(g_bits, BIT_ALL, pdTRUE, pdTRUE, portMAX_DELAY);
        if ((got & BIT_ALL) == BIT_ALL) {
            lcd_printf(2, "ARMED");
            (void)beep_play(BEEP_CHIME, NULL);
            vTaskDelay(pdMS_TO_TICKS(800));
        }
    }
}

void ex05_step03_start(void)
{
    g_bits = xEventGroupCreate();
    if (g_bits == NULL) {
        return;
    }
    (void)xTaskCreate(sense_task, "sense", 4096, NULL, 5, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 4, NULL);
}
