/*
 * esp32s3_bsp.h — ESP-IDF port-internal seam (NOT portable).
 * Shared between the generic port (ports/esp_idf.c) and the
 * board-specific glue (ports/boards/<sku>/). App/middleware code
 * never sees ESP-IDF types.
 */
#ifndef ESP32S3_BOARD_H
#define ESP32S3_BOARD_H

#include "driver/i2c_master.h"
#include "../ui/display.h"

/* shared I2C bus — es3c28p SDA16/SCL15 (codec+touch), ws28p
 * SDA11/SCL10 (IMU+RTC). Lazy-created, single owner. */
i2c_master_bus_handle_t esp32s3_i2c0(void);

/* ws28p dedicated touch bus (SDA1/SCL3) — unused on es3c28p. */
i2c_master_bus_handle_t ws28p_tp_i2c(void);

/* per-board hook called by bsp_init() — LED/amp/i2c bus setup */
void bsp_misc_init(void);

/* per-board panel driver — fills display_dev_t, 0 on success */
int bsp_display_init(display_dev_t *dev);

#endif /* ESP32S3_BOARD_H */
