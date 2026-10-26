/*
 * seq.h — control-side note/effect sequencer. NOT an audio stage:
 * it runs on the executor (call seq_tick every ~1-10 ms) and retunes
 * the VCO / gates the envelope. A preset is a table of steps:
 *
 *   static const rb_sfx_seq_step_t alarm[] = {
 *       { 880, RB_WAVE_SHAPE_SQUARE, 150 },
 *       { 660, RB_WAVE_SHAPE_SQUARE, 150 },
 *       { 880, RB_WAVE_SHAPE_SQUARE, 150 },
 *       { 0,   0,                 200 },   // freq 0 = rest (gate off)
 *   };
 */
#ifndef RB_MW_SEQ_H
#define RB_MW_SEQ_H

#include "../wave.h"
#include "adsr.h"
#include "vco.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float freq_hz;      /* 0 = rest step (envelope gated off) */
    rb_wave_shape_t shape; /* ignored when freq_hz == 0 */
    uint16_t dur_ms;
} rb_sfx_seq_step_t;

typedef struct {
    rb_sfx_vco_t *vco;     /* retuned per step                       */
    rb_sfx_adsr_t *env;    /* gated per step (may be NULL = free run) */
    const rb_sfx_seq_step_t *steps;
    int n_steps;
    int loop;           /* 1 = repeat, 0 = one-shot then idle      */
    uint32_t max_loops; /* 0 = honor `loop`; N = stop after N plays */
    uint32_t dur_ms;    /* 0 = unlimited; N = stop after N ms      */
} rb_sfx_seq_cfg_t;

typedef struct {
    rb_sfx_vco_t *vco;
    rb_sfx_adsr_t *env;
    const rb_sfx_seq_step_t *steps;
    int n_steps;
    int loop;
    uint32_t max_loops;
    uint32_t budget_ms;  /* counts down; 0 budget = unlimited */
    uint32_t plays;
    int idx;
    uint32_t t_left_ms;
    int running;
} rb_sfx_seq_t;

int rb_sfx_seq_init(rb_sfx_seq_t *s, const rb_sfx_seq_cfg_t *cfg);
void rb_sfx_seq_start(rb_sfx_seq_t *s);   /* (re)start from step 0 */
void rb_sfx_seq_stop(rb_sfx_seq_t *s);    /* gate off + idle */
int rb_sfx_seq_running(const rb_sfx_seq_t *s);
/* call every ~1-10 ms from control context */
void rb_sfx_seq_tick(rb_sfx_seq_t *s, uint32_t dt_ms);

#ifdef __cplusplus
}
#endif

#endif /* RB_MW_SEQ_H */
