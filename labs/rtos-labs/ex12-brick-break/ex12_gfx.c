#include "ex12_gfx.h"

#include "bsp.h"
#include "rb_screen.h"
#include "ui/gfx.h"

#include <stdio.h>

#define COL_BG RBGFX_RGB(8, 12, 28)
#define COL_PAD RBGFX_RGB(40, 200, 255)
#define COL_BALL RBGFX_WHITE
#define COL_HUD RBGFX_WHITE

static const uint16_t s_row_color[EX12_BRICK_ROWS] = {
    RBGFX_RGB(220, 50, 40),
    RBGFX_RGB(240, 140, 30),
    RBGFX_RGB(240, 210, 40),
    RBGFX_RGB(40, 180, 70),
};

static rb_display_t s_disp;

static void fill_rect_safe(int16_t x, int16_t y, uint16_t w, uint16_t h,
                           uint16_t color)
{
    bsp_display_lock();
    rbgfx_fill_rect(&s_disp.gfx, x, y, w, h, color);
    bsp_display_unlock();
}

static void fill_circle_safe(int16_t cx, int16_t cy, uint16_t r, uint16_t color)
{
    bsp_display_lock();
    rbgfx_fill_circle(&s_disp.gfx, cx, cy, r, color);
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

int ex12_gfx_begin(void)
{
    if (rb_display_open(&s_disp) != 0) {
        return -1;
    }
    return 0;
}

uint16_t ex12_gfx_width(void)
{
    return rb_display_width(&s_disp);
}

uint16_t ex12_gfx_height(void)
{
    return rb_display_height(&s_disp);
}

static void paint_hud(const ex12_game_t *g)
{
    char line[40];

    (void)snprintf(line, sizeof(line), "SCORE %lu", (unsigned long)g->score);
    text_safe(6, 4, line, COL_HUD, COL_BG);
    (void)snprintf(line, sizeof(line), "L%d", (int)g->lives);
    text_safe((int16_t)(g->screen_w / 2 - 8), 4, line, COL_HUD, COL_BG);
    text_safe((int16_t)(g->screen_w - 48), 4, g->live_imu ? "LIVE" : "SIM ",
              g->live_imu ? RBGFX_GREEN : RBGFX_YELLOW, COL_BG);
}

static void paint_world(const ex12_game_t *g)
{
    unsigned i;

    fill_rect_safe(0, 0, g->screen_w, g->screen_h, COL_BG);
    paint_hud(g);

    for (i = 0; i < EX12_BRICK_ROWS * EX12_BRICK_COLS; i++) {
        const ex12_brick_t *b = &g->bricks[i];
        uint16_t col;

        if (!b->alive) {
            continue;
        }
        col = s_row_color[b->row < EX12_BRICK_ROWS ? b->row : 0];
        fill_rect_safe((int16_t)b->x, (int16_t)b->y, (uint16_t)b->w,
                       (uint16_t)b->h, col);
    }

    fill_rect_safe((int16_t)g->pad.x, (int16_t)g->pad.y, (uint16_t)g->pad.w,
                   (uint16_t)g->pad.h, COL_PAD);
    fill_circle_safe((int16_t)g->ball.x, (int16_t)g->ball.y,
                     (uint16_t)g->ball.r, COL_BALL);
}

static void paint_title(const ex12_game_t *g)
{
    fill_rect_safe(0, 0, g->screen_w, g->screen_h, COL_BG);
    text_safe((int16_t)(g->screen_w / 2 - 56), 70, "BRICK BREAKER", RBGFX_CYAN,
              COL_BG);
    text_safe((int16_t)(g->screen_w / 2 - 52), 100, "tilt = paddle", COL_HUD,
              COL_BG);
    text_safe((int16_t)(g->screen_w / 2 - 64), 130, "TAP/BOOT start",
              RBGFX_YELLOW, COL_BG);
    text_safe((int16_t)(g->screen_w / 2 - 72), 155, "TAP again launch", COL_HUD,
              COL_BG);
    if (g->best > 0u) {
        char line[32];
        (void)snprintf(line, sizeof(line), "best %lu", (unsigned long)g->best);
        text_safe((int16_t)(g->screen_w / 2 - 32), 185, line, COL_HUD, COL_BG);
    }
}

static void paint_overlay(const ex12_game_t *g, const char *title,
                          const char *sub)
{
    char line[32];

    paint_world(g);
    fill_rect_safe(24, 70, (uint16_t)(g->screen_w - 48), 90, RBGFX_BLACK);
    text_safe((int16_t)(g->screen_w / 2 - 40), 85, title, RBGFX_RED, RBGFX_BLACK);
    (void)snprintf(line, sizeof(line), "score %lu", (unsigned long)g->score);
    text_safe((int16_t)(g->screen_w / 2 - 40), 110, line, COL_HUD, RBGFX_BLACK);
    text_safe((int16_t)(g->screen_w / 2 - 56), 135, sub, RBGFX_YELLOW,
              RBGFX_BLACK);
}

void ex12_gfx_paint(const ex12_game_t *g)
{
    switch (g->phase) {
    case EX12_PHASE_TITLE:
        paint_title(g);
        break;
    case EX12_PHASE_LIFE:
        paint_overlay(g, "LIFE -1", "wait...");
        break;
    case EX12_PHASE_OVER:
        paint_overlay(g, "GAME OVER", "tap/BOOT retry");
        break;
    case EX12_PHASE_CLEAR:
        paint_overlay(g, "CLEAR!", "tap/BOOT again");
        break;
    default:
        paint_world(g);
        if (g->ball.stuck) {
            text_safe((int16_t)(g->screen_w / 2 - 40),
                      (int16_t)(g->screen_h / 2), "TAP launch", RBGFX_YELLOW,
                      COL_BG);
        }
        break;
    }
    flush_safe();
}
