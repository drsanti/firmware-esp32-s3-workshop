/*
 * ex06_canvas — LCD layout + bouncing-ball paint helpers for ex06.
 * Keeps FreeRTOS mutex logic out of the drawing code.
 */
#ifndef RTOS_LABS_EX06_CANVAS_H
#define RTOS_LABS_EX06_CANVAS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { EX06_BALL_A = 0, EX06_BALL_B = 1, EX06_BALL_COUNT = 2 };

/* Called when the MUTEX toggle is clicked; is_on 1 = mutex enabled. */
typedef void (*ex06_mutex_toggle_fn)(int is_on, void *user);

/* Build status / canvas / button UI. 0 = ok. */
int ex06_canvas_begin(ex06_mutex_toggle_fn on_toggle, void *user);

/* Wipe canvas and sync previous-draw positions (mode switch). */
void ex06_canvas_reset(void);

/* MUTEX ON path: erase old rects, draw both balls, one flush. */
void ex06_canvas_paint_clean(void);

/* MUTEX OFF path: blank → A → both, with panel flushes between. */
void ex06_canvas_paint_strobe(void);

/* Bounce one ball inside the canvas. */
void ex06_canvas_advance(unsigned ball_idx);

/* Touch poll + status lines (mutex_on / frame counters). */
void ex06_canvas_ui_tick(int mutex_on, uint32_t frames_a, uint32_t frames_b);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX06_CANVAS_H */
