/*
 * rtc.h — wall-clock glue between the board RTC and stdcmd's
 * mono<->unix offset. Board-independent: the bsp_* seam supplies
 * civil UTC datetimes; this module owns epoch conversion and the
 * seed/persist lifecycle.
 *
 *   rb_rtc_boot_seed();              // once, after bsp_init
 *   // TimeSync lands later -> rb_rtc_persist() inside the handler
 */
#ifndef ROBOTOS_RTC_H
#define ROBOTOS_RTC_H

#include "../bsp.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* epoch <-> civil UTC; both are pure conversion, no I/O */
int64_t rb_rtc_dt_to_unix_ns(const bsp_datetime_t *t);
void    rb_rtc_unix_to_dt(int64_t unix_ns, bsp_datetime_t *t);

/* Read the RTC once and seed stdcmd's wall clock — no-op on boards
 * without an RTC (bsp_rtc_read returns -1) or after a TimeSync
 * already landed. */
void rb_rtc_boot_seed(void);

/* Persist a wall timestamp into the RTC — called from the TimeSync
 * handler; no-op where bsp_rtc_set is unimplemented. */
void rb_rtc_persist(int64_t unix_ns);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RTC_H */
