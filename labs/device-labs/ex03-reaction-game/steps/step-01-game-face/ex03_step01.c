/*
 * Lab 03 / Step 01 — Game face
 *
 * WHAT THIS CODE DOES
 *   Success: you can name each state before adding input.
 *
 * HOW IT WORKS
 *   Architecture
 *   State machine
 *   Demo cycle only (no input yet):
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Need a clear game face before timing logic.
 *   - Need paced redraw + yield.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: you can name each state before adding input.
 *
 * WHAT YOU LEARN HERE
 *   Same pace + yield lesson as earlier labs. Step 02 will use delay for
 *   the random WAIT (which blocks the task — fine for a first working
 *   game, then we improve it).
 *   Not yet: timers, notify, BOOT. Those appear when later requirements
 *   force them.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex03_step01.h"

#include "bsp.h"
#include "lab_react.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define HOLD_MS 800u

void ex03_step01_start(void)
{
    const react_st_t cycle[] = {
        REACT_ST_READY,
        REACT_ST_WAIT,
        REACT_ST_GO,
        REACT_ST_RESULT,
    };
    unsigned i = 0;
    react_init();
    react_start();
    react_set_hint("demo cycle");

    for (;;) {
        react_set_state(cycle[i]);
        react_set_detail("phase %u", i);
        i = (i + 1u) % 4u;

        vTaskDelay(pdMS_TO_TICKS(HOLD_MS));
    }
}
