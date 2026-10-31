/*
 * Lab 00 / Step 05 — BOOT → LED chase
 *
 * WHAT THIS CODE DOES
 *   Success: pressing BOOT advances the lit LED (0→1→2→3→0), bumps
 *   presses: N, and plays a soft chime.
 *
 * HOW IT WORKS
 *   ex00_step05_start() runs after bsp_init() when selected in the lab
 *   switcher.
 *   Follow the code below; architecture detail is in README.md.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   The two workshop boards do not share the same buttons — need one
 *   control that works on both:
 *   bsp_buttons() returns a bitmask of currently pressed buttons. Bit 1
 *   is BOOT on both SKUs, so one main.c works with either -DBOARD=.
 *   Ignore PWR on ws28p (it is tied to power-hold / battery).
 *   Architecture
 *   Physical BOOT is polled in UI; LED chase and press count stay in the
 *   same task.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: pressing BOOT advances the lit LED (0→1→2→3→0), bumps
 *   presses: N, and plays a soft chime.
 *
 * WHAT YOU LEARN HERE
 *   Same split as steps 03–04
 *   BOOT press is an event → xTaskNotifyGive. Chime stays in audio.
 *   Why poll with vTaskDelay(20) instead of a GPIO ISR?
 *   For a human finger, 20 ms is plenty. Lab 00 stays in task context.
 *   ISRs appear when a later device truly needs them.
 *   Why 20 ms also helps debounce
 *   Mechanical buttons bounce. Sampling at ~50 Hz plus rising-edge logic
 *   (now && !was) ignores a held BOOT and most bounce bursts without a
 *   separate debounce state machine. Faster polling without edge detect
 *   would multi-c...
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex00_step05.h"

#include "bsp.h"
#include "lab_beep.h"
#include "lab_led_row.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

/* bsp_buttons: bit0=PWR (ws28p only), bit1=BOOT (both boards). */
#define BOOT_BTN_MASK 2u
#define LED_COUNT 4u
#define POLL_MS 20u

static TaskHandle_t g_audio;

static void notify_beep(void)
{
    if (g_audio != NULL) {
        (void)xTaskNotifyGive(g_audio);
    }
}

/* Audio side: wait for notify, play chime (do not touch LCD / buttons). */
static void audio_task(void *arg)
{
    (void)arg;
    for (;;) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        (void)beep_play(BEEP_CHIME, NULL);
    }
}

static void ui_task(void *arg)
{
    uint32_t was = 0;
    unsigned active = 0;
    unsigned presses = 0;

    (void)arg;

    led_row_init();
    led_row_start(); /* LED 0 lit, presses: 0 */

    for (;;) {
        uint32_t now = bsp_buttons() & BOOT_BTN_MASK;

        /* Rising edge on BOOT: advance chase, bump count, beep. */
        if (now != 0u && was == 0u) {
            presses++;
            active = (active + 1u) % LED_COUNT;
            led_row_set_active(active);
            led_row_set_presses(presses);
            notify_beep();
        }
        was = now;

        vTaskDelay(pdMS_TO_TICKS(POLL_MS));
    }
}

void ex00_step05_start(void)
{
/* Create audio first so g_audio is valid before any press. */
    (void)xTaskCreate(audio_task, "audio", 4096, NULL, 5, &g_audio);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
