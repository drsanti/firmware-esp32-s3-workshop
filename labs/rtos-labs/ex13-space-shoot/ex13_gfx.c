#include "ex13_gfx.h"

#include "bsp.h"
#include "rb_screen.h"
#include "ui/gfx.h"

#include <stdio.h>

#define COL_BG RBGFX_RGB(4, 6, 20)
#define COL_SHIP RBGFX_RGB(40, 200, 255)
#define COL_SHOT RBGFX_RGB(255, 230, 80)
#define COL_ENEMY RBGFX_RGB(220, 60, 50)
#define COL_STAR RBGFX_RGB(180, 180, 200)
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

int ex13_gfx_begin(void)
{
    if (rb_display_open(&s_disp) != 0) {
        return -1;
    }
    return 0;
}

uint16_t ex13_gfx_width(void)
{
    return rb_display_width(&s_disp);
}

uint16_t ex13_gfx_height(void)
{
    return rb_display_height(&s_disp);
}

static void paint_stars(const ex13_game_t *g)
{
    /* Deterministic star field scrolled by star_scroll. */
    static const int16_t sx[] = {20, 60, 100, 140, 180, 220, 40, 160, 250, 90};
    static const int16_t sy[] = {10, 50, 90, 30, 120, 70, 150, 180, 40, 200};
    unsigned i;
    int period = (int)g->screen_h + 20;

    for (i = 0; i < sizeof(sx) / sizeof(sx[0]); i++) {
        int y = (sy[i] + (int)g->star_scroll * (1 + (int)(i % 3))) % period;
        fill_rect_safe(sx[i] % (int16_t)g->screen_w, (int16_t)y, 2, 2, COL_STAR);
    }
}

static void paint_ship(const ex13_ship_t *s)
{
    int16_t x = (int16_t)s->x;
    int16_t y = (int16_t)s->y;
    int16_t w = (int16_t)s->w;
    int16_t h = (int16_t)s->h;

    /* Simple triangle-ish ship from rects */
    fill_rect_safe((int16_t)(x + w / 2 - 2), y, 4, (uint16_t)h, COL_SHIP);
    fill_rect_safe(x, (int16_t)(y + h / 2), (uint16_t)w, (uint16_t)(h / 2),
                   COL_SHIP);
    fill_rect_safe((int16_t)(x + 4), (int16_t)(y + h - 4), 4, 4, RBGFX_YELLOW);
    fill_rect_safe((int16_t)(x + w - 8), (int16_t)(y + h - 4), 4, 4,
                   RBGFX_YELLOW);
}

static void paint_world(const ex13_game_t *g)
{
    char line[40];
    unsigned i;

    fill_rect_safe(0, 0, g->screen_w, g->screen_h, COL_BG);
    paint_stars(g);

    (void)snprintf(line, sizeof(line), "SCORE %lu", (unsigned long)g->score);
    text_safe(6, 4, line, COL_HUD, COL_BG);
    (void)snprintf(line, sizeof(line), "L%d", (int)g->lives);
    text_safe((int16_t)(g->screen_w / 2 - 8), 4, line, COL_HUD, COL_BG);
    text_safe((int16_t)(g->screen_w - 48), 4, g->live_imu ? "LIVE" : "SIM ",
              g->live_imu ? RBGFX_GREEN : RBGFX_YELLOW, COL_BG);

    for (i = 0; i < EX13_MAX_SHOTS; i++) {
        if (!g->shots[i].alive) {
            continue;
        }
        fill_rect_safe((int16_t)g->shots[i].x, (int16_t)g->shots[i].y,
                       (uint16_t)g->shots[i].w, (uint16_t)g->shots[i].h,
                       COL_SHOT);
    }
    for (i = 0; i < EX13_MAX_ENEMIES; i++) {
        if (!g->enemies[i].alive) {
            continue;
        }
        fill_rect_safe((int16_t)g->enemies[i].x, (int16_t)g->enemies[i].y,
                       (uint16_t)g->enemies[i].w, (uint16_t)g->enemies[i].h,
                       COL_ENEMY);
        fill_rect_safe((int16_t)(g->enemies[i].x + 4),
                       (int16_t)(g->enemies[i].y + 4), 4, 4, RBGFX_WHITE);
        fill_rect_safe((int16_t)(g->enemies[i].x + g->enemies[i].w - 8),
                       (int16_t)(g->enemies[i].y + 4), 4, 4, RBGFX_WHITE);
    }
    paint_ship(&g->ship);
}

static void paint_title(const ex13_game_t *g)
{
    fill_rect_safe(0, 0, g->screen_w, g->screen_h, COL_BG);
    text_safe((int16_t)(g->screen_w / 2 - 52), 70, "SPACE SHOOT", RBGFX_CYAN,
              COL_BG);
    text_safe((int16_t)(g->screen_w / 2 - 48), 100, "tilt to move", COL_HUD,
              COL_BG);
    text_safe((int16_t)(g->screen_w / 2 - 48), 120, "auto fire on", COL_HUD,
              COL_BG);
    text_safe((int16_t)(g->screen_w / 2 - 64), 155, "TAP / BOOT start",
              RBGFX_YELLOW, COL_BG);
    if (g->best > 0u) {
        char line[32];
        (void)snprintf(line, sizeof(line), "best %lu", (unsigned long)g->best);
        text_safe((int16_t)(g->screen_w / 2 - 32), 185, line, COL_HUD, COL_BG);
    }
}

static void paint_overlay(const ex13_game_t *g, const char *title,
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

void ex13_gfx_paint(const ex13_game_t *g)
{
    switch (g->phase) {
    case EX13_PHASE_TITLE:
        paint_title(g);
        break;
    case EX13_PHASE_LIFE:
        paint_overlay(g, "LIFE -1", "wait...", RBGFX_YELLOW);
        break;
    case EX13_PHASE_OVER:
        paint_overlay(g, "GAME OVER", "tap/BOOT retry", RBGFX_RED);
        break;
    default:
        paint_world(g);
        break;
    }
    flush_safe();
}
