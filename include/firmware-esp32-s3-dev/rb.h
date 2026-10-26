/*
 * rb.h — tiny robotos-style firmware API.
 *
 * The idea: a firmware developer uses node/publisher/subscriber/
 * log primitives — the same mental model as ROS or the robotos
 * desktop SDK — while the library owns framing, sequencing,
 * heartbeat, and the transport.
 *
 *   rb_node_t node;
 *   rb_serial_transport_t link;      // UART backend
 *   rb_pub_t imu_pub;
 *   rb_sub_t cmd_sub;
 *
 *   bsp_init();                     // your MCU init (bsp.h)
 *   rb_transport_serial_init(&link);
 *   rb_node_init(&node, &(rb_node_cfg_t){
 *       .transport = &link.base,
 *       .name = "motor_01",
 *       .heartbeat_ms = 1000,
 *   });
 *   rb_pub_init(&node, &imu_pub, RSP_MSG_IMU);
 *   rb_sub_init(&node, &cmd_sub, 0x80, on_cmd, NULL);
 *
 *   for (;;) {
 *       rb_spin_once(&node);          // pump RX, heartbeat, dispatch
 *       rb_publish(&imu_pub, &sample, sizeof(sample));
 *   }
 *
 * Memory: everything is caller-allocated or fixed-size pools — no
 * malloc. Tune the pools via the RB_*_MAX macros below before
 * including this header (or edit the defaults).
 *
 * Transports: a vtable of three functions — the serial backend uses
 * the spec's COBS/CRC framing (docs/embedded/serial-bridge-spec.md);
 * a Zenoh-Pico backend (v0.4) or a test null backend can plug in the
 * same way.
 */
#ifndef ROBOTOS_RB_H
#define ROBOTOS_RB_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Endpoint pool sizes — they size arrays inside `struct rb_node`,
 * so values MUST match what the archive was built with. Retuning
 * requires rebuilding librobotos_fw.a from source; do not redefine
 * these against a binary distribution. */
#ifndef RB_MAX_PUBS
#define RB_MAX_PUBS 16
#endif
#ifndef RB_MAX_SUBS
#define RB_MAX_SUBS 16
#endif

/* Return convention: all failures are negative (-errno-style), so a
 * caller's `if (rc < 0)` catches every error across the library. */
typedef enum {
    RB_OK = 0,
    RB_INVALID_ARG    = -22,  /* EINVAL  */
    RB_NO_ROOM        = -12,  /* ENOMEM — pub/sub pool or TXQ full   */
    RB_NOT_CONNECTED  = -113, /* EHOSTUNREACH — transport not ready */
    RB_BAD_FRAME      = -74,  /* EBADMSG — payload over wire cap    */
} rb_status_t;

typedef struct rb_node rb_node_t;
typedef struct rb_pub rb_pub_t;
typedef struct rb_sub rb_sub_t;

/* ---------------- transport vtable ----------------
 *
 * The vtable sits at the *frame* level, not the byte level: the
 * serial backend encodes spec-framing (COBS+CRC) around the payload,
 * while a Zenoh-Pico backend (v0.4) would put() bytes directly.
 * poll_rx drains whatever RX the backend has and must call
 * rb_on_frame(node, msg_type, payload, len) per complete frame.
 */
typedef struct rb_transport rb_transport_t;
struct rb_transport {
    /* Send one payload under msg_type with the given wire seq.
     * RB_OK = accepted by the transport. */
    rb_status_t (*write_frame)(rb_transport_t *t, uint8_t msg_type,
                               uint16_t seq, const uint8_t *payload,
                               uint16_t len);
    /* Pump the backend's RX path. Never blocks on byte availability. */
    void (*poll_rx)(rb_transport_t *t, rb_node_t *node);
    /* Monotonic nanoseconds. */
    uint64_t (*mono_ns)(rb_transport_t *t);
};

/* Called by transport backends on each complete inbound frame.
 * req_seq = the requester's wire sequence (COBS frame seq / zenoh
 * envelope seq) echoed back in CmdAck so the host can correlate —
 * 0 when the transport carries no request id. Returns the handler's
 * rc, or -1 when no sub claimed the type. */
int rb_on_frame(rb_node_t *n, uint8_t msg_type, uint64_t req_seq,
                const uint8_t *payload, uint16_t len);

/* ---------------- public objects ---------------- */

/* Returns 0 on success, negative errno-style on failure — the rc is
 * echoed back to the requester via the node's cmd_ack hook. */
typedef int (*rb_sub_cb)(rb_sub_t *sub, const uint8_t *payload,
                         uint16_t len, void *ctx);

struct rb_pub {
    rb_node_t *node;
    uint8_t msg_type;
};

struct rb_sub {
    uint8_t msg_type;
    rb_sub_cb cb;
    void *ctx;
};

typedef struct {
    rb_transport_t *transport;       /* required */
    const char *name;                /* informational only; may be NULL */
    uint32_t heartbeat_ms;           /* 0 = heartbeat disabled */
} rb_node_cfg_t;

struct rb_node {
    rb_transport_t *transport;
    const char *name;
    uint32_t heartbeat_ms;
    uint64_t next_heartbeat_ns;
    uint16_t seq;                    /* per-sender wire sequence */

    rb_pub_t *pubs[RB_MAX_PUBS];
    rb_sub_t *subs[RB_MAX_SUBS];

    /* optional: fired after every inbound frame dispatch — app uses it
     * to emit CmdAck. rc is the sub's return, -1 = unmatched type. */
    void (*cmd_ack)(rb_node_t *n, uint8_t msg_type, uint64_t req_seq,
                    int rc, void *user);
    void *cmd_ack_user;

    /* cmd dedup — the same cmd can arrive twice when a host publishes
     * to a topic the board also hears directly (serial bridge forward
     * + zenoh sub). Ring of recent (payload hash, timestamp) — a
     * second identical cmd inside the window is a transport dup. */
    struct {
        uint32_t hash;
        uint64_t at;
    } cmd_dedup[8];
    int cmd_dedup_i;

    /* counters — same "counted, never silent" posture as the host */
    uint32_t n_rx_frames;
    uint32_t n_rx_unknown;   /* valid frames with no subscriber */
    uint32_t n_tx_frames;
    uint32_t n_tx_failed;
};

/* ---------------- API ---------------- */

rb_status_t rb_node_init(rb_node_t *n, const rb_node_cfg_t *cfg);

rb_status_t rb_pub_init(rb_node_t *n, rb_pub_t *pub, uint8_t msg_type);

/* guard the RB_MAX_* build-time contract — a consumer compiled with
 * different pool sizes would corrupt the node struct layout; call
 * with the values the app was built against */
static inline int rb_abi_check(uint32_t pubs, uint32_t subs) {
    return (pubs == RB_MAX_PUBS && subs == RB_MAX_SUBS) ? 0 : -22;
}
rb_status_t rb_sub_init(rb_node_t *n, rb_sub_t *sub, uint8_t msg_type,
                        rb_sub_cb cb, void *ctx);

/* Frame and send payload under pub->msg_type. RB_OK = accepted by the
 * transport — never end-to-end delivery (same honesty rule as the
 * desktop SDK). */
rb_status_t rb_publish(rb_pub_t *pub, const void *payload,
                       uint16_t len);

/* Node-level send for callers that already know their msg_type —
 * used internally by rb_log/heartbeat; fine for ad-hoc use too.
 * Advances the node's wire sequence. */
rb_status_t rb_send(rb_node_t *n, uint8_t msg_type, const uint8_t *payload,
                    uint16_t len);

/* Framed debug text (MSG_LOG) — lands on /robot/{id}/device/log via
 * the bridge, or is harmlessly dropped if logging is unwired. */
void rb_log(rb_node_t *n, const char *fmt, ...);

/* Pump the transport: drain RX, dispatch subscribers, emit heartbeat
 * when due. Call in a loop (bare metal) or from a comms task. Never
 * blocks on RX; write() may block per the port's UART driver. */
void rb_spin_once(rb_node_t *n);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_H */
