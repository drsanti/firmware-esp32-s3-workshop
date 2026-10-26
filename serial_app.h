/*
 * serial_app.h — example app entry points (implemented by app.c).
 * Port files call app_run() from main()/app_main().
 *
 * (Header name kept for the ports' include; the app itself is just an
 * example on top of the rb_* library in rb.h.)
 */
#ifndef ROBOTOS_SERIAL_APP_H
#define ROBOTOS_SERIAL_APP_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void app_init(void);
void app_poll(void);
void app_run(void);

/* Optional UI/dashboard module — NOT registered in the executor.
 * Its refresh does blocking SPI flushes; ports run it on their own
 * low-priority task so it can't stall control-rate modules.
 * Returns NULL when the board has no display. */
struct rb_module *app_ui_module(void);

/* Called by the port with the xTaskCreate result so the wire diag
 * line can show whether the UI task actually started. rc==pdPASS(1)
 * is good; anything else means the screen stays at its last frame
 * while the board keeps running. */
void app_ui_task_rc(int rc);

/* Audio render entry — fills `frames` interleaved L/R int16 samples
 * at 16 kHz. Ports run it from a dedicated audio task paced by
 * bsp_audio_write(). Returns 0 when the board has no audio. */
size_t app_audio_fill(int16_t *stereo, size_t frames);
int app_has_audio(void);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_SERIAL_APP_H */
