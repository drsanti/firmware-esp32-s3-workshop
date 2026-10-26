/*
 * bsp_pins.h — ES3C28P (ternion ESP32-S3 reference board) pin map.
 *
 * The generic ESP-IDF port code (ports/esp_idf.c) reads these macros;
 * every number here comes from the board schematic — changing boards
 * means swapping this file, not the drivers.
 */
#ifndef BOARD_PINS_H
#define BOARD_PINS_H

#define BOARD_SKU "es3c28p"
#define BOARD_NAME "ternion-esp32s3"

/* shared I2C bus: ES8311 codec (0x18) + FT6336 touch (0x38) */
#define BOARD_I2C0_SDA 16
#define BOARD_I2C0_SCL 15

/* WS2812 RGB LED (GRB), driven over RMT */
#define BOARD_LED_GPIO 42

/* LiPo divider -> ADC */
#define BOARD_BATT_GPIO 9

/* expansion-header UART (the serial-bridge link when USB-JTAG is
 * not in use) */
#define BOARD_LINK_TX_GPIO 2
#define BOARD_LINK_RX_GPIO 3

#endif /* BOARD_PINS_H */
