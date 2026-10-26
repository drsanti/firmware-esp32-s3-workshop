/*
 * rb_serial.h — UART transport backend for rb_*, implementing the
 * spec's wire format via protocol.h and byte I/O via bsp.h.
 *
 * Usage:
 *   static rb_serial_transport_t link;
 *   rb_transport_serial_init(&link);          // binds bsp.h
 *   cfg.transport = &link.base;
 */
#ifndef ROBOTOS_RB_SERIAL_H
#define ROBOTOS_RB_SERIAL_H

#include "protocol.h"
#include "rb.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    rb_transport_t base;
    rsp_decoder_t rx;         /* streaming frame decoder (spec §3.1) */
    /* backend counters — raw decode outcomes, byte level */
    uint32_t n_rx_rejected;   /* COBS/len/CRC failures              */
    uint32_t n_rx_junk;       /* bytes outside frames (debug text)  */
} rb_serial_transport_t;

void rb_transport_serial_init(rb_serial_transport_t *t);
uint32_t rb_serial_txq_dropped(void); /* tx queue overflow */

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_SERIAL_H */
