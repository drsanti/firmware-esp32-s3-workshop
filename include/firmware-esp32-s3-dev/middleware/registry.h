/*
 * registry.h — module table for the robotos executor (@api rb_mod_*).
 *
 * A module is a name + vtable + user context. Registration calls
 * init() immediately (fail-fast at bring-up); the executor calls
 * execute() every `period_ms` ticks and deinit() on unregister.
 *
 * Rules (same posture as the rb_* core):
 *   - fixed pool, no heap: RB_MOD_MAX_MODULES entries
 *   - register/unregister BEFORE the executor task starts —
 *     the table is not internally locked
 *   - execute() must never block; it runs on the tick task
 *
 * Preferred include: rb_mod.h
 */
#ifndef ROBOTOS_RB_MOD_REGISTRY_H
#define ROBOTOS_RB_MOD_REGISTRY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RB_MOD_MAX_MODULES
#define RB_MOD_MAX_MODULES 8
#endif

/* consecutive execute() errors before the module is disabled */
#ifndef RB_MOD_MAX_ERRORS
#define RB_MOD_MAX_ERRORS 8
#endif

typedef struct rb_module rb_module_t;
typedef int  (*rb_mod_init_fn)(rb_module_t *m);   /* 0 ok, <0 = skip */
typedef int  (*rb_mod_exec_fn)(rb_module_t *m);   /* 0 ok, <0 = error */
typedef void (*rb_mod_deinit_fn)(rb_module_t *m);

/*
 * Feature plumbing contract — every pipeline stage speaks
 * `void sink(float, void*)` out and/or `float src(void*)` in, so
 * stages compose: stage.sink = next.feed.
 */
typedef void (*rb_mod_sink_t)(float sample, void *user);
typedef float (*rb_mod_src_t)(void *user);

struct rb_module {
    const char *name;
    void *ctx;              /* module state, e.g. a sine_t*   */
    rb_mod_init_fn init;
    rb_mod_exec_fn execute; /* invoked every period_ms ticks  */
    rb_mod_deinit_fn deinit;/* may be NULL                    */
    uint32_t period_ms;     /* 0 or 1 = every 1 ms tick       */
    /* executor bookkeeping — do not touch */
    uint32_t next_due_ms;
    uint32_t n_runs;
    uint32_t n_errors;
    uint8_t enabled;
};

/*
 * Register a module: calls init() immediately; on success the module
 * is scheduled for its first execute() next tick.
 * Returns 0 ok, -1 table full or init failed.
 */
int rb_mod_register(rb_module_t *m);

/* Unregister by pointer or by name (NULL-safe). Calls deinit(). */
int rb_mod_unregister(rb_module_t *m);
rb_module_t *rb_mod_find(const char *name);

/* Iteration for the executor — not for app code. */
uint8_t rb_mod_count(void);
rb_module_t *rb_mod_at(uint8_t i);

/* Table stats for the executor/loggers. */
extern uint32_t g_rb_mod_tick_miss; /* ticks where a module ran late */

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_MOD_REGISTRY_H */
