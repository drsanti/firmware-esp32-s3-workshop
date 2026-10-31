/*
 * ex15_game — Frogger lanes + phases.
 */
#ifndef RTOS_LABS_EX15_GAME_H
#define RTOS_LABS_EX15_GAME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    EX15_PHASE_TITLE = 0,
    EX15_PHASE_PLAY = 1,
    EX15_PHASE_LIFE = 2,
    EX15_PHASE_WIN = 3,
    EX15_PHASE_OVER = 4
};

enum {
    EX15_ROWS = 9,
    EX15_MAX_ACTORS = 20,
    EX15_GOALS = 5,
    EX15_START_LIVES = 3,
    EX15_ACT_CAR = 0,
    EX15_ACT_LOG = 1
};

/* Row kinds (bottom = 0) */
enum {
    EX15_LANE_SAFE = 0,
    EX15_LANE_ROAD = 1,
    EX15_LANE_WATER = 2,
    EX15_LANE_GOAL = 3
};

typedef struct {
    float x;
    float w;
    float speed; /* +right / -left */
    uint8_t row;
    uint8_t kind; /* CAR / LOG */
    uint8_t alive;
} ex15_actor_t;

typedef struct {
    uint8_t phase;
    uint8_t live_imu;
    uint8_t lives;
    uint8_t wave;
    uint16_t life_flash;
    uint16_t move_cd;
    uint16_t hop_lock;
    uint32_t score;
    uint32_t best;
    float tilt;
    float frog_x;
    uint8_t frog_row;
    uint8_t goals[EX15_GOALS];
    uint8_t goals_filled;
    ex15_actor_t actors[EX15_MAX_ACTORS];
    uint8_t lane_kind[EX15_ROWS];
    uint16_t screen_w;
    uint16_t screen_h;
    uint16_t cell_w;
    uint16_t cell_h;
} ex15_game_t;

typedef enum {
    EX15_SFX_NONE = 0,
    EX15_SFX_START,
    EX15_SFX_HOP,
    EX15_SFX_GOAL,
    EX15_SFX_LOSE,
    EX15_SFX_WIN
} ex15_sfx_t;

void ex15_game_init(ex15_game_t *g, uint16_t w, uint16_t h);
ex15_sfx_t ex15_game_on_start(ex15_game_t *g);
ex15_sfx_t ex15_game_tick(ex15_game_t *g, float ax, int live_imu, int hop);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX15_GAME_H */
