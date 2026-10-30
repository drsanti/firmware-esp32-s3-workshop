/*
 * ex09_ui — LCD status + mode toggle for the reload-timer lab.
 * Keeps timer / delay logic out of the drawing code.
 */
#ifndef RTOS_LABS_EX09_UI_H
#define RTOS_LABS_EX09_UI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { EX09_MODE_DELAY = 0, EX09_MODE_TIMER = 1 };

/* Called when the mode toggle is clicked; use_timer 1 = software timer. */
typedef void (*ex09_mode_toggle_fn)(int use_timer, void *user);

/* Build status term + toggle. 0 = ok. */
int ex09_ui_begin(ex09_mode_toggle_fn on_toggle, void *user);

/*
 * Touch poll + redraw.
 * flash_on 1 = show a visible beat pulse on the status line.
 */
void ex09_ui_tick(int use_timer, uint32_t beats, int flash_on, unsigned period_ms);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX09_UI_H */
