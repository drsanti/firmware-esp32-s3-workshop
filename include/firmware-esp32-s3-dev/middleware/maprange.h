/*
 * maprange.h — range-mapping middleware stage.
 *
 * Linear transform y = out_min + (x - in_min) * scale, optional clamp.
 * Three ways to drive it:
 *
 *   1. Chained (feed-driven) — wire into another module's sink:
 *        sine_cfg = { .sink = rb_map_feed, .sink_user = &g_map };
 *      output forwards to maprange's own sink. Runs synchronously on
 *      the upstream module's tick — no executor needed.
 *
 *   2. Registered (tick-driven / pull) — set cfg.src and register:
 *        rb_map_mod.ctx = &g_map;
 *        rb_mod_register(&rb_map_mod);
 *      execute() calls src() each period, maps, sinks. Use this when
 *      the upstream isn't a chain stage (e.g. a raw sensor value, a
 *      variable, an rb_pub getter).
 *
 *   3. Pure function — rb_map_apply(&m, x).
 *
 * No heap, no blocking. Both modes update m->last.
 */
#ifndef RB_MW_MAPRANGE_H
#define RB_MW_MAPRANGE_H

#include <stdint.h>

#include "registry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float in_min, in_max;
    float out_min, out_max;
    uint8_t clamp;           /* clamp output into [out_min,out_max] */
    rb_mod_sink_t sink;          /* optional next stage                 */
    void *sink_user;
    rb_mod_src_t src;            /* pull-mode input (registry mode)     */
    void *src_user;
} rb_map_cfg_t;

typedef struct {
    float in_min;
    float scale;             /* (out_max-out_min)/(in_max-in_min)   */
    float out_min, out_max;
    uint8_t clamp;
    rb_mod_sink_t sink;
    void *sink_user;
    rb_mod_src_t src;
    void *src_user;
    float last;              /* last output value                   */
} rb_map_t;

/* 0 ok; -1 on NULL or in_min == in_max (division by zero). */
int rb_map_init(rb_map_t *m, const rb_map_cfg_t *cfg);

/* Pure transform; also updates m->last. No sink call. */
float rb_map_apply(rb_map_t *m, float x);

/* Chain entry point — sink-compatible signature. Applies the map,
 * stores .last, forwards to cfg.sink if set. */
void rb_map_feed(float x, void *user);

/* Ready-made registry module for pull mode. Set .ctx to an
 * initialised rb_map_t* (with cfg.src set) and .period_ms. */
extern rb_module_t rb_map_mod;

#ifdef __cplusplus
}
#endif

#endif /* RB_MW_MAPRANGE_H */
