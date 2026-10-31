/*
 * Lab 03 / Step 03 — Busy wait (**broken**)
 *
 * WHAT THIS CODE DOES
 *   Success: you can name why spinning is worse than vTaskDelay.
 *
 * HOW IT WORKS
 *   Architecture
 *   State machine
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Spinning until a tick deadline uses 100% CPU and never yields.
 *   - No chance to refresh a “still waiting…” hint.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: you can name why spinning is worse than vTaskDelay.
 *
 * WHAT YOU LEARN HERE
 *   - Same game flow as step 02.
 *   - WAIT uses a busy-spin until the deadline.
 *   - You should notice the UI is stuck / device feels “dead” during
 *   WAIT.
 *   Success: you can name why spinning is worse than vTaskDelay.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex03_step03.h"

#include "bsp.h"
#include "lab_react.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define BOOT_BTN_MASK 2u
#define POLL_MS 20u
#define WAIT_MIN_MS 1000u
#define WAIT_SPAN_MS 2000u
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

void ex03_step03_start(void)
{
    uint32_t boot_was = 0;
    TickType_t go_tick;
    TickType_t deadline;
    uint32_t wait_ms;
    uint32_t ms;
    unsigned spin = 0;
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
        react_set_detail("BUSY wait %u", (unsigned)wait_ms);
        react_set_hint("banner freezes");

        deadline = xTaskGetTickCount() + pdMS_TO_TICKS(wait_ms);
        /*
         * INTENTIONAL BUG: spin until deadline — no yield.
         * UI cannot refresh; other tasks starve. Do not ship this.
         */
        while (xTaskGetTickCount() < deadline) {
            spin++;
        }
        (void)spin;

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
        (void)boot_pressed(&boot_was);
    }
}
