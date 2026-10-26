/*
 * robotos serial-bridge wire protocol — firmware side.
 *
 * Spec: docs/embedded/serial-bridge-spec.md
 *
 * Wire format:
 *   0x00 [COBS(msg_type|seq|len|payload|crc16)] 0x00
 *
 * This header is deliberately self-contained and portable C99: it
 * compiles under ESP-IDF, ModusToolbox/PSoC Creator, Arduino, Zephyr,
 * or bare metal. Firmware supplies its own UART TX/RX; this file only
 * does framing. All multi-byte fields are little-endian.
 *
 * Firmware contract (spec §9):
 *   - never emit 0x00 inside a frame (COBS guarantees this for you)
 *   - serialize UART writes: send_frame and debug output must be
 *     mutually exclusive so text cannot land inside a frame
 *   - disable RX echo on the protocol UART
 *   - bytes between delimiters that aren't valid frames are legal:
 *     raw debug text is tolerated (dropped by the host)
 */
#ifndef ROBOTOS_SERIAL_PROTOCOL_H
#define ROBOTOS_SERIAL_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- msg_type registry (spec §4) ---- */
#define RSP_MSG_LOG        0x00 /* MCU->host framed debug text (optional) */
#define RSP_MSG_HEARTBEAT  0x01 /* MCU->host link heartbeat (optional)    */
#define RSP_MSG_RESET_INFO 0x02 /* MCU->host boot/reset reason            */
/* 0x20-0x7F: MCU->host sensor/telemetry (user-defined)                 */
#define RSP_MSG_IMU        0x20 /* ImuSample                            */
#define RSP_MSG_LED_STATE  0x21 /* LedState — actuator shadow           */
#define RSP_MSG_WAVE_STATE 0x22 /* WaveState — module shadow state      */
#define RSP_MSG_AUDIO_LEVEL 0x23 /* AudioLevel @ ~10 Hz while mic on   */
#define RSP_MSG_MIC_PCM    0x24 /* MicPcm — raw upstream audio         */
#define RSP_MSG_BATTERY    0x25 /* BatteryStatus @ ~10 s               */
#define RSP_MSG_WIFI_STATUS 0x26 /* WifiStatus — on change + ~5 s      */
#define RSP_MSG_CMD_ACK   0x27 /* CmdAck — one per dispatched cmd     */
#define RSP_MSG_BOARD_INFO 0x28 /* BoardInfo — uuid+mac+ip+uptime      */
#define RSP_MSG_PARAM_EVENT 0x29 /* ParamEvent — reply + set events    */
#define RSP_MSG_TOUCH      0x2A /* TouchEvent — edge-triggered        */
#define RSP_MSG_UI_EVENT   0x2B /* UiEvent — ui nav/lifecycle         */
#define RSP_MSG_KEY_EVENT  0x2C /* KeyEvent — physical button presses */
#define RSP_MSG_IMU_BATCH  0x2D /* packed IMU run — BLE notify only   */
/* 0x80-0xEF: host->MCU commands (user-defined)                         */
#define RSP_PROTO_VERSION  1    /* bump on wire-surface additions   */

#define RSP_MSG_CMD_BASE   0x80 /* legacy name — prefer CMD_LED      */
#define RSP_MSG_CMD_LED    0x80 /* LedCmd — the 0x8* base command    */
#define RSP_MSG_CMD_WAVE   0x83 /* example: configure a wave module     */
#define RSP_MSG_CMD_SFX    0x84 /* example: trigger a sound-effect preset */
#define RSP_MSG_CMD_SFX_PARAM 0x85 /* poke one DSP stage parameter      */
#define RSP_MSG_CMD_MIC    0x86 /* MicCmd — mic capture control            */
#define RSP_MSG_CMD_WIFI   0x87 /* WifiConfig — creds + zenoh locator (NVS)*/
#define RSP_MSG_CMD_ROUTE  0x88 /* RouteCfg — uplink transport select     */
#define RSP_MSG_CMD_WIFI_CTL 0x89 /* WifiCtl — enable/disable/reconnect  */
#define RSP_MSG_CMD_PARAM  0x8a /* ParamCmd — get/set/list runtime params */
#define RSP_MSG_CMD_TIME   0x8b /* TimeSync — host stamps unix clock    */
#define RSP_MSG_CMD_REBOOT 0x8c /* RebootCmd — soft reset, ack first    */
#define RSP_MSG_CMD_SFX_PROG 0x8d /* SfxProg — upload user seq table   */
#define RSP_MSG_CMD_LED_FX  0x8e /* LedFx — semantic effect (blink/duty)*/
#define RSP_MSG_CMD_DRAW   0x90 /* DrawCmd — host-driven display ops   */
#define RSP_MSG_CMD_UI    0x91 /* UiCmd — remote page lifecycle        */
/* 0xF0-0xFF: reserved                                                  */

/* ---- limits (spec §3.2) ---- */
#define RSP_MAX_PAYLOAD    1024u
/* frame body = 1 type + 2 seq + 2 len + payload + 2 crc              */
#define RSP_BODY_OVERHEAD  7u
/* worst-case COBS expansion of the body is body + body/254 + 1,
 * plus the two 0x00 delimiters: 1031 + 4 + 1 + 2 = 1038. Anything
 * smaller silently rejects the largest legal frames.               */
#define RSP_WIRE_MAX       (RSP_MAX_PAYLOAD + RSP_BODY_OVERHEAD +     \
                            ((RSP_MAX_PAYLOAD + RSP_BODY_OVERHEAD) / 254u) + 3u)

/* CRC-16/XMODEM: poly 0x1021, init 0x0000, no reflection, xorout 0 */
static inline uint16_t rsp_crc16(const uint8_t *data, size_t len) {
    uint16_t crc = 0x0000;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int b = 0; b < 8; b++) {
            crc = (crc & 0x8000u) ? (uint16_t)((crc << 1) ^ 0x1021u)
                                  : (uint16_t)(crc << 1);
        }
    }
    return crc;
}

/*
 * COBS encode. `dst` must hold at least src_len + src_len/254 + 1.
 * Returns the encoded length (never contains 0x00).
 */
static inline size_t rsp_cobs_encode(const uint8_t *src, size_t src_len,
                                     uint8_t *dst) {
    const uint8_t *read = src;
    uint8_t *write = dst;
    const uint8_t *end = src + src_len;
    uint8_t *code_ptr = write++;
    uint8_t code = 1;
    while (read < end) {
        if (*read == 0) {
            *code_ptr = code;
            code_ptr = write++;
            code = 1;
        } else {
            *write++ = *read;
            if (++code == 0xFF) {
                *code_ptr = code;
                code_ptr = write++;
                code = 1;
            }
        }
        read++;
    }
    *code_ptr = code;
    return (size_t)(write - dst);
}

/*
 * COBS decode. `dst`/`dst_len` bound the output — decoded length can
 * equal src_len (every input byte produces at most one output byte),
 * so callers must pass a real capacity. Returns decoded length, or 0
 * on malformed input or output overflow.
 */
static inline size_t rsp_cobs_decode(const uint8_t *src, size_t src_len,
                                     uint8_t *dst, size_t dst_len) {
    const uint8_t *read = src;
    const uint8_t *end = src + src_len;
    uint8_t *write = dst;
    uint8_t *const dst_end = dst + dst_len;
    while (read < end) {
        uint8_t code = *read;
        if (code == 0 || read + code > end + 1) {
            return 0;
        }
        read++;
        if (write + (code - 1) > dst_end) {
            return 0;
        }
        for (uint8_t i = 1; i < code; i++) {
            if (read >= end) {
                return 0; /* truncated final group */
            }
            *write++ = *read++;
        }
        if (code != 0xFF && read < end) {
            if (write >= dst_end) {
                return 0;
            }
            *write++ = 0;
        }
    }
    return (size_t)(write - dst);
}

/*
 * Encode one wire frame including delimiters into `out`.
 * `out` must hold at least RSP_WIRE_MAX bytes.
 * `seq` is the caller-maintained per-sender counter.
 * Returns frame length, or 0 if payload_len exceeds RSP_MAX_PAYLOAD.
 */
static inline size_t rsp_frame_encode(uint8_t msg_type, uint16_t seq,
                                      const uint8_t *payload,
                                      uint16_t payload_len,
                                      uint8_t *out) {
    if (payload_len > RSP_MAX_PAYLOAD) {
        return 0;
    }
    uint8_t body[5 + RSP_MAX_PAYLOAD + 2];
    body[0] = msg_type;
    body[1] = (uint8_t)(seq & 0xFF);
    body[2] = (uint8_t)(seq >> 8);
    body[3] = (uint8_t)(payload_len & 0xFF);
    body[4] = (uint8_t)(payload_len >> 8);
    for (uint16_t i = 0; i < payload_len; i++) {
        body[5 + i] = payload[i];
    }
    uint16_t crc = rsp_crc16(body, (size_t)(5 + payload_len));
    body[5 + payload_len] = (uint8_t)(crc & 0xFF);
    body[6 + payload_len] = (uint8_t)(crc >> 8);

    out[0] = 0x00;
    size_t n = rsp_cobs_encode(body, (size_t)(7 + payload_len), out + 1);
    out[1 + n] = 0x00;
    return n + 2;
}

/*
 * Streaming frame decoder. Feed bytes via rsp_frame_feed(); returns
 * the decoded frame length (>0) once per complete, CRC-valid frame,
 * filling `msg_type`, `seq`, and `payload` (capacity RSP_MAX_PAYLOAD).
 * Anything not inside delimiters — or failing CRC — is discarded:
 * that is how raw debug text coexists on the same wire (spec §3.1).
 */
typedef struct {
    uint8_t buf[RSP_WIRE_MAX];
    size_t len;
    int in_frame;
} rsp_decoder_t;

static inline void rsp_decoder_init(rsp_decoder_t *d) {
    d->len = 0;
    d->in_frame = 0;
}

static inline size_t rsp_frame_feed(rsp_decoder_t *d, uint8_t byte,
                                    uint8_t *msg_type, uint16_t *seq,
                                    uint8_t *payload,
                                    uint16_t *payload_len) {
    if (byte == 0x00) {
        /* A delimiter both closes the current frame AND opens the
         * next one — adjacent "00 00" between frames must not flip
         * the decoder out of phase. Decode whatever was collected,
         * then always (re)enter framing for the following frame. */
        uint8_t body[RSP_WIRE_MAX];
        size_t had = d->in_frame ? d->len : 0;
        d->in_frame = 1;
        d->len = 0;
        if (had == 0) {
            return 0;
        }
        size_t n = rsp_cobs_decode(d->buf, had, body, sizeof(body));
        if (n < 7) {
            return 0;
        }
        uint16_t plen = (uint16_t)(body[3] | ((uint16_t)body[4] << 8));
        if (plen > RSP_MAX_PAYLOAD || n != (size_t)(7 + plen)) {
            return 0;
        }
        uint16_t want = (uint16_t)(body[5 + plen] | ((uint16_t)body[6 + plen] << 8));
        if (rsp_crc16(body, 5 + plen) != want) {
            return 0;
        }
        *msg_type = body[0];
        *seq = (uint16_t)(body[1] | ((uint16_t)body[2] << 8));
        for (uint16_t i = 0; i < plen; i++) {
            payload[i] = body[5 + i];
        }
        *payload_len = plen;
        return n;
    }
    if (!d->in_frame) {
        return 0; /* junk outside a frame: debug text, noise */
    }
    if (d->len >= sizeof(d->buf)) {
        /* overflow: abandon this frame, wait for next delimiter */
        d->in_frame = 0;
        d->len = 0;
        return 0;
    }
    d->buf[d->len++] = byte;
    return 0;
}

/* ---- payload schemas ------------------------------------------------
 * Payload encoding is canonical proto3 — schemas live in
 *   interfaces/robotos/serial/v1/serial.proto
 * and the generated bindings are firmware/serial_proto/serial.pb.h
 * (nanopb) plus prost types robotos.serial.v1 on the host. The C
 * packed-struct table that used to live here was superseded —
 * msg_type alone selects the message type (spec §5.1).            */

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_SERIAL_PROTOCOL_H */
