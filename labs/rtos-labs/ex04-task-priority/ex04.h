#ifndef RTOS_LABS_EX04_H
#define RTOS_LABS_EX04_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Three workers (prio 6/5/4) pinned to core 0; auto YIELD vs HOG phases.
 * Returns after create; tasks keep running under the scheduler.
 */
void ex04_start(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX04_H */
