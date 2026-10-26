/*
 * drawcmd.h — host-driven display: decode DrawCmd batches, queue the
 * ops, let a single task drain them into gfx calls. Keeps the two
 * discipline rules: proto decode happens on the dispatch task, gfx
 * (SPI) only happens on the task that owns the framebuffer.
 *
 *   sub cb (dispatch task):  rb_draw_feed(&d, payload, len);
 *   ui task per tick:        rb_draw_drain(&d, apply_cb, user);
 *                            if (rb_draw_mode(&d)) skip_ui_paint;
 */
#ifndef RB_MW_DRAWCMD_H
#define RB_MW_DRAWCMD_H

#include <stdint.h>
#include <string.h>

#include "../serial_proto/serial_proto.h"
#include "registry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_DRAW_MAX_OPS 32   /* ops queued per drain window         */
#define RB_DRAW_MAX_PX  512  /* BLIT pixel bytes (options-locked)   */

typedef robotos_serial_v1_DrawOp rb_draw_op_t;

typedef struct {
    /* queue — one writer (dispatch task), one reader (owner task) */
    rb_draw_op_t  ops[RB_DRAW_MAX_OPS];
    uint8_t   op_page[RB_DRAW_MAX_OPS]; /* which remote page (pid)  */
    uint8_t   head, tail;
    uint8_t   feed_page;                /* cmd.page_id of in-flight */
    uint8_t   mode;         /* last mode request: 1 host, 2 local   */
    uint8_t   mode_seq;     /* bumps on EVERY mode write (edges)    */
    uint32_t  n_dropped;    /* ops dropped when the queue was full  */
    uint32_t  n_dec_err;    /* malformed frames                     */
} rb_draw_t;

typedef void (*rb_draw_op_cb_t)(const rb_draw_op_t *op, void *user);

int  rb_draw_init(rb_draw_t *d);
void rb_draw_deinit(rb_draw_t *d);
/* Called on the sub-dispatch task — decodes DrawCmd, applies `mode`,
 * queues ops. Safe vs a concurrent drain: single-producer/single-
 * consumer ring guarded by bsp_critical_*. */
void rb_draw_feed(rb_draw_t *d, const uint8_t *payload, uint16_t len);
/* Called on the fb-owner task — pops all queued ops; only entries
 * tagged for `front_page` run cb (offscreen ops drop, counted —
 * remote pages have no retained surface; hosts repaint on SHOWN). */
uint8_t rb_draw_drain(rb_draw_t *d, uint8_t front_page,
                      rb_draw_op_cb_t cb, void *user);
/* mode request protocol — feed() bumps mode_seq on every nonzero
 * mode field; the fb owner compares seqs and reconciles navigation
 * once per edge (so a user tapping away isn't re-forced unless the
 * host writes mode again). */
uint8_t rb_draw_mode(const rb_draw_t *d);
uint8_t rb_draw_mode_seq(const rb_draw_t *d);
uint8_t rb_draw_pending(const rb_draw_t *d);

#ifdef __cplusplus
}
#endif

#endif /* RB_MW_DRAWCMD_H */
