/*
 * mic.h — capture module: bsp_audio_read -> DSP chain -> taps.
 *
 * Same pull-chain contract as the SFX render path (rb_sfx_stage_t,
 * sound.h): the mic is the implicit *source* — each captured sample
 * enters the chain as `in` and stage[0] sees the raw float. Compose
 * front-end stages in app order, e.g.:
 *
 *   static const rb_sfx_stage_t MIC_CHAIN[] = {
 *       { &hpf,  rb_sfx_biquad_apply   },  // DC offset removal
 *       { &lpf,  rb_sfx_biquad_apply   },  // anti-hiss
 *       { &clip, rb_sfx_softclip_apply },  // limiter before upstream
 *   };
 *
 * Chain output fans out to two taps (both optional):
 *   level_cb — windowed RMS/peak, ~10 Hz while enabled (wire 0x23)
 *   pcm_cb   — post-chain mono int16 chunks (wire 0x24 stream)
 *
 * The module runs on the executor at a fixed period; every tick it
 * drains one I2S block. Never blocks — a slow read drops the block.
 */
#ifndef RB_MW_MIC_H
#define RB_MW_MIC_H

#include "../../serial_proto/serial.pb.h" /* generated proto types */

#include "../registry.h"
#include "sound.h"

#ifdef __cplusplus
extern "C" {
#endif

/* executor period = one I2S block of 16 kHz stereo frames */
#define RB_MIC_BLOCK_FRAMES 64 /* 4 ms */
#define RB_MIC_PCM_CHUNK    240 /* samples per 0x24 frame (payload-limited) */
#define RB_MIC_MAX_STAGES   8

/* window for level telemetry, in samples (100 ms @ 16 kHz) */
#define RB_MIC_LEVEL_WINDOW 1600

typedef void (*rb_mic_level_cb_t)(uint16_t rms, uint16_t peak,
                               uint8_t clipped, void *user);
typedef void (*rb_mic_pcm_cb_t)(const int16_t *samples, int n, void *user);

typedef struct mic {
    /* --- config: set at init / via rb_mic_apply_cmd --- */
    rb_sfx_stage_t       chain[RB_MIC_MAX_STAGES];
    int               chain_n;
    rb_mic_level_cb_t    level_cb;
    void             *level_user;
    rb_mic_pcm_cb_t      pcm_cb;
    void             *pcm_user;
    uint8_t           enabled;
    uint8_t           stream;
    uint8_t           gain_pct;
    uint8_t           mute;
    uint8_t           stage_mask; /* bit i runs chain[i] (0 = run all) */
    /* --- runtime --- */
    float    sum2;
    float    peak;
    uint32_t acc_n;
    int16_t  pcm[RB_MIC_PCM_CHUNK];
    int      pcm_n;
    /* last emitted level window — UI reads these (written by the
     * executor task; 16-bit reads are atomic on all our MCUs) */
    uint16_t last_rms;
    uint16_t last_peak;
    uint8_t  last_clipped;
} rb_mic_t;

/* cfg-only init; does not touch hardware — bsp_mic_* are called on
 * the first enabled tick (ports return -1 cleanly without a mic). */
void rb_mic_init(rb_mic_t *m, const rb_sfx_stage_t *chain, int chain_n,
              rb_mic_level_cb_t level_cb, void *level_user,
              rb_mic_pcm_cb_t pcm_cb, void *pcm_user);

/* wire decoder for CMD_MIC (0x86): applies fields marked present
 * (proto3 optional — absent fields stay untouched). Returns 0 ok,
 * <0 on board reject. Safe from any task — bsp_mic_* are register
 * writes / channel toggles, not per-sample state. */
int  rb_mic_apply_cmd(rb_mic_t *m, const robotos_serial_v1_MicCmd *cmd);

/* executor module wrapper */
void rb_mic_module_bind(rb_module_t *mod, rb_mic_t *mic);

#ifdef __cplusplus
}
#endif

#endif /* RB_MW_MIC_H */
