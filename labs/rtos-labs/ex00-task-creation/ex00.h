#ifndef RTOS_LABS_EX00_H
#define RTOS_LABS_EX00_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Create blink + count tasks (same prio, different vTaskDelay periods).
 * Returns after xTaskCreate; tasks keep running under the scheduler.
 */
void ex00_start(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX00_H */
