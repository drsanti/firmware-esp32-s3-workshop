/*
 * ex02_ui — LCD status + LIGHT/BUSY toggle for the OS-tick lab.
 * Keeps tick / hook logic out of the drawing code.
 */
#ifndef RTOS_LABS_EX02_UI_H
#define RTOS_LABS_EX02_UI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Called when the mode toggle is clicked; busy 1 = heavy work in tick hook. */
typedef void (*ex02_mode_toggle_fn)(int busy, void *user);

/* Build status term + toggle. 0 = ok. */
int ex02_ui_begin(ex02_mode_toggle_fn on_toggle, void *user);

/*
 * Touch poll + redraw.
 * busy 1 = BUSY hook; hook0/hook1 = per-core hook hit counts;
 * ui_frames = how many times this UI loop ran (drops when hook is BUSY).
 */
void ex02_ui_tick(int busy, uint32_t tick_count, uint32_t hook0, uint32_t hook1,
                  uint32_t ui_frames, unsigned tick_hz, unsigned tick_ms);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX02_UI_H */
