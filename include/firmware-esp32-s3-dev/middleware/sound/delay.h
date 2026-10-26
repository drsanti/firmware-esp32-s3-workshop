/*
 * delay.h — feedback echo stage. Caller-owned ring buffer:
 *   static float dbuf[SFX_DELAY_BUF(250)];   // 250 ms
 *   rb_sfx_delay_init(&d, dbuf, 250.0f, 0.45f, 0.35f);
 */
#ifndef RB_MW_DELAY_H
#define RB_MW_DELAY_H

#include "sound.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ms -> samples at 16 kHz; integer math so it's legal as a
 * file-scope array size: (ms) * 16000 / 1000 = (ms) * 16 */
#define SFX_DELAY_BUF(ms) ((int)((ms) * 16))

typedef struct {
    float *buf;      /* caller-owned ring, size = delay samples */
    int len;
    int wr;
    float feedback;  /* 0..0.95 — echo regeneration */
    float mix;       /* 0..1 — wet level */
} rb_sfx_delay_t;

int rb_sfx_delay_init(rb_sfx_delay_t *d, float *buf, float time_ms,
                   float feedback, float mix);
float rb_sfx_delay_apply(void *ctx, float in);
void rb_sfx_delay_set_feedback(rb_sfx_delay_t *d, float fb);
void rb_sfx_delay_set_mix(rb_sfx_delay_t *d, float mix);

#ifdef __cplusplus
}
#endif

#endif /* RB_MW_DELAY_H */
