/*
 * ex_imu_panel — instrument UI for IMU pipeline exercises.
 *
 *   title | (imu readout | |a| gauge) | footer
 *
 *   ex_imu_panel_begin();
 *   ex_imu_panel_set(acc, gyro, mag_mps2, live, c_sample, c_compute);
 */
#ifndef RTOS_LABS_EX_IMU_PANEL_H
#define RTOS_LABS_EX_IMU_PANEL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Build the screen tree. 0 = ok. */
int ex_imu_panel_begin(void);

/*
 * Update widgets from one coherent sample.
 * acc/gyro: m/s^2 and rad/s (gyro may be NULL → zeros).
 * mag_mps2: |a|; live: 1 = hardware IMU, 0 = sim.
 * c_sample / c_compute: core ids for the footer (-1 to hide).
 */
void ex_imu_panel_set(const float acc[3], const float gyro[3], float mag_mps2,
                      int live, int c_sample, int c_compute);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX_IMU_PANEL_H */
