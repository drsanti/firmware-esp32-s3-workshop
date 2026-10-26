/*
 * fx_sfx.h — SFX pending-queue + user-program API (@api rb_fx_sfx_*).
 * Requests post from any task; the executor drains via
 * rb_fx_sfx_take() so request->apply lands on the same task as the
 * seq tick.
 */
#ifndef ROBOTOS_RB_FX_SFX_H
#define ROBOTOS_RB_FX_SFX_H

#include <stdint.h>
#include "seq.h"

#ifdef __cplusplus
extern "C" {
#endif

int  rb_fx_sfx_request(uint8_t preset, uint8_t vol, uint8_t loops,
                       uint8_t dur_s);
int  rb_fx_sfx_take(int *preset, uint8_t *vol, uint8_t *loops,
                    uint8_t *dur_s);

#define RB_FX_SFX_USER_STEPS_MAX 64
int  rb_fx_sfx_prog_load(const void *pb_decoded);
const rb_sfx_seq_step_t *rb_fx_sfx_user(int *n);

#ifdef __cplusplus
}
#endif
#endif
