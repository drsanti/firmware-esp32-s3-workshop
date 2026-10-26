/*
 * gfx.h — framebuffer-mode RGB565 graphics for live animation.
 *
 * Model (animation-first, chosen over row-streaming): a full
 * framebuffer lives in PSRAM; all draw ops write to it and extend a
 * dirty bounding box; rbgfx_flush() pushes only the dirty region
 * through the display device. Live plots/spectrograms stay cheap —
 * a scrolling scope trace dirties one column strip per frame.
 *
 *   rbgfx_init(&g, fb, 320, 240, &disp);   // fb from heap_caps(SPIRAM)
 *   rbgfx_fill(&g, BLACK);  rbgfx_text(&g, 8,8, "ok", WHITE);
 *   rbgfx_flush(&g);                       // dirty region only
 *
 * Not ISR-safe, not task-safe — render from one context (the status
 * module runs on the executor tick; keep flush there too).
 */
#ifndef RB_UI_GFX_H
#define RB_UI_GFX_H

#include <stdint.h>
#include "display.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RBGFX_RGB(r, g, b) \
    ((uint16_t)((((r)&0xF8u) << 8) | (((g)&0xFCu) << 3) | ((b) >> 3)))
#define RBGFX_BLACK   0x0000u
#define RBGFX_WHITE   0xFFFFu
#define RBGFX_RED     0xF800u
#define RBGFX_GREEN   0x07E0u
#define RBGFX_BLUE    0x001Fu
#define RBGFX_YELLOW  0xFFE0u
#define RBGFX_CYAN    0x07FFu
#define RBGFX_MAGENTA 0xF81Fu
#define RBGFX_GRAY    0x7BEFu
#define RBGFX_DGRAY   0x39E7u

typedef struct {
    display_dev_t *dev;
    uint16_t *fb;        /* width*height pixels, caller-owned (PSRAM) */
    uint16_t width, height;
    /* dirty bounding box, inclusive; dirty=0 means clean */
    uint16_t dx0, dy0, dx1, dy1;
    uint8_t dirty;
} rbgfx_t;

/* fb must be width*height*2 bytes, heap-allocatable in PSRAM. */
int rbgfx_init(rbgfx_t *g, uint16_t *fb, uint16_t w, uint16_t h,
             display_dev_t *dev);

/* Drawing — all extend the dirty region, none touch the panel. */
void rbgfx_fill(rbgfx_t *g, uint16_t color);
void rbgfx_fill_rect(rbgfx_t *g, int16_t x, int16_t y, uint16_t w,
                   uint16_t h, uint16_t color);
void rbgfx_pixel(rbgfx_t *g, int16_t x, int16_t y, uint16_t color);
void rbgfx_hline(rbgfx_t *g, int16_t x, int16_t y, uint16_t w, uint16_t c);
void rbgfx_vline(rbgfx_t *g, int16_t x, int16_t y, uint16_t h, uint16_t c);
/* filled circle — row spans via hline so clipping/dirty come free */
void rbgfx_fill_circle(rbgfx_t *g, int16_t cx, int16_t cy, uint16_t r,
                     uint16_t color);
/* Filled annular sector. Angles in degrees from +X (math); screen Y
 * grows down (y = cy - sin*r). outer_r > inner_r.
 * sweep_deg > 0 = CCW from start; < 0 = CW; ±360 = full ring. */
void rbgfx_fill_arc(rbgfx_t *g, int16_t cx, int16_t cy, uint16_t outer_r,
                    uint16_t inner_r, int16_t start_deg, int16_t sweep_deg,
                    uint16_t color);
/* filled round-rect; rad is corner radius (clamped to min(w,h)/2) */
void rbgfx_fill_round_rect(rbgfx_t *g, int16_t x, int16_t y, uint16_t w,
                           uint16_t h, uint16_t rad, uint16_t color);
/* 8x16 bitmap text; returns next x. scale 1..3 (pixel blocks). */
int16_t rbgfx_text(rbgfx_t *g, int16_t x, int16_t y, const char *s,
                 uint16_t color, uint16_t bg);
int16_t rbgfx_text_scaled(rbgfx_t *g, int16_t x, int16_t y, const char *s,
                          uint16_t color, uint16_t bg, uint8_t scale);
int16_t rbgfx_text_fmt(rbgfx_t *g, int16_t x, int16_t y, uint16_t color,
                     uint16_t bg, const char *fmt, ...);

/* Push the dirty region through dev->flush; marks the FB clean.
 * No-op when clean or when there is no device. Returns flush rc. */
/* BLIT: row-major RGB565 into a rect — clipped to the framebuffer;
 * returns px written. Pixel data is whatever the caller decoded. */
int rbgfx_blit(rbgfx_t *g, int16_t x, int16_t y, uint16_t w, uint16_t h,
             const uint16_t *px);
int rbgfx_flush(rbgfx_t *g);

#ifdef __cplusplus
}
#endif

#endif /* RB_UI_GFX_H */
