/*
 * ex12_game — Breakout physics + phases.
 */
#ifndef RTOS_LABS_EX12_GAME_H
#define RTOS_LABS_EX12_GAME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    EX12_PHASE_TITLE = 0,
    EX12_PHASE_PLAY = 1,
    EX12_PHASE_LIFE = 2,
    EX12_PHASE_OVER = 3,
    EX12_PHASE_CLEAR = 4
};

enum { EX12_BRICK_COLS = 8, EX12_BRICK_ROWS = 4, EX12_START_LIVES = 3 };

typedef struct {
    float x;
    float y;
    float w;
    float h;
} ex12_paddle_t;

typedef struct {
    float x;
    float y;
    float r;
    float vx;
    float vy;
    uint8_t stuck; /* 1 = sitting on paddle before launch */
} ex12_ball_t;

typedef struct {
    float x;
    float y;
    float w;
    float h;
    uint8_t alive;
    uint8_t row; /* color / score weight */
} ex12_brick_t;

typedef struct {
    uint8_t phase;
    uint8_t live_imu;
    uint8_t lives;
    uint16_t life_flash; /* frames left in LIFE overlay */
    uint32_t score;
    uint32_t best;
    float tilt;
    ex12_paddle_t pad;
    ex12_ball_t ball;
    ex12_brick_t bricks[EX12_BRICK_ROWS * EX12_BRICK_COLS];
    uint16_t screen_w;
    uint16_t screen_h;
} ex12_game_t;

typedef enum {
    EX12_SFX_NONE = 0,
    EX12_SFX_START,
    EX12_SFX_CLICK,
    EX12_SFX_LOSE,
    EX12_SFX_CLEAR
} ex12_sfx_t;

void ex12_game_init(ex12_game_t *g, uint16_t w, uint16_t h);
ex12_sfx_t ex12_game_on_start(ex12_game_t *g);
ex12_sfx_t ex12_game_tick(ex12_game_t *g, float ax, int live_imu);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX12_GAME_H */
