#ifndef ROBOTOS_RB_UPLINK_H_IMPL
#define ROBOTOS_RB_UPLINK_H_IMPL

/*
 * rb_uplink — multi-transport publish (serial COBS ∥ zenoh ∥ BLE).
 * Each stream registers a serial pub AND (optionally) a zenoh pub id;
 * the route mask picks which transports carry it.
 *
 *   static rb_pub_t imu_pub;
 *   static rb_uplink_t u_imu;
 *   ...
 *   rb_pub_init(&node, &imu_pub, RSP_MSG_IMU);
 *   rb_uplink_bind(&u_imu, &imu_pub, "robotos.serial.v1.ImuSample");
 *   then: rb_uplink_zpub(&u_imu, zpeer_pub_declare(...));
 *   rb_uplink_pub(&u_imu, buf, len);        // → active routes
 *
 * Preferred include: rb_uplink.h
 */

#include <stdint.h>

#include "../rb.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_UPLINK_ROUTE_SERIAL 0x01
#define RB_UPLINK_ROUTE_ZENOH  0x02
#define RB_UPLINK_ROUTE_BLE    0x04

typedef struct {
    rb_pub_t *serial;  /* NULL → serial side skipped */
    int zpub;          /* zpeer pub id, -1 when session down */
    uint8_t ble;       /* opt-in: heavy streams stay off BLE      */
    const char *schema;
} rb_uplink_t;

void rb_uplink_init(uint8_t route_mask);
void rb_uplink_bind(rb_uplink_t *u, rb_pub_t *serial, const char *schema);
void rb_uplink_zpub(rb_uplink_t *u, int zpub);
void rb_uplink_ble(rb_uplink_t *u, int on); /* per-stream BLE opt-in */
uint8_t rb_uplink_route(void);
int rb_uplink_set_route(uint8_t mask); /* -2 on invalid mask */

void rb_uplink_pub(rb_uplink_t *u, const uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_UPLINK_H_IMPL */
