/*
 * key.h — physical-button edge/press classifier (@api rb_key_*).
 *
 * Feed raw pressed-bitmasks at poll rate; rb_key_feed() debounces each
 * bit, emits DOWN on press and UP + classified SHORT/LONG/DOUBLE on
 * release. local_sink gets every event in-process (UI/action hooks);
 * cfg.uplink publishes KeyEvent (msg 0x2c) on the wire.
 *
 * Single-context: feed from one task (executor tick). Button ordinal
 * mapping lives in the port's bsp_buttons() (bsp.h).
 *
 * Preferred include: rb_key.h
 */
#ifndef ROBOTOS_RB_KEY_H_IMPL
#define ROBOTOS_RB_KEY_H_IMPL

#include <stdint.h>

#include "registry.h"
#include "uplink.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_KEY_MAX_BUTTONS 8

/* Event ordinals — 1:1 mirror of robotos.serial.v1.KeyEv */
#define RB_KEY_EV_UP     0
#define RB_KEY_EV_DOWN   1
#define RB_KEY_EV_SHORT  2
#define RB_KEY_EV_LONG   3
#define RB_KEY_EV_DOUBLE 4

typedef void (*rb_key_sink_t)(uint8_t key, uint8_t event,
                              uint32_t hold_ms, void *user);

typedef struct {
    uint16_t debounce_ms; /* 0 -> 30                          */
    uint16_t long_ms;     /* 0 -> 3000: LONG threshold        */
    uint16_t dbl_ms;      /* 0 -> 400:  DOUBLE window         */
    /* Injected sampler — wire bsp_buttons (or a fake in tests).
     * A callback, not a direct HAL call: key.o must not reference
     * bsp_buttons or the .a grows a back-edge into consumer
     * archives (same pattern as rb_touch_cfg_t.sample). NULL -> idle. */
    uint32_t (*poll)(void);
    rb_key_sink_t local_sink; /* in-process consumer — NULL ok    */
    void         *local_user;
    rb_uplink_t     *uplink;     /* wire publish — NULL disables     */
} rb_key_cfg_t;

typedef struct {
    rb_key_cfg_t cfg;
    struct {
        uint8_t  down;      /* debounced state                    */
        uint8_t  raw;       /* raw candidate, counted to debounce */
        uint32_t raw_since; /* when raw candidate started         */
        uint32_t down_ms;   /* press timestamp                    */
        uint32_t up_ms;     /* last release — double window       */
    } k[RB_KEY_MAX_BUTTONS];
} rb_key_t;

int  rb_key_init(rb_key_t *k, const rb_key_cfg_t *cfg);
/* Feed one raw bitmask sample — call every poll tick. */
void rb_key_feed(rb_key_t *k, uint32_t mask, uint32_t now_ms);
void rb_key_deinit(rb_key_t *k);

/* Executor glue — fills `m` with the standard poll module
 * (poll() + rb_key_feed). period_ms 0 -> 20 ms default. Caller
 * registers with rb_mod_register(m). */
void rb_key_module_bind(rb_module_t *m, rb_key_t *k, uint32_t period_ms);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_KEY_H_IMPL */
