/*
 * rb_imu_readout.h — DISPLAY accel bipolar bars + gyro text + status.
 *
 * Companion to rb_level for IMU dashboards (product page_imu layout).
 *
 *   rb_imu_readout_init(&ro);
 *   rb_imu_readout_set(&ro, acc, gyro, ok);
 */
#ifndef ROBOTOS_RB_IMU_READOUT_H
#define ROBOTOS_RB_IMU_READOUT_H

#include <stdint.h>

#include "bsp.h"
#include "rb_screen.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_IMU_READOUT_STATUS_MAX 16

typedef struct rb_imu_readout {
    rb_widget_t base; /* must be first */
    float acc[3];
    float gyro[3];
    uint8_t live;
    uint8_t ok;
    float acc_span;  /* ±span maps to full half-bar; default 19.62 (±2 g) */
    uint8_t bar_r, bar_g, bar_b;
    uint8_t gyro_r, gyro_g, gyro_b;
    uint8_t tx_r, tx_g, tx_b;
    char status[RB_IMU_READOUT_STATUS_MAX]; /* empty = live / no IMU */
} rb_imu_readout_t;

void rb_imu_readout_init(rb_imu_readout_t *ro);
int  rb_imu_readout_apply_geometry(rb_imu_readout_t *ro);

void rb_imu_readout_set(rb_imu_readout_t *ro, const float acc[3],
                        const float gyro[3], int live);
/* Override footer (e.g. "IMU sim"). NULL/"" restores live/no IMU. */
void rb_imu_readout_set_status(rb_imu_readout_t *ro, const char *text);
void rb_imu_readout_set_acc_span(rb_imu_readout_t *ro, float span_mps2);
void rb_imu_readout_set_bar_rgb(rb_imu_readout_t *ro, uint8_t r, uint8_t g,
                                uint8_t b);
void rb_imu_readout_set_gyro_rgb(rb_imu_readout_t *ro, uint8_t r, uint8_t g,
                                 uint8_t b);
void rb_imu_readout_set_text_rgb(rb_imu_readout_t *ro, uint8_t r, uint8_t g,
                                 uint8_t b);

void rb_imu_readout_redraw(rb_imu_readout_t *ro);
int  rb_imu_readout_contains(const rb_imu_readout_t *ro, int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_IMU_READOUT_H */
