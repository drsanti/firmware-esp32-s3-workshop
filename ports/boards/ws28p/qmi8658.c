/*
 * ports/boards/ws28p/qmi8658.c — Waveshare ESP32-S3-Touch-LCD-2.8
 * (SKU 27690) IMU (board_imu_read impl).
 *
 * QMI8658 6-axis accel+gyro, I2C 0x6B on the shared peripheral bus
 * (SDA11/SCL10). Minimal bring-up: WHO_AM_I check, ±4g accel and
 * ±512 dps gyro at 112 Hz — the app polls at its own 50 Hz sensor
 * tick, so no FIFO/interrupt wiring (INT1/INT2 stay unused).
 */
#ifdef ESP_PLATFORM

#include "../../board.h"
#include "../../esp32s3_board.h"

#include <string.h>

#include "driver/i2c_master.h"
#include "esp_err.h"

#define QMI8658_ADDR   0x6B
#define QMI8658_WHOAMI 0x00 /* expect 0x05 */
#define QMI8658_CTRL1  0x02 /* serial iface: keep 4-wire SPI off, addr auto-inc */
#define QMI8658_CTRL2  0x03 /* accel: range[6:4] | ODR[3:0] */
#define QMI8658_CTRL3  0x04 /* gyro:  range[6:4] | ODR[3:0] */
#define QMI8658_CTRL7  0x08 /* bit0 aEN, bit1 gEN */
#define QMI8658_AX_L   0x35 /* accel XYZ, 6 bytes */
#define QMI8658_GX_L   0x3B /* gyro  XYZ, 6 bytes */

/* CTRL2=0x16: aFS=1 ±4g -> 8192 LSB/g; CTRL3=0x56: gFS=5 ±512dps
 * -> 64 LSB/dps */
#define ACC_SCALE  (9.80665f / 8192.0f)
#define GYR_SCALE  (3.14159265f / 180.0f / 64.0f)

static i2c_master_dev_handle_t s_imu;

static int qmi_rd(uint8_t reg, uint8_t *data, size_t n) {
    return i2c_master_transmit_receive(s_imu, &reg, 1, data, n, 10)
               == ESP_OK ? 0 : -1;
}

static int qmi_wr(uint8_t reg, uint8_t v) {
    uint8_t wb[2] = {reg, v};
    return i2c_master_transmit(s_imu, wb, 2, 10) == ESP_OK ? 0 : -1;
}

static int qmi_init(void) {
    i2c_device_config_t dev = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = QMI8658_ADDR,
        .scl_speed_hz = 400000,
    };
    if (esp32s3_i2c0() == NULL ||
        i2c_master_bus_add_device(esp32s3_i2c0(), &dev, &s_imu) !=
            ESP_OK) {
        return -1;
    }
    uint8_t who = 0;
    if (qmi_rd(QMI8658_WHOAMI, &who, 1) != 0 || who != 0x05 ||
        qmi_wr(QMI8658_CTRL7, 0x00) ||
        qmi_wr(QMI8658_CTRL1, 0x40) || /* 2MHz osc on, addr auto-inc */
        qmi_wr(QMI8658_CTRL2, 0x16) || /* aFS=1 ±4g, ODR ~112Hz */
        qmi_wr(QMI8658_CTRL3, 0x56) || /* gFS=5 ±512dps, ODR ~112Hz */
        qmi_wr(QMI8658_CTRL7, 0x43)) { /* aEN|gEN (vendor byte: 0x43) */
        i2c_master_bus_rm_device(s_imu); /* allow init retry */
        s_imu = NULL;
        return -1;
    }
    return 0;
}

int board_imu_read(float accel_mps2[3], float gyro_rads[3]) {
    if (s_imu == NULL && qmi_init() != 0) {
        return -1;
    }
    uint8_t a[6], g[6];
    if (qmi_rd(QMI8658_AX_L, a, sizeof a) ||
        qmi_rd(QMI8658_GX_L, g, sizeof g)) {
        return -1;
    }
    for (int i = 0; i < 3; i++) {
        int16_t av = (int16_t)((uint16_t)a[i * 2] | ((uint16_t)a[i * 2 + 1] << 8));
        int16_t gv = (int16_t)((uint16_t)g[i * 2] | ((uint16_t)g[i * 2 + 1] << 8));
        accel_mps2[i] = av * ACC_SCALE;
        gyro_rads[i]  = gv * GYR_SCALE;
    }
    return 0;
}

#endif /* ESP_PLATFORM */
