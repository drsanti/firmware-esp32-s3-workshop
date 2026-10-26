/*
 * executor.h — 1 ms cooperative scheduler over the module registry.
 *
 * rb_mod_executor_tick(now_ms) is the ONLY portable entry point —
 * feed it a millisecond monotonic clock, it runs every due module.
 * No OS dependency: unit-testable on the host, wrappable in a
 * FreeRTOS task on target.
 *
 * IMPORTANT: run it from a task, never from the tick ISR — modules
 * do UART/log work that is not ISR-safe. ports provide the task glue:
 *   for(;;) { vTaskDelayUntil(&x, 1); rb_mod_executor_tick(millis()); }
 *
 * Preferred include: rb_mod.h
 */
#ifndef ROBOTOS_RB_MOD_EXECUTOR_H
#define ROBOTOS_RB_MOD_EXECUTOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Dispatch due modules. `now_ms` is any monotonic millisecond tick
 * (xTaskGetTickCount()*portTICK_PERIOD_MS, or board time). Wraps are
 * fine — arithmetic is on uint32 deltas.
 */
void rb_mod_executor_tick(uint32_t now_ms);

/* Aggregates for diagnostics/log topics. */
uint32_t rb_mod_executor_runs(void);   /* total module executions    */
uint32_t rb_mod_executor_errors(void); /* total module error returns */

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_MOD_EXECUTOR_H */
