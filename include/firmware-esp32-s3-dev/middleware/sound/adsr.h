/*
 * adsr.h — attack/decay/sustain/release envelope stage.
 * y = in * env; env driven by gate() transitions from control context.
 */
#ifndef RB_MW_ADSR_H
#define RB_MW_ADSR_H

#include "sound.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ADSR_IDLE = 0,
    ADSR_ATTACK,
    ADSR_DECAY,
    ADSR_SUSTAIN,
    ADSR_RELEASE,
} adsr_phase_t;

typedef struct {
    float attack_ms;
    float decay_ms;
    float sustain;      /* level 0..1 */
    float release_ms;
} rb_sfx_adsr_cfg_t;

typedef struct {
    float a_rate, d_rate, r_rate; /* env increment per sample */
    float sustain;
    float a_ms, d_ms, r_ms;       /* stored for live retune */
    float env;
    adsr_phase_t phase;
} rb_sfx_adsr_t;

int rb_sfx_adsr_init(rb_sfx_adsr_t *e, const rb_sfx_adsr_cfg_t *cfg);
void rb_sfx_adsr_gate(rb_sfx_adsr_t *e, int on); /* note on/off */
float rb_sfx_adsr_apply(void *ctx, float in);
/* nonzero while a note is sounding (attack..release) */
int rb_sfx_adsr_active(const rb_sfx_adsr_t *e);
/* live retune: field 0=attack_ms 1=decay_ms 2=sustain 3=release_ms */
int rb_sfx_adsr_set(rb_sfx_adsr_t *e, uint8_t field, float v);

#ifdef __cplusplus
}
#endif

#endif /* RB_MW_ADSR_H */
