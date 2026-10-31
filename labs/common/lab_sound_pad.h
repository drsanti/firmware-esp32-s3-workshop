/*
 * lab_sound_pad.h - scaffold for a touch sound pad.
 *
 * Students create buttons + callbacks; this helper owns display,
 * layout, and the touch task. Knobs are opt-in (omit if unused).
 *
 *   sound_pad_init();
 *   sound_pad_add_gain_slider(NULL, NULL);
 *   sound_pad_add_echo_slider(NULL, NULL);
 *   sound_pad_add_release_slider(NULL, NULL);
 *   sound_pad_add_button("Lo", RB_STYLE_INFO, on_lo, NULL);
 *   sound_pad_start();
 */
#ifndef ROBOTOS_LAB_SOUND_PAD_H
#define ROBOTOS_LAB_SOUND_PAD_H

#include "rb_style.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SOUND_PAD_MAX_BUTTONS
#define SOUND_PAD_MAX_BUTTONS 3
#endif

/* Called on button CLICK (finger lift on the button). */
typedef void (*sound_pad_click_fn)(void *user);

/* Called when a 0..100 knob changes (Gain / Echo / Release). */
typedef void (*sound_pad_knob_fn)(uint8_t percent, void *user);

/* Back-compat alias. */
typedef sound_pad_knob_fn sound_pad_gain_fn;

/* Open LCD + title. Call before add_button / add_*_slider. */
void sound_pad_init(void);

/*
 * Optional knobs (0..100). If on_change is NULL, helper applies
 * beep_set_gain / beep_set_echo / beep_set_release for you.
 * Must be before sound_pad_start().
 * Returns 0 on success, -1 if not inited / started / already added.
 */
int sound_pad_add_gain_slider(sound_pad_knob_fn on_change, void *user);
int sound_pad_add_echo_slider(sound_pad_knob_fn on_change, void *user);
int sound_pad_add_release_slider(sound_pad_knob_fn on_change, void *user);

/*
 * Add one sound button (up to SOUND_PAD_MAX_BUTTONS).
 * Returns 0 on success, -1 if full / not inited / already started.
 */
int sound_pad_add_button(const char *label, rb_style_t style,
                         sound_pad_click_fn on_click, void *user);

/* Reflow UI and start the touch task. Call once after all widgets. */
void sound_pad_start(void);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_LAB_SOUND_PAD_H */
