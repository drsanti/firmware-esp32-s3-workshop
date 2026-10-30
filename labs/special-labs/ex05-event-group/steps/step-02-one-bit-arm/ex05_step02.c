/*
 * S5 / step-02 - arm on BOOT only (broken / false arm while moving).
 *
 * Details: README.md
 */

#include "ex05_step02.h"
#include "bsp.h"
#include "imu_sim.h"
#include "lab_beep.h"
#include "lab_lcd.h"

#include <math.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define BOOT_BTN_MASK 2u
#define STILL_MPS2 1.2f
#define SAMPLE_DT_S 0.05f

static int boot_edge(uint32_t *was)
{
    uint32_t now = bsp_buttons() & BOOT_BTN_MASK;
    int edge = (now != 0u && *was == 0u) ? 1 : 0;
    *was = now;
    return edge;
}

void ex05_step02_start(void)
{
    float acc[3];
    float gyro[3];
    float t = 0.0f;
    uint32_t rng = 1u;
    uint32_t boot_was = 0;
    int still;
    float horiz;
    int armed = 0;

    (void)lcd_begin(3);
    lcd_printf(0, "S5 BOOT only");
    lcd_printf(1, "STILL -");
    lcd_printf(2, "WAITING");

    for (;;) {
        if (bsp_imu_read(acc, gyro) != 0) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += SAMPLE_DT_S;
        }
        horiz = sqrtf(acc[0] * acc[0] + acc[1] * acc[1]);
        still = (horiz < STILL_MPS2);
        lcd_printf(1, "STILL %s", still ? "yes" : "NO");

        /* BROKEN: ignore STILL — arm on BOOT alone. */
        if (!armed && boot_edge(&boot_was)) {
            armed = 1;
            lcd_printf(2, "ARMED?");
            (void)beep_play(BEEP_CHIME, NULL);
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
