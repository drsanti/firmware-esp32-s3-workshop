#ifndef RTOS_LABS_EX15_H
#define RTOS_LABS_EX15_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * IMU Frogger: tilt strafe, tap/BOOT hop, cars + logs + goals.
 * Returns after create; tasks keep running under the scheduler.
 */
void ex15_start(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX15_H */
