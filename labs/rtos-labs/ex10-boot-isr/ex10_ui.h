/*
 * ex10_ui — LCD status + POLL/ISR mode toggle for the boot-isr lab.
 * Keeps FromISR / poll logic out of the drawing code.
 */
#ifndef RTOS_LABS_EX10_UI_H
#define RTOS_LABS_EX10_UI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { EX10_MODE_POLL = 0, EX10_MODE_ISR = 1 };

/* Called when the mode toggle is clicked; use_isr 1 = GPIO ISR path. */
typedef void (*ex10_mode_toggle_fn)(int use_isr, void *user);

/* Build status term + toggle. 0 = ok. */
int ex10_ui_begin(ex10_mode_toggle_fn on_toggle, void *user);

/*
 * Touch poll + redraw.
 * flash_on 1 = visible press pulse; ui_ticks shows UI stays alive.
 */
void ex10_ui_tick(int use_isr, uint32_t presses, uint32_t ui_ticks,
                  int flash_on);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX10_UI_H */
