/*
 * lab_spirit.h - spirit-level face: large rb_level bubble + thin status.
 *
 * Layout: small top bar ("Spirit" | LEVEL/TILT), bubble fills the rest.
 * Helper owns display. Student code sets tilt (and later IMU).
 *
 *   spirit_init();
 *   spirit_start();
 *   spirit_set_accel_mps2(ax, ay);  // m/s^2 — moves bubble, updates LEVEL
 */
#ifndef ROBOTOS_LAB_SPIRIT_H
#define ROBOTOS_LAB_SPIRIT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Open LCD + title. Call before spirit_start(). */
void spirit_init(void);

/* Build layout (bubble + status). Call once after init. */
void spirit_start(void);

/*
 * Tilt in m/s^2 (same units as bsp_imu_read accel).
 * Updates bubble and the LEVEL / TILT status line.
 */
void spirit_set_accel_mps2(float ax, float ay);

/* 1 = sensor path ok (bright core). Fake tilt in step 01 can pass 1. */
void spirit_set_live(int on);

/* 1 if last tilt was inside the LEVEL deadband. */
int spirit_is_level(void);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_LAB_SPIRIT_H */
