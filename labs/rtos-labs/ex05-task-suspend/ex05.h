#ifndef RTOS_LABS_EX05_H
#define RTOS_LABS_EX05_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Two workers + toggle buttons: vTaskSuspend / vTaskResume per task.
 * Returns after create; tasks keep running under the scheduler.
 */
void ex05_start(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX05_H */
