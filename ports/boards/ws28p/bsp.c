/*
 * board.c — Waveshare ESP32-S3-Touch-LCD-2.8 (SKU 27690) board glue:
 * shared I2C bus + dedicated touch I2C bus. This board has no
 * WS2812 LED and no battery ADC — the corresponding bsp.h hooks
 * are graceful stubs.
 */
#include <string.h>

#include "bsp_pins.h"
#include "../../bsp.h"
#include "../../esp32s3_bsp.h"

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include <stdio.h>

/* Shared I2C bus: QMI8658 IMU (0x6B) + PCF85063 RTC (0x51).
 * Lazy init so ports that never touch the bus don't pay for it. */
static i2c_master_bus_handle_t s_i2c0;

i2c_master_bus_handle_t esp32s3_i2c0(void) {
    if (s_i2c0 == NULL) {
        i2c_master_bus_config_t cfg = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = BOARD_I2C0_SDA,
            .scl_io_num = BOARD_I2C0_SCL,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .flags.enable_internal_pullup = 1,
        };
        if (i2c_new_master_bus(&cfg, &s_i2c0) != ESP_OK) {
            return NULL;
        }
    }
    return s_i2c0;
}

/* Touch controller (CST3530 @0x58 V2 / CST328 @0x1A V1) sits on its
 * own bus — separate
 * from the shared peripheral bus so display traffic and IMU reads
 * never contend with touch polls. */
static i2c_master_bus_handle_t s_i2c_tp;

i2c_master_bus_handle_t ws28p_tp_i2c(void) {
    if (s_i2c_tp == NULL) {
        i2c_master_bus_config_t cfg = {
            .i2c_port = BOARD_TP_I2C_PORT,
            .sda_io_num = BOARD_TP_SDA,
            .scl_io_num = BOARD_TP_SCL,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .flags.enable_internal_pullup = 1,
        };
        if (i2c_new_master_bus(&cfg, &s_i2c_tp) != ESP_OK) {
            return NULL;
        }
    }
    return s_i2c_tp;
}

void bsp_led_set(int on) { (void)on; /* no user LED on this board */ }
void bsp_led_rgb(uint8_t r, uint8_t g, uint8_t b) {
    (void)r; (void)g; (void)b;
}

const char *bsp_hw_model(void) { return BOARD_SKU; }

uint32_t bsp_caps(void) {
    return BOARD_CAP_IMU|BOARD_CAP_RTC|BOARD_CAP_BATT|BOARD_CAP_BTN;
}

/* Buttons — PWR = GPIO6 (active low, doubles as battery power
 * control), BOOT = GPIO0 (strap pin; readable at runtime). */
#define WS28P_BTN_PWR  6
#define WS28P_PWR_HOLD 7  /* SYS_EN — output high keeps battery on */
#define WS28P_BTN_BOOT 0

uint32_t bsp_buttons(void) {
    uint32_t m = 0;
    if (!gpio_get_level(WS28P_BTN_PWR)) {
        m |= 1;
    }
    if (!gpio_get_level(WS28P_BTN_BOOT)) {
        m |= 2;
    }
    return m;
}

int bsp_pwr_hold(int en) {
    gpio_set_level(WS28P_PWR_HOLD, en ? 1 : 0);
    return 0;
}

void bsp_misc_init(void) {
    /* Power latch FIRST — on battery a released PWR button cuts the
     * rail unless SYS_EN is high; the vendor demo latches the same
     * way. Harmless on USB power. */
    gpio_config_t io = {
        .pin_bit_mask = (1ull << WS28P_PWR_HOLD) |
                        (1ull << WS28P_BTN_PWR) |
                        (1ull << WS28P_BTN_BOOT),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 1,
    };
    gpio_config(&io);
    gpio_set_direction(WS28P_PWR_HOLD, GPIO_MODE_OUTPUT);
    gpio_set_level(WS28P_PWR_HOLD, 1);

    /* Force the lazy resources up while only the main task runs —
     * bsp_imu_read/bsp_touch_read are then race-free when both
     * the app and ui tasks call them later. */
    esp32s3_i2c0();
    ws28p_tp_i2c();
    float acc[3], gyr[3];
    bsp_imu_read(acc, gyr);
    bsp_touch_sample_t smp = {0};
    bsp_touch_read(&smp);
    bsp_datetime_t dt; /* dev handle creation is single-task-safe
                          * here; the app seeds wall time later */
    bsp_rtc_read(&dt);
}
