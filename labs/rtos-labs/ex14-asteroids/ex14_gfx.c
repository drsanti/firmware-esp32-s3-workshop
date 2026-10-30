#include "ex14_gfx.h"

#include "bsp.h"
#include "rb_screen.h"
#include "ui/gfx.h"

#include <math.h>
#include <stdio.h>

#define COL_BG RBGFX_RGB(2, 2, 10)
#define COL_SHIP RBGFX_RGB(220, 230, 255)
#define COL_SHOT RBGFX_RGB(255, 220, 80)
#define COL_ROCK RBGFX_RGB(150, 150, 170)
#define COL_HUD RBGFX_WHITE

static rb_display_t s_disp;

static void fill_rect_safe(int16_t x, int16_t y, uint16_t w, uint16_t h,
                           uint16_t color)
{
    bsp_display_lock();
    rbgfx_fill_rect(&s_disp.gfx, x, y, w, h, color);
    bsp_display_unlock();
}

static void text_safe(int16_t x, int16_t y, const char *s, uint16_t fg,
                      uint16_t bg)
{
    bsp_display_lock();
    (void)rbgfx_text_scaled(&s_disp.gfx, x, y, s, fg, bg, 1);
    bsp_display_unlock();
}

static void flush_safe(void)
{
    bsp_display_lock();
    (void)rbgfx_flush(&s_disp.gfx);
    bsp_display_unlock();
}

int ex14_gfx_begin(void)
{
    if (rb_display_open(&s_disp) != 0) {
        return -1;
    }
    return 0;
}

uint16_t ex14_gfx_width(void)
{
    return rb_display_width(&s_disp);
}

uint16_t ex14_gfx_height(void)
{
    return rb_display_height(&s_disp);
}

static void paint_dot(float x, float y, uint16_t color)
{
    fill_rect_safe((int16_t)x, (int16_t)y, 2, 2, color);
}

static void paint_ship(const ex14_ship_t *s, int blink_off)
{
    float c = cosf(s->angle);
    float sn = sinf(s->angle);
    float px;
    float py;
    float lx;
    float ly;
    float rx;
    float ry;

    if (blink_off) {
        return;
    }

    /* Nose + two wings as filled rects at tip / flanks */
    px = s->x + c * 10.0f;
    py = s->y + sn * 10.0f;
    lx = s->x + (-c * 6.0f - sn * 7.0f);
    ly = s->y + (-sn * 6.0f + c * 7.0f);
    rx = s->x + (-c * 6.0f + sn * 7.0f);
    ry = s->y + (-sn * 6.0f - c * 7.0f);

    fill_rect_safe((int16_t)px - 1, (int16_t)py - 1, 3, 3, COL_SHIP);
    fill_rect_safe((int16_t)lx - 1, (int16_t)ly - 1, 3, 3, COL_SHIP);
    fill_rect_safe((int16_t)rx - 1, (int16_t)ry - 1, 3, 3, COL_SHIP);
    fill_rect_safe((int16_t)s->x - 1, (int16_t)s->y - 1, 3, 3, RBGFX_CYAN);
}

static void paint_rock(const ex14_rock_t *r)
{
    float rad = ex14_rock_radius(r->size);
    int16_t d = (int16_t)(rad * 1.6f);

    if (d < 6) {
        d = 6;
    }
    fill_rect_safe((int16_t)(r->x - (float)d * 0.5f),
                   (int16_t)(r->y - (float)d * 0.5f), (uint16_t)d, (uint16_t)d,
                   COL_ROCK);
    fill_rect_safe((int16_t)(r->x - 1), (int16_t)(r->y - 1), 2, 2, RBGFX_WHITE);
}

static void paint_world(const ex14_game_t *g)
{
    char line[40];
    unsigned i;
    int blink = (g->invuln > 0u) && ((g->invuln / 4u) & 1u);

    fill_rect_safe(0, 0, g->screen_w, g->screen_h, COL_BG);

    (void)snprintf(line, sizeof(line), "SCORE %lu", (unsigned long)g->score);
    text_safe(6, 4, line, COL_HUD, COL_BG);
    (void)snprintf(line, sizeof(line), "L%d W%d", (int)g->lives, (int)g->wave);
    text_safe((int16_t)(g->screen_w / 2 - 24), 4, line, COL_HUD, COL_BG);
    text_safe((int16_t)(g->screen_w - 48), 4, g->live_imu ? "LIVE" : "SIM ",
              g->live_imu ? RBGFX_GREEN : RBGFX_YELLOW, COL_BG);

    for (i = 0; i < EX14_MAX_ROCKS; i++) {
        if (g->rocks[i].alive) {
            paint_rock(&g->rocks[i]);
        }
    }
    for (i = 0; i < EX14_MAX_SHOTS; i++) {
        if (g->shots[i].alive) {
            paint_dot(g->shots[i].x, g->shots[i].y, COL_SHOT);
        }
    }
    paint_ship(&g->ship, blink);
}

static void paint_title(const ex14_game_t *g)
{
    fill_rect_safe(0, 0, g->screen_w, g->screen_h, COL_BG);
    text_safe((int16_t)(g->screen_w / 2 - 40), 60, "ASTEROIDS", RBGFX_CYAN,
              COL_BG);
    text_safe((int16_t)(g->screen_w / 2 - 48), 95, "tilt = rotate", COL_HUD,
              COL_BG);
    text_safe((int16_t)(g->screen_w / 2 - 48), 115, "BOOT = thrust", COL_HUD,
              COL_BG);
    text_safe((int16_t)(g->screen_w / 2 - 48), 135, "auto fire on", COL_HUD,
              COL_BG);
    text_safe((int16_t)(g->screen_w / 2 - 64), 165, "TAP / BOOT start",
              RBGFX_YELLOW, COL_BG);
    if (g->best > 0u) {
        char line[32];
        (void)snprintf(line, sizeof(line), "best %lu", (unsigned long)g->best);
        text_safe((int16_t)(g->screen_w / 2 - 32), 195, line, COL_HUD, COL_BG);
    }
}

static void paint_overlay(const ex14_game_t *g, const char *title,
                          const char *sub, uint16_t title_fg)
{
    char line[32];

    paint_world(g);
    fill_rect_safe(24, 70, (uint16_t)(g->screen_w - 48), 90, RBGFX_BLACK);
    text_safe((int16_t)(g->screen_w / 2 - 40), 85, title, title_fg, RBGFX_BLACK);
    (void)snprintf(line, sizeof(line), "score %lu", (unsigned long)g->score);
    text_safe((int16_t)(g->screen_w / 2 - 40), 110, line, COL_HUD, RBGFX_BLACK);
    text_safe((int16_t)(g->screen_w / 2 - 56), 135, sub, RBGFX_YELLOW,
              RBGFX_BLACK);
}

void ex14_gfx_paint(const ex14_game_t *g)
{
    switch (g->phase) {
    case EX14_PHASE_TITLE:
        paint_title(g);
        break;
    case EX14_PHASE_LIFE:
        paint_overlay(g, "LIFE -1", "wait...", RBGFX_YELLOW);
        break;
    case EX14_PHASE_OVER:
        paint_overlay(g, "GAME OVER", "tap/BOOT retry", RBGFX_RED);
        break;
    default:
        paint_world(g);
        break;
    }
    flush_safe();
}
