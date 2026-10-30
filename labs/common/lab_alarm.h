/*
 * lab_alarm.h - Motion Alarm status face (title + big state + detail).
 *
 *   alarm_init();
 *   alarm_start();
 *   alarm_set_state(ALARM_ST_ARMED);
 *   alarm_set_detail("shake to trip");
 */
#ifndef ROBOTOS_LAB_ALARM_H
#define ROBOTOS_LAB_ALARM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ALARM_ST_IDLE = 0, /* disarmed — ignore shakes */
    ALARM_ST_ARMED,    /* watching for shake */
    ALARM_ST_ALARM,    /* tripped / sounding */
} alarm_st_t;

void alarm_init(void);
void alarm_start(void);
void alarm_set_state(alarm_st_t st);
void alarm_set_detail(const char *fmt, ...);
void alarm_set_live(int on); /* 1 = IMU live, 0 = sim (status hint) */

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_LAB_ALARM_H */
