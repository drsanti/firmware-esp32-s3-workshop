#include "ex11_game.h"

#include <stdlib.h>
#include <string.h>

#define CAR_W 28.0f
#define CAR_H 36.0f
#define OBS_W 34.0f
#define OBS_H 30.0f
#define SHOULDER 36.0f
#define DEADZONE 0.35f
#define STEER_GAIN 9.0f
#define BASE_SPEED 3.2f
#define MAX_SPEED 9.0f
#define SPAWN_EVERY 28u
#define SCORE_EVERY 6u

static uint32_t s_rng = 1u;
static uint16_t s_spawn_cd;
static uint16_t s_score_cd;
static uint16_t s_click_cd;

static uint32_t rnd_u(void)
{
    s_rng = s_rng * 1664525u + 1013904223u;
    return s_rng;
}

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

static float lane_center_x(const ex11_game_t *g, unsigned lane)
{
    float lane_w = (float)g->road_w / (float)EX11_LANES;
    return (float)g->road_x + lane_w * ((float)lane + 0.5f);
}

static int aabb(float ax, float ay, float aw, float ah, float bx, float by,
                float bw, float bh)
{
    return (ax < bx + bw) && (ax + aw > bx) && (ay < by + bh) && (ay + ah > by);
}

void ex11_game_init(ex11_game_t *g, uint16_t w, uint16_t h)
{
    memset(g, 0, sizeof(*g));
    g->screen_w = w;
    g->screen_h = h;
    g->road_x = (uint16_t)SHOULDER;
    g->road_w = (uint16_t)((float)w - 2.0f * SHOULDER);
    g->phase = EX11_PHASE_TITLE;
    g->best = 0;
    g->speed = BASE_SPEED;
    g->car.w = CAR_W;
    g->car.h = CAR_H;
    g->car.x = (float)w * 0.5f - CAR_W * 0.5f;
    g->car.y = (float)h - CAR_H - 18.0f;
    s_spawn_cd = SPAWN_EVERY;
    s_score_cd = SCORE_EVERY;
    s_click_cd = 0;
}

void ex11_game_reset_play(ex11_game_t *g)
{
    unsigned i;

    g->phase = EX11_PHASE_PLAY;
    g->score = 0;
    g->speed = BASE_SPEED;
    g->scroll = 0;
    g->tilt = 0;
    g->car.x = (float)g->screen_w * 0.5f - CAR_W * 0.5f;
    for (i = 0; i < EX11_MAX_OBS; i++) {
        g->obs[i].alive = 0;
    }
    s_spawn_cd = SPAWN_EVERY / 2u;
    s_score_cd = SCORE_EVERY;
    s_click_cd = 0;
}

ex11_sfx_t ex11_game_on_start(ex11_game_t *g)
{
    if (g->phase == EX11_PHASE_PLAY) {
        return EX11_SFX_NONE;
    }
    ex11_game_reset_play(g);
    return EX11_SFX_START;
}

static void spawn_obs(ex11_game_t *g)
{
    unsigned i;
    unsigned lane = rnd_u() % EX11_LANES;

    for (i = 0; i < EX11_MAX_OBS; i++) {
        if (g->obs[i].alive) {
            continue;
        }
        g->obs[i].alive = 1;
        g->obs[i].w = OBS_W;
        g->obs[i].h = OBS_H;
        g->obs[i].x = lane_center_x(g, lane) - OBS_W * 0.5f;
        g->obs[i].y = -OBS_H - 4.0f;
        return;
    }
}

ex11_sfx_t ex11_game_tick(ex11_game_t *g, float ax, int live_imu)
{
    ex11_sfx_t sfx = EX11_SFX_NONE;
    unsigned i;
    float road_left;
    float road_right;
    float steer;

    g->live_imu = live_imu ? 1u : 0u;

    if (g->phase != EX11_PHASE_PLAY) {
        return EX11_SFX_NONE;
    }

    /* Low-pass tilt; deadzone then steer. */
    g->tilt = g->tilt * 0.75f + ax * 0.25f;
    steer = g->tilt;
    if (steer > -DEADZONE && steer < DEADZONE) {
        steer = 0.0f;
    }
    g->car.x += steer * STEER_GAIN;

    road_left = (float)g->road_x + 2.0f;
    road_right = (float)g->road_x + (float)g->road_w - g->car.w - 2.0f;
    g->car.x = clampf(g->car.x, road_left, road_right);

    g->scroll += g->speed;
    if (g->scroll > 28.0f) {
        g->scroll -= 28.0f;
    }

    if (s_spawn_cd > 0u) {
        s_spawn_cd--;
    } else {
        spawn_obs(g);
        s_spawn_cd = (uint16_t)(SPAWN_EVERY - (unsigned)(g->speed * 1.5f));
        if (s_spawn_cd < 14u) {
            s_spawn_cd = 14u;
        }
    }

    for (i = 0; i < EX11_MAX_OBS; i++) {
        if (!g->obs[i].alive) {
            continue;
        }
        g->obs[i].y += g->speed;
        if (g->obs[i].y > (float)g->screen_h + 8.0f) {
            g->obs[i].alive = 0;
            continue;
        }
        if (aabb(g->car.x, g->car.y, g->car.w, g->car.h, g->obs[i].x,
                 g->obs[i].y, g->obs[i].w, g->obs[i].h)) {
            g->phase = EX11_PHASE_CRASH;
            if (g->score > g->best) {
                g->best = g->score;
            }
            return EX11_SFX_CRASH;
        }
        /* Near-miss click when obstacle passes car closely. */
        if (s_click_cd == 0u &&
            g->obs[i].y + g->obs[i].h > g->car.y - 6.0f &&
            g->obs[i].y < g->car.y + 4.0f) {
            float dx = (g->obs[i].x + g->obs[i].w * 0.5f) -
                       (g->car.x + g->car.w * 0.5f);
            if (dx < 0.0f) {
                dx = -dx;
            }
            if (dx < 48.0f && dx > 22.0f) {
                sfx = EX11_SFX_CLICK;
                s_click_cd = 18u;
            }
        }
    }

    if (s_click_cd > 0u) {
        s_click_cd--;
    }

    if (s_score_cd > 0u) {
        s_score_cd--;
    } else {
        g->score += (uint32_t)(g->speed * 2.0f);
        s_score_cd = SCORE_EVERY;
        if (g->speed < MAX_SPEED) {
            g->speed += 0.012f;
        }
    }

    return sfx;
}
