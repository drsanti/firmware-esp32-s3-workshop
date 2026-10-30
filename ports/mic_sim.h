/*
 * mic_sim.h — synthetic mono PCM when the board has no microphone.
 *
 * Tone + slow AM envelope + noise (demo-quality, no libm).
 *
 *   int16_t mono[64];
 *   float t = 0.f;
 *   uint32_t rng = 1;
 *   mic_sim_fill(mono, 64, t, &rng);
 *   t += 64.f / 16000.f;
 */
#ifndef ROBOTOS_MIC_SIM_H
#define ROBOTOS_MIC_SIM_H

#include <stddef.h>
#include <stdint.h>

#ifndef MIC_SIM_PI
#define MIC_SIM_PI 3.14159265358979323846f
#endif

#ifndef MIC_SIM_FS_HZ
#define MIC_SIM_FS_HZ 16000.0f
#endif

/* Soft sin — wrap to [-pi, pi], 5th-order Taylor. Demo-quality only. */
static inline float mic_sim_sin(float x)
{
    const float pi = MIC_SIM_PI;
    const float twopi = 2.0f * MIC_SIM_PI;
    float y2;

    x = x - twopi * (float)(int)(x / twopi + (x >= 0.0f ? 0.5f : -0.5f));
    if (x > pi) {
        x -= twopi;
    }
    if (x < -pi) {
        x += twopi;
    }
    y2 = x * x;
    return x * (1.0f - y2 * (1.0f / 6.0f - y2 * (1.0f / 120.0f)));
}

/* Uniform-ish noise in ~[-1, 1]. */
static inline float mic_sim_noise(uint32_t *rng)
{
    uint32_t r;
    int n;

    r = (*rng * 1664525u) + 1013904223u;
    *rng = r;
    n = (int)((r >> 16) & 0xffffu) - 32768;
    return (float)n / 32768.0f;
}

/* Fill n mono int16 samples starting at time t_sec (sample 0). */
static inline void mic_sim_fill(int16_t *mono, int n, float t_sec,
                                uint32_t *rng)
{
    const float twopi = 2.0f * MIC_SIM_PI;
    float dt = 1.0f / MIC_SIM_FS_HZ;
    float t;
    int i;

    if (mono == NULL || n <= 0 || rng == NULL) {
        return;
    }
    if (*rng == 0u) {
        *rng = 1u;
    }

    t = t_sec;
    for (i = 0; i < n; i++) {
        /* Louder, more AM so SCROLL/VU move clearly in sim mode. */
        float env = 0.55f + 0.40f * mic_sim_sin(twopi * 1.4f * t);
        float carrier = mic_sim_sin(twopi * 440.0f * t);
        float y = env * carrier + 0.06f * mic_sim_noise(rng);
        int v;

        if (y > 1.0f) {
            y = 1.0f;
        }
        if (y < -1.0f) {
            y = -1.0f;
        }
        v = (int)(y * 28000.0f);
        if (v > 32767) {
            v = 32767;
        }
        if (v < -32768) {
            v = -32768;
        }
        mono[i] = (int16_t)v;
        t += dt;
    }
}

#endif /* ROBOTOS_MIC_SIM_H */
