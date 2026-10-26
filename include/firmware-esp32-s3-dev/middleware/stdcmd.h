#ifndef ROBOTOS_RB_CMD_STDCMD_H
#define ROBOTOS_RB_CMD_STDCMD_H

/*
 * rb_cmd — stock handlers for the board-management cmd set so a
 * product doesn't rewrite them: reboot (deferred, ack flushes first),
 * clock sync (mono↔unix offset), uplink route mask.
 *
 *   rb_sub_init(&node, &g_reboot_sub, RSP_MSG_CMD_REBOOT,
 *               rb_cmd_reboot, NULL);
 *   rb_sub_init(&node, &g_time_sub, RSP_MSG_CMD_TIME,
 *               rb_cmd_timesync, NULL);
 *   rb_sub_init(&node, &g_route_sub, RSP_MSG_CMD_ROUTE,
 *               rb_cmd_route, NULL);
 *   executor/main loop:
 *   rb_cmd_poll();           // fires a pending reboot at deadline
 *   int64_t wall = rb_cmd_unix_ns();  // mono+offset when synced
 *
 * A pre-reboot hook (e.g. zpeer_close so presence drops instantly)
 * installs via rb_cmd_reboot_hook().
 *
 * Preferred include: rb_cmd.h
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct rb_sub;
struct rb_node;

/* 0x8c RebootCmd — deferred soft reset; delay_ms 0xFFFFFFFF cancels.
 * rc: 0 ok, -1 decode, -2 unsupported mode. */
int rb_cmd_reboot(struct rb_sub *sub, const uint8_t *payload,
                  uint16_t len, void *user);
/* fire the pending reboot at its deadline — call every app tick */
void rb_cmd_poll(void);
typedef void (*rb_cmd_hook_t)(void *user);
void rb_cmd_reboot_hook(rb_cmd_hook_t fn, void *user);

/* 0x8b TimeSync — host stamps unix_ns; board keeps mono↔unix
 * offset (runtime only, re-sync each boot). */
int rb_cmd_timesync(struct rb_sub *sub, const uint8_t *payload,
                    uint16_t len, void *user);
int64_t rb_cmd_unix_ns(void);   /* wall time, or -1 when unsynced */
int     rb_cmd_time_synced(void);
void    rb_cmd_seed_time(int64_t unix_ns); /* RTC boot seed — host
                                              * sync overrides */

/* 0x88 RouteCfg — uplink transport mask (serial∥zenoh). */
int rb_cmd_route(struct rb_sub *sub, const uint8_t *payload,
                 uint16_t len, void *user);

/* shared log helper — rb_cmd handlers route rb_log through the node
 * set here (app calls once at init). */
void rb_cmd_bind_log(struct rb_node *node);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_CMD_STDCMD_H */
