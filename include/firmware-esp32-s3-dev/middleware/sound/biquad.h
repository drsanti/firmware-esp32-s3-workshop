/*
 * biquad.h — second-order filter stage (RBJ biquad, direct form 1).
 * v1 supports lowpass (the classic synth tone) and highpass.
 */
#ifndef RB_MW_BIQUAD_H
#define RB_MW_BIQUAD_H

#include "sound.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float b0, b1, b2, a1, a2; /* normalized coeffs */
    float x1, x2, y1, y2;     /* state */
    float fc, q;              /* remembered for single-param tweaks */
    int hp;                   /* 0 = lpf, 1 = hpf */
} rb_sfx_biquad_t;

void rb_sfx_biquad_init(rb_sfx_biquad_t *b);
/* RBJ lowpass; fc_hz clamped to < FS/2, q ~0.5..20 */
int rb_sfx_biquad_set_lpf(rb_sfx_biquad_t *b, float fc_hz, float q);
/* RBJ highpass */
int rb_sfx_biquad_set_hpf(rb_sfx_biquad_t *b, float fc_hz, float q);
/* single-param updates — recompute coeffs keeping the other */
int rb_sfx_biquad_set_fc(rb_sfx_biquad_t *b, float fc_hz);
int rb_sfx_biquad_set_q(rb_sfx_biquad_t *b, float q);
float rb_sfx_biquad_apply(void *ctx, float in);

#ifdef __cplusplus
}
#endif

#endif /* RB_MW_BIQUAD_H */
