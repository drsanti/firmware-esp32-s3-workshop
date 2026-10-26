/*
 * sound.h — DSP chain contract for robotos SFX (ADR 0016).
 *
 * Pull-model audio pipeline: each stage is a struct + an
 * `apply(ctx, in) -> out` function transforming ONE sample. The
 * audio render task folds the stage array once per output sample at
 * RB_SFX_FS — never on the executor tick.
 *
 *   static rb_sfx_stage_t chain[] = {
 *       { &vco,      rb_sfx_vco_apply      },
 *       { &bq,       rb_sfx_biquad_apply   },
 *       { &env,      rb_sfx_adsr_apply     },
 *       { &echo,     rb_sfx_delay_apply    },
 *   };
 *   float y = rb_sfx_chain(chain, 4, 0.0f);  // source stage ignores `in`
 *
 * Control-rate work (sequencer steps, param changes, adsr_gate) is a
 * separate concern — call it from the executor or a cmd handler, not
 * from the render loop.
 */
#ifndef RB_MW_SOUND_H
#define RB_MW_SOUND_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RB_SFX_FS 16000.0f /* render sample rate, Hz, mono 16-bit out */

typedef struct rb_sfx_stage {
    void *ctx;
    float (*apply)(void *ctx, float in);
} rb_sfx_stage_t;

static inline float rb_sfx_chain(const rb_sfx_stage_t *stages, int n,
                              float in) {
    float y = in;
    for (int i = 0; i < n; i++) {
        y = stages[i].apply(stages[i].ctx, y);
    }
    return y;
}

/* float -1..+1 -> int16 PCM */
static inline int16_t rb_sfx_pcm16(float y) {
    if (y > 1.0f) y = 1.0f;
    if (y < -1.0f) y = -1.0f;
    return (int16_t)(y * 32767.0f);
}

#ifdef __cplusplus
}
#endif

#endif /* RB_MW_SOUND_H */
