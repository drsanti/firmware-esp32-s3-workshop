#ifndef RTOS_LABS_EX10_H
#define RTOS_LABS_EX10_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * BOOT button: poll vs GPIO ISR + vTaskNotifyGiveFromISR.
 * Returns after create; tasks keep running under the scheduler.
 */
void ex10_start(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX10_H */
