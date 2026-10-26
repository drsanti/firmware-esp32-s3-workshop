#ifndef RB_MW_BLE_NUS_H
#define RB_MW_BLE_NUS_H

/*
 * ble_nus — BLE as a third wire transport (NUS-style GATT pipe).
 *
 * Carries the SAME COBS+CRC16 frames as the UART — received bytes
 * feed the shared rsp_frame_feed decoder and land on rb_on_frame
 * exactly like serial; outbound frames are rsp_frame_encode'd and
 * handed to the port-supplied notify fn. Handlers, CmdAcks, and
 * dedup are all transport-agnostic already, so BLE is free.
 *
 * Streaming policy:
 *   - IMU task only writes the ring (rb_ble_imu_push). A busy radio
 *     does not drop that sample.
 *   - rb_ble_poll drains the ring into 0x2D notifies. A failed
 *     notify leaves the samples in the ring.
 *   - CTRL (acks, params, UI) stays a small FIFO, also drained
 *     here. Priority CTRL goes first.
 *
 * The module is portable C — the BLE/GATT plumbing lives in the
 * port layer (ports/esp32s3_ble.c on NimBLE); it registers:
 *   rb_ble_set_tx(notify_fn)   outbound frame → GATT notify
 *   rb_ble_feed(bytes, len)    inbound GATT write → decoder
 *   rb_ble_notify_done()       port: NOTIFY_TX / credit return
 * A port gates heavy streams per-uplink via RouteCfg bit 0x04.
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct rb_node;

typedef int (*rb_ble_tx_t)(const uint8_t *frame, size_t len,
                            void *user);

void rb_ble_init(struct rb_node *node);
/* port registers its GATT notify writer */
void rb_ble_set_tx(rb_ble_tx_t fn, void *user);
/* port feeds raw GATT rx bytes (may be chunked arbitrarily) —
 * runs on the BLE host task: decodes COBS frames and QUEUES them
 * for dispatch (handlers can block — wifi connect, NVS writes). */
void rb_ble_feed(const uint8_t *data, size_t len);
/* drain CTRL + IMU ring → notify; dispatch cmd queue — each tick */
void rb_ble_poll(void);
/* encode + enqueue one frame — 0 ok, -1 no client/no tx fn */
int  rb_ble_write(uint8_t msg_type, const uint8_t *payload,
                   uint16_t len);

int  rb_ble_connected(void);   /* a GATT client is subscribed */
/* rb_log passthrough — ports use it since printf/ESP_LOGI don't
 * reach the wire console on all boards */
void rb_ble_logf(const char *fmt, ...);
uint32_t rb_ble_dropped(void); /* queue overflow / oversized frame */
uint32_t rb_ble_rx_bytes(void);
uint32_t rb_ble_tx_bytes(void);
void rb_ble_set_connected(int on); /* port calls on conn/subscribe */

/* credit: port calls when an indication is confirmed (or times out) */
void rb_ble_notify_done(void);
void rb_ble_indicate_done(void);

/* IMU stream rate on BLE — 0|10|20|25|50. Serial/zenoh unaffected. */
int  rb_ble_imu_hz(void);
int  rb_ble_set_imu_hz(int hz); /* 0 ok, -22 invalid */

/* Port reports the live ATT MTU (0 if unknown). Batches stay in
 * the ring until the MTU can carry at least one sample. */
void rb_ble_set_mtu(int mtu);

/* IMU task only. Phase-locks to the 20 ms grid and stores one
 * sample. 0 = stored or not due. -1 = ring full (oldest dropped). */
int  rb_ble_imu_push(uint64_t t_ns, float ax, float ay, float az,
                  float gx, float gy, float gz);

#ifdef __cplusplus
}
#endif

#endif /* RB_MW_BLE_NUS_H */
