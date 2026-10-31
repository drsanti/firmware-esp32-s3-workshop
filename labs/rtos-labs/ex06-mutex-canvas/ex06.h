#ifndef RTOS_LABS_EX06_H
#define RTOS_LABS_EX06_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Two painters share one canvas; toggle FreeRTOS mutex around each stroke.
 * Boot with mutex OFF so the race is visible first.
 */
void ex06_start(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX06_H */
