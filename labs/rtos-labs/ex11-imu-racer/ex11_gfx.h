/*
 * ex11_gfx — full-screen rbgfx paint for the IMU racer.
 */
#ifndef RTOS_LABS_EX11_GFX_H
#define RTOS_LABS_EX11_GFX_H

#include "ex11_game.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Open display. 0 = ok. */
int ex11_gfx_begin(void);

/* Draw one frame from game state and flush. */
void ex11_gfx_paint(const ex11_game_t *g);

uint16_t ex11_gfx_width(void);
uint16_t ex11_gfx_height(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX11_GFX_H */
