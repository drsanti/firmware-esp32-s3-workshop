#ifndef RTOS_LABS_EX03_H
#define RTOS_LABS_EX03_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Dual-core IMU pipeline: sample on core 0, |a| on core 1, UI via queues.
 * Returns after create; tasks keep running.
 */
void ex03_start(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX03_H */
