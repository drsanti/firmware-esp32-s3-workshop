/*
 * Lab 01 / Step 03 — Busy loop (broken on purpose)
 *
 * WHAT THIS CODE DOES
 *   This step is a failed design you are meant to feel, not ship.
 *   - Same spirit bubble + IMU/sim as step 02.
 *   - When you enter LEVEL, play a chime.
 *
 * HOW IT WORKS
 *   Architecture
 *   Sense, draw, and blocking audio share one loop — the bubble cannot
 *   refresh during beep_play.
 *   State machine
 *   LEVEL detection uses a rising edge; the chime runs inside the only UI
 *   task.
 *   Correct direction (step 04+): separate who may block (audio) from who
 *   must keep drawing (UI), and pass tilt data with a queue.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - A nice product feature (“beep when level”) seems small to add.
 *   - beep_play blocks until the note (and echo) finish — hundreds of ms.
 *   - Sense + draw + long audio share one thread → smooth motion dies.
 *
 * PROBLEM THIS STEP SOLVES
 *   This step is a failed design you are meant to feel, not ship.
 *   - Same spirit bubble + IMU/sim as step 02.
 *
 * WHAT YOU LEARN HERE
 *   What FreeRTOS is doing here
 *   Still only one task loop (app_main) plus vTaskDelay. FreeRTOS cannot
 *   “fix” a bad architecture: while beep_play blocks inside your task,
 *   that task is not ready to draw — even though the kernel is fine.
 *   Why beep_play blocks
 *   It fills PCM and writes to the speaker until the envelope/echo tail
 *   ends (see lab_beep). That is correct for an audio task. It is wrong
 *   for the only task that also owns the spirit UI.
 *   Compare to Lab 00
 *   Lab 00 step 03 already taught: click path must not call beep_*; use
 *   notify and an audio_task. Here the “click” is “entered LEVEL,” but
 *   the same rule applies — we break it so you feel the spirit level
 *   fail.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex01_step03.h"

#include "bsp.h"
#include "imu_sim.h"
#include "lab_beep.h"
#include "lab_spirit.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define FRAME_MS 33u
#define SAMPLE_DT_S 0.033f

void ex01_step03_start(void)
{
    float acc[3];
    float gyro[3];
    float t = 0.0f;
    uint32_t rng = 1u;
    int real;
    int was_real = -1;
    int was_level = 0;
    int level;
    spirit_init();
    spirit_start();

    for (;;) {
        real = (bsp_imu_read(acc, gyro) == 0);
        if (!real) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += SAMPLE_DT_S;
        }
        if (real != was_real) {
            was_real = real;
            spirit_set_live(1);
        }

        spirit_set_accel_mps2(acc[0], acc[1]);
        level = spirit_is_level();

        /*
         * INTENTIONAL BUG: play sound in the sense/draw loop.
         * beep_play blocks for the whole chime — bubble stops updating.
         * Do not copy this into a real product.
         */
        if (level && !was_level) {
            (void)beep_play(BEEP_CHIME, NULL);
        }
        was_level = level;

        vTaskDelay(pdMS_TO_TICKS(FRAME_MS));
    }
}
