/*
 * lab_beep.c - lab tones on rb_sfx.
 *
 * Voice matched to firmware-esp32-s3-dev app_audio:
 *   VCO (square + vibrato LFO) -> LPF -> softclip -> ADSR -> echo
 */
#include "lab_beep.h"

#include "bsp.h"
#include "middleware/rb_sfx.h"
#include "middleware/rb_wave.h"

#include <string.h>

#ifndef LAB_BEEP_MAX_MS
#define LAB_BEEP_MAX_MS 200
#endif

#ifndef LAB_BEEP_ECHO_MS
#define LAB_BEEP_ECHO_MS 220
#endif

#ifndef LAB_BEEP_CHUNK
#define LAB_BEEP_CHUNK 64
#endif

static uint8_t s_gain = 40;    /* quieter default — avoid speaker blast */
static uint8_t s_echo = 45;    /* matches page_sfx default echo */
static uint8_t s_release = 30; /* ~120 ms */
static int s_ready;

static rb_wave_t s_lfo;
static rb_sfx_vco_t s_vco;
static rb_sfx_biquad_t s_lpf;
static rb_sfx_softclip_t s_clip;
static rb_sfx_adsr_t s_env;
static rb_sfx_delay_t s_echo_fx;
static float s_dbuf[SFX_DELAY_BUF(LAB_BEEP_ECHO_MS)];

static float lfo_read(void *user)
{
    return ((rb_wave_t *)user)->last;
}

static void apply_echo_knob(void)
{
    /* page_sfx: echo slider -> feedback 0..0.9; mix stays ~0.35 */
    float fb = ((float)s_echo / 100.0f) * 0.9f;
    float mix = 0.20f + ((float)s_echo / 100.0f) * 0.35f; /* 0.20..0.55 */
    if (fb > 0.9f) {
        fb = 0.9f;
    }
    if (mix > 1.0f) {
        mix = 1.0f;
    }
    rb_sfx_delay_set_feedback(&s_echo_fx, fb);
    rb_sfx_delay_set_mix(&s_echo_fx, mix);
}

static void apply_release_knob(void)
{
    float ms = (float)s_release * 4.0f; /* 0..400, page_sfx scale */
    (void)rb_sfx_adsr_set(&s_env, 3, ms);
}

static int ensure_ready(void)
{
    if (s_ready) {
        return 0;
    }
    if (bsp_audio_init() != 0) {
        return -1;
    }

    (void)rb_wave_init(&s_lfo, &(rb_wave_cfg_t){
                                   .freq_hz = 6.0f,
                                   .amplitude = 1.0f,
                               });
    (void)rb_sfx_vco_init(&s_vco, &(rb_sfx_vco_cfg_t){
                                      .freq_hz = 880.0f,
                                      .amplitude = 0.28f,
                                      .shape = RB_WAVE_SHAPE_SQUARE,
                                      .fm_src = lfo_read,
                                      .fm_user = &s_lfo,
                                      .fm_depth_hz = 10.0f,
                                  });
    rb_sfx_biquad_init(&s_lpf);
    (void)rb_sfx_biquad_set_lpf(&s_lpf, 2400.0f, 0.9f);
    (void)rb_sfx_softclip_init(&s_clip, 1.6f);
    (void)rb_sfx_adsr_init(&s_env, &(rb_sfx_adsr_cfg_t){
                                       .attack_ms = 8.0f,
                                       .decay_ms = 60.0f,
                                       .sustain = 0.6f,
                                       .release_ms = 120.0f,
                                   });
    (void)rb_sfx_delay_init(&s_echo_fx, s_dbuf, (float)LAB_BEEP_ECHO_MS,
                            0.45f, 0.35f);
    apply_echo_knob();
    apply_release_knob();
    s_ready = 1;
    return 0;
}

static size_t fill_stereo(int16_t *stereo, size_t frames)
{
    static const rb_sfx_stage_t chain[] = {
        {(void *)&s_vco, rb_sfx_vco_apply},
        {(void *)&s_lpf, rb_sfx_biquad_apply},
        {(void *)&s_clip, rb_sfx_softclip_apply},
        {(void *)&s_env, rb_sfx_adsr_apply},
        {(void *)&s_echo_fx, rb_sfx_delay_apply},
    };
    size_t i;

    for (i = 0; i < frames; i++) {
        (void)rb_wave_execute_dt(&s_lfo, 1.0f / RB_SFX_FS);
        float y = rb_sfx_chain(chain, 5, 0.0f);
        int16_t s = rb_sfx_pcm16(y);
        stereo[2u * i] = s;
        stereo[2u * i + 1u] = s;
    }
    return frames;
}

static void write_silence(unsigned chunks)
{
    static int16_t z[LAB_BEEP_CHUNK * 2];
    unsigned n;

    memset(z, 0, sizeof(z));
    for (n = 0; n < chunks; n++) {
        (void)bsp_audio_write(z, LAB_BEEP_CHUNK);
    }
}

void beep_set_gain(uint8_t percent)
{
    if (percent > 100u) {
        percent = 100u;
    }
    s_gain = percent;
}

uint8_t beep_get_gain(void)
{
    return s_gain;
}

void beep_set_echo(uint8_t percent)
{
    if (percent > 100u) {
        percent = 100u;
    }
    s_echo = percent;
    if (s_ready) {
        apply_echo_knob();
    }
}

uint8_t beep_get_echo(void)
{
    return s_echo;
}

void beep_set_release(uint8_t percent)
{
    if (percent > 100u) {
        percent = 100u;
    }
    s_release = percent;
    if (s_ready) {
        apply_release_knob();
    }
}

uint8_t beep_get_release(void)
{
    return s_release;
}

static int tone_with_tail(unsigned hz, unsigned ms, unsigned tail_ms)
{
    static int16_t pcm[LAB_BEEP_CHUNK * 2];
    unsigned gate_frames;
    unsigned tail_frames;
    unsigned left;
    unsigned n;

    if (hz < 40u) {
        hz = 40u;
    }
    if (ms < 1u) {
        ms = 1u;
    }
    if (ms > (unsigned)LAB_BEEP_MAX_MS) {
        ms = (unsigned)LAB_BEEP_MAX_MS;
    }

    if (ensure_ready() != 0) {
        return -1;
    }

    bsp_audio_volume(s_gain);
    rb_sfx_vco_set_freq(&s_vco, (float)hz);
    rb_sfx_adsr_gate(&s_env, 1);

    gate_frames = ((unsigned)RB_SFX_FS * ms) / 1000u;
    if (gate_frames < 1u) {
        gate_frames = 1u;
    }

    left = gate_frames;
    while (left > 0u) {
        n = left > (unsigned)LAB_BEEP_CHUNK ? (unsigned)LAB_BEEP_CHUNK : left;
        (void)fill_stereo(pcm, n);
        if (bsp_audio_write(pcm, n) == 0) {
            rb_sfx_adsr_gate(&s_env, 0);
            return -1;
        }
        left -= n;
    }

    rb_sfx_adsr_gate(&s_env, 0);

    tail_frames = ((unsigned)RB_SFX_FS * tail_ms) / 1000u;
    left = tail_frames;
    while (left > 0u) {
        n = left > (unsigned)LAB_BEEP_CHUNK ? (unsigned)LAB_BEEP_CHUNK : left;
        (void)fill_stereo(pcm, n);
        (void)bsp_audio_write(pcm, n);
        left -= n;
        if (!rb_sfx_adsr_active(&s_env) && s_echo == 0u) {
            break;
        }
    }

    write_silence(1u);
    return 0;
}

/* Apply non-zero voice fields; caller restores knobs after play. */
static void voice_from_params(const beep_params_t *p)
{
    if (p == 0) {
        return;
    }
    if (p->gain != 0u) {
        beep_set_gain(p->gain);
    }
    if (p->echo != 0u) {
        beep_set_echo(p->echo);
    }
    if (p->release != 0u) {
        beep_set_release(p->release);
    }
}

static int play_chime_notes(void)
{
    if (tone_with_tail(523u, 100u, 90u) != 0) {
        return -1;
    }
    if (tone_with_tail(659u, 100u, 90u) != 0) {
        return -1;
    }
    return tone_with_tail(784u, 180u, 450u);
}

int beep_play(beep_kind_t kind, const beep_params_t *p)
{
    uint8_t save_g = s_gain;
    uint8_t save_e = s_echo;
    uint8_t save_r = s_release;
    int rc = -1;

    /* Soft presets when kind has no overrides (avoid speaker blast). */
    if (kind == BEEP_CHIME && p == 0) {
        uint8_t soft = save_g;
        if (soft > 35u) {
            soft = 35u;
        }
        if (soft < 15u) {
            soft = 15u;
        }
        beep_set_gain(soft);
        beep_set_release(22);
        beep_set_echo(25);
    } else if (kind == BEEP_CLICK && p == 0) {
        /* Metronome / ticks fire often — keep quieter than tones. */
        uint8_t soft = save_g;
        if (soft > 20u) {
            soft = 20u;
        }
        if (soft < 8u) {
            soft = 8u;
        }
        beep_set_gain(soft);
        beep_set_release(12);
        beep_set_echo(8);
    } else {
        voice_from_params(p);
    }

    switch (kind) {
    case BEEP_TONE: {
        unsigned hz = (p != 0 && p->hz != 0u) ? (unsigned)p->hz : 880u;
        unsigned ms = (p != 0 && p->ms != 0u) ? (unsigned)p->ms : 150u;
        rc = tone_with_tail(hz, ms, 450u);
        break;
    }
    case BEEP_CLICK: {
        unsigned hz = (p != 0 && p->hz != 0u) ? (unsigned)p->hz : 1200u;
        unsigned ms = (p != 0 && p->ms != 0u) ? (unsigned)p->ms : 40u;
        rc = tone_with_tail(hz, ms, 120u);
        break;
    }
    case BEEP_CHIME:
        rc = play_chime_notes();
        break;
    case BEEP_ALARM:
        if (tone_with_tail(880u, 120u, 40u) != 0) {
            rc = -1;
            break;
        }
        rc = tone_with_tail(660u, 140u, 350u);
        break;
    default:
        rc = -1;
        break;
    }

    beep_set_gain(save_g);
    beep_set_echo(save_e);
    beep_set_release(save_r);
    return rc;
}

int beep_tone(unsigned hz, unsigned ms)
{
    beep_params_t p = {0};
    p.hz = (uint16_t)hz;
    p.ms = (uint16_t)ms;
    return beep_play(BEEP_TONE, &p);
}

int beep_once(void)
{
    return beep_play(BEEP_TONE, &(beep_params_t){.hz = 880u, .ms = 80u});
}

int beep_chime(void)
{
    return beep_play(BEEP_CHIME, NULL);
}
