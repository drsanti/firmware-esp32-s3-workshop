#ifndef RTOS_LABS_EX07_H
#define RTOS_LABS_EX07_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Counting semaphore pool: N workers, M slots, toggle OFF (oversubscribe)
 * vs ON (Take/Give). Returns after create; tasks keep running.
 */
void ex07_start(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX07_H */
