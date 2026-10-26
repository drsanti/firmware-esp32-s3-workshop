/*
 * esp_idf.c — ESP32-S3 (ESP-IDF) chip-generic board HAL.
 *
 * Board-agnostic: clocks, identity, critical sections, the serial
 * link, boot-button factory reset. Board-specific pins live in
 * ports/boards/<sku>/bsp_pins.h (selected by the BOARD cmake var);
 * board glue (i2c buses, LED, display/touch/audio drivers) lives in
 * ports/boards/<sku>/ next to bsp_pins.h.
 *
 * Two link variants, compile-time select (RSP_LINK_USB_JTAG):
 *                          USB-Serial/JTAG: ESP32-S3 on-chip USB
 *                          (GPIO19/20 on every ESP32-S3 dev board).
 *                          UART1 on the board's expansion pins: keep
 *                          ESP_LOG pointed at UART0 so debug text
 *                          can't leak onto the protocol stream.
 *
 * Build: add rb.c, rb_serial.c, app.c, ../protocol.h include dir and
 * this file to your component; app_main() calls app_run().
 */

#include <esp_heap_caps.h>
#include <stdio.h>
#include <string.h>

#include "../bsp.h"
#include "../protocol.h"
#include "../serial_app.h"
#include "bsp_pins.h"

#include "driver/gpio.h"
#include "esp32s3_bsp.h"
#include "esp_cpu.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

/* Factory reset — hold BOOT (GPIO0) through the first ~1.5 s of boot
 * to erase NVS: the recovery path for `ble.enabled=off`, bad wifi
 * creds, or a stale robot.id bricking remote access. */
static void boot_button_factory_reset(void) {
    gpio_config_t c = {
        .pin_bit_mask = 1ULL << GPIO_NUM_0,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&c);
    if (gpio_get_level(GPIO_NUM_0) != 0) {
        return;
    }
    for (int i = 0; i < 30; i++) {
        if (gpio_get_level(GPIO_NUM_0) != 0) {
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    nvs_flash_init(); /* may be first touch — erase is standalone-safe */
    nvs_flash_erase();
    ESP_EARLY_LOGW("board", "BOOT held — NVS erased (factory reset)");
}

#ifdef RSP_LINK_USB_JTAG
/* ---------------- bring-up link: USB-Serial/JTAG ---------------- */
#include "driver/usb_serial_jtag.h"

void bsp_init(void) {
    usb_serial_jtag_driver_config_t cfg = {
        .tx_buffer_size = 4096,
        .rx_buffer_size = 4096,
    };
    usb_serial_jtag_driver_install(&cfg);
    /* No echo: the USB-Serial/JTAG driver never echoes RX. */

    boot_button_factory_reset();
    bsp_misc_init();
}

void bsp_uart_write(const uint8_t *data, size_t len) {
    /* CDC-style pipe: no real baud rate; if no host is attached the
     * TX buffer fills and this returns short — drop-and-count is the
     * spec posture, so we don't retry. */
    usb_serial_jtag_write_bytes(data, len, 0);
}

int bsp_uart_read(uint8_t *byte) {
    return usb_serial_jtag_read_bytes(byte, 1, 0) == 1;
}

#else
/* ---------------- spec link: hardware UART1 ---------------- */
#include "driver/uart.h"

#define LINK_UART    UART_NUM_1
#define LINK_BAUD    921600
#define RX_BUF_SIZE  4096

void bsp_init(void) {
    boot_button_factory_reset();
    uart_config_t cfg = {
        .baud_rate = LINK_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(LINK_UART, RX_BUF_SIZE, 2048, 0, NULL, 0);
    uart_param_config(LINK_UART, &cfg);
    uart_set_pin(LINK_UART, BOARD_LINK_TX_GPIO, BOARD_LINK_RX_GPIO,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    /* Spec §3.4: no RX echo on the protocol UART — the ESP-IDF driver
     * never echoes, nothing to disable. */

    bsp_misc_init();
}

void bsp_uart_write(const uint8_t *data, size_t len) {
    /* uart_write_bytes is serialized internally by the driver on
     * ESP-IDF (TX ring + mux); if you bypass it with raw FIFO writes,
     * add a FreeRTOS mutex here (spec §3.3). */
    uart_write_bytes(LINK_UART, data, len);
}

int bsp_uart_read(uint8_t *byte) {
    return uart_read_bytes(LINK_UART, byte, 1, 0) == 1;
}
#endif /* RSP_LINK_USB_JTAG */

unsigned long bsp_free_heap(void) { return esp_get_free_heap_size(); }

/* <BOARD_NAME>-<last3mac> — stable per-chip id; used as the zenoh
 * publisher_uuid so multiple boards on one mesh stay distinct. */
#include "esp_mac.h"
#include "esp_system.h"
int bsp_identity(char *buf, size_t n) {
    uint8_t mac[6] = {0};
    if (esp_efuse_mac_get_default(mac) != 0) {
        return -1;
    }
    int len = snprintf(buf, n, BOARD_NAME "-%02x%02x%02x",
                       mac[3], mac[4], mac[5]);
    return len < 0 || (size_t)len >= n ? -1 : len;
}

void bsp_reboot(void) { esp_restart(); }

int bsp_mac_str(char *buf, size_t n) {
    uint8_t mac[6] = {0};
    if (esp_efuse_mac_get_default(mac) != 0) {
        return -1;
    }
    int len = snprintf(buf, n, "%02x:%02x:%02x:%02x:%02x:%02x",
                       mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return len < 0 || (size_t)len >= n ? -1 : len;
}

static portMUX_TYPE s_crit_mux = portMUX_INITIALIZER_UNLOCKED;

void bsp_critical_enter(void) {
    taskENTER_CRITICAL(&s_crit_mux);
}
void bsp_critical_exit(void) {
    taskEXIT_CRITICAL(&s_crit_mux);
}

uint64_t bsp_mono_ns(void) {
    return (uint64_t)esp_timer_get_time() * 1000ull; /* us -> ns */
}

void bsp_yield(void) {
    /* app_run() is a tight poll on the main task — without this the
     * idle task starves and the task watchdog resets the chip. */
    vTaskDelay(pdMS_TO_TICKS(1));
}

struct display_dev *bsp_display(void) {
    static display_dev_t dev;
    static int tried, ok;
    if (!tried) {
        tried = 1;
        ok = bsp_display_init(&dev) == 0;
    }
    return ok ? &dev : NULL;
}

void *bsp_fb_alloc(size_t bytes) {
    return heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
}

void bsp_fb_free(void *p) {
    heap_caps_free(p);
}

static SemaphoreHandle_t s_disp_mu;

void bsp_display_lock(void) {
    if (s_disp_mu == 0) {
        s_disp_mu = xSemaphoreCreateRecursiveMutex();
    }
    if (s_disp_mu) {
        xSemaphoreTakeRecursive(s_disp_mu, portMAX_DELAY);
    }
}

void bsp_display_unlock(void) {
    if (s_disp_mu) {
        xSemaphoreGiveRecursive(s_disp_mu);
    }
}

/* ---- params NVS seam (pulled by middleware/params.c) ---- */
#define PARAM_NVS_NS "rbparam"

static int param_nvs_ready(void)
{
    static int ready = -1;
    esp_err_t e;

    if (ready >= 0) {
        return ready;
    }
    e = nvs_flash_init();
    if (e == ESP_ERR_NVS_NO_FREE_PAGES ||
        e == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        (void)nvs_flash_erase();
        e = nvs_flash_init();
    }
    ready = (e == ESP_OK) ? 1 : 0;
    return ready;
}

int bsp_param_get(const char *name, char *buf, size_t n)
{
    nvs_handle_t h;
    int rc;

    if (!name || !buf || !n || !param_nvs_ready()) {
        return -1;
    }
    if (nvs_open(PARAM_NVS_NS, NVS_READONLY, &h) != ESP_OK) {
        return -1;
    }
    rc = nvs_get_str(h, name, buf, &n) == ESP_OK ? 0 : -1;
    nvs_close(h);
    return rc;
}

int bsp_param_set(const char *name, const char *value)
{
    nvs_handle_t h;
    int rc;

    if (!name || !value || !param_nvs_ready()) {
        return -1;
    }
    if (nvs_open(PARAM_NVS_NS, NVS_READWRITE, &h) != ESP_OK) {
        return -1;
    }
    rc = (nvs_set_str(h, name, value) == ESP_OK &&
          nvs_commit(h) == ESP_OK)
             ? 0
             : -1;
    nvs_close(h);
    return rc;
}
