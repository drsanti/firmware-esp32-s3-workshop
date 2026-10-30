/*
 * ex08_ui — LCD checklist + bit/mode toggles for the event-group lab.
 * Keeps WaitBits / SetBits logic out of the drawing code.
 */
#ifndef RTOS_LABS_EX08_UI_H
#define RTOS_LABS_EX08_UI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { EX08_BIT_A = (1u << 0), EX08_BIT_B = (1u << 1) };
enum { EX08_BIT_COUNT = 2 };
enum { EX08_PHASE_WAIT = 0, EX08_PHASE_FIRE = 1 };

/* Bit toggle: bit_idx 0=A 1=B; is_on 1 = set that bit. */
typedef void (*ex08_bit_toggle_fn)(unsigned bit_idx, int is_on, void *user);

/* Mode toggle: wait_all 1 = AND (all bits), 0 = OR (any bit). */
typedef void (*ex08_mode_toggle_fn)(int wait_all, void *user);

/* Build status term + A/B/MODE toggles. 0 = ok. */
int ex08_ui_begin(ex08_bit_toggle_fn on_bit, ex08_mode_toggle_fn on_mode,
                  void *user);

/* After WaitBits clearOnExit, force A/B buttons OFF to match cleared bits. */
void ex08_ui_reset_bit_buttons(void);

/* Touch poll + redraw. bits = current EventBits snapshot. */
void ex08_ui_tick(int wait_all, int phase, uint32_t bits, uint32_t fire_count);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX08_UI_H */
