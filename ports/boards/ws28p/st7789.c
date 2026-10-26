/*
 * ports/boards/ws28p/st7789.c — Waveshare ESP32-S3-Touch-LCD-2.8
 * (SKU 27690) display device (display.h impl).
 *
 * ST7789 240x320 on SPI2: MOSI45 SCK40 CS42 DC41 RST39, BL5 (LEDC
 * PWM). Unlike the ES3C28P's ILI9341, this panel has a real RST pin —
 * a soft reboot can hard-reset the glass, no SPI resync needed.
 * Landscape orientation (MADCTL MY|MV|RGB) gives a 320x240 view.
 *
 * RGB565 over SPI is BIG-endian; the S3 stores u16 little-endian —
 * every pixel is byte-swapped into a DMA-capable swap buffer. A
 * dirty band larger than the buffer is sent in row-aligned chunks,
 * each with its own PASET (a new RAMWR without PASET resets the
 * write pointer — see bf_ili9341_spi for the same trap).
 */
#ifdef ESP_PLATFORM

#include "../../../ui/display.h"
#include "bsp_pins.h"

#include <string.h>

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"
#include "esp_heap_caps.h"
#include "esp_rom_sys.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#define LCD_MOSI GPIO_NUM_45
#define LCD_SCK  GPIO_NUM_40
#define LCD_CS   GPIO_NUM_42
#define LCD_DC   GPIO_NUM_41
#define LCD_RST  GPIO_NUM_39
#define LCD_BL   GPIO_NUM_5

#define LCD_W        240 /* panel-native portrait */
#define LCD_H        320
#define LCD_PCLK_HZ  (40 * 1000 * 1000)
#define SWAP_ROWS    32  /* DMA swap buffer: 32 rows x 320px x 2B */
/* panel <-> glass mapping offsets, applied to CASET/PASET */
#define LCD_X_OFF    0
#define LCD_Y_OFF    0

#define ST7789_SWRESET 0x01
#define ST7789_CASET   0x2A
#define ST7789_PASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_MADCTL  0x36
#define ST7789_INVON   0x21
#define ST7789_SLPOUT  0x11
#define ST7789_DISPON  0x29

typedef struct {
    esp_lcd_panel_io_handle_t io;
    uint8_t *swap;      /* internal-RAM bounce buffer */
    size_t swap_bytes;
    SemaphoreHandle_t flush_done; /* given when a queued tx_color's
                                   * DMA finishes — tx_color is ASYNC,
                                   * swap must not be refilled until then */
} lcd_state_t;

static lcd_state_t g_lcd;

static bool on_color_done(esp_lcd_panel_io_handle_t io,
                          esp_lcd_panel_io_event_data_t *edata,
                          void *user) {
    (void)io; (void)edata; (void)user;
    xSemaphoreGiveFromISR(g_lcd.flush_done, NULL);
    return false;
}

static int cmd(uint8_t c, const uint8_t *data, size_t len) {
    return esp_lcd_panel_io_tx_param(g_lcd.io, c, data, len);
}

static void init_panel(void) {
    /* real RST pin (GPIO39): hardware reset beats SWRESET — takes
     * the panel to a known state after any mid-flight transaction */
    gpio_config_t io = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ull << LCD_RST),
    };
    gpio_config(&io);
    gpio_set_level(LCD_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(LCD_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(120)); /* post-reset settle per datasheet */

    /* standard ST7789 sequence (Waveshare demo / datasheet flow) */
    static const struct {
        uint8_t c, len;
        uint8_t d[14];
    } seq[] = {
        {0x36, 1, {0xA0}},      /* MADCTL MY|MV|RGB = landscape */
        {0x3A, 1, {0x55}},      /* COLMOD 16bpp RGB565 */
        {0xB2, 5, {0x0C, 0x0C, 0x00, 0x33, 0x33}}, /* PORCTRL */
        {0xB7, 1, {0x35}},      /* GCTRL */
        {0xBB, 1, {0x19}},      /* VCOMS */
        {0xC0, 1, {0x2C}},      /* LCMCTRL */
        {0xC2, 1, {0x01}},      /* VDVVRHEN */
        {0xC3, 1, {0x12}},      /* VRHS */
        {0xC4, 1, {0x20}},      /* VDVS */
        {0xC6, 1, {0x0F}},      /* FRATE 60 Hz */
        {0xD0, 2, {0xA4, 0xA1}},/* PWCTRL1 */
        {0xE0, 14, {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F,
                    0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23}}, /* PVGAMCTRL */
        {0xE1, 14, {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F,
                    0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23}}, /* NVGAMCTRL */
    };
    for (size_t i = 0; i < sizeof(seq) / sizeof(seq[0]); i++) {
        cmd(seq[i].c, seq[i].d, seq[i].len);
    }
    cmd(ST7789_INVON, NULL, 0); /* IPS panels need inversion ON */
    cmd(ST7789_SLPOUT, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(120));
    cmd(ST7789_DISPON, NULL, 0);
}

static int bl_init(void) {
    ledc_timer_config_t t = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    if (ledc_timer_config(&t) != ESP_OK) {
        return -1;
    }
    ledc_channel_config_t ch = {
        .gpio_num = LCD_BL,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 255,
        .hpoint = 0,
    };
    return ledc_channel_config(&ch) == ESP_OK ? 0 : -1;
}

static int lcd_backlight(void *ctx, uint8_t percent) {
    (void)ctx;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0,
                  (uint32_t)percent * 255 / 100);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    return 0;
}

static int lcd_flush(void *ctx, uint16_t x0, uint16_t y0, uint16_t x1,
                     uint16_t y1, const uint16_t *data, uint16_t stride) {
    (void)ctx;
    /* defense in depth: gfx clips dirty boxes already, but a bad
     * flush window must never overrun the swap gather */
    if (x1 >= LCD_H) x1 = LCD_H - 1;
    if (y1 >= LCD_W) y1 = LCD_W - 1;
    if (x0 > x1 || y0 > y1) {
        return -1; /* underflow would overrun the swap buf */
    }
    uint16_t w = x1 - x0 + 1;
    uint16_t h = y1 - y0 + 1;
    uint16_t rows_per = (uint16_t)(g_lcd.swap_bytes / (2u * w));
    if (rows_per == 0) {
        rows_per = 1;
    }
    const uint16_t *p = data;
    uint16_t y = y0;
    while (y <= y1) {
        uint16_t rows = h < rows_per ? h : rows_per;
        /* stride-aware gather: swap buffer is packed, source rows
         * are `stride` pixels apart */
        uint8_t *dst = g_lcd.swap;
        const uint16_t *s = p;
        for (uint16_t r = 0; r < rows; r++) {
            for (uint16_t c = 0; c < w; c++) {
                *dst++ = (uint8_t)(s[c] >> 8);
                *dst++ = (uint8_t)s[c];
            }
            s += stride;
        }
        uint16_t cx0 = x0 + LCD_X_OFF, cx1 = x1 + LCD_X_OFF;
        uint16_t cy0 = y + LCD_Y_OFF, cy1 = y + rows - 1 + LCD_Y_OFF;
        uint8_t col[4] = {cx0 >> 8, cx0, cx1 >> 8, (uint8_t)cx1};
        uint8_t row[4] = {cy0 >> 8, cy0, cy1 >> 8, (uint8_t)cy1};
        if (cmd(ST7789_CASET, col, 4) ||
            cmd(ST7789_PASET, row, 4) ||
            esp_lcd_panel_io_tx_color(g_lcd.io, ST7789_RAMWR,
                                      g_lcd.swap,
                                      (size_t)w * rows * 2) != ESP_OK) {
            return -1;
        }
        /* tx_color returns after QUEUEING — DMA may still be reading
         * swap. Wait for the completion callback before the next
         * chunk refills it, or the panel shows torn/shifted rows. */
        xSemaphoreTake(g_lcd.flush_done, pdMS_TO_TICKS(100));
        p += (size_t)rows * stride;
        y += rows;
        h -= rows;
    }
    return 0;
}

int bsp_display_init(display_dev_t *dev) {
    spi_bus_config_t bus = {
        .mosi_io_num = LCD_MOSI,
        .miso_io_num = -1,      /* write-only panel */
        .sclk_io_num = LCD_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_H * 2 * SWAP_ROWS,
    };
    if (spi_bus_initialize(SPI2_HOST, &bus, SPI_DMA_CH_AUTO) != ESP_OK) {
        return -1;
    }
    esp_lcd_panel_io_spi_config_t io = {
        .cs_gpio_num = LCD_CS,
        .dc_gpio_num = LCD_DC,
        .spi_mode = 0,
        .pclk_hz = LCD_PCLK_HZ,
        .trans_queue_depth = 4,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    if (esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST,
                                 &io, &g_lcd.io) != ESP_OK) {
        return -1;
    }
    g_lcd.flush_done = xSemaphoreCreateCounting(4, 0);
    esp_lcd_panel_io_callbacks_t cbs = {.on_color_trans_done =
                                            on_color_done};
    esp_lcd_panel_io_register_event_callbacks(g_lcd.io, &cbs, NULL);
    g_lcd.swap_bytes = LCD_H * 2 * SWAP_ROWS; /* 320*2*32 = 20 KB */
    g_lcd.swap =
        heap_caps_malloc(g_lcd.swap_bytes, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if (g_lcd.swap == NULL) {
        return -1;
    }
    init_panel();
    bl_init();
    lcd_backlight(NULL, 80); /* 80% is kind on the rail + eyes */
    if (dev) {
        dev->ctx = &g_lcd;
        dev->res.width = LCD_H;  /* landscape: 320 x 240 */
        dev->res.height = LCD_W;
        dev->flush = lcd_flush;
        dev->backlight = lcd_backlight;
        dev->power = NULL;
    }
    return 0;
}

#endif /* ESP_PLATFORM */
