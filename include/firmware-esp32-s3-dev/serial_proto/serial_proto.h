/*
 * serial_proto.h — proto3 wire payloads for the serial bridge.
 *
 * .proto schema: interfaces/robotos/serial/v1/serial.proto
 * Generated:     serial.pb.{c,h} via nanopb_generator.py
 * Runtime:       vendor/nanopb (pb_encode/pb_decode — static, no heap)
 *
 * This is the single encode/decode entry point; msg_type values stay
 * in protocol.h (framing layer), payload schemas live here.
 */
#ifndef RB_SERIAL_PROTO_H
#define RB_SERIAL_PROTO_H

#include "../vendor/nanopb/pb_decode.h"
#include "../vendor/nanopb/pb_encode.h"
#include "serial.pb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Returns encoded length, 0 on failure (payload buffer too small). */
static inline size_t sp_encode(const pb_msgdesc_t *fields,
                               const void *msg, uint8_t *buf,
                               size_t cap) {
    pb_ostream_t s = pb_ostream_from_buffer(buf, cap);
    return pb_encode(&s, fields, msg) ? s.bytes_written : 0;
}

/* 1 on clean decode, 0 on malformed/truncated payload. */
static inline int sp_decode(const pb_msgdesc_t *fields, void *msg,
                            const uint8_t *buf, size_t len) {
    pb_istream_t s = pb_istream_from_buffer(buf, len);
    return pb_decode(&s, fields, msg) ? 1 : 0;
}

#ifdef __cplusplus
}
#endif

#endif /* RB_SERIAL_PROTO_H */
