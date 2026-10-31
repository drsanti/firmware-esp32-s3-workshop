#ifndef RTOS_LABS_EX12_GFX_H
#define RTOS_LABS_EX12_GFX_H

#include "ex12_game.h"

#ifdef __cplusplus
extern "C" {
#endif

int ex12_gfx_begin(void);
void ex12_gfx_paint(const ex12_game_t *g);
uint16_t ex12_gfx_width(void);
uint16_t ex12_gfx_height(void);

#ifdef __cplusplus
}
#endif

#endif /* RTOS_LABS_EX12_GFX_H */
