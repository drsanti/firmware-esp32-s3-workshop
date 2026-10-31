#ifndef RTOS_LABS_EX11_H
#define RTOS_LABS_EX11_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * IMU road racer capstone: tilt to steer, dodge obstacles, SFX via queue.
 * Returns after create; tasks keep running under the scheduler.
 */
void ex11_start(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX11_H */
