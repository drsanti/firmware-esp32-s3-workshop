/*
 * zpeer_null.c — drop-in null zenoh peer for consumers who use
 * `uplink`/`zlink` without a zenoh-pico session (serial-only builds).
 *
 * Compile this file *instead of* ports/zpeer_espidf.c: all zpeer_*
 * calls fail-safe (-1) so the archive's zlink/uplink objects link
 * cleanly and zenoh simply never comes up. If your board does run
 * zenoh-pico, implement zpeer.h against its session API instead
 * (see ports/zpeer_espidf.c as the reference).
 */
#include "../middleware/zpeer.h"

int zpeer_open(const char *mode, const char *locator) {
    (void)mode; (void)locator; return -1;
}
int zpeer_ready(void) { return 0; }
int zpeer_pub_declare(const char *keyexpr) { (void)keyexpr; return -1; }
int zpeer_put(int pub_idx, const uint8_t *payload, size_t len) {
    (void)pub_idx; (void)payload; (void)len; return -1;
}
int zpeer_put_msg(int pub_idx, const char *schema_id,
                  const uint8_t *payload, size_t len) {
    (void)pub_idx; (void)schema_id; (void)payload; (void)len;
    return -1;
}
int zpeer_sub_declare(const char *keyexpr, zpeer_sub_cb_t cb,
                      void *user) {
    (void)keyexpr; (void)cb; (void)user; return -1;
}
int zpeer_presence_declare(const char *keyexpr) {
    (void)keyexpr; return -1;
}
void zpeer_poll(void) {}
void zpeer_close(void) {}
int zpeer_read_task_running(void) { return 0; }
int zpeer_puts(void) { return 0; }
int zpeer_put_fails(void) { return 0; }
int zpeer_keepalives(void) { return 0; }
