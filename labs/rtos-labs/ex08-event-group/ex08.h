#ifndef RTOS_LABS_EX08_H
#define RTOS_LABS_EX08_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Event group: bits A/B + WaitBits (AND vs OR). Returns after create;
 * tasks keep running under the scheduler.
 */
void ex08_start(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX08_H */
