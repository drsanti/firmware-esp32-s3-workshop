/*
 * zpeer.h — Zenoh-Pico peer link (ADR-0017).
 *
 * Long-term replacement for the COBS serial bridge: the MCU joins
 * the robotos zenoh fabric directly as a peer and puts/subscribes
 * key expressions that mirror bridge topics 1:1 (leading '/' is
 * stripped — zenoh keys cannot start with one):
 *
 *   /robot/01/sensor/imu          -> robot/01/sensor/imu
 *   /robot/01/actuator/led/cmd    -> robot/01/actuator/led/cmd
 *
 * Payloads are the SAME proto3 bytes as the serial wire
 * (interfaces/robotos/serial/v1/serial.proto) — schema, not
 * transport, is the contract.
 *
 * Transport is chosen by the connect locator string:
 *   serial/2.3#baudrate=921600    UART1 on GPIO2(tx)/GPIO3(rx)
 *   tcp/<host>:7447               WiFi + TCP to a router/peer
 *
 * No heap in the caller path: publishers are a fixed pool.
 */
#ifndef RB_ZPEER_H
#define RB_ZPEER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ZPEER_MAX_PUBS 12
#define ZPEER_MAX_SUBS 16

/* Subscriber callback — payload is the raw message bytes (proto3
 * encoded), called from zenoh-pico's read task; keep it short. */
typedef void (*zpeer_sub_cb_t)(const uint8_t *payload, size_t len,
                               void *user);

/* Open a session. `mode` = "peer" (serial/P2P) or "client"
 * (router). `locator` = zenoh connect locator; NULL -> scout.
 * Returns 0 on session open, <0 on failure. Safe to call once. */
int zpeer_open(const char *mode, const char *locator);

/* 1 while a session is usable. */
int zpeer_ready(void);

/* Declare a publisher on keyexpr. Returns index >=0, <0 when the
 * pool is full or no session. */
int zpeer_pub_declare(const char *keyexpr);

/* Put raw bytes on a declared publisher. Returns 0 ok. */
int zpeer_put(int pub_idx, const uint8_t *payload, size_t len);

/* Wrap `payload` in a robotos.envelope.v1.Envelope (schema_id +
 * publisher uuid + per-publisher seq + monotonic timestamp) before
 * putting. Use when the keyexpr is a robotos topic —
 * `robotos/<domain>/topic/<path>` — so host subscribers get a real
 * robotos message, not raw serial-schema bytes. */
/* drains queued puts — call every app tick */
void zpeer_poll(void);
int zpeer_put_msg(int pub_idx, const char *schema_id,
                  const uint8_t *payload, size_t len);

/* Declare a subscriber on keyexpr. Returns index >=0, <0 on fail. */
int zpeer_sub_declare(const char *keyexpr, zpeer_sub_cb_t cb,
                      void *user);

/* Declare a liveliness token on keyexpr — session-backed presence:
 * watchers see a PUT while the session lives and a DELETE when it
 * drops (no stale heartbeat). One token per session; a second call
 * replaces the first. Returns 0 ok. */
int zpeer_presence_declare(const char *keyexpr);

/* Shut the session down (board ports may stub). */
void zpeer_close(void);

/* Diagnostics: runtime task liveness + put counters. */
int  zpeer_read_task_running(void); /* 1 if zenoh-pico rx task alive */
int  zpeer_puts(void);              /* successful puts so far        */
int  zpeer_put_fails(void);         /* failed puts so far            */
int  zpeer_keepalives(void);        /* manual keepalives sent        */

#ifdef __cplusplus
}
#endif

#endif /* RB_ZPEER_H */
