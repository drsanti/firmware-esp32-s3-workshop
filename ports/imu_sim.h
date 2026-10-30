/*
 * imu_sim.h — synthetic 6-axis when the board has no IMU.
 *
 * Accel (m/s^2): phase-offset sines + noise, az biased near +g.
 * Gyro  (rad/s): slower sines + noise.
 *
 * Soft sin (no libm) for a tiny demo helper.
 *
 *   uint32_t rng = 1;
 *   float t = 0.f;
 *   imu_sim_fill(acc, gyro, t, &rng);
 *   t += 0.033f;
 */
#ifndef ROBOTOS_IMU_SIM_H
#define ROBOTOS_IMU_SIM_H

#include <stddef.h>
#include <stdint.h>

#ifndef IMU_SIM_PI
#define IMU_SIM_PI 3.14159265358979323846f
#endif

/* Soft sin — wrap to [-pi, pi], 5th-order Taylor. Demo-quality only. */
static inline float imu_sim_sin(float x)
{
    const float pi = IMU_SIM_PI;
    const float twopi = 2.0f * IMU_SIM_PI;
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
static inline float imu_sim_noise(uint32_t *rng)
{
    uint32_t r;
    int n;

    r = (*rng * 1664525u) + 1013904223u;
    *rng = r;
    n = (int)((r >> 16) & 0xffffu) - 32768;
    return (float)n / 32768.0f;
}

static inline void imu_sim_fill(float acc[3], float gyro[3], float t_sec,
                                uint32_t *rng)
{
    float twopi = 2.0f * IMU_SIM_PI;
    float n;

    if (acc == NULL || gyro == NULL || rng == NULL) {
        return;
    }
    if (*rng == 0u) {
        *rng = 1u;
    }

    n = imu_sim_noise(rng) * 0.25f;
    acc[0] = 3.0f * imu_sim_sin(twopi * 0.37f * t_sec) + n;
    n = imu_sim_noise(rng) * 0.25f;
    acc[1] = 3.0f * imu_sim_sin(twopi * 0.51f * t_sec + 1.3f) + n;
    n = imu_sim_noise(rng) * 0.20f;
    acc[2] = 9.81f + 1.5f * imu_sim_sin(twopi * 0.23f * t_sec + 0.4f) + n;

    n = imu_sim_noise(rng) * 0.05f;
    gyro[0] = 0.9f * imu_sim_sin(twopi * 0.61f * t_sec) + n;
    n = imu_sim_noise(rng) * 0.05f;
    gyro[1] = 0.7f * imu_sim_sin(twopi * 0.43f * t_sec + 2.1f) + n;
    n = imu_sim_noise(rng) * 0.05f;
    gyro[2] = 0.5f * imu_sim_sin(twopi * 0.29f * t_sec + 0.8f) + n;
}

#endif /* ROBOTOS_IMU_SIM_H */
