#include "ex14_game.h"

#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEADZONE 0.30f
#define ROT_GAIN 0.12f
#define THRUST 0.18f
#define DRAG 0.992f
#define MAX_SPEED 4.5f
#define SHOT_SPEED 5.5f
#define SHOT_TTL 28u
#define FIRE_EVERY 8u
#define INVULN_FRAMES 60u
#define LIFE_FLASH_FRAMES 40u
#define SHIP_R 8.0f
#define ROCK_R_S 8.0f
#define ROCK_R_M 14.0f
#define ROCK_R_L 22.0f

static uint32_t s_rng = 1u;

static uint32_t rnd_u(void)
{
    s_rng = s_rng * 1664525u + 1013904223u;
    return s_rng;
}

static float rnd_f(float lo, float hi)
{
    return lo + (hi - lo) * ((float)(rnd_u() & 0xffffu) / 65535.0f);
}

float ex14_rock_radius(uint8_t size)
{
    if (size == EX14_ROCK_LARGE) {
        return ROCK_R_L;
    }
    if (size == EX14_ROCK_MED) {
        return ROCK_R_M;
    }
    return ROCK_R_S;
}

static void wrap(float *v, float lim)
{
    if (*v < 0.0f) {
        *v += lim;
    } else if (*v >= lim) {
        *v -= lim;
    }
}

static void wrap_xy(ex14_game_t *g, float *x, float *y)
{
    wrap(x, (float)g->screen_w);
    wrap(y, (float)g->screen_h);
}

static int circle_hit(float ax, float ay, float ar, float bx, float by, float br)
{
    float dx = ax - bx;
    float dy = ay - by;
    float rr = ar + br;

    return (dx * dx + dy * dy) <= (rr * rr);
}

static void clear_shots(ex14_game_t *g)
{
    unsigned i;

    for (i = 0; i < EX14_MAX_SHOTS; i++) {
        g->shots[i].alive = 0;
    }
}

static void clear_rocks(ex14_game_t *g)
{
    unsigned i;

    for (i = 0; i < EX14_MAX_ROCKS; i++) {
        g->rocks[i].alive = 0;
    }
}

static int count_rocks(const ex14_game_t *g)
{
    unsigned i;
    int n = 0;

    for (i = 0; i < EX14_MAX_ROCKS; i++) {
        if (g->rocks[i].alive) {
            n++;
        }
    }
    return n;
}

static int spawn_rock(ex14_game_t *g, float x, float y, float vx, float vy,
                      uint8_t size)
{
    unsigned i;

    for (i = 0; i < EX14_MAX_ROCKS; i++) {
        if (g->rocks[i].alive) {
            continue;
        }
        g->rocks[i].alive = 1;
        g->rocks[i].size = size;
        g->rocks[i].x = x;
        g->rocks[i].y = y;
        g->rocks[i].vx = vx;
        g->rocks[i].vy = vy;
        return 1;
    }
    return 0;
}

static void spawn_wave(ex14_game_t *g)
{
    unsigned n = 3u + (unsigned)g->wave;
    unsigned i;
    float cx = (float)g->screen_w * 0.5f;
    float cy = (float)g->screen_h * 0.5f;
    float speed = 0.7f + 0.12f * (float)g->wave;

    if (n > 8u) {
        n = 8u;
    }
    clear_rocks(g);
    for (i = 0; i < n; i++) {
        float ang = rnd_f(0.0f, (float)(2.0 * M_PI));
        float dist = rnd_f(70.0f, 130.0f);
        float x = cx + cosf(ang) * dist;
        float y = cy + sinf(ang) * dist;
        float dir = rnd_f(0.0f, (float)(2.0 * M_PI));
        float sp = speed * rnd_f(0.7f, 1.3f);

        wrap_xy(g, &x, &y);
        (void)spawn_rock(g, x, y, cosf(dir) * sp, sinf(dir) * sp,
                         EX14_ROCK_LARGE);
    }
}

static void reset_ship(ex14_game_t *g)
{
    g->ship.x = (float)g->screen_w * 0.5f;
    g->ship.y = (float)g->screen_h * 0.5f;
    g->ship.vx = 0;
    g->ship.vy = 0;
    g->ship.angle = (float)(-M_PI * 0.5); /* nose up */
    g->tilt = 0;
    g->fire_cd = FIRE_EVERY;
    g->invuln = INVULN_FRAMES;
    clear_shots(g);
}

static void reset_play(ex14_game_t *g, int full)
{
    reset_ship(g);
    if (full) {
        g->score = 0;
        g->lives = EX14_START_LIVES;
        g->wave = 1;
        spawn_wave(g);
    }
}

void ex14_game_init(ex14_game_t *g, uint16_t w, uint16_t h)
{
    memset(g, 0, sizeof(*g));
    g->screen_w = w;
    g->screen_h = h;
    g->phase = EX14_PHASE_TITLE;
    g->best = 0;
    g->lives = EX14_START_LIVES;
    g->wave = 1;
    reset_ship(g);
}

ex14_sfx_t ex14_game_on_start(ex14_game_t *g)
{
    if (g->phase == EX14_PHASE_PLAY || g->phase == EX14_PHASE_LIFE) {
        return EX14_SFX_NONE;
    }
    reset_play(g, 1);
    g->phase = EX14_PHASE_PLAY;
    return EX14_SFX_START;
}

static void fire_shot(ex14_game_t *g)
{
    unsigned i;
    float c = cosf(g->ship.angle);
    float s = sinf(g->ship.angle);

    for (i = 0; i < EX14_MAX_SHOTS; i++) {
        if (g->shots[i].alive) {
            continue;
        }
        g->shots[i].alive = 1;
        g->shots[i].ttl = SHOT_TTL;
        g->shots[i].x = g->ship.x + c * SHIP_R;
        g->shots[i].y = g->ship.y + s * SHIP_R;
        g->shots[i].vx = g->ship.vx + c * SHOT_SPEED;
        g->shots[i].vy = g->ship.vy + s * SHOT_SPEED;
        return;
    }
}

static void split_rock(ex14_game_t *g, const ex14_rock_t *src)
{
    uint8_t next;
    float base;
    unsigned k;

    if (src->size == EX14_ROCK_LARGE) {
        next = EX14_ROCK_MED;
        base = 1.2f;
    } else if (src->size == EX14_ROCK_MED) {
        next = EX14_ROCK_SMALL;
        base = 1.6f;
    } else {
        return;
    }
    for (k = 0; k < 2u; k++) {
        float dir = rnd_f(0.0f, (float)(2.0 * M_PI));
        float sp = base * rnd_f(0.8f, 1.4f);

        (void)spawn_rock(g, src->x, src->y, cosf(dir) * sp, sinf(dir) * sp,
                         next);
    }
}

static uint32_t rock_score(uint8_t size)
{
    if (size == EX14_ROCK_LARGE) {
        return 20u;
    }
    if (size == EX14_ROCK_MED) {
        return 50u;
    }
    return 100u;
}

static void lose_life(ex14_game_t *g)
{
    clear_shots(g);
    if (g->lives > 1u) {
        g->lives--;
        g->phase = EX14_PHASE_LIFE;
        g->life_flash = LIFE_FLASH_FRAMES;
        reset_ship(g);
        return;
    }
    g->lives = 0;
    g->phase = EX14_PHASE_OVER;
    if (g->score > g->best) {
        g->best = g->score;
    }
}

ex14_sfx_t ex14_game_tick(ex14_game_t *g, float ax, int live_imu, int thrust)
{
    ex14_sfx_t sfx = EX14_SFX_NONE;
    float steer;
    float speed;
    unsigned i;
    unsigned j;

    g->live_imu = live_imu ? 1u : 0u;

    if (g->phase == EX14_PHASE_LIFE) {
        if (g->life_flash > 0u) {
            g->life_flash--;
        } else {
            g->phase = EX14_PHASE_PLAY;
        }
        /* Still drift rocks during flash */
    }

    if (g->phase != EX14_PHASE_PLAY && g->phase != EX14_PHASE_LIFE) {
        return EX14_SFX_NONE;
    }

    /* Rotate from tilt */
    g->tilt = g->tilt * 0.75f + ax * 0.25f;
    steer = g->tilt;
    if (steer > -DEADZONE && steer < DEADZONE) {
        steer = 0.0f;
    }
    g->ship.angle += steer * ROT_GAIN;

    if (g->phase == EX14_PHASE_PLAY && thrust) {
        g->ship.vx += cosf(g->ship.angle) * THRUST;
        g->ship.vy += sinf(g->ship.angle) * THRUST;
    }

    g->ship.vx *= DRAG;
    g->ship.vy *= DRAG;
    speed = sqrtf(g->ship.vx * g->ship.vx + g->ship.vy * g->ship.vy);
    if (speed > MAX_SPEED && speed > 0.0f) {
        float s = MAX_SPEED / speed;
        g->ship.vx *= s;
        g->ship.vy *= s;
    }

    g->ship.x += g->ship.vx;
    g->ship.y += g->ship.vy;
    wrap_xy(g, &g->ship.x, &g->ship.y);

    if (g->invuln > 0u) {
        g->invuln--;
    }

    if (g->phase == EX14_PHASE_PLAY) {
        if (g->fire_cd > 0u) {
            g->fire_cd--;
        } else {
            fire_shot(g);
            g->fire_cd = FIRE_EVERY;
        }
    }

    for (i = 0; i < EX14_MAX_SHOTS; i++) {
        if (!g->shots[i].alive) {
            continue;
        }
        g->shots[i].x += g->shots[i].vx;
        g->shots[i].y += g->shots[i].vy;
        wrap_xy(g, &g->shots[i].x, &g->shots[i].y);
        if (g->shots[i].ttl > 0u) {
            g->shots[i].ttl--;
        } else {
            g->shots[i].alive = 0;
        }
    }

    for (i = 0; i < EX14_MAX_ROCKS; i++) {
        if (!g->rocks[i].alive) {
            continue;
        }
        g->rocks[i].x += g->rocks[i].vx;
        g->rocks[i].y += g->rocks[i].vy;
        wrap_xy(g, &g->rocks[i].x, &g->rocks[i].y);
    }

    /* Shots vs rocks */
    for (i = 0; i < EX14_MAX_ROCKS; i++) {
        float rr;

        if (!g->rocks[i].alive) {
            continue;
        }
        rr = ex14_rock_radius(g->rocks[i].size);
        for (j = 0; j < EX14_MAX_SHOTS; j++) {
            if (!g->shots[j].alive) {
                continue;
            }
            if (circle_hit(g->shots[j].x, g->shots[j].y, 2.0f, g->rocks[i].x,
                           g->rocks[i].y, rr)) {
                ex14_rock_t copy = g->rocks[i];

                g->shots[j].alive = 0;
                g->rocks[i].alive = 0;
                g->score += rock_score(copy.size);
                split_rock(g, &copy);
                sfx = EX14_SFX_HIT;
                break;
            }
        }
    }

    /* Ship vs rocks */
    if (g->phase == EX14_PHASE_PLAY && g->invuln == 0u) {
        for (i = 0; i < EX14_MAX_ROCKS; i++) {
            float rr;

            if (!g->rocks[i].alive) {
                continue;
            }
            rr = ex14_rock_radius(g->rocks[i].size);
            if (circle_hit(g->ship.x, g->ship.y, SHIP_R * 0.7f, g->rocks[i].x,
                           g->rocks[i].y, rr)) {
                lose_life(g);
                return EX14_SFX_LOSE;
            }
        }
    }

    if (g->phase == EX14_PHASE_PLAY && count_rocks(g) == 0) {
        if (g->wave < 255u) {
            g->wave++;
        }
        spawn_wave(g);
        g->invuln = INVULN_FRAMES / 2u;
        if (sfx == EX14_SFX_NONE) {
            sfx = EX14_SFX_BOOM;
        }
    }

    return sfx;
}
