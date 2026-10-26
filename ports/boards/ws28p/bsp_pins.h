/*
 * bsp_pins.h — Waveshare ESP32-S3-Touch-LCD-2.8 (SKU 27690) pin map.
 * From the official schematic + wiki "Internal Hardware Connection".
 *
 * The touch controller is on its OWN I2C bus (SDA1/SCL3) — the
 * shared bus (SDA11/SCL10) carries QMI8658 IMU (0x6B) + PCF85063
 * RTC (0x51).
 */
#ifndef BOARD_PINS_H
#define BOARD_PINS_H

#define BOARD_SKU "ws28p"
#define BOARD_NAME "ternion-ws28p"

/* shared I2C bus: QMI8658 IMU (0x6B) + PCF85063 RTC (0x51) */
#define BOARD_I2C0_SDA 11
#define BOARD_I2C0_SCL 10

/* touch has a dedicated I2C bus */
#define BOARD_TP_I2C_PORT I2C_NUM_1
#define BOARD_TP_SDA      1
#define BOARD_TP_SCL      3
#define BOARD_TP_INT      4
#define BOARD_TP_RST      2

/* no WS2812 — bsp_led_* are stubs on this board */

/* LiPo divider -> ADC1_CH7 (GPIO8), pack ≈ pin x3 */
#define BOARD_BATT_GPIO 8

/* expansion-header UART (the serial-bridge link when USB-JTAG is
 * not in use) */
#define BOARD_LINK_TX_GPIO 43
#define BOARD_LINK_RX_GPIO 44

#endif /* BOARD_PINS_H */
