#ifndef RTOS_LABS_EX13_H
#define RTOS_LABS_EX13_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * IMU space shooter: tilt ship, auto-fire, dodge/destroy enemies.
 * Returns after create; tasks keep running under the scheduler.
 */
void ex13_start(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX13_H */
