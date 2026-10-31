#include "ex13_game.h"

#include <string.h>

#define SHIP_W 22.0f
#define SHIP_H 18.0f
#define SHOT_W 3.0f
#define SHOT_H 10.0f
#define ENEMY_W 20.0f
#define ENEMY_H 14.0f
#define DEADZONE 0.35f
#define STEER_GAIN 11.0f
#define SHOT_SPEED 7.0f
#define FIRE_EVERY 10u
#define SPAWN_EVERY 28u
#define LIFE_FLASH_FRAMES 40u
#define ENEMY_BASE_VY 1.8f

static uint32_t s_rng = 1u;

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

static int aabb(float ax, float ay, float aw, float ah, float bx, float by,
                float bw, float bh)
{
    return (ax < bx + bw) && (ax + aw > bx) && (ay < by + bh) && (ay + ah > by);
}

static void clear_actors(ex13_game_t *g)
{
    unsigned i;

    for (i = 0; i < EX13_MAX_SHOTS; i++) {
        g->shots[i].alive = 0;
    }
    for (i = 0; i < EX13_MAX_ENEMIES; i++) {
        g->enemies[i].alive = 0;
    }
}

static void reset_play(ex13_game_t *g, int full)
{
    g->ship.w = SHIP_W;
    g->ship.h = SHIP_H;
    g->ship.x = (float)g->screen_w * 0.5f - SHIP_W * 0.5f;
    g->ship.y = (float)g->screen_h - SHIP_H - 16.0f;
    g->tilt = 0;
    g->fire_cd = FIRE_EVERY;
    g->spawn_cd = SPAWN_EVERY / 2u;
    g->star_scroll = 0;
    clear_actors(g);
    if (full) {
        g->score = 0;
        g->lives = EX13_START_LIVES;
    }
}

void ex13_game_init(ex13_game_t *g, uint16_t w, uint16_t h)
{
    memset(g, 0, sizeof(*g));
    g->screen_w = w;
    g->screen_h = h;
    g->phase = EX13_PHASE_TITLE;
    g->best = 0;
    g->lives = EX13_START_LIVES;
    reset_play(g, 1);
}

ex13_sfx_t ex13_game_on_start(ex13_game_t *g)
{
    if (g->phase == EX13_PHASE_PLAY || g->phase == EX13_PHASE_LIFE) {
        return EX13_SFX_NONE;
    }
    reset_play(g, 1);
    g->phase = EX13_PHASE_PLAY;
    return EX13_SFX_START;
}

static void fire_shot(ex13_game_t *g)
{
    unsigned i;

    for (i = 0; i < EX13_MAX_SHOTS; i++) {
        if (g->shots[i].alive) {
            continue;
        }
        g->shots[i].alive = 1;
        g->shots[i].w = SHOT_W;
        g->shots[i].h = SHOT_H;
        g->shots[i].x = g->ship.x + g->ship.w * 0.5f - SHOT_W * 0.5f;
        g->shots[i].y = g->ship.y - SHOT_H;
        return;
    }
}

static void spawn_enemy(ex13_game_t *g)
{
    unsigned i;
    float max_x = (float)g->screen_w - ENEMY_W - 4.0f;

    for (i = 0; i < EX13_MAX_ENEMIES; i++) {
        if (g->enemies[i].alive) {
            continue;
        }
        g->enemies[i].alive = 1;
        g->enemies[i].w = ENEMY_W;
        g->enemies[i].h = ENEMY_H;
        g->enemies[i].x = 4.0f + (float)(rnd_u() % (uint32_t)(max_x - 4.0f));
        g->enemies[i].y = -ENEMY_H - 2.0f;
        g->enemies[i].vy =
            ENEMY_BASE_VY + (float)(g->score / 500u) * 0.35f;
        if (g->enemies[i].vy > 4.5f) {
            g->enemies[i].vy = 4.5f;
        }
        return;
    }
}

static void lose_life(ex13_game_t *g)
{
    clear_actors(g);
    if (g->lives > 1u) {
        g->lives--;
        g->phase = EX13_PHASE_LIFE;
        g->life_flash = LIFE_FLASH_FRAMES;
        g->ship.x = (float)g->screen_w * 0.5f - SHIP_W * 0.5f;
        return;
    }
    g->lives = 0;
    g->phase = EX13_PHASE_OVER;
    if (g->score > g->best) {
        g->best = g->score;
    }
}

ex13_sfx_t ex13_game_tick(ex13_game_t *g, float ax, int live_imu)
{
    ex13_sfx_t sfx = EX13_SFX_NONE;
    float steer;
    unsigned i;
    unsigned j;

    g->live_imu = live_imu ? 1u : 0u;

    if (g->phase == EX13_PHASE_LIFE) {
        if (g->life_flash > 0u) {
            g->life_flash--;
        } else {
            g->phase = EX13_PHASE_PLAY;
            g->fire_cd = FIRE_EVERY;
            g->spawn_cd = SPAWN_EVERY;
        }
    }

    if (g->phase != EX13_PHASE_PLAY && g->phase != EX13_PHASE_LIFE) {
        return EX13_SFX_NONE;
    }

    g->tilt = g->tilt * 0.75f + ax * 0.25f;
    steer = g->tilt;
    if (steer > -DEADZONE && steer < DEADZONE) {
        steer = 0.0f;
    }
    g->ship.x += steer * STEER_GAIN;
    g->ship.x =
        clampf(g->ship.x, 4.0f, (float)g->screen_w - g->ship.w - 4.0f);

    g->star_scroll += 1.6f;
    if (g->star_scroll > 40.0f) {
        g->star_scroll -= 40.0f;
    }

    if (g->phase == EX13_PHASE_LIFE) {
        return EX13_SFX_NONE;
    }

    /* Auto-fire */
    if (g->fire_cd > 0u) {
        g->fire_cd--;
    } else {
        fire_shot(g);
        g->fire_cd = FIRE_EVERY;
        sfx = EX13_SFX_SHOT;
    }

    if (g->spawn_cd > 0u) {
        g->spawn_cd--;
    } else {
        spawn_enemy(g);
        g->spawn_cd = SPAWN_EVERY;
        if (g->score > 800u && g->spawn_cd > 16u) {
            g->spawn_cd = (uint16_t)(SPAWN_EVERY - g->score / 800u);
            if (g->spawn_cd < 16u) {
                g->spawn_cd = 16u;
            }
        }
    }

    for (i = 0; i < EX13_MAX_SHOTS; i++) {
        if (!g->shots[i].alive) {
            continue;
        }
        g->shots[i].y -= SHOT_SPEED;
        if (g->shots[i].y + g->shots[i].h < 0.0f) {
            g->shots[i].alive = 0;
        }
    }

    for (i = 0; i < EX13_MAX_ENEMIES; i++) {
        if (!g->enemies[i].alive) {
            continue;
        }
        g->enemies[i].y += g->enemies[i].vy;
        if (g->enemies[i].y > (float)g->screen_h) {
            g->enemies[i].alive = 0;
            lose_life(g);
            return EX13_SFX_LOSE;
        }
        if (aabb(g->ship.x, g->ship.y, g->ship.w, g->ship.h, g->enemies[i].x,
                 g->enemies[i].y, g->enemies[i].w, g->enemies[i].h)) {
            g->enemies[i].alive = 0;
            lose_life(g);
            return EX13_SFX_LOSE;
        }
        for (j = 0; j < EX13_MAX_SHOTS; j++) {
            if (!g->shots[j].alive) {
                continue;
            }
            if (aabb(g->shots[j].x, g->shots[j].y, g->shots[j].w,
                     g->shots[j].h, g->enemies[i].x, g->enemies[i].y,
                     g->enemies[i].w, g->enemies[i].h)) {
                g->shots[j].alive = 0;
                g->enemies[i].alive = 0;
                g->score += 50u;
                return EX13_SFX_HIT;
            }
        }
    }

    return sfx;
}
