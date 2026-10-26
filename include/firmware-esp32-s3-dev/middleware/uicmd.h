/*
 * uicmd.h — UiCmd (0x91) decode + deferred-apply queue, same
 * discipline as drawcmd: proto decode on the dispatch task, effects
 * (rui_* calls, page-pool ops) only on the fb-owner task via drain.
 *
 *   sub cb (dispatch task):  rb_ui_feed(&q, payload, len);
 *   ui task per tick:        rb_ui_drain(&q, apply_cb, user);
 */
#ifndef RB_MW_UICMD_H
#define RB_MW_UICMD_H

#include <stdint.h>
#include <string.h>

#include "../serial_proto/serial_proto.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_UI_MAX_REQ 4   /* pending cmds — hosts rarely batch     */

typedef robotos_serial_v1_UiCmd rb_ui_req_t;

typedef struct {
    rb_ui_req_t req[RB_UI_MAX_REQ];
    uint8_t     head, tail;
    uint32_t    n_dropped;    /* cmds dropped when the queue was full */
    uint32_t    n_dec_err;    /* malformed frames                     */
} rb_ui_t;

typedef void (*rb_ui_cb_t)(const rb_ui_req_t *req, void *user);

int  rb_ui_init(rb_ui_t *q);
void rb_ui_deinit(rb_ui_t *q);
void rb_ui_feed(rb_ui_t *q, const uint8_t *payload, uint16_t len);
uint8_t rb_ui_drain(rb_ui_t *q, rb_ui_cb_t cb, void *user);

#ifdef __cplusplus
}
#endif

#endif /* RB_MW_UICMD_H */
