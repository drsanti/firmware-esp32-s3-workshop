#ifndef RTOS_LABS_EX15_GFX_H
#define RTOS_LABS_EX15_GFX_H

#include "ex15_game.h"

#ifdef __cplusplus
extern "C" {
#endif

int ex15_gfx_begin(void);
void ex15_gfx_paint(const ex15_game_t *g);
uint16_t ex15_gfx_width(void);
uint16_t ex15_gfx_height(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX15_GFX_H */
