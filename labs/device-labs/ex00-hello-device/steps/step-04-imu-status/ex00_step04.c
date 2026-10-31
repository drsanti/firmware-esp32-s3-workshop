/*
 * Lab 00 / Step 04 — IMU status
 *
 * WHAT THIS CODE DOES
 *   Success: LCD shows title + live/sim status + updating ax ay az; beep
 *   at boot and again when ay rises above 5 (tilt the board).
 *
 * HOW IT WORKS
 *   ex00_step04_start() runs after bsp_init() when selected in the lab
 *   switcher.
 *   Follow the code below; architecture detail is in README.md.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   Not every board has a working IMU in the workshop kit — live and sim
 *   must both count as success.
 *   ws28p often shows IMU live (QMI8658). es3c28p may show IMU sim if no
 *   sensor path — that is still a successful lab.
 *   Architecture
 *   UI owns the IMU loop and LCD; discrete cues use the same notify
 *   bridge as step 03.
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: LCD shows title + live/sim status + updating ax ay az; beep
 *   at boot and again when ay rises above 5 (tilt the board).
 *
 * WHAT YOU LEARN HERE
 *   Same notify bridge as step 03
 *   Boot ready and ay threshold both use xTaskNotifyGive → audio_task →
 *   beep_play(BEEP_CHIME). Why reuse notify?
 *   IMU numbers are sampled and printed in UI. Sound is still an event.
 *   Lab 01 will introduce a queue when tilt must move as a stream between
 *   tasks without freezing the UI.
 *   Why vTaskDelay(200 ms) (~5 Hz)?
 *   pdMS_TO_TICKS(200) converts ms → ticks. This is a relative delay
 *   again: fine for a status panel. A hard 50 Hz control loop would
 *   prefer vTaskDelayUntil (see Lab 01 step 01 write-up).
 *   Why an edge flag for ay (ay_hi)?
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex00_step04.h"

#include "bsp.h"
#include "imu_sim.h"
#include "lab_beep.h"
#include "lab_lcd.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define SAMPLE_DT_S 0.2f
#define AY_BEEP_THRESH 5.0f /* m/s^2 — notify when ay exceeds this */

static TaskHandle_t g_audio;

static void notify_beep(void)
{
    if (g_audio != NULL) {
        (void)xTaskNotifyGive(g_audio);
    }
}

/* Audio side: wait for notify, play tone (do not touch LCD / IMU). */
static void audio_task(void *arg)
{
    (void)arg;
    for (;;) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        (void)beep_play(BEEP_CHIME, NULL); /* C-E-G rising triad */
    }
}

static void ui_task(void *arg)
{
    float acc[3];
    float gyro[3];
    float t = 0.0f;
    uint32_t rng = 1u;
    int real;
    int was_real = -1;
    int ay_hi = 0; /* edge detect so we do not beep every 200 ms */

    (void)arg;

    (void)lcd_begin(3);
    lcd_printf(0, "IMU status");
    lcd_set_color(1, RB_LABEL_YELLOW);
    lcd_printf(1, "IMU ...");
    lcd_printf(2, "ax ay az");

    /* Boot ready cue (step-03 pattern). */
    vTaskDelay(pdMS_TO_TICKS(50));
    notify_beep();

    for (;;) {
        /* Prefer hardware IMU; fall back to sim when read fails. */
        real = (bsp_imu_read(acc, gyro) == 0);
        if (!real) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += SAMPLE_DT_S;
        }
        if (real != was_real) {
            was_real = real;
            lcd_set_color(1, real ? RB_LABEL_GREEN : RB_LABEL_MAGENTA);
            lcd_printf(1, real ? "IMU live" : "IMU sim");
        }
        lcd_printf(2, "%.1f %.1f %.1f", (double)acc[0], (double)acc[1],
                   (double)acc[2]);

        /* ay = acc[1]; beep once each time it rises above the threshold. */
        if (acc[1] > AY_BEEP_THRESH) {
            if (!ay_hi) {
                ay_hi = 1;
                notify_beep();
            }
        } else {
            ay_hi = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void ex00_step04_start(void)
{
/* Create audio first so g_audio is valid before the boot notify. */
    (void)xTaskCreate(audio_task, "audio", 4096, NULL, 5, &g_audio);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
