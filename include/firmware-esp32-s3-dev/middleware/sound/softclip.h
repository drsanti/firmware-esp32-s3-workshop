/*
 * softclip.h — tanh waveshaper: gentle saturation → grit/alarms.
 * y = tanh(drive * in) / tanh(drive) — unity gain at small signals.
 */
#ifndef RB_MW_SOFTCLIP_H
#define RB_MW_SOFTCLIP_H

#include "sound.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float drive;   /* >= 1.0; 1 = nearly clean, 8+ = aggressive */
    float inv_tn;  /* 1/tanh(drive), precomputed normalization */
} rb_sfx_softclip_t;

int rb_sfx_softclip_init(rb_sfx_softclip_t *c, float drive);
void rb_sfx_softclip_set_drive(rb_sfx_softclip_t *c, float drive);
float rb_sfx_softclip_apply(void *ctx, float in);

#ifdef __cplusplus
}
#endif

#endif /* RB_MW_SOFTCLIP_H */
