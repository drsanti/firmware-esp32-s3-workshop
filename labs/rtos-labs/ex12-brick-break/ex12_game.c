#include "ex12_game.h"

#include <string.h>

#define PAD_W 56.0f
#define PAD_H 10.0f
#define BALL_R 5.0f
#define BRICK_GAP 3.0f
#define DEADZONE 0.35f
#define STEER_GAIN 10.0f
#define BALL_SPEED 3.6f
#define LIFE_FLASH_FRAMES 45u

static float clampf(float v, float lo, float hi)
{
    if (v < lo) {
        return lo;
    }
    if (v > hi) {
        return hi;
    }
    return v;
}

static int circle_rect(float cx, float cy, float r, float rx, float ry, float rw,
                       float rh)
{
    float nx = clampf(cx, rx, rx + rw);
    float ny = clampf(cy, ry, ry + rh);
    float dx = cx - nx;
    float dy = cy - ny;
    return (dx * dx + dy * dy) <= (r * r);
}

static void layout_bricks(ex12_game_t *g)
{
    unsigned r;
    unsigned c;
    float top = 28.0f;
    float side = 8.0f;
    float area_w = (float)g->screen_w - side * 2.0f;
    float bw = (area_w - BRICK_GAP * (float)(EX12_BRICK_COLS - 1)) /
               (float)EX12_BRICK_COLS;
    float bh = 14.0f;

    for (r = 0; r < EX12_BRICK_ROWS; r++) {
        for (c = 0; c < EX12_BRICK_COLS; c++) {
            ex12_brick_t *b = &g->bricks[r * EX12_BRICK_COLS + c];
            b->alive = 1;
            b->row = (uint8_t)r;
            b->w = bw;
            b->h = bh;
            b->x = side + (float)c * (bw + BRICK_GAP);
            b->y = top + (float)r * (bh + BRICK_GAP);
        }
    }
}

static void ball_on_paddle(ex12_game_t *g)
{
    g->ball.stuck = 1;
    g->ball.r = BALL_R;
    g->ball.vx = 0;
    g->ball.vy = 0;
    g->ball.x = g->pad.x + g->pad.w * 0.5f;
    g->ball.y = g->pad.y - BALL_R - 1.0f;
}

static void launch_ball(ex12_game_t *g)
{
    g->ball.stuck = 0;
    g->ball.vx = BALL_SPEED * 0.6f;
    g->ball.vy = -BALL_SPEED;
}

static void reset_level(ex12_game_t *g, int full)
{
    g->pad.w = PAD_W;
    g->pad.h = PAD_H;
    g->pad.x = (float)g->screen_w * 0.5f - PAD_W * 0.5f;
    g->pad.y = (float)g->screen_h - 28.0f;
    g->tilt = 0;
    if (full) {
        g->score = 0;
        g->lives = EX12_START_LIVES;
        layout_bricks(g);
    }
    ball_on_paddle(g);
}

void ex12_game_init(ex12_game_t *g, uint16_t w, uint16_t h)
{
    memset(g, 0, sizeof(*g));
    g->screen_w = w;
    g->screen_h = h;
    g->phase = EX12_PHASE_TITLE;
    g->best = 0;
    g->lives = EX12_START_LIVES;
    reset_level(g, 1);
}

ex12_sfx_t ex12_game_on_start(ex12_game_t *g)
{
    if (g->phase == EX12_PHASE_PLAY) {
        /* Launch if ball stuck; else ignore mid-play tap. */
        if (g->ball.stuck) {
            launch_ball(g);
            return EX12_SFX_CLICK;
        }
        return EX12_SFX_NONE;
    }
    if (g->phase == EX12_PHASE_LIFE) {
        g->phase = EX12_PHASE_PLAY;
        ball_on_paddle(g);
        return EX12_SFX_NONE;
    }
    if (g->phase == EX12_PHASE_OVER || g->phase == EX12_PHASE_CLEAR ||
        g->phase == EX12_PHASE_TITLE) {
        reset_level(g, 1);
        g->phase = EX12_PHASE_PLAY;
        return EX12_SFX_START;
    }
    return EX12_SFX_NONE;
}

static unsigned bricks_left(const ex12_game_t *g)
{
    unsigned i;
    unsigned n = 0;

    for (i = 0; i < EX12_BRICK_ROWS * EX12_BRICK_COLS; i++) {
        if (g->bricks[i].alive) {
            n++;
        }
    }
    return n;
}

ex12_sfx_t ex12_game_tick(ex12_game_t *g, float ax, int live_imu)
{
    ex12_sfx_t sfx = EX12_SFX_NONE;
    float steer;
    unsigned i;

    g->live_imu = live_imu ? 1u : 0u;

    if (g->phase == EX12_PHASE_LIFE) {
        if (g->life_flash > 0u) {
            g->life_flash--;
        } else {
            g->phase = EX12_PHASE_PLAY;
            ball_on_paddle(g);
        }
        /* Still allow paddle move during flash. */
    }

    if (g->phase != EX12_PHASE_PLAY && g->phase != EX12_PHASE_LIFE) {
        return EX12_SFX_NONE;
    }

    g->tilt = g->tilt * 0.75f + ax * 0.25f;
    steer = g->tilt;
    if (steer > -DEADZONE && steer < DEADZONE) {
        steer = 0.0f;
    }
    g->pad.x += steer * STEER_GAIN;
    g->pad.x = clampf(g->pad.x, 4.0f, (float)g->screen_w - g->pad.w - 4.0f);

    if (g->ball.stuck) {
        g->ball.x = g->pad.x + g->pad.w * 0.5f;
        g->ball.y = g->pad.y - BALL_R - 1.0f;
        return EX12_SFX_NONE;
    }

    if (g->phase == EX12_PHASE_LIFE) {
        return EX12_SFX_NONE;
    }

    g->ball.x += g->ball.vx;
    g->ball.y += g->ball.vy;

    /* Walls */
    if (g->ball.x - g->ball.r < 0.0f) {
        g->ball.x = g->ball.r;
        g->ball.vx = -g->ball.vx;
        sfx = EX12_SFX_CLICK;
    } else if (g->ball.x + g->ball.r > (float)g->screen_w) {
        g->ball.x = (float)g->screen_w - g->ball.r;
        g->ball.vx = -g->ball.vx;
        sfx = EX12_SFX_CLICK;
    }
    if (g->ball.y - g->ball.r < 18.0f) {
        g->ball.y = 18.0f + g->ball.r;
        g->ball.vy = -g->ball.vy;
        sfx = EX12_SFX_CLICK;
    }

    /* Paddle */
    if (g->ball.vy > 0.0f &&
        circle_rect(g->ball.x, g->ball.y, g->ball.r, g->pad.x, g->pad.y,
                    g->pad.w, g->pad.h)) {
        float hit = (g->ball.x - (g->pad.x + g->pad.w * 0.5f)) / (g->pad.w * 0.5f);
        hit = clampf(hit, -1.0f, 1.0f);
        g->ball.vy = -BALL_SPEED;
        g->ball.vx = hit * BALL_SPEED * 1.2f;
        g->ball.y = g->pad.y - g->ball.r - 0.5f;
        sfx = EX12_SFX_CLICK;
    }

    /* Bricks */
    for (i = 0; i < EX12_BRICK_ROWS * EX12_BRICK_COLS; i++) {
        ex12_brick_t *b = &g->bricks[i];
        float cx;
        float cy;

        if (!b->alive) {
            continue;
        }
        if (!circle_rect(g->ball.x, g->ball.y, g->ball.r, b->x, b->y, b->w,
                         b->h)) {
            continue;
        }
        b->alive = 0;
        g->score += (uint32_t)(EX12_BRICK_ROWS - b->row) * 10u;
        cx = b->x + b->w * 0.5f;
        cy = b->y + b->h * 0.5f;
        if ((g->ball.x - cx) * (g->ball.x - cx) >
            (g->ball.y - cy) * (g->ball.y - cy)) {
            g->ball.vx = -g->ball.vx;
        } else {
            g->ball.vy = -g->ball.vy;
        }
        sfx = EX12_SFX_CLICK;
        break; /* one brick per frame */
    }

    if (bricks_left(g) == 0u) {
        g->phase = EX12_PHASE_CLEAR;
        if (g->score > g->best) {
            g->best = g->score;
        }
        return EX12_SFX_CLEAR;
    }

    /* Miss bottom */
    if (g->ball.y - g->ball.r > (float)g->screen_h) {
        if (g->lives > 1u) {
            g->lives--;
            g->phase = EX12_PHASE_LIFE;
            g->life_flash = LIFE_FLASH_FRAMES;
            return EX12_SFX_LOSE;
        }
        g->lives = 0;
        g->phase = EX12_PHASE_OVER;
        if (g->score > g->best) {
            g->best = g->score;
        }
        return EX12_SFX_LOSE;
    }

    return sfx;
}
