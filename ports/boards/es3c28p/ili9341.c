/*
 * ports/esp32s3_ili9341.c — ES3C28P display device (display.h impl).
 *
 * ILI9341V 240x320 on SPI2: SCK12 MOSI11 MISO13 CS10 DC46, BL45
 * (LEDC PWM), reset tied to CHIP_PU per the board schematic.
 * Landscape orientation (MADCTL MV|MX|BGR) gives a 320x240 view.
 *
 * RGB565 over SPI is BIG-endian; the S3 stores u16 little-endian —
 * every pixel is byte-swapped into a DMA-capable swap buffer. A
 * dirty band larger than the buffer is sent in row-aligned chunks,
 * each with its own PASET (a new RAMWR without PASET resets the
 * write pointer — see bf_ili9341_spi for the same trap).
 */
#ifdef ESP_PLATFORM

#include "../../../ui/display.h"

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

#define LCD_CS   GPIO_NUM_10
#define LCD_DC   GPIO_NUM_46
#define LCD_SCK  GPIO_NUM_12
#define LCD_MOSI GPIO_NUM_11
#define LCD_MISO GPIO_NUM_13
#define LCD_BL   GPIO_NUM_45

#define LCD_W        240 /* panel-native portrait */
#define LCD_H        320
#define LCD_PCLK_HZ  (40 * 1000 * 1000)
#define SWAP_ROWS    32  /* DMA swap buffer: 32 rows x 320px x 2B */
/* panel <-> glass mapping offsets, applied to CASET/PASET. Some
 * ILI9341 modules shift the visible window by a few rows/cols —
 * bump these if edges crop. */
#define LCD_X_OFF    0
#define LCD_Y_OFF    0

#define ILI9341_CASET  0x2A
#define ILI9341_SWRESET 0x01 /* panel's only reset on soft reboot —
                              * RST is tied to CHIP_PU, so esp_restart
                              * leaves the panel in whatever SPI state
                              * it was in (e.g. mid-RAMWR data phase). */
#define ILI9341_PASET  0x2B
#define ILI9341_RAMWR  0x2C
#define ILI9341_MADCTL 0x36
#define ILI9341_INVON  0x21 /* ES3C28P panel needs inversion ON */
#define ILI9341_SLPOUT 0x11
#define ILI9341_DISPON 0x29

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
    /* SWRESET first: the panel's RST pin is tied to CHIP_PU, so a
     * soft reboot leaves the ILI9341 in whatever transaction state it
     * was in when the CPU reset — a mid-flight RAMWR can leave the
     * decoder expecting pixel data and ignoring commands. SWRESET
     * aborts that and forces a known state; needs >=120 ms settle. */
    cmd(ILI9341_SWRESET, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(150));

    /* vendor ILI9341V sequence */
    static const struct {
        uint8_t c, len;
        uint8_t d[15];
    } seq[] = {
        {0xCF, 3, {0x00, 0xC1, 0x30}},
        {0xED, 4, {0x64, 0x03, 0x12, 0x81}},
        {0xE8, 3, {0x85, 0x00, 0x78}},
        {0xCB, 5, {0x39, 0x2C, 0x00, 0x34, 0x02}},
        {0xF7, 1, {0x20}},
        {0xEA, 2, {0x00, 0x00}},
        {0xC0, 1, {0x13}},
        {0xC1, 1, {0x13}},
        {0xC5, 2, {0x22, 0x35}},
        {0xC7, 1, {0xBD}},
        {0x36, 1, {0xA8}},      /* MADCTL MV|MX|BGR = landscape */
        {0xB6, 2, {0x0A, 0xA2}},
        {0x3A, 1, {0x55}},      /* COLMOD 16bpp RGB565 */
        {0xF6, 2, {0x01, 0x30}},
        {0xB1, 2, {0x00, 0x1B}},
        {0xF2, 1, {0x00}},
        {0x26, 1, {0x01}},
        {0xE0, 15, {0x0F, 0x35, 0x31, 0x0B, 0x0F, 0x06, 0x49, 0xA7,
                    0x33, 0x07, 0x0F, 0x03, 0x0C, 0x0A, 0x00}},
        {0xE1, 15, {0x00, 0x0A, 0x0F, 0x04, 0x11, 0x08, 0x36, 0x58,
                    0x4D, 0x07, 0x10, 0x0C, 0x32, 0x34, 0x0F}},
    };
    for (size_t i = 0; i < sizeof(seq) / sizeof(seq[0]); i++) {
        cmd(seq[i].c, seq[i].d, seq[i].len);
    }
    cmd(ILI9341_INVON, NULL, 0); /* this panel inverts — else black
                                  * renders white and colors shift */
    cmd(ILI9341_SLPOUT, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(120));
    cmd(ILI9341_DISPON, NULL, 0);
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
        if (cmd(ILI9341_CASET, col, 4) ||
            cmd(ILI9341_PASET, row, 4) ||
            esp_lcd_panel_io_tx_color(g_lcd.io, ILI9341_RAMWR,
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

/* Panel SPI resync — the ILI9341's RST pin is tied to CHIP_PU, so
 * esp_restart() leaves the panel mid-transaction (e.g. expecting the
 * tail of a RAMWR data byte). Its bit counter is then offset and even
 * SWRESET arrives misaligned. Before the SPI driver claims the pins,
 * clock out 16 dummy SCK cycles with CS high so the panel flushes any
 * partial byte and sees a clean transaction boundary. */
static void lcd_spi_resync(void) {
    gpio_config_t io = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ull << LCD_CS) | (1ull << LCD_SCK) |
                        (1ull << LCD_MOSI),
    };
    gpio_config(&io);
    gpio_set_level(LCD_CS, 1);
    gpio_set_level(LCD_MOSI, 1);
    for (int i = 0; i < 16; i++) {
        gpio_set_level(LCD_SCK, 0);
        esp_rom_delay_us(1);
        gpio_set_level(LCD_SCK, 1);
        esp_rom_delay_us(1);
    }
    /* one CS low/high frame around an extra 8 clocks — completes any
     * byte the panel was mid-way through, then ends the transaction */
    gpio_set_level(LCD_CS, 0);
    esp_rom_delay_us(1);
    for (int i = 0; i < 8; i++) {
        gpio_set_level(LCD_SCK, 0);
        esp_rom_delay_us(1);
        gpio_set_level(LCD_SCK, 1);
        esp_rom_delay_us(1);
    }
    gpio_set_level(LCD_CS, 1);
    esp_rom_delay_us(10);
}

int bsp_display_init(display_dev_t *dev) {
    lcd_spi_resync();
    spi_bus_config_t bus = {
        .mosi_io_num = LCD_MOSI,
        .miso_io_num = LCD_MISO,
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
    lcd_backlight(NULL, 80); /* not power-related — LED flicker was
                              * bit-bang corruption; 80% is kind on
                              * the rail + eyes */
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
