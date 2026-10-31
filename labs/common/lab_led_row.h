/*
 * lab_led_row.h - four LCD indicator LEDs + press counter.
 *
 * Helper owns display / layout / rb_led geometry. Student code only
 * calls set_active / set_presses after start.
 *
 *   led_row_init();
 *   led_row_start();                 // LED 0 lit, presses: 0
 *   led_row_set_active(2);           // only index 2 lit (0..3)
 *   led_row_set_presses(7);          // "presses: 7"
 */
#ifndef ROBOTOS_LAB_LED_ROW_H
#define ROBOTOS_LAB_LED_ROW_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LED_ROW_COUNT
#define LED_ROW_COUNT 4
#endif

/* Open LCD + title. Call before led_row_start(). */
void led_row_init(void);

/* Build layout, light LED 0, show presses: 0. Call once after init. */
void led_row_start(void);

/* Light exactly one LED (index 0..LED_ROW_COUNT-1). Others off. */
void led_row_set_active(unsigned index);

/* Update the "presses: N" label. */
void led_row_set_presses(unsigned n);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_LAB_LED_ROW_H */
