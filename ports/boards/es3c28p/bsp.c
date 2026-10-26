/*
 * board.c — ES3C28P board glue: shared I2C bus + WS2812 LED.
 * Extracted from ports/esp_idf.c when the firmware went two-board;
 * chip-generic code stayed there, board-specific lives here.
 */
#include <string.h>

#include "bsp_pins.h"
#include "../../bsp.h"
#include "../../esp32s3_bsp.h"

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_encoder.h"

/* Shared I2C bus: ES8311 codec + FT6336 touch. Lazy init so ports
 * that never touch the bus don't pay for it. */
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

/* ES3C28P onboard RGB LED: WS2812-compatible, BOARD_LED_GPIO.
 * RMT-driven (not bit-banged): hardware generates the 400/800ns
 * pulses, so timing is immune to ISRs, cache stalls and other
 * tasks — bit-banging flickered whenever SPI DMA was busy. */
#define RMT_RES_HZ 40000000u /* 25 ns/tick */

static rmt_channel_handle_t s_led_rmt;
static rmt_encoder_handle_t s_led_enc;
static const rmt_transmit_config_t s_led_tx = {
    .loop_count = 0,
    .flags.eot_level = 0, /* line idles low = the >50us reset */
};

static void ws2812_init(void) {
    rmt_tx_channel_config_t ch = {
        .gpio_num = BOARD_LED_GPIO,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = RMT_RES_HZ,
        .mem_block_symbols = 48, /* 1 block covers the 24-bit frame */
        .trans_queue_depth = 4,
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&ch, &s_led_rmt));
    /* WS2812: T0H 400ns/T0L 850ns, T1H 800ns/T1L 450ns, MSB first */
    rmt_bytes_encoder_config_t enc = {
        .bit0 = {.level0 = 1, .duration0 = 16,  /* 400 ns */
                 .level1 = 0, .duration1 = 34}, /* 850 ns */
        .bit1 = {.level0 = 1, .duration0 = 32,  /* 800 ns */
                 .level1 = 0, .duration1 = 18}, /* 450 ns */
        .flags.msb_first = 1,
    };
    ESP_ERROR_CHECK(rmt_new_bytes_encoder(&enc, &s_led_enc));
    ESP_ERROR_CHECK(rmt_enable(s_led_rmt));
}

static void ws2812_write_grb(uint32_t grb) {
    uint8_t buf[3] = {(uint8_t)(grb >> 16), (uint8_t)(grb >> 8),
                      (uint8_t)grb};
    /* queued — ISR streams it; line idles low afterwards = reset */
    rmt_transmit(s_led_rmt, s_led_enc, buf, sizeof(buf),
                 &s_led_tx);
}

void bsp_led_set(int on) {
    /* dim green when "on", off otherwise (GRB order) */
    ws2812_write_grb(on ? 0x0A0000u : 0x000000u);
}

void bsp_led_rgb(uint8_t r, uint8_t g, uint8_t b) {
    /* scale down to keep the single LED from drawing hard on 3V3 rail */
    ws2812_write_grb(((uint32_t)g / 4 << 16) |
                     ((uint32_t)r / 4 << 8) |
                     ((uint32_t)b / 4));
}

int bsp_imu_read(float accel_mps2[3], float gyro_rads[3]) {
    (void)accel_mps2; (void)gyro_rads;
    return -1; /* ES3C28P has no IMU */
}

const char *bsp_hw_model(void) { return BOARD_SKU; }

uint32_t bsp_caps(void) {
    return BOARD_CAP_LED|BOARD_CAP_MIC|BOARD_CAP_BATT|BOARD_CAP_BTN;
}

/* BOOT button — GPIO0 strap pin, readable at runtime (active low).
 * Same ordinal as ws28p: bit1 = BOOT; this board has no PWR (bit0)
 * and no power-hold latch. */
uint32_t bsp_buttons(void) {
    return gpio_get_level(0) ? 0 : 2;
}
int bsp_pwr_hold(int en) { (void)en; return -1; }
int bsp_rtc_read(bsp_datetime_t *t) { (void)t; return -1; }
int bsp_rtc_set(const bsp_datetime_t *t) { (void)t; return -1; }

void bsp_misc_init(void) {
    ws2812_init(); /* RMT channel + encoder */
    gpio_set_level(BOARD_LED_GPIO, 0); /* WS2812 data idles low */
    /* BOOT on GPIO0 — input w/ pullup; strap pin is already input at
     * reset but an explicit direction + pullup is cheap insurance */
    gpio_config_t io = { .pin_bit_mask = 1ull << 0,
                         .mode = GPIO_MODE_INPUT, .pull_up_en = 1 };
    gpio_config(&io);
}
