/*
 * lab_metro.h - Metronome face (BPM + beat + jitter lines).
 *
 *   metro_init();
 *   metro_start();
 *   metro_set_bpm(120);
 *   metro_flash_beat();
 *   metro_set_jitter(3, 12);  // last, max (ms)
 */
#ifndef ROBOTOS_LAB_METRO_H
#define ROBOTOS_LAB_METRO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void metro_init(void);
void metro_start(void);
void metro_set_bpm(unsigned bpm);
void metro_flash_beat(void); /* brief visual pulse on the beat row */
void metro_set_beat_idle(void);
void metro_set_jitter(int last_ms, int max_ms);
void metro_set_hint(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_LAB_METRO_H */
