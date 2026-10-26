/*
 * ports/boards/ws28p/cst3xx.c — Waveshare ESP32-S3-Touch-LCD-2.8
 * (SKU 27690) touch device (bsp_touch_read impl).
 *
 * The board ships in two revisions:
 *   V1: CST328  @ 0x1A — 16-bit register protocol
 *   V2: CST3530 @ 0x58 — 32-bit command protocol (this unit)
 * First bsp_touch_read probes both addresses and latches the
 * responder; the two paths below each follow Waveshare's official
 * esp_lcd_touch_* components (Apache-2.0).
 *
 *   CST328  : rd 1B @0xD005 -> count nibble; rd 27B @0xD000;
 *             wr 0 @0xD005 -> report consumed
 *   CST3530 : wr cmd 0xD0070000 + rd 32B;
 *             buf[2]==0xFF -> report; buf[3] = keycnt<<4|fingers;
 *             chksum = sum(0x55 + pts bytes) LE in buf[0..1];
 *             always wr cmd 0xD00002AB after a read
 *
 * Raw coords are panel-native portrait (0..239 x, 0..319 y); the
 * display runs MADCTL MY|MV landscape, so logical x = 319 - ry,
 * y = rx — identical transform to the ES3C28P's FT6336 map.
 */
#ifdef ESP_PLATFORM

#include "../../bsp.h"
#include "../../esp32s3_bsp.h"
#include "bsp_pins.h"

#include <string.h>

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* dedicated touch bus — provided by board.c */

#define CST328_ADDR      0x1A
#define CST328_REG_CNT   0xD005 /* 1B report count (write 0 = ack) */
#define CST328_REG_XY    0xD000 /* 27B point report              */
#define CST3530_ADDR     0x58
#define CST3530_CMD_READ  0xD0070000u
#define CST3530_CMD_CLEAR 0xD00002ABu
#define CST_PTS_MAX      5

static i2c_master_dev_handle_t s_touch;
static int s_chip; /* 0 unknown, 1 = CST328, 2 = CST3530 */

/* ---------------- CST3530 (V2) ---------------- */

static void cst3530_cmd_buf(uint32_t cmd, uint8_t b[4]) {
    b[0] = (uint8_t)(cmd >> 24);
    b[1] = (uint8_t)(cmd >> 16);
    b[2] = (uint8_t)(cmd >> 8);
    b[3] = (uint8_t)cmd;
}

static int cst3530_poll(bsp_touch_sample_t *s) {
    uint8_t cmd[4];
    cst3530_cmd_buf(CST3530_CMD_READ, cmd);
    uint8_t r[32];
    int rc = i2c_master_transmit_receive(s_touch, cmd, 4, r, sizeof r, 10);
    cst3530_cmd_buf(CST3530_CMD_CLEAR, cmd);
    i2c_master_transmit(s_touch, cmd, 4, 10); /* always clear */
    if (rc != ESP_OK) {
        return -1; /* bus fault — not "no fingers" */
    }
    if (r[2] != 0xFF) {
        return 0;
    }
    uint8_t cnt = r[3] & 0x0F;
    uint8_t keys = r[3] >> 4;
    if (cnt == 0 || cnt > CST_PTS_MAX) {
        return 0;
    }
    if ((keys + cnt) * 5 > 28) {
        return 0; /* corrupt report — records would read past r[32] */
    }
    uint16_t sum = 0x55;
    for (uint8_t i = 0; i < (keys + cnt) * 5; i++) {
        sum += r[4 + i];
    }
    if (sum != ((uint16_t)r[0] | ((uint16_t)r[1] << 8))) {
        return 0; /* corrupt frame — drop */
    }
    for (uint8_t i = 0; i < cnt && s->n < RB_TOUCH_MAX_POINTS; i++) {
        size_t o = (keys + i) * 5;
        if ((r[o + 8] >> 4) == 0) {
            continue; /* event nibble 0 = no active contact */
        }
        uint16_t rx = (uint16_t)r[o + 4] | ((uint16_t)(r[o + 7] & 0x0F) << 8);
        uint16_t ry = (uint16_t)r[o + 5] | ((uint16_t)(r[o + 7] & 0xF0) << 4);
        if (rx > 239) rx = 239;
        if (ry > 319) ry = 319;
        s->pts[s->n].x = (int16_t)(319 - ry);
        s->pts[s->n].y = (int16_t)rx;
        s->pts[s->n].event = 2; /* contact — the edge detector
                                 * derives DOWN/UP from presence */
        s->n++;
    }
    return 0;
}

/* ---------------- CST328 (V1) ---------------- */

static int cst328_read(uint16_t reg, uint8_t *data, size_t n) {
    uint8_t rb[2] = {(uint8_t)(reg >> 8), (uint8_t)reg};
    return i2c_master_transmit_receive(s_touch, rb, 2, data, n, 10)
               == ESP_OK ? 0 : -1;
}

static int cst328_write(uint16_t reg, uint8_t v) {
    uint8_t wb[3] = {(uint8_t)(reg >> 8), (uint8_t)reg, v};
    return i2c_master_transmit(s_touch, wb, 3, 10) == ESP_OK ? 0 : -1;
}

static int cst328_poll(bsp_touch_sample_t *s) {
    uint8_t cnt;
    if (cst328_read(CST328_REG_CNT, &cnt, 1) != 0) {
        return -1;
    }
    cnt &= 0x0F;
    if (cnt == 0 || cnt > CST_PTS_MAX) {
        if (cnt) {
            cst328_write(CST328_REG_CNT, 0); /* garbage — ack + drop */
        }
        return 0;
    }
    uint8_t r[27];
    if (cst328_read(CST328_REG_XY, r, sizeof r) != 0) {
        return -1;
    }
    cst328_write(CST328_REG_CNT, 0); /* report consumed */
    s->n = cnt > RB_TOUCH_MAX_POINTS ? RB_TOUCH_MAX_POINTS : cnt;
    for (uint8_t i = 0; i < s->n; i++) {
        /* Waveshare's decode: point 0's 5-byte record sits at
         * report offset 1; later points follow at a 5-byte stride
         * from offset 8 (the count+sync bytes break the uniform
         * layout). */
        size_t o = i == 0 ? 1 : 8 + (i - 1) * 5;
        uint16_t rx = ((uint16_t)r[o] << 4) | ((r[o + 2] & 0xF0) >> 4);
        uint16_t ry = ((uint16_t)r[o + 1] << 4) | (r[o + 2] & 0x0F);
        if (rx > 239) rx = 239;
        if (ry > 319) ry = 319;
        s->pts[i].x = (int16_t)(319 - ry);
        s->pts[i].y = (int16_t)rx;
        s->pts[i].event = 2;
    }
    return 0;
}

/* ---------------- shared ---------------- */

static void tp_reset(void) {
    gpio_config_t io = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ull << BOARD_TP_RST,
    };
    gpio_config(&io);
    gpio_set_level(BOARD_TP_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(BOARD_TP_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(50)); /* >=50ms to first report */
}

static int touch_probe(uint16_t addr, int chip) {
    if (ws28p_tp_i2c() == NULL ||
        i2c_master_probe(ws28p_tp_i2c(), addr, 20) != ESP_OK) {
        return -1;
    }
    i2c_device_config_t dev = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = 400000,
    };
    if (i2c_master_bus_add_device(ws28p_tp_i2c(), &dev, &s_touch) !=
        ESP_OK) {
        return -1;
    }
    s_chip = chip;
    return 0;
}

int bsp_touch_read(bsp_touch_sample_t *s) {
    if (s == NULL) {
        return -1;
    }
    s->n = 0;
    s->gesture = BOARD_GEST_NONE;
    if (s_touch == NULL) {
        tp_reset();
        /* V2 first (0x58), then V1 (0x1A) */
        if (touch_probe(CST3530_ADDR, 2) != 0 &&
            touch_probe(CST328_ADDR, 1) != 0) {
            return -1; /* no touch hw */
        }
    }
    return s_chip == 2 ? cst3530_poll(s) : cst328_poll(s);
}

#endif /* ESP_PLATFORM */
