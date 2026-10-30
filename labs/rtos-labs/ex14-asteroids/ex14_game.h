/*
 * ex14_game — Asteroids physics + phases.
 */
#ifndef RTOS_LABS_EX14_GAME_H
#define RTOS_LABS_EX14_GAME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    EX14_PHASE_TITLE = 0,
    EX14_PHASE_PLAY = 1,
    EX14_PHASE_LIFE = 2,
    EX14_PHASE_OVER = 3
};

enum {
    EX14_MAX_SHOTS = 8,
    EX14_MAX_ROCKS = 24,
    EX14_START_LIVES = 3,
    EX14_ROCK_SMALL = 0,
    EX14_ROCK_MED = 1,
    EX14_ROCK_LARGE = 2
};

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    float angle; /* radians */
} ex14_ship_t;

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    uint8_t alive;
    uint8_t ttl;
} ex14_shot_t;

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    uint8_t size; /* SMALL/MED/LARGE */
    uint8_t alive;
} ex14_rock_t;

typedef struct {
    uint8_t phase;
    uint8_t live_imu;
    uint8_t lives;
    uint8_t wave;
    uint16_t life_flash;
    uint16_t invuln;
    uint16_t fire_cd;
    uint32_t score;
    uint32_t best;
    float tilt;
    ex14_ship_t ship;
    ex14_shot_t shots[EX14_MAX_SHOTS];
    ex14_rock_t rocks[EX14_MAX_ROCKS];
    uint16_t screen_w;
    uint16_t screen_h;
} ex14_game_t;

typedef enum {
    EX14_SFX_NONE = 0,
    EX14_SFX_START,
    EX14_SFX_HIT,
    EX14_SFX_BOOM,
    EX14_SFX_LOSE
} ex14_sfx_t;

void ex14_game_init(ex14_game_t *g, uint16_t w, uint16_t h);
ex14_sfx_t ex14_game_on_start(ex14_game_t *g);
ex14_sfx_t ex14_game_tick(ex14_game_t *g, float ax, int live_imu, int thrust);

float ex14_rock_radius(uint8_t size);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX14_GAME_H */
