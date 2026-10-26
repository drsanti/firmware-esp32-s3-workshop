/*
 * ft6336.c — ES3C28P capacitive touch (FT6336, 2-point).
 * Polls the shared I2C bus (addr 0x38); maps the chip's native
 * portrait space (X 0..239, Y 0..319) onto our 320x240 landscape
 * display: out_x = 319 - raw_y, out_y = raw_x  (rotation 90,
 * calibration-verified mapping from the byte-flow adapter).
 *
 * Single 12-byte read from reg 0x01 covers gesture + point count +
 * both contact points; P2 lives at regs 0x09..0x0C.
 */
#ifdef ESP_PLATFORM

#include "../../bsp.h"
#include "../../esp32s3_bsp.h"
#include "esp_err.h"

#define FT6336_ADDR      0x38
#define FT6336_REG_BASE  0x01 /* gesture id reg; td_status + P1 + P2
                                 follow at 0x02..0x0C */
#define FT6336_PTS_MAX   2

/* FT6336 chip gesture ids -> normalized BOARD_GEST_* */
static uint8_t map_gesture(uint8_t g) {
    switch (g) {
    case 0x10: return BOARD_GEST_UP;
    case 0x14: return BOARD_GEST_DOWN;
    case 0x18: return BOARD_GEST_LEFT;
    case 0x1C: return BOARD_GEST_RIGHT;
    case 0x20: return BOARD_GEST_CLICK;
    case 0x22: return BOARD_GEST_DBL;
    case 0x48: return BOARD_GEST_ZIN;
    case 0x49: return BOARD_GEST_ZOUT;
    default:   return BOARD_GEST_NONE;
    }
}

static i2c_master_dev_handle_t s_touch;

int bsp_touch_read(bsp_touch_sample_t *s) {
    if (s == NULL) {
        return -1;
    }
    s->n = 0;
    s->gesture = BOARD_GEST_NONE;
    if (s_touch == NULL) {
        i2c_device_config_t dev = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = FT6336_ADDR,
            .scl_speed_hz = 100000,
        };
        if (esp32s3_i2c0() == NULL ||
            i2c_master_bus_add_device(esp32s3_i2c0(), &dev, &s_touch) !=
                ESP_OK) {
            return -1; /* no touch hw */
        }
    }
    uint8_t reg = FT6336_REG_BASE;
    uint8_t buf[12]; /* 0x01..0x0C: gest, td, P1 XH XL YH YL w m,
                        P2 XH XL YH YL */
    if (i2c_master_transmit_receive(s_touch, &reg, 1, buf, sizeof buf,
                                    10) != ESP_OK /* ~1.3ms transfer;
                                    keep the executor stall under
                                    one poll period if the bus wedges */) {
        return 0;
    }
    s->gesture = map_gesture(buf[0]);
    uint8_t n = buf[1] & 0x0F;
    if (n > FT6336_PTS_MAX) {
        return 0; /* garbage nibble — drop the sample; stale registers
                     * would fabricate phantom contacts */
    }
    s->n = n;
    for (uint8_t i = 0; i < n; i++) {
        const uint8_t *p = &buf[2 + i * 6];
        uint16_t rx = ((p[0] & 0x0F) << 8) | p[1];
        uint16_t ry = ((p[2] & 0x0F) << 8) | p[3];
        /* 12-bit raw can exceed the panel at the glass edges — clamp
         * before the rotation map or out_x goes negative */
        if (rx > 239) rx = 239;
        if (ry > 319) ry = 319;
        s->pts[i].x = (int16_t)(319 - ry);
        s->pts[i].y = (int16_t)rx;
        s->pts[i].event = (p[0] >> 6) & 0x03; /* 0 down 1 up 2 contact */
    }
    return 0;
}

#endif /* ESP_PLATFORM */
