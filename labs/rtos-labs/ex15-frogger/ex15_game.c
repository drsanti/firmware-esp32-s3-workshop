#include "ex15_game.h"

#include <string.h>

#define DEADZONE 0.40f
#define MOVE_CD 8u
#define HOP_LOCK 6u
#define LIFE_FLASH_FRAMES 35u
#define FROG_W_FRAC 0.7f

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

static void setup_lanes(ex15_game_t *g)
{
    /* Bottom → top */
    g->lane_kind[0] = EX15_LANE_SAFE;
    g->lane_kind[1] = EX15_LANE_ROAD;
    g->lane_kind[2] = EX15_LANE_ROAD;
    g->lane_kind[3] = EX15_LANE_ROAD;
    g->lane_kind[4] = EX15_LANE_SAFE;
    g->lane_kind[5] = EX15_LANE_WATER;
    g->lane_kind[6] = EX15_LANE_WATER;
    g->lane_kind[7] = EX15_LANE_WATER;
    g->lane_kind[8] = EX15_LANE_GOAL;
}

static int spawn_actor(ex15_game_t *g, uint8_t row, uint8_t kind, float x,
                       float w, float speed)
{
    unsigned i;

    for (i = 0; i < EX15_MAX_ACTORS; i++) {
        if (g->actors[i].alive) {
            continue;
        }
        g->actors[i].alive = 1;
        g->actors[i].row = row;
        g->actors[i].kind = kind;
        g->actors[i].x = x;
        g->actors[i].w = w;
        g->actors[i].speed = speed;
        return 1;
    }
    return 0;
}

static void clear_actors(ex15_game_t *g)
{
    unsigned i;

    for (i = 0; i < EX15_MAX_ACTORS; i++) {
        g->actors[i].alive = 0;
    }
}

static void spawn_traffic(ex15_game_t *g)
{
    float cw = (float)g->cell_w;
    float boost = 0.15f * (float)(g->wave - 1u);

    clear_actors(g);

    /* Road cars */
    (void)spawn_actor(g, 1, EX15_ACT_CAR, 0.0f, cw * 1.8f, 1.4f + boost);
    (void)spawn_actor(g, 1, EX15_ACT_CAR, cw * 6.0f, cw * 1.6f, 1.4f + boost);
    (void)spawn_actor(g, 2, EX15_ACT_CAR, cw * 2.0f, cw * 2.0f, -1.7f - boost);
    (void)spawn_actor(g, 2, EX15_ACT_CAR, cw * 8.0f, cw * 1.5f, -1.7f - boost);
    (void)spawn_actor(g, 3, EX15_ACT_CAR, cw * 1.0f, cw * 1.4f, 2.0f + boost);
    (void)spawn_actor(g, 3, EX15_ACT_CAR, cw * 5.5f, cw * 1.8f, 2.0f + boost);

    /* Water logs */
    (void)spawn_actor(g, 5, EX15_ACT_LOG, 0.0f, cw * 3.2f, 1.1f + boost * 0.5f);
    (void)spawn_actor(g, 5, EX15_ACT_LOG, cw * 7.0f, cw * 2.8f, 1.1f + boost * 0.5f);
    (void)spawn_actor(g, 6, EX15_ACT_LOG, cw * 1.5f, cw * 3.5f, -1.3f - boost * 0.5f);
    (void)spawn_actor(g, 6, EX15_ACT_LOG, cw * 8.0f, cw * 2.5f, -1.3f - boost * 0.5f);
    (void)spawn_actor(g, 7, EX15_ACT_LOG, cw * 0.5f, cw * 2.6f, 0.9f + boost * 0.4f);
    (void)spawn_actor(g, 7, EX15_ACT_LOG, cw * 5.0f, cw * 3.0f, 0.9f + boost * 0.4f);
}

static void reset_frog(ex15_game_t *g)
{
    g->frog_row = 0;
    g->frog_x = (float)g->screen_w * 0.5f - (float)g->cell_w * 0.5f;
    g->move_cd = 0;
    g->hop_lock = 0;
    g->tilt = 0;
}

static void clear_goals(ex15_game_t *g)
{
    unsigned i;

    for (i = 0; i < EX15_GOALS; i++) {
        g->goals[i] = 0;
    }
    g->goals_filled = 0;
}

static void reset_play(ex15_game_t *g, int full)
{
    reset_frog(g);
    if (full) {
        g->score = 0;
        g->lives = EX15_START_LIVES;
        g->wave = 1;
        clear_goals(g);
        spawn_traffic(g);
    }
}

void ex15_game_init(ex15_game_t *g, uint16_t w, uint16_t h)
{
    memset(g, 0, sizeof(*g));
    g->screen_w = w;
    g->screen_h = h;
    g->cell_h = (uint16_t)(h / EX15_ROWS);
    if (g->cell_h < 12u) {
        g->cell_h = 12u;
    }
    g->cell_w = (uint16_t)(w / 12u);
    if (g->cell_w < 12u) {
        g->cell_w = 12u;
    }
    g->phase = EX15_PHASE_TITLE;
    g->best = 0;
    g->lives = EX15_START_LIVES;
    g->wave = 1;
    setup_lanes(g);
    reset_frog(g);
}

ex15_sfx_t ex15_game_on_start(ex15_game_t *g)
{
    if (g->phase == EX15_PHASE_PLAY || g->phase == EX15_PHASE_LIFE) {
        return EX15_SFX_NONE;
    }
    reset_play(g, 1);
    g->phase = EX15_PHASE_PLAY;
    return EX15_SFX_START;
}

static float frog_w(const ex15_game_t *g)
{
    return (float)g->cell_w * FROG_W_FRAC;
}

static int aabb(float ax, float aw, float bx, float bw)
{
    return (ax < bx + bw) && (ax + aw > bx);
}

static void lose_life(ex15_game_t *g)
{
    if (g->lives > 1u) {
        g->lives--;
        g->phase = EX15_PHASE_LIFE;
        g->life_flash = LIFE_FLASH_FRAMES;
        reset_frog(g);
        return;
    }
    g->lives = 0;
    g->phase = EX15_PHASE_OVER;
    if (g->score > g->best) {
        g->best = g->score;
    }
}

static int on_log(ex15_game_t *g, float *carry)
{
    unsigned i;
    float fw = frog_w(g);

    *carry = 0.0f;
    for (i = 0; i < EX15_MAX_ACTORS; i++) {
        if (!g->actors[i].alive || g->actors[i].kind != EX15_ACT_LOG) {
            continue;
        }
        if (g->actors[i].row != g->frog_row) {
            continue;
        }
        if (aabb(g->frog_x, fw, g->actors[i].x, g->actors[i].w)) {
            *carry = g->actors[i].speed;
            return 1;
        }
    }
    return 0;
}

static int hit_car(const ex15_game_t *g)
{
    unsigned i;
    float fw = frog_w(g);

    for (i = 0; i < EX15_MAX_ACTORS; i++) {
        if (!g->actors[i].alive || g->actors[i].kind != EX15_ACT_CAR) {
            continue;
        }
        if (g->actors[i].row != g->frog_row) {
            continue;
        }
        if (aabb(g->frog_x, fw, g->actors[i].x, g->actors[i].w)) {
            return 1;
        }
    }
    return 0;
}

static int goal_index(const ex15_game_t *g)
{
    float slot = (float)g->screen_w / (float)EX15_GOALS;
    int idx = (int)((g->frog_x + frog_w(g) * 0.5f) / slot);

    if (idx < 0) {
        idx = 0;
    }
    if (idx >= EX15_GOALS) {
        idx = EX15_GOALS - 1;
    }
    return idx;
}

static ex15_sfx_t try_goal(ex15_game_t *g)
{
    int idx;

    if (g->lane_kind[g->frog_row] != EX15_LANE_GOAL) {
        return EX15_SFX_NONE;
    }
    idx = goal_index(g);
    if (g->goals[idx]) {
        /* Occupied pad — bounce back / die lightly: lose life */
        lose_life(g);
        return EX15_SFX_LOSE;
    }
    g->goals[idx] = 1;
    g->goals_filled++;
    g->score += 200u + 50u * (uint32_t)g->wave;
    reset_frog(g);

    if (g->goals_filled >= EX15_GOALS) {
        g->wave++;
        clear_goals(g);
        spawn_traffic(g);
        g->phase = EX15_PHASE_WIN;
        g->life_flash = 45u; /* reuse flash timer for win banner */
        return EX15_SFX_WIN;
    }
    return EX15_SFX_GOAL;
}

ex15_sfx_t ex15_game_tick(ex15_game_t *g, float ax, int live_imu, int hop)
{
    ex15_sfx_t sfx = EX15_SFX_NONE;
    float steer;
    float lim;
    unsigned i;
    uint8_t kind;

    g->live_imu = live_imu ? 1u : 0u;

    if (g->phase == EX15_PHASE_LIFE || g->phase == EX15_PHASE_WIN) {
        if (g->life_flash > 0u) {
            g->life_flash--;
        } else {
            g->phase = EX15_PHASE_PLAY;
        }
    }

    if (g->phase != EX15_PHASE_PLAY && g->phase != EX15_PHASE_LIFE &&
        g->phase != EX15_PHASE_WIN) {
        return EX15_SFX_NONE;
    }

    /* Move actors always so world keeps flowing */
    for (i = 0; i < EX15_MAX_ACTORS; i++) {
        float max_x;

        if (!g->actors[i].alive) {
            continue;
        }
        g->actors[i].x += g->actors[i].speed;
        max_x = (float)g->screen_w + g->actors[i].w;
        if (g->actors[i].speed > 0.0f && g->actors[i].x > (float)g->screen_w) {
            g->actors[i].x = -g->actors[i].w;
        } else if (g->actors[i].speed < 0.0f &&
                   g->actors[i].x + g->actors[i].w < 0.0f) {
            g->actors[i].x = max_x - g->actors[i].w;
        }
    }

    if (g->phase != EX15_PHASE_PLAY) {
        return EX15_SFX_NONE;
    }

    if (g->move_cd > 0u) {
        g->move_cd--;
    }
    if (g->hop_lock > 0u) {
        g->hop_lock--;
    }

    g->tilt = g->tilt * 0.7f + ax * 0.3f;
    steer = g->tilt;
    if (steer > -DEADZONE && steer < DEADZONE) {
        steer = 0.0f;
    }
    if (g->move_cd == 0u && steer != 0.0f) {
        g->frog_x += (steer > 0.0f) ? (float)g->cell_w : -(float)g->cell_w;
        g->move_cd = MOVE_CD;
    }

    if (hop && g->hop_lock == 0u) {
        if (g->frog_row + 1u < EX15_ROWS) {
            g->frog_row++;
            g->score += 10u;
            g->hop_lock = HOP_LOCK;
            sfx = EX15_SFX_HOP;
        }
    }

    /* Ride logs */
    kind = g->lane_kind[g->frog_row];
    if (kind == EX15_LANE_WATER) {
        float carry = 0.0f;

        if (on_log(g, &carry)) {
            g->frog_x += carry;
        } else {
            lose_life(g);
            return EX15_SFX_LOSE;
        }
    }

    lim = (float)g->screen_w - frog_w(g);
    g->frog_x = clampf(g->frog_x, 0.0f, lim);
    if (g->frog_x <= 0.0f || g->frog_x >= lim) {
        if (kind == EX15_LANE_WATER) {
            lose_life(g);
            return EX15_SFX_LOSE;
        }
    }

    if (kind == EX15_LANE_ROAD && hit_car(g)) {
        lose_life(g);
        return EX15_SFX_LOSE;
    }

    if (kind == EX15_LANE_GOAL) {
        ex15_sfx_t gs = try_goal(g);

        if (gs != EX15_SFX_NONE) {
            return gs;
        }
    }

    return sfx;
}
