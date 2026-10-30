/*
 * lab_spirit.c - display / rb_level bubble / LEVEL status.
 *
 * Layout: thin top bar (title | status), bubble fills the rest.
 * Equal-row layout would shrink the level; we size rects by hand.
 */
#include "lab_spirit.h"

#include "rb_label.h"
#include "rb_level.h"
#include "rb_screen.h"

#ifndef SPIRIT_LEVEL_G
#define SPIRIT_LEVEL_G 0.08f /* |ax_g|,|ay_g| below this => LEVEL */
#endif

/* Top chrome: pad + text row (scale-1 glyphs are 16 px tall) + gap. */
#ifndef SPIRIT_TOP_PAD
#define SPIRIT_TOP_PAD 4
#endif
#ifndef SPIRIT_TEXT_H
#define SPIRIT_TEXT_H 20 /* >= font BASE_H (16) so glyphs are not cropped */
#endif
#ifndef SPIRIT_GAP
#define SPIRIT_GAP 8 /* space between text row and bubble */
#endif

#define G_MPS2 9.81f

static rb_display_t s_disp;
static rb_screen_t s_screen;
static rb_label_t s_title;
static rb_label_t s_status;
static rb_level_t s_level;
static int s_inited;
static int s_started;
static int s_is_level;
static int s_was_level = -1;

void spirit_init(void)
{
    if (s_inited) {
        return;
    }

    (void)rb_display_open(&s_disp);
    (void)rb_screen_init(&s_screen, &s_disp);

    rb_label_init(&s_title);
    (void)rb_label_set_scale(&s_title, 1);
    rb_label_set_color(&s_title, RB_LABEL_CYAN, RB_LABEL_BLACK);
    rb_label_printf(&s_title, "Spirit");

    rb_label_init(&s_status);
    (void)rb_label_set_scale(&s_status, 1);
    rb_label_set_color(&s_status, RB_LABEL_YELLOW, RB_LABEL_BLACK);
    rb_label_printf(&s_status, "TILT");

    rb_level_init(&s_level);
    rb_level_set_show_cross(&s_level, 1);
    rb_level_set_live(&s_level, 1);
    s_level.pad_px = 4; /* less inset => larger field */

    s_is_level = 0;
    s_was_level = -1;
    s_inited = 1;
    s_started = 0;
}

void spirit_start(void)
{
    uint16_t W;
    uint16_t H;
    uint16_t top_pad = (uint16_t)SPIRIT_TOP_PAD;
    uint16_t text_h = (uint16_t)SPIRIT_TEXT_H;
    uint16_t gap = (uint16_t)SPIRIT_GAP;
    uint16_t header;
    uint16_t half;
    int16_t text_y;
    int16_t level_y;

    if (!s_inited || s_started) {
        return;
    }

    W = rb_display_width(&s_disp);
    H = rb_display_height(&s_disp);
    header = (uint16_t)(top_pad + text_h + gap);
    if (header + 64u > H) {
        top_pad = 2;
        text_h = 18;
        gap = 4;
        header = (uint16_t)(top_pad + text_h + gap);
    }
    half = (uint16_t)(W / 2u);
    text_y = (int16_t)top_pad;
    level_y = (int16_t)(top_pad + text_h + gap);

    /* Top strip: title (left) + status (right) — tall enough for 16 px glyphs. */
    s_title.base.disp = &s_disp;
    s_title.base.rect.x = 4;
    s_title.base.rect.y = text_y;
    s_title.base.rect.w = (uint16_t)(half - 6u);
    s_title.base.rect.h = text_h;
    (void)rb_label_apply_geometry(&s_title);

    s_status.base.disp = &s_disp;
    s_status.base.rect.x = (int16_t)half;
    s_status.base.rect.y = text_y;
    s_status.base.rect.w = (uint16_t)(W - half - 4u);
    s_status.base.rect.h = text_h;
    (void)rb_label_apply_geometry(&s_status);

    /* Bubble below a small gap under the text row. */
    s_level.base.disp = &s_disp;
    s_level.base.rect.x = 0;
    s_level.base.rect.y = level_y;
    s_level.base.rect.w = W;
    s_level.base.rect.h = (uint16_t)(H - (uint16_t)level_y);
    (void)rb_level_apply_geometry(&s_level);

    /* Keep a screen root so flush/path stays consistent with other labs. */
    rb_screen_set_root(&s_screen, &s_level.base);

    s_started = 1;
    spirit_set_accel_mps2(0.0f, 0.0f);
}

void spirit_set_live(int on)
{
    if (!s_started) {
        return;
    }
    rb_level_set_live(&s_level, on ? 1 : 0);
}

void spirit_set_accel_mps2(float ax, float ay)
{
    float ax_g;
    float ay_g;
    int level;

    if (!s_started) {
        return;
    }

    rb_level_set_accel_mps2(&s_level, ax, ay);

    ax_g = ax / G_MPS2;
    ay_g = ay / G_MPS2;
    if (ax_g < 0.0f) {
        ax_g = -ax_g;
    }
    if (ay_g < 0.0f) {
        ay_g = -ay_g;
    }
    level = (ax_g < SPIRIT_LEVEL_G && ay_g < SPIRIT_LEVEL_G) ? 1 : 0;
    s_is_level = level;

    if (level != s_was_level) {
        s_was_level = level;
        if (level) {
            rb_label_set_color(&s_status, RB_LABEL_GREEN, RB_LABEL_BLACK);
            rb_label_printf(&s_status, "LEVEL");
        } else {
            rb_label_set_color(&s_status, RB_LABEL_YELLOW, RB_LABEL_BLACK);
            rb_label_printf(&s_status, "TILT");
        }
    }
}

int spirit_is_level(void)
{
    return s_is_level;
}
