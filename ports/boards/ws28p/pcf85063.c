/*
 * pcf85063.c — PCF85063 battery-backed RTC @ 0x51 on the ws28p's
 * shared I2C bus. Time regs 0x04-0x0A are packed BCD; year is
 * offset 1970 (so 2026 = 56). Register map verified against the
 * vendor demo PCF85063_Driver.
 */
#include "../../bsp.h"
#include "../../esp32s3_bsp.h"
#include <string.h>
#ifdef ESP_PLATFORM

#include "driver/i2c_master.h"
#include "esp_err.h"

#define PCF85063_ADDR      0x51
#define PCF85063_REG_TIME  0x04  /* sec..year, 7 BCD bytes */
#define PCF85063_REG_CTRL1 0x00

static i2c_master_dev_handle_t s_rtc;

static uint8_t to_bcd(int v) { return (uint8_t)((v / 10) * 16 + v % 10); }
static int     fr_bcd(uint8_t v) { return (v >> 4) * 10 + (v & 0x0F); }

static int pcf_init(void) {
    i2c_device_config_t dev = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = PCF85063_ADDR,
        .scl_speed_hz = 400000,
    };
    if (esp32s3_i2c0() == NULL ||
        i2c_master_bus_add_device(esp32s3_i2c0(), &dev, &s_rtc) !=
            ESP_OK) {
        return -1;
    }
    return 0;
}

int bsp_rtc_read(bsp_datetime_t *t) {
    if (s_rtc == NULL && pcf_init() != 0) {
        return -1;
    }
    uint8_t r[7];
    uint8_t reg = PCF85063_REG_TIME;
    if (i2c_master_transmit_receive(s_rtc, &reg, 1, r, sizeof r, 10)
            != ESP_OK) {
        return -1;
    }
    t->second = fr_bcd(r[0] & 0x7F); /* bit7 = oscillator-stop flag */
    t->minute = fr_bcd(r[1] & 0x7F);
    t->hour   = fr_bcd(r[2] & 0x3F); /* 24H mode */
    t->day    = fr_bcd(r[3] & 0x3F);
    t->wday   = fr_bcd(r[4] & 0x07);
    t->month  = fr_bcd(r[5] & 0x1F);
    t->year   = fr_bcd(r[6]) + 1970;
    /* bit7 of the seconds reg is the oscillator-stop flag — factory
     * chips ship with it set until first write. Content may still
     * decode; let the caller's sanity floor judge rather than hide
     * the value entirely */
    return 0;
}

int bsp_rtc_set(const bsp_datetime_t *t) {
    if (s_rtc == NULL && pcf_init() != 0) {
        return -1;
    }
    uint8_t buf[8] = {PCF85063_REG_TIME,
                      to_bcd(t->second), to_bcd(t->minute),
                      to_bcd(t->hour), to_bcd(t->day), to_bcd(t->wday),
                      to_bcd(t->month), to_bcd(t->year - 1970)};
    return i2c_master_transmit(s_rtc, buf, sizeof buf, 10) == ESP_OK
               ? 0
               : -1;
}

#endif /* ESP_PLATFORM */
