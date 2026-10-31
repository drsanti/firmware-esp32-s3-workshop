/*
 * ex13_game — space-shooter physics + phases.
 */
#ifndef RTOS_LABS_EX13_GAME_H
#define RTOS_LABS_EX13_GAME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    EX13_PHASE_TITLE = 0,
    EX13_PHASE_PLAY = 1,
    EX13_PHASE_LIFE = 2,
    EX13_PHASE_OVER = 3
};

enum {
    EX13_MAX_SHOTS = 8,
    EX13_MAX_ENEMIES = 10,
    EX13_START_LIVES = 3
};

typedef struct {
    float x;
    float y;
    float w;
    float h;
} ex13_ship_t;

typedef struct {
    float x;
    float y;
    float w;
    float h;
    uint8_t alive;
} ex13_shot_t;

typedef struct {
    float x;
    float y;
    float w;
    float h;
    float vy;
    uint8_t alive;
} ex13_enemy_t;

typedef struct {
    uint8_t phase;
    uint8_t live_imu;
    uint8_t lives;
    uint16_t life_flash;
    uint16_t fire_cd;
    uint16_t spawn_cd;
    uint32_t score;
    uint32_t best;
    float tilt;
    float star_scroll;
    ex13_ship_t ship;
    ex13_shot_t shots[EX13_MAX_SHOTS];
    ex13_enemy_t enemies[EX13_MAX_ENEMIES];
    uint16_t screen_w;
    uint16_t screen_h;
} ex13_game_t;

typedef enum {
    EX13_SFX_NONE = 0,
    EX13_SFX_START,
    EX13_SFX_SHOT,
    EX13_SFX_HIT,
    EX13_SFX_LOSE
} ex13_sfx_t;

void ex13_game_init(ex13_game_t *g, uint16_t w, uint16_t h);
ex13_sfx_t ex13_game_on_start(ex13_game_t *g);
ex13_sfx_t ex13_game_tick(ex13_game_t *g, float ax, int live_imu);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX13_GAME_H */
