#include "ex11_gfx.h"

#include "bsp.h"
#include "rb_screen.h"
#include "ui/gfx.h"

#include <stdio.h>

#define COL_GRASS RBGFX_RGB(20, 90, 30)
#define COL_ROAD RBGFX_RGB(40, 40, 48)
#define COL_DASH RBGFX_RGB(220, 200, 60)
#define COL_CAR RBGFX_RGB(40, 180, 255)
#define COL_OBS RBGFX_RGB(220, 50, 40)
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

int ex11_gfx_begin(void)
{
    if (rb_display_open(&s_disp) != 0) {
        return -1;
    }
    return 0;
}

uint16_t ex11_gfx_width(void)
{
    return rb_display_width(&s_disp);
}

uint16_t ex11_gfx_height(void)
{
    return rb_display_height(&s_disp);
}

static void paint_road(const ex11_game_t *g)
{
    unsigned lane;
    float lane_w;
    int16_t y;
    int16_t dash_h = 14;
    int16_t gap = 14;

    fill_rect_safe(0, 0, g->screen_w, g->screen_h, COL_GRASS);
    fill_rect_safe((int16_t)g->road_x, 0, g->road_w, g->screen_h, COL_ROAD);

    lane_w = (float)g->road_w / (float)EX11_LANES;
    for (lane = 1; lane < EX11_LANES; lane++) {
        int16_t lx = (int16_t)((float)g->road_x + lane_w * (float)lane);
        int period = dash_h + gap;
        /* +scroll so dashes travel down with obstacles (car “moves” up road). */
        int off = (int)g->scroll % period;

        for (y = (int16_t)(off - period); y < (int16_t)g->screen_h;
             y = (int16_t)(y + period)) {
            fill_rect_safe(lx, y, 3, (uint16_t)dash_h, COL_DASH);
        }
    }
}

static void paint_play(const ex11_game_t *g)
{
    char line[32];
    unsigned i;

    paint_road(g);

    for (i = 0; i < EX11_MAX_OBS; i++) {
        if (!g->obs[i].alive) {
            continue;
        }
        fill_rect_safe((int16_t)g->obs[i].x, (int16_t)g->obs[i].y,
                       (uint16_t)g->obs[i].w, (uint16_t)g->obs[i].h, COL_OBS);
    }

    fill_rect_safe((int16_t)g->car.x, (int16_t)g->car.y, (uint16_t)g->car.w,
                   (uint16_t)g->car.h, COL_CAR);
    /* Simple windshield highlight */
    fill_rect_safe((int16_t)(g->car.x + 6), (int16_t)(g->car.y + 6), 16, 10,
                   RBGFX_WHITE);

    (void)snprintf(line, sizeof(line), "SCORE %lu", (unsigned long)g->score);
    text_safe(8, 6, line, COL_HUD, COL_ROAD);
    text_safe((int16_t)(g->screen_w - 56), 6, g->live_imu ? "LIVE" : "SIM ",
              g->live_imu ? RBGFX_GREEN : RBGFX_YELLOW, COL_ROAD);
}

static void paint_title(const ex11_game_t *g)
{
    fill_rect_safe(0, 0, g->screen_w, g->screen_h, RBGFX_BLACK);
    text_safe((int16_t)(g->screen_w / 2 - 40), 70, "IMU RACER", RBGFX_CYAN,
              RBGFX_BLACK);
    text_safe((int16_t)(g->screen_w / 2 - 52), 100, "tilt to steer", COL_HUD,
              RBGFX_BLACK);
    text_safe((int16_t)(g->screen_w / 2 - 64), 140, "TAP / BOOT start",
              RBGFX_YELLOW, RBGFX_BLACK);
    if (g->best > 0u) {
        char line[32];
        (void)snprintf(line, sizeof(line), "best %lu", (unsigned long)g->best);
        text_safe((int16_t)(g->screen_w / 2 - 32), 170, line, COL_HUD,
                  RBGFX_BLACK);
    }
}

static void paint_crash(const ex11_game_t *g)
{
    char line[32];

    paint_road(g);
    fill_rect_safe((int16_t)g->car.x, (int16_t)g->car.y, (uint16_t)g->car.w,
                   (uint16_t)g->car.h, RBGFX_RED);

    fill_rect_safe(40, 70, (uint16_t)(g->screen_w - 80), 90, RBGFX_BLACK);
    text_safe((int16_t)(g->screen_w / 2 - 28), 85, "CRASH!", RBGFX_RED,
              RBGFX_BLACK);
    (void)snprintf(line, sizeof(line), "score %lu", (unsigned long)g->score);
    text_safe((int16_t)(g->screen_w / 2 - 40), 110, line, COL_HUD, RBGFX_BLACK);
    text_safe((int16_t)(g->screen_w / 2 - 60), 135, "tap/BOOT retry",
              RBGFX_YELLOW, RBGFX_BLACK);
}

void ex11_gfx_paint(const ex11_game_t *g)
{
    if (g->phase == EX11_PHASE_TITLE) {
        paint_title(g);
    } else if (g->phase == EX11_PHASE_CRASH) {
        paint_crash(g);
    } else {
        paint_play(g);
    }
    flush_safe();
}
