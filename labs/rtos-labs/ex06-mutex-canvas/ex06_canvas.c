#include "ex06_canvas.h"

#include "bsp.h"
#include "rb_button.h"
#include "rb_screen.h"
#include "rb_style.h"
#include "rb_term.h"
#include "ui/gfx.h"

#include <stdio.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define BLOB_W 36
#define BLOB_H 26
#define GAP_MS 70u
#define UI_MS 50u
#define COLOR_A RBGFX_RGB(20, 60, 180)
#define COLOR_B RBGFX_RGB(20, 140, 60)

typedef struct {
    int16_t x;
    int16_t y;
    int16_t px;
    int16_t py;
    int16_t vx;
    int16_t vy;
    uint16_t fill;
    char tag;
} ball_t;

static rb_display_t s_disp;
static rb_screen_t s_screen;
static rb_layout_t s_layout;
static rb_term_t s_status;
static rb_term_t s_canvas_pad;
static rb_button_t s_btn;
static rb_rect_t s_canvas;
static ball_t s_ball[EX06_BALL_COUNT];
static ex06_mutex_toggle_fn s_on_toggle;
static void *s_on_toggle_user;
static uint8_t s_status_ready;

static void fill_rect_safe(int16_t x, int16_t y, uint16_t w, uint16_t h,
                           uint16_t color)
{
    bsp_display_lock();
    rbgfx_fill_rect(&s_disp.gfx, x, y, w, h, color);
    bsp_display_unlock();
}

static void hline_safe(int16_t x, int16_t y, uint16_t w, uint16_t color)
{
    bsp_display_lock();
    rbgfx_hline(&s_disp.gfx, x, y, w, color);
    bsp_display_unlock();
}

static void vline_safe(int16_t x, int16_t y, uint16_t h, uint16_t color)
{
    bsp_display_lock();
    rbgfx_vline(&s_disp.gfx, x, y, h, color);
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

static void clear_canvas(void)
{
    fill_rect_safe(s_canvas.x, s_canvas.y, s_canvas.w, s_canvas.h, RBGFX_DGRAY);
}

static void draw_ball(const ball_t *b)
{
    char tag[2] = {b->tag, '\0'};

    fill_rect_safe(b->x, b->y, BLOB_W, BLOB_H, b->fill);
    hline_safe(b->x, b->y, BLOB_W, RBGFX_WHITE);
    hline_safe(b->x, (int16_t)(b->y + BLOB_H - 1), BLOB_W, RBGFX_WHITE);
    vline_safe(b->x, b->y, BLOB_H, RBGFX_WHITE);
    vline_safe((int16_t)(b->x + BLOB_W - 1), b->y, BLOB_H, RBGFX_WHITE);
    text_safe((int16_t)(b->x + 12), (int16_t)(b->y + 5), tag, RBGFX_WHITE,
              b->fill);
}

static void clamp_ball(ball_t *b)
{
    int16_t x0 = s_canvas.x;
    int16_t y0 = s_canvas.y;
    int16_t x1 = (int16_t)(s_canvas.x + (int16_t)s_canvas.w - BLOB_W);
    int16_t y1 = (int16_t)(s_canvas.y + (int16_t)s_canvas.h - BLOB_H);

    if (x1 < x0) {
        x1 = x0;
    }
    if (y1 < y0) {
        y1 = y0;
    }
    if (b->x < x0) {
        b->x = x0;
        b->vx = (int16_t)(-b->vx);
    } else if (b->x > x1) {
        b->x = x1;
        b->vx = (int16_t)(-b->vx);
    }
    if (b->y < y0) {
        b->y = y0;
        b->vy = (int16_t)(-b->vy);
    } else if (b->y > y1) {
        b->y = y1;
        b->vy = (int16_t)(-b->vy);
    }
}

static void sync_prev(void)
{
    unsigned i;

    for (i = 0; i < EX06_BALL_COUNT; i++) {
        s_ball[i].px = s_ball[i].x;
        s_ball[i].py = s_ball[i].y;
    }
}

static void on_button(rb_button_t *btn, rb_button_event_t ev, void *user)
{
    (void)user;
    if (ev != RB_BUTTON_EV_CLICK) {
        return;
    }
    if (s_on_toggle != NULL) {
        s_on_toggle(rb_button_is_on(btn) ? 1 : 0, s_on_toggle_user);
    }
}

static void layout_chrome(void)
{
    uint16_t H = s_disp.gfx.height;
    uint16_t W = s_disp.gfx.width;
    uint16_t gap = 4;
    uint16_t status_h = 40;
    uint16_t btn_h = 48;
    uint16_t canvas_h = (uint16_t)(H - status_h - btn_h - gap * 2);

    s_status.base.rect.x = 0;
    s_status.base.rect.y = 0;
    s_status.base.rect.w = W;
    s_status.base.rect.h = status_h;
    (void)rb_term_apply_geometry(&s_status);

    s_canvas_pad.base.rect.x = 0;
    s_canvas_pad.base.rect.y = (int16_t)(status_h + gap);
    s_canvas_pad.base.rect.w = W;
    s_canvas_pad.base.rect.h = canvas_h;
    (void)rb_term_apply_geometry(&s_canvas_pad);

    s_btn.base.rect.x = 0;
    s_btn.base.rect.y = (int16_t)(status_h + gap + canvas_h + gap);
    s_btn.base.rect.w = W;
    s_btn.base.rect.h = btn_h;
    (void)rb_button_apply_geometry(&s_btn);

    s_canvas = s_canvas_pad.base.rect;
}

int ex06_canvas_begin(ex06_mutex_toggle_fn on_toggle, void *user)
{
    s_on_toggle = on_toggle;
    s_on_toggle_user = user;
    s_status_ready = 0;

    if (rb_display_open(&s_disp) != 0) {
        return -1;
    }
    if (rb_screen_init(&s_screen, &s_disp) != 0) {
        return -1;
    }
    if (rb_layout_init(&s_layout, &s_screen, RB_LAYOUT_GRID, 3, 1, 4) != 0) {
        return -1;
    }

    rb_term_init(&s_status);
    (void)rb_term_set_scale(&s_status, 1);
    (void)rb_layout_add(&s_layout, &s_status.base);

    rb_term_init(&s_canvas_pad);
    (void)rb_term_set_scale(&s_canvas_pad, 1);
    (void)rb_layout_add(&s_layout, &s_canvas_pad.base);

    rb_button_init(&s_btn);
    rb_button_set_mode(&s_btn, RB_BUTTON_MODE_TOGGLE);
    rb_button_set_labels(&s_btn, "ON", "OFF");
    rb_button_set_style(&s_btn, RB_STYLE_WARNING);
    rb_button_set_indicator_size(&s_btn, 80, 2);
    rb_button_set_on(&s_btn, 0);
    rb_button_set_on_event(&s_btn, on_button, NULL);
    (void)rb_layout_add(&s_layout, &s_btn.base);

    rb_screen_set_root(&s_screen, &s_layout.base);
    rb_screen_reflow(&s_screen);
    layout_chrome();

    clear_canvas();
    flush_safe();

    s_ball[EX06_BALL_A].fill = COLOR_A;
    s_ball[EX06_BALL_A].tag = 'A';
    s_ball[EX06_BALL_A].x = (int16_t)(s_canvas.x + 20);
    s_ball[EX06_BALL_A].y = (int16_t)(s_canvas.y + 18);
    s_ball[EX06_BALL_A].vx = 3;
    s_ball[EX06_BALL_A].vy = 2;

    s_ball[EX06_BALL_B].fill = COLOR_B;
    s_ball[EX06_BALL_B].tag = 'B';
    s_ball[EX06_BALL_B].x = (int16_t)(s_canvas.x + 80);
    s_ball[EX06_BALL_B].y = (int16_t)(s_canvas.y + 40);
    s_ball[EX06_BALL_B].vx = -2;
    s_ball[EX06_BALL_B].vy = 3;

    clamp_ball(&s_ball[EX06_BALL_A]);
    clamp_ball(&s_ball[EX06_BALL_B]);
    sync_prev();
    return 0;
}

void ex06_canvas_reset(void)
{
    clear_canvas();
    flush_safe();
    sync_prev();
}

void ex06_canvas_paint_clean(void)
{
    unsigned i;

    for (i = 0; i < EX06_BALL_COUNT; i++) {
        fill_rect_safe(s_ball[i].px, s_ball[i].py, BLOB_W, BLOB_H, RBGFX_DGRAY);
    }
    draw_ball(&s_ball[EX06_BALL_A]);
    draw_ball(&s_ball[EX06_BALL_B]);
    flush_safe();
    sync_prev();
}

void ex06_canvas_paint_strobe(void)
{
    clear_canvas();
    flush_safe();
    vTaskDelay(pdMS_TO_TICKS(GAP_MS));

    draw_ball(&s_ball[EX06_BALL_A]);
    flush_safe();
    vTaskDelay(pdMS_TO_TICKS(GAP_MS));

    draw_ball(&s_ball[EX06_BALL_B]);
    flush_safe();
    vTaskDelay(pdMS_TO_TICKS(GAP_MS));

    sync_prev();
}

void ex06_canvas_advance(unsigned ball_idx)
{
    ball_t *b;

    if (ball_idx >= EX06_BALL_COUNT) {
        return;
    }
    b = &s_ball[ball_idx];
    b->x = (int16_t)(b->x + b->vx);
    b->y = (int16_t)(b->y + b->vy);
    clamp_ball(b);
}

void ex06_canvas_ui_tick(int mutex_on, uint32_t frames_a, uint32_t frames_b)
{
    char line[RB_TERM_LOG_MAX_COLS];
    bsp_touch_sample_t smp;

    if (!s_status_ready) {
        rb_term_set_padding(&s_status, 4, 2, 4, 2);
        (void)rb_term_apply_geometry(&s_status);
        s_status_ready = 1;
    }

    if (bsp_touch_read(&smp) == 0) {
        (void)rb_button_feed_touch(&s_btn, &smp);
    }

    (void)snprintf(line, sizeof(line), "mutex %s  %s", mutex_on ? "ON " : "OFF",
                   mutex_on ? "CLEAN" : "STROBE");
    rb_term_set_line_ex(&s_status, 0, RB_TERM_WHITE, RB_TERM_BLACK, line);
    (void)snprintf(line, sizeof(line), "A#%lu  B#%lu",
                   (unsigned long)frames_a, (unsigned long)frames_b);
    rb_term_set_line_ex(&s_status, 1, RB_TERM_CYAN, RB_TERM_BLACK, line);

    vTaskDelay(pdMS_TO_TICKS(UI_MS));
}
