#ifndef RB_MW_ZLINK_H
#define RB_ZLINK_KE_MAX 96
#define RB_MW_ZLINK_H

/*
 * zlink — the board's own zenoh-pico session bring-up, library form.
 * NVS creds → wifi connect → zpeer_open("client") → declare every
 * upstream pub (into each stream's rb_uplink_t) and every downlink cmd
 * sub → presence liveliness token.
 *
 * Cmds arriving over zenoh are Envelope-decoded and queued — the
 * pico read task can't block — and drained onto the app's executor
 * context via rb_zlink_poll(), which feeds rb_on_frame (same dispatch
 * as the serial wire, including CmdAck).
 *
 *   static const rb_zlink_pub_t ZPUBS[] = {
 *       {"robotos/0/topic/robot/01/sensor/imu", &u_imu}, ... };
 *   static const rb_zlink_sub_t ZSUBS[] = {
 *       {"robotos/0/topic/robot/01/actuator/led/cmd", RSP_MSG_CMD_BASE},
 *       ... };
 *   rb_zlink_init(&(rb_zlink_cfg_t){
 *       .node = &node, .pubs = ZPUBS, .n_pubs = N,
 *       .subs = ZSUBS, .n_subs = M,
 *       .presence_ke = "robotos/0/presence/ternion-esp32s3-xxx",
 *       .settle_ms = rtos_delay,
 *   });
 *   rb_zlink_bring_up();            // after boot / on reconnect cmd
 *   rb_zlink_poll();                // every app tick
 *   rb_zlink_down();                // on wifi disable
 */

#include <stdint.h>

#include "../rb.h"
#include "uplink.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *ke;     /* zenoh topic keyexpr          */
    rb_uplink_t *u;        /* stream the declared pub binds to */
} rb_zlink_pub_t;

typedef struct {
    const char *ke;     /* zenoh topic keyexpr          */
    uint8_t type;       /* serial msg_type dispatched to */
} rb_zlink_sub_t;

typedef struct {
    rb_node_t *node;            /* log target + rb_on_frame dispatch */
    const rb_zlink_pub_t *pubs;
    int n_pubs;
    const rb_zlink_sub_t *subs;
    int n_subs;
    const char *presence_ke;    /* liveliness token keyexpr or NULL */
    void (*settle_ms)(uint32_t ms); /* post-open declare settle     */
} rb_zlink_cfg_t;

void rb_zlink_init(const rb_zlink_cfg_t *cfg);

/* creds→wifi→zpeer→declares→presence. Guarded against reentry (a cmd
 * can arrive on serial AND zenoh simultaneously). Returns the first
 * zenoh pub idx on success (badge/status read), -1 before zpeer. */
int  rb_zlink_bring_up(void);
/* close the session and clear every bound zpub id. */
void rb_zlink_down(void);
/* drain queued zenoh cmds → rb_on_frame. Call each app tick. */
void rb_zlink_poll(void);

int      rb_zlink_up(void);                  /* session declared?        */
int      rb_zlink_wifi_configured(void);     /* creds exist in NVS       */
uint32_t rb_zlink_zcmd_rx(void);             /* zenoh cmds received      */
uint32_t rb_zlink_zcmd_dropped(void);        /* queue overflow / decode  */

#ifdef __cplusplus
}
#endif

#endif /* RB_MW_ZLINK_H */
