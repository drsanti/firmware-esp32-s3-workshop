#ifndef RTOS_LABS_EX01_H
#define RTOS_LABS_EX01_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Pin one task to core 0 and one to core 1 (xTaskCreatePinnedToCore).
 * Returns after create; tasks keep running under the dual-core scheduler.
 */
void ex01_start(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX01_H */
