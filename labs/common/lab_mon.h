/*
 * lab_mon.h - Task Monitor face (title + up to 3 task rows).
 *
 *   mon_init();
 *   mon_start();
 *   mon_set_title("Task Mon p1");
 *   mon_set_row(0, "ui   R 5 312");
 *   mon_set_hint("Q 2/8 BOOT page");
 */
#ifndef ROBOTOS_LAB_MON_H
#define ROBOTOS_LAB_MON_H

#ifdef __cplusplus
extern "C" {
#endif

#define MON_ROWS 3u

void mon_init(void);
void mon_start(void);
void mon_set_title(const char *fmt, ...);
void mon_set_row(unsigned index, const char *fmt, ...);
void mon_set_hint(const char *fmt, ...);
void mon_clear_rows(void);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_LAB_MON_H */
