/*
 * touch.h — capacitive-touch event module (@api rb_touch_*).
 * Ports feed raw bsp_touch_sample_t frames; rb_touch_feed() turns
 * them into clean DOWN/MOVE/UP events with stable point ids,
 * deadband, and a rate cap, then fans out to a local sink (e.g. the
 * UI) and an uplink (TouchEvent on the wire).
 *
 * Preferred include: rb_touch.h
 */
#ifndef ROBOTOS_RB_TOUCH_H_IMPL
#define ROBOTOS_RB_TOUCH_H_IMPL

#include <stdint.h>

#include "../bsp.h"
#include "uplink.h"
#include "registry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_TOUCH_EV_UP   0 /* synthesized — the contact vanished */
#define RB_TOUCH_EV_DOWN 1
#define RB_TOUCH_EV_MOVE 2

/* The sink gets the full raw sample (points + gesture) — controller
 * event flags, NOT wire RB_TOUCH_EV_*. Runs on the same task that
 * calls rb_touch_feed. */
typedef void (*rb_touch_sink_t)(const bsp_touch_sample_t *s,
                                void *user);

typedef struct {
    uint16_t     deadband_px; /* MOVE publish threshold; 0 -> 2     */
    uint16_t     rate_cap_hz; /* MOVE cap; 0 -> 60, 1..1000         */
    /* Injected sampler — wire bsp_touch_read (or a fake in tests).
     * A callback, not a direct HAL call: touch.o must not reference
     * bsp_* symbols or the .a grows a back-edge into consumer
     * archives (circular-archive link failures). NULL -> no-op. */
    int (*sample)(bsp_touch_sample_t *s);
    rb_touch_sink_t local_sink; /* in-process consumer (UI) — NULL ok */
    void           *local_user;
    rb_uplink_t    *uplink;     /* wire publish — NULL disables       */
} rb_touch_cfg_t;

typedef struct {
    rb_touch_cfg_t cfg;
    /* last published frame — id-stable point set */
    struct {
        uint8_t  id;            /* allocated; never reused while up  */
        uint8_t  down;
        int16_t  x, y;          /* latest sampled position          */
        int16_t  pub_x, pub_y;  /* last PUBLISHED position          */
        uint32_t move_ms;       /* per-point MOVE rate budget       */
        int16_t  down_x, down_y;/* DOWN anchor — flick detection    */
        uint32_t down_ms;       /* anchor timestamp                 */
    } pts[RB_TOUCH_MAX_POINTS];
    uint8_t     gesture_seen;   /* last reported gesture            */
    uint8_t     next_id;        /* id allocator (wraps; re-rolls    */
} rb_touch_t;                   /*   on collision with a live id)   */

int  rb_touch_init(rb_touch_t *t, const rb_touch_cfg_t *cfg);
void rb_touch_feed(rb_touch_t *t, const bsp_touch_sample_t *s,
                   uint32_t now_ms);
void rb_touch_deinit(rb_touch_t *t);

/* Executor glue — fills `m` with the standard poll module.
 * period_ms 0 → 10 ms default. Caller: rb_mod_register(m). */
void rb_touch_module_bind(rb_module_t *m, rb_touch_t *t,
                          uint32_t period_ms);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_TOUCH_H_IMPL */
