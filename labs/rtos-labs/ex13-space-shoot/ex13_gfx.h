#ifndef RTOS_LABS_EX13_GFX_H
#define RTOS_LABS_EX13_GFX_H

#include "ex13_game.h"

#ifdef __cplusplus
extern "C" {
#endif

int ex13_gfx_begin(void);
void ex13_gfx_paint(const ex13_game_t *g);
uint16_t ex13_gfx_width(void);
uint16_t ex13_gfx_height(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX13_GFX_H */
