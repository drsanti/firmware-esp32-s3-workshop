/*
 * vco.h — audio-rate oscillator stage: the existing wave module
 * adapted to per-sample rendering, with optional FM (vibrato).
 *
 *   rb_sfx_vco_t v;
 *   rb_wave_t lfo;  rb_wave_init(&lfo, &(rb_wave_cfg_t){ .freq_hz=6, ... });
 *   rb_sfx_vco_init(&v, &(rb_sfx_vco_cfg_t){
 *       .freq_hz = 440, .shape = RB_WAVE_SHAPE_SQUARE,
 *       .fm_src = lfo_src, .fm_user = &lfo, .fm_depth_hz = 8 });
 */
#ifndef RB_MW_VCO_H
#define RB_MW_VCO_H

#include "../wave.h"
#include "sound.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float freq_hz;
    float amplitude;    /* 0..1, keep headroom for the chain */
    float offset;
    float phase_deg;
    rb_wave_shape_t shape;
    /* optional frequency modulation (vibrato) — fm_src() returns
     * roughly -1..+1, scaled by fm_depth_hz */
    float (*fm_src)(void *user);
    void *fm_user;
    float fm_depth_hz;
} rb_sfx_vco_cfg_t;

typedef struct {
    rb_wave_t osc;
    float base_freq;
    float (*fm_src)(void *user);
    void *fm_user;
    float fm_depth;
} rb_sfx_vco_t;

int rb_sfx_vco_init(rb_sfx_vco_t *v, const rb_sfx_vco_cfg_t *cfg);
float rb_sfx_vco_apply(void *ctx, float in);   /* in ignored — source */
void rb_sfx_vco_set_freq(rb_sfx_vco_t *v, float hz);

#ifdef __cplusplus
}
#endif

#endif /* RB_MW_VCO_H */
