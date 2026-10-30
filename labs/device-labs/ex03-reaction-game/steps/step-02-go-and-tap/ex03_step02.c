/*
 * Lab 03 / Step 02 — GO and tap
 *
 * WHAT THIS CODE DOES
 *   Success: plausible times (tens–hundreds of ms) after GO.
 *
 * HOW IT WORKS
 *   Architecture
 *   Sequence
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Need BOOT edge detection (same as Lab 00/02).
 *   - Need to capture time at GO and at press.
 *   - vTaskDelay for WAIT blocks this task — works, but UI cannot do
 *   other
 *   work during the wait (timer comes in step 04).
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: plausible times (tens–hundreds of ms) after GO.
 *
 * WHAT YOU LEARN HERE
 *   vTaskDelay for WAIT is a blocking wait: this task does nothing else
 *   until it ends. Step 03 shows a worse busy-spin; step 04 uses a timer
 *   so WAIT can still update the UI.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex03_step02.h"

#include "bsp.h"
#include "lab_react.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define BOOT_BTN_MASK 2u
#define POLL_MS 20u
#define WAIT_MIN_MS 1000u
#define WAIT_SPAN_MS 2000u /* random extra 0..1999 → total 1–3 s */
#define RESULT_HOLD_MS 2000u

static uint32_t g_rng = 1u;

static uint32_t rnd_u32(void)
{
    g_rng = g_rng * 1664525u + 1013904223u;
    return g_rng;
}

static int boot_pressed(uint32_t *was)
{
    uint32_t now = bsp_buttons() & BOOT_BTN_MASK;
    int edge = (now != 0u && *was == 0u) ? 1 : 0;
    *was = now;
    return edge;
}

void ex03_step02_start(void)
{
    uint32_t boot_was = 0;
    TickType_t go_tick;
    uint32_t wait_ms;
    uint32_t ms;
    react_init();
    react_start();

    for (;;) {
        react_set_state(REACT_ST_READY);
        react_set_detail("BOOT to start");
        react_set_hint(" ");

        while (!boot_pressed(&boot_was)) {
            vTaskDelay(pdMS_TO_TICKS(POLL_MS));
        }

        wait_ms = WAIT_MIN_MS + (rnd_u32() % WAIT_SPAN_MS);
        react_set_state(REACT_ST_WAIT);
        react_set_detail("wait %u ms", (unsigned)wait_ms);
        react_set_hint("do not press");

        /* Blocks this task for the whole wait — UI cannot do other work. */
        vTaskDelay(pdMS_TO_TICKS(wait_ms));

        go_tick = xTaskGetTickCount();
        react_set_state(REACT_ST_GO);
        react_set_detail("BOOT now!");
        react_set_hint(" ");

        while (!boot_pressed(&boot_was)) {
            vTaskDelay(pdMS_TO_TICKS(POLL_MS));
        }

        ms = (uint32_t)((xTaskGetTickCount() - go_tick) * portTICK_PERIOD_MS);
        react_set_state(REACT_ST_RESULT);
        react_set_detail("%u ms", (unsigned)ms);
        react_set_hint("BOOT = next");

        vTaskDelay(pdMS_TO_TICKS(RESULT_HOLD_MS));
        (void)boot_pressed(&boot_was); /* clear sticky edge */
    }
}
