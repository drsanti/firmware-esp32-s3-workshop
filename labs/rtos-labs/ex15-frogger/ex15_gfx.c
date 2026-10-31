#include "ex15_gfx.h"

#include "bsp.h"
#include "rb_screen.h"
#include "ui/gfx.h"

#include <stdio.h>

#define COL_SAFE RBGFX_RGB(30, 90, 40)
#define COL_ROAD RBGFX_RGB(40, 40, 48)
#define COL_WATER RBGFX_RGB(20, 50, 140)
#define COL_GOAL_BANK RBGFX_RGB(20, 70, 35)
#define COL_CAR RBGFX_RGB(220, 60, 50)
#define COL_LOG RBGFX_RGB(120, 70, 30)
#define COL_FROG RBGFX_RGB(60, 220, 80)
#define COL_HUD RBGFX_WHITE
#define COL_PAD RBGFX_RGB(10, 40, 20)
#define COL_PAD_FULL RBGFX_RGB(255, 200, 40)

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

int ex15_gfx_begin(void)
{
    if (rb_display_open(&s_disp) != 0) {
        return -1;
    }
    return 0;
}

uint16_t ex15_gfx_width(void)
{
    return rb_display_width(&s_disp);
}

uint16_t ex15_gfx_height(void)
{
    return rb_display_height(&s_disp);
}

static int16_t row_y(const ex15_game_t *g, uint8_t row)
{
    return (int16_t)(g->screen_h - (uint16_t)(row + 1u) * g->cell_h);
}

static uint16_t lane_color(uint8_t kind)
{
    switch (kind) {
    case EX15_LANE_ROAD:
        return COL_ROAD;
    case EX15_LANE_WATER:
        return COL_WATER;
    case EX15_LANE_GOAL:
        return COL_GOAL_BANK;
    default:
        return COL_SAFE;
    }
}

static void paint_lanes(const ex15_game_t *g)
{
    unsigned r;

    for (r = 0; r < EX15_ROWS; r++) {
        fill_rect_safe(0, row_y(g, (uint8_t)r), g->screen_w, g->cell_h,
                       lane_color(g->lane_kind[r]));
    }
}

static void paint_goals(const ex15_game_t *g)
{
    float slot = (float)g->screen_w / (float)EX15_GOALS;
    unsigned i;
    int16_t y = row_y(g, 8);
    uint16_t h = (uint16_t)(g->cell_h - 4u);

    for (i = 0; i < EX15_GOALS; i++) {
        int16_t x = (int16_t)(slot * (float)i + 4.0f);
        uint16_t w = (uint16_t)(slot - 8.0f);

        fill_rect_safe(x, (int16_t)(y + 2), w, h,
                       g->goals[i] ? COL_PAD_FULL : COL_PAD);
    }
}

static void paint_actors(const ex15_game_t *g)
{
    unsigned i;
    uint16_t h = (uint16_t)(g->cell_h - 6u);

    for (i = 0; i < EX15_MAX_ACTORS; i++) {
        int16_t y;
        uint16_t col;

        if (!g->actors[i].alive) {
            continue;
        }
        y = (int16_t)(row_y(g, g->actors[i].row) + 3);
        col = (g->actors[i].kind == EX15_ACT_CAR) ? COL_CAR : COL_LOG;
        fill_rect_safe((int16_t)g->actors[i].x, y, (uint16_t)g->actors[i].w, h,
                       col);
    }
}

static void paint_frog(const ex15_game_t *g)
{
    float fw = (float)g->cell_w * 0.7f;
    uint16_t fh = (uint16_t)(g->cell_h - 6u);
    int16_t y = (int16_t)(row_y(g, g->frog_row) + 3);

    fill_rect_safe((int16_t)g->frog_x, y, (uint16_t)fw, fh, COL_FROG);
    fill_rect_safe((int16_t)(g->frog_x + 2.0f), (int16_t)(y + 2), 3, 3,
                   RBGFX_WHITE);
    fill_rect_safe((int16_t)(g->frog_x + fw - 5.0f), (int16_t)(y + 2), 3, 3,
                   RBGFX_WHITE);
}

static void paint_hud(const ex15_game_t *g)
{
    char line[40];

    (void)snprintf(line, sizeof(line), "SCORE %lu", (unsigned long)g->score);
    text_safe(4, 2, line, COL_HUD, COL_SAFE);
    (void)snprintf(line, sizeof(line), "L%d W%d", (int)g->lives, (int)g->wave);
    text_safe((int16_t)(g->screen_w / 2 - 24), 2, line, COL_HUD, COL_SAFE);
    text_safe((int16_t)(g->screen_w - 48), 2, g->live_imu ? "LIVE" : "SIM ",
              g->live_imu ? RBGFX_GREEN : RBGFX_YELLOW, COL_SAFE);
}

static void paint_world(const ex15_game_t *g)
{
    paint_lanes(g);
    paint_goals(g);
    paint_actors(g);
    paint_frog(g);
    paint_hud(g);
}

static void paint_title(const ex15_game_t *g)
{
    fill_rect_safe(0, 0, g->screen_w, g->screen_h, COL_WATER);
    text_safe((int16_t)(g->screen_w / 2 - 36), 70, "FROGGER", RBGFX_GREEN,
              COL_WATER);
    text_safe((int16_t)(g->screen_w / 2 - 56), 105, "tilt = left/right", COL_HUD,
              COL_WATER);
    text_safe((int16_t)(g->screen_w / 2 - 56), 125, "tap/BOOT = hop", COL_HUD,
              COL_WATER);
    text_safe((int16_t)(g->screen_w / 2 - 64), 160, "TAP / BOOT start",
              RBGFX_YELLOW, COL_WATER);
    if (g->best > 0u) {
        char line[32];
        (void)snprintf(line, sizeof(line), "best %lu", (unsigned long)g->best);
        text_safe((int16_t)(g->screen_w / 2 - 32), 190, line, COL_HUD, COL_WATER);
    }
}

static void paint_overlay(const ex15_game_t *g, const char *title,
                          const char *sub, uint16_t title_fg)
{
    char line[32];

    paint_world(g);
    fill_rect_safe(24, 80, (uint16_t)(g->screen_w - 48), 80, RBGFX_BLACK);
    text_safe((int16_t)(g->screen_w / 2 - 40), 95, title, title_fg, RBGFX_BLACK);
    (void)snprintf(line, sizeof(line), "score %lu", (unsigned long)g->score);
    text_safe((int16_t)(g->screen_w / 2 - 40), 120, line, COL_HUD, RBGFX_BLACK);
    text_safe((int16_t)(g->screen_w / 2 - 56), 140, sub, RBGFX_YELLOW,
              RBGFX_BLACK);
}

void ex15_gfx_paint(const ex15_game_t *g)
{
    switch (g->phase) {
    case EX15_PHASE_TITLE:
        paint_title(g);
        break;
    case EX15_PHASE_LIFE:
        paint_overlay(g, "SPLAT!", "wait...", RBGFX_RED);
        break;
    case EX15_PHASE_WIN:
        paint_overlay(g, "WAVE CLEAR", "next...", RBGFX_GREEN);
        break;
    case EX15_PHASE_OVER:
        paint_overlay(g, "GAME OVER", "tap/BOOT retry", RBGFX_RED);
        break;
    default:
        paint_world(g);
        break;
    }
    flush_safe();
}
