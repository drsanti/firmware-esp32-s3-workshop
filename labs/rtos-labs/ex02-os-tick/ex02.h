#ifndef RTOS_LABS_EX02_H
#define RTOS_LABS_EX02_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * OS tick + tick hook. Returns after create; tasks / hook keep running.
 */
void ex02_start(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX02_H */
