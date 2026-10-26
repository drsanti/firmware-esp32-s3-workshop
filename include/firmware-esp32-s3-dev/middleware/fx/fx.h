#ifndef ROBOTOS_RB_FX_H_IMPL
#define ROBOTOS_RB_FX_H_IMPL

/*
 * rb_fx — semantic effects API for WS2812 LED (+ SFX queue in
 * sound/fx_sfx.h). Wire cmd handlers and LCD UI land on the same
 * functions so host-driven and touch-driven behavior match.
 *
 * Preferred include: rb_fx.h
 */

#include <stdint.h>

#include "../registry.h"
#include "../sound/seq.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*rb_fx_led_out_t)(uint8_t r, uint8_t g, uint8_t b,
                                void *ud);

typedef struct {
    rb_fx_led_out_t out;
    void *out_ud;
    volatile uint32_t *manual_until_ms;
} rb_fx_led_ctx_t;

typedef enum {
    RB_FX_LED_OFF = 0,
    RB_FX_LED_SOLID = 1,
    RB_FX_LED_BLINK = 2,
    RB_FX_LED_FADE = 3,
    RB_FX_LED_RAINBOW = 4,
} rb_fx_led_effect_t;

typedef struct {
    uint16_t delay_ms, on_ms, off_ms, end_ms;
    uint8_t  intensity;
    uint16_t cycles;
} rb_fx_led_pattern_t;

void rb_fx_led_init(const rb_fx_led_ctx_t *ctx);
void rb_fx_led_apply(const rb_fx_led_pattern_t pat[3],
                     rb_fx_led_effect_t e);
void rb_fx_led_solid(uint8_t r, uint8_t g, uint8_t b);
void rb_fx_led_stop(void);

extern rb_module_t rb_fx_led_module;

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_FX_H_IMPL */
