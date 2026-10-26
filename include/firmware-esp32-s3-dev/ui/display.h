/*
 * display.h — portable RGB565 display device contract.
 *
 * Same pattern as rb_transport: portable code (gfx, status screens,
 * plot widgets) talks to this vtable; each port implements it
 * (e.g. ports/boards/ws28p/st7789.c). No heap inside
 * implementations beyond init-time driver allocation.
 */
#ifndef RB_UI_DISPLAY_H
#define RB_UI_DISPLAY_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t width;   /* pixels, in the current orientation */
    uint16_t height;
} disp_res_t;

/*
 * Flush a rectangle of RGB565 pixels to the panel.
 * `data` points to the top-left pixel of the rect inside a
 * row-major buffer whose row stride is `stride` pixels
 * (== rect width for packed buffers, full width for FB views).
 * Implementation handles byte-swap/DMA/chunking internally.
 */
typedef int (*disp_flush_fn)(void *ctx, uint16_t x0, uint16_t y0,
                             uint16_t x1, uint16_t y1,
                             const uint16_t *data, uint16_t stride);
/* Backlight 0..100 percent. May be NULL. */
typedef int (*disp_backlight_fn)(void *ctx, uint8_t percent);
/* Display on/off. May be NULL. */
typedef int (*disp_power_fn)(void *ctx, int on);

typedef struct display_dev {
    void *ctx;
    disp_res_t res;
    disp_flush_fn flush;
    disp_backlight_fn backlight;
    disp_power_fn power;
} display_dev_t;

#ifdef __cplusplus
}
#endif

#endif /* RB_UI_DISPLAY_H */
