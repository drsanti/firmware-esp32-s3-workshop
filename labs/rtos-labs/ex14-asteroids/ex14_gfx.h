#ifndef RTOS_LABS_EX14_GFX_H
#define RTOS_LABS_EX14_GFX_H

#include "ex14_game.h"

#ifdef __cplusplus
extern "C" {
#endif

int ex14_gfx_begin(void);
void ex14_gfx_paint(const ex14_game_t *g);
uint16_t ex14_gfx_width(void);
uint16_t ex14_gfx_height(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX14_GFX_H */
