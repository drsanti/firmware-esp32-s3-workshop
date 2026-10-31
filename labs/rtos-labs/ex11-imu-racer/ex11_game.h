/*
 * ex11_game — physics + state for the IMU road racer.
 */
#ifndef RTOS_LABS_EX11_GAME_H
#define RTOS_LABS_EX11_GAME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    EX11_PHASE_TITLE = 0,
    EX11_PHASE_PLAY = 1,
    EX11_PHASE_CRASH = 2
};

enum { EX11_MAX_OBS = 6, EX11_LANES = 3 };

typedef struct {
    float x; /* center of car, road coords */
    float y;
    float w;
    float h;
} ex11_car_t;

typedef struct {
    float x;
    float y;
    float w;
    float h;
    uint8_t alive;
} ex11_obs_t;

typedef struct {
    uint8_t phase;
    uint8_t live_imu; /* 1 = real IMU, 0 = sim */
    uint32_t score;
    uint32_t best;
    float scroll; /* dash offset */
    float speed;  /* px per frame */
    float tilt;   /* filtered steer input */
    ex11_car_t car;
    ex11_obs_t obs[EX11_MAX_OBS];
    uint16_t screen_w;
    uint16_t screen_h;
    uint16_t road_x;
    uint16_t road_w;
} ex11_game_t;

typedef enum {
    EX11_SFX_NONE = 0,
    EX11_SFX_START,
    EX11_SFX_CLICK,
    EX11_SFX_CRASH
} ex11_sfx_t;

void ex11_game_init(ex11_game_t *g, uint16_t w, uint16_t h);
void ex11_game_reset_play(ex11_game_t *g);

/* ax = horizontal accel (m/s^2). Returns sfx event if any. */
ex11_sfx_t ex11_game_tick(ex11_game_t *g, float ax, int live_imu);

/* Title / crash → play. Returns sfx. */
ex11_sfx_t ex11_game_on_start(ex11_game_t *g);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX11_GAME_H */
