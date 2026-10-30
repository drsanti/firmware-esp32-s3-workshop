/*
 * lab_beep.h - short tones via robotos rb_sfx (16 kHz).
 *
 * Voice: square + vibrato, LPF, ADSR, echo (firmware app_audio style).
 *
 *   beep_play(BEEP_CHIME, NULL);                 // preset, current knobs
 *   beep_play(BEEP_TONE, &(beep_params_t){       // one note, fine-tune
 *       .hz = 660, .ms = 120, .gain = 35,
 *   });
 *   beep_set_gain(80);                           // live knobs (sound pad)
 *
 * Params: NULL = kind defaults + current knobs.
 * Non-NULL: 0 on gain/echo/release = keep current knob;
 *           0 on hz/ms = kind default.
 */
#ifndef ROBOTOS_LAB_BEEP_H
#define ROBOTOS_LAB_BEEP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BEEP_TONE = 0, /* single note (default 880 Hz / 150 ms) */
    BEEP_CHIME,    /* rising C-E-G triad */
    BEEP_ALARM,    /* two-tone attention */
    BEEP_CLICK,    /* short tick */
} beep_kind_t;

typedef struct {
    uint8_t  gain;    /* 0..100 loudness; 0 = keep current */
    uint8_t  echo;    /* 0..100 echo;     0 = keep current */
    uint8_t  release; /* 0..100 fade-out; 0 = keep current */
    uint16_t hz;      /* BEEP_TONE / CLICK; 0 = kind default */
    uint16_t ms;      /* gate duration; 0 = kind default */
} beep_params_t;

/* Play one sound. Blocks until release/echo tail finishes. */
int beep_play(beep_kind_t kind, const beep_params_t *p);

/* Live knobs (sound pad G/E/R). Used when params leave a field at 0. */
void beep_set_gain(uint8_t percent);
uint8_t beep_get_gain(void);

void beep_set_echo(uint8_t percent);
uint8_t beep_get_echo(void);

void beep_set_release(uint8_t percent);
uint8_t beep_get_release(void);

/* Thin wrappers (same as beep_play with fixed args). */
int beep_tone(unsigned hz, unsigned ms);
int beep_once(void);
int beep_chime(void);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_LAB_BEEP_H */
