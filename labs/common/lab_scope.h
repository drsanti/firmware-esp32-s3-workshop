/*
 * lab_scope.h - Vibration Scope face (HISTORY chart + status line).
 *
 *   scope_init();
 *   scope_start();
 *   scope_push(ax);           // one sample (m/s^2) into ring buffer
 *   scope_set_status("30 Hz");
 */
#ifndef ROBOTOS_LAB_SCOPE_H
#define ROBOTOS_LAB_SCOPE_H

#ifdef __cplusplus
extern "C" {
#endif

void scope_init(void);
void scope_start(void);
void scope_set_title(const char *title); /* NULL = default */
void scope_set_status(const char *fmt, ...);
void scope_push(float ax_mps2);
void scope_set_live(int on);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_LAB_SCOPE_H */
