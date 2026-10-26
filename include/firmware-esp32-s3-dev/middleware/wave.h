/*
 * wave.h — periodic waveform generator middleware.
 *
 * Contract: rb_wave_execute() is called by the RTOS every 1 ms and
 * returns the next sample. Caller decides where the sample goes
 * (DAC, LED brightness, publish payload, ...).
 *
 * No heap, no blocking — safe at control-loop rates.
 */
#ifndef RB_MW_WAVE_H
#define RB_MW_WAVE_H

#include <stdint.h>

#include "registry.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Sample output uses the shared rb_mod_sink_t contract (registry.h) —
 * wire a stage's .sink to the next stage's feed. */

/* All shapes output the same frame: y = offset + amplitude*f(phase),
 * f in [-1, 1]. Square is bipolar — set offset==amplitude for 0..1. */
typedef enum {
    RB_WAVE_SHAPE_SIN = 0,
    RB_WAVE_SHAPE_SQUARE,   /* +A for half a period, -A for the rest */
    RB_WAVE_SHAPE_TRIANGLE, /* linear rise then linear fall        */
    RB_WAVE_SHAPE_SAWTOOTH, /* linear rise, instant fall           */
} rb_wave_shape_t;

typedef struct {
    float freq_hz;   /* output frequency                       */
    float amplitude; /* peak amplitude (y = offset + A*f(ph))  */
    float offset;    /* DC offset                              */
    float phase0;    /* initial phase, radians                 */
    rb_wave_shape_t shape;     /* default SIN when zeroed         */
    float duty_pct;    /* SQUARE only: on-time 1..99 (0→50%)   */
    rb_mod_sink_t sink;      /* optional per-sample output hook  */
    void *sink_user;
} rb_wave_cfg_t;

typedef struct {
    float phase;     /* running phase, radians (kept < 2π)     */
    float freq_hz;   /* dt comes from the caller, not a bake   */
    float amplitude;
    float offset;
    rb_wave_shape_t shape;
    float duty_pct;  /* SQUARE only: 1..99 (0 treated as 50) */
    rb_mod_sink_t sink;
    void *sink_user;
    float last;      /* most recent sample                   */
    int32_t cycles_left;   /* -1 = free-run (see rb_wave_burst) */
    uint8_t running;
} rb_wave_t;

/* dt is fixed at 1 ms — the RTOS tick contract. */
#define RB_WAVE_DT_S 0.001f

/* Returns 0 on success, -1 on bad cfg (NULL, freq <= 0). */
int rb_wave_init(rb_wave_t *s, const rb_wave_cfg_t *cfg);
void rb_wave_deinit(rb_wave_t *s);

/* Next sample, advancing phase by `dt_s` seconds. The module glue
 * passes period_ms-derived dt; direct callers may use rb_wave_execute()
 * which keeps the 1 ms convention. Returns `offset` when stopped. */
float rb_wave_execute_dt(rb_wave_t *s, float dt_s);
float rb_wave_execute(rb_wave_t *s);

/* Optional helpers: retune without deinit/init cycle. */
void rb_wave_set_freq(rb_wave_t *s, float freq_hz);
void rb_wave_set_amplitude(rb_wave_t *s, float amplitude);
void rb_wave_set_offset(rb_wave_t *s, float offset);
void rb_wave_stop(rb_wave_t *s);
void rb_wave_start(rb_wave_t *s);

/* Phase: phase0 in cfg sets it at init; rb_wave_set_phase retunes the
 * running phase (e.g. 3 channels at 0 / 120° / 240°). */
void rb_wave_set_phase(rb_wave_t *s, float phase_rad);
void rb_wave_set_shape(rb_wave_t *s, rb_wave_shape_t shape);
void rb_wave_set_duty(rb_wave_t *s, float duty_pct); /* SQUARE only */
#define RB_WAVE_DEG(d) ((d) * 0.017453292519943295f) /* deg -> rad */

/*
 * One-shot/burst: emit exactly `cycles` full periods then stop
 * (output settles at `offset`). Alerts like "blink 3 times" or
 * "beep once" without host-side timers.
 */
void rb_wave_burst(rb_wave_t *s, int32_t cycles);

/* ---- executor module glue --------------------------------------
 * Single instance: rb_wave_mod is ready-made — set .ctx/.period_ms.
 * Multiple instances (e.g. RGB = 3 waves at 120°): declare your own
 * rb_module_t per wave and bind it:
 *   rb_module_t mod_r;
 *   rb_wave_module_bind(&mod_r, &wave_r, "wave_r", 10);
 *   rb_mod_register(&mod_r);
 * ---------------------------------------------------------------- */
extern rb_module_t rb_wave_mod;
void rb_wave_module_bind(rb_module_t *m, rb_wave_t *s, const char *name,
                      uint32_t period_ms);

#ifdef __cplusplus
}
#endif

#endif /* RB_MW_WAVE_H */
