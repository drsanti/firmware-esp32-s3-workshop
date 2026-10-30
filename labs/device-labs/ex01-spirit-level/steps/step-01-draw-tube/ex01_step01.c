/*
 * Lab 01 / Step 01 — Draw the spirit level
 *
 * WHAT THIS CODE DOES
 *   Success: the bubble walks left/right on its own; the status flips
 *   LEVEL ↔ TILT as it crosses center.
 *
 * HOW IT WORKS
 *   Not yet: tasks, queues, notify, IMU. Those appear when later
 *   requirements force them.
 *   Architecture
 *   One task drives fake tilt into the spirit helper; delay sets frame
 *   rate and yield.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - We need an instrument UI, not a screen full of raw numbers.
 *   - We must redraw often, but a tight for (;;) with no pause would burn
 *   CPU and fight the scheduler.
 *   - The IMU comes in a later step — for now we still need to prove the
 *   face of the device.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: the bubble walks left/right on its own; the status flips
 *   LEVEL ↔ TILT as it crosses center.
 *
 * WHAT YOU LEARN HERE
 *   Not yet: tasks, queues, notify, IMU. Those appear when later
 *   requirements force them.
 *   Architecture
 *   One task drives fake tilt into the spirit helper; delay sets frame
 *   rate and yield.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex01_step01.h"

#include "bsp.h"
#include "lab_spirit.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define FRAME_MS 33u       /* ~30 Hz UI */
#define AX_STEP_MPS2 0.35f /* fake tilt sweep */
#define AX_MAX_MPS2 8.0f

void ex01_step01_start(void) {
  float ax = 0.0f;
  float ay = 0.0f;
  int dir = 1;
  spirit_init();
  spirit_start();
  spirit_set_live(1);

  for (;;) {
    /* Triangle sweep on ax so the bubble walks left/right. */
    ax += (float)dir * AX_STEP_MPS2;
    if (ax > AX_MAX_MPS2) {
      ax = AX_MAX_MPS2;
      dir = -1;
    } else if (ax < -AX_MAX_MPS2) {
      ax = -AX_MAX_MPS2;
      dir = 1;
    }
    ay = 0.0f;

    spirit_set_accel_mps2(ax, ay);

    /* Yield so the idle/other work can run; also sets frame rate. */
    vTaskDelay(pdMS_TO_TICKS(FRAME_MS));
  }
}
