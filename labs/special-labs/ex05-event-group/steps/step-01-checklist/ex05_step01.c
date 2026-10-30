/*
 * S5 / step-01 - checklist face: show BOOT edge latch + STILL flag.
 *
 * Details: README.md
 */

#include "ex05_step01.h"
#include "bsp.h"
#include "imu_sim.h"
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

void ex05_step01_start(void)
{
    float acc[3];
    float gyro[3];
    float t = 0.0f;
    uint32_t rng = 1u;
    uint32_t boot_was = 0;
    int boot_latched = 0;
    int still;
    float horiz;

    (void)lcd_begin(3);
    lcd_printf(0, "S5 checklist");
    lcd_printf(1, "BOOT  -");
    lcd_printf(2, "STILL -");

    for (;;) {
        if (boot_edge(&boot_was)) {
            boot_latched = 1;
        }

        if (bsp_imu_read(acc, gyro) != 0) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += SAMPLE_DT_S;
        }
        horiz = sqrtf(acc[0] * acc[0] + acc[1] * acc[1]);
        still = (horiz < STILL_MPS2);

        lcd_printf(1, "BOOT  %s", boot_latched ? "yes" : "-");
        lcd_printf(2, "STILL %s", still ? "yes" : "-");
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
