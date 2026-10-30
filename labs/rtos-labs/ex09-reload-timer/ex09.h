#ifndef RTOS_LABS_EX09_H
#define RTOS_LABS_EX09_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Auto-reload software timer vs vTaskDelay beat. Returns after create;
 * tasks / timer keep running under the scheduler.
 */
void ex09_start(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX09_H */
