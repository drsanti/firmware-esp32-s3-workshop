#ifndef ROBOTOS_RB_PARAM_PARAMS_H
#define ROBOTOS_RB_PARAM_PARAMS_H

#define RB_PARAM_OP_GET     0
#define RB_PARAM_OP_SET     1
#define RB_PARAM_OP_LIST    2
#define RB_PARAM_OP_SET_NVS 3

/*
 * rb_param — table-driven runtime parameters with a ParamCmd (0x8a)
 * wire handler and optional NVS persistence. Values ride the wire as
 * text; each entry owns get/set callbacks and a revision counter.
 *
 *   static rb_param_t PARAMS[] = {
 *       {"mic.gain", "i64", "micgain", 1, mg_get, mg_set, 0},
 *       {"mic.enabled", "bool", NULL, 0, me_get, me_set, 0},
 *   };
 *   rb_param_init(PARAMS, LEN(PARAMS), my_event_emitter, NULL);
 *   rb_sub_init(&node, &g_param_cmd, RSP_MSG_CMD_PARAM,
 *               rb_param_on_cmd, NULL);
 *
 * The emitter callback publishes a ParamEvent uplink (get/set replies
 * and set-notifications) — the app supplies it so params stays
 * transport-agnostic.
 *
 * Preferred include: rb_param.h
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*rb_param_get_fn)(char *buf, size_t n);
typedef int (*rb_param_set_fn)(const char *value);

typedef struct {
    const char *name;   /* "mic.gain"                          */
    const char *type;   /* "i64" | "f64" | "bool" | "str"      */
    const char *nvs;    /* persist key for op 3, or NULL        */
    int persist;        /* boot-time NVS read                   */
    rb_param_get_fn get;
    rb_param_set_fn set;
    uint32_t revision;
} rb_param_t;

/* emitted after every get/set/list element — the app publishes a
 * ParamEvent uplink through it. */
typedef void (*rb_param_event_fn)(const char *name, const char *value,
                                  const char *type, int rc,
                                  uint32_t revision, void *user);

void rb_param_init(rb_param_t *table, int n, rb_param_event_fn emit,
                   void *emit_user);
/* rb_sub handler for RSP_MSG_CMD_PARAM — register with rb_sub_init. */
struct rb_sub;
int rb_param_on_cmd(struct rb_sub *sub, const uint8_t *payload,
                    uint16_t len, void *user);
/* Boot-time restore: for entries with .persist and .nvs, read
 * bsp_param_get(key) and apply .set. Call once at init. */
int rb_param_restore(void);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_PARAM_PARAMS_H */
