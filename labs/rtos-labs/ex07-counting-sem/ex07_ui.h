/*
 * ex07_ui — LCD layout + status for the counting-sem pool lab.
 * Keeps FreeRTOS Take/Give logic out of the drawing code.
 */
#ifndef RTOS_LABS_EX07_UI_H
#define RTOS_LABS_EX07_UI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { EX07_WORKER_COUNT = 4, EX07_SLOTS = 2 };
enum { EX07_ST_WAIT = 0, EX07_ST_HOLD = 1 };

/* Called when the POOL toggle is clicked; is_on 1 = counting sem enabled. */
typedef void (*ex07_pool_toggle_fn)(int is_on, void *user);

/* Build status term + toggle. 0 = ok. */
int ex07_ui_begin(ex07_pool_toggle_fn on_toggle, void *user);

/*
 * Touch poll + redraw.
 * free_n < 0 means show "free --" (pool OFF).
 * state[i] / done[i] for i in 0..EX07_WORKER_COUNT-1.
 */
void ex07_ui_tick(int pool_on, int free_n, unsigned in_use,
                  const volatile uint8_t *state,
                  const volatile uint32_t *done);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX07_UI_H */
