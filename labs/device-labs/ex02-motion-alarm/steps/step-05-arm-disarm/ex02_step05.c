/*
 * Lab 02 / Step 05 — Arm / disarm
 *
 * WHAT THIS CODE DOES
 *   Success: disarm → shake does nothing; arm → shake alarms; UI stays
 *   live.
 *
 * HOW IT WORKS
 *   Architecture
 *   State machine
 *   BOOT disarm returns ARMED to IDLE; after ALARM hold, return to ARMED
 *   (see sketch).
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   - Product rules: ignore motion when idle; avoid spam while sounding.
 *   - Same BOOT edge pattern as Lab 00 step 05 (bsp_buttons bit1).
 *
 * PROBLEM THIS STEP SOLVES
 *   Success: disarm → shake does nothing; arm → shake alarms; UI stays
 *   live.
 *
 * WHAT YOU LEARN HERE
 *   No new FreeRTOS API. The lesson is state + the tools you already
 *   have:
 *   BOOT handling is plain C edge detection — same idea as Lab 00 LEDs.
 *
 * Full labsheet: README.md in this folder.
 */
#include "ex02_step05.h"

#include "bsp.h"
#include "imu_sim.h"
#include "lab_alarm.h"
#include "lab_beep.h"

#include <math.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/queue.h"
#include "freertos/task.h"

/* bsp_buttons: bit0=PWR (ws28p only), bit1=BOOT (both boards). */
#define BOOT_BTN_MASK 2u
#define SAMPLE_MS 40u
#define SAMPLE_DT_S 0.04f
#define SHAKE_THRESH_MPS2 2.5f
#define ALARM_HOLD_MS 1200u
#define EVT_QUEUE_LEN 4u

typedef struct {
    uint8_t kind;
} alarm_evt_t;

static QueueHandle_t g_evt;

static void audio_task(void *arg)
{
    alarm_evt_t evt;

    (void)arg;
    for (;;) {
        if (xQueueReceive(g_evt, &evt, portMAX_DELAY) == pdTRUE) {
            (void)evt;
            (void)beep_play(BEEP_ALARM, NULL);
        }
    }
}

static void detect_task(void *arg)
{
    float acc[3];
    float gyro[3];
    float t = 0.0f;
    uint32_t rng = 1u;
    int real;
    int was_real = -1;
    int was_shake = 0;
    int shake;
    float horiz;
    alarm_st_t st = ALARM_ST_ARMED;
    TickType_t alarm_until = 0;
    uint32_t boot_was = 0;
    alarm_evt_t evt;

    (void)arg;

    alarm_init();
    alarm_start();
    alarm_set_state(st);
    alarm_set_detail("BOOT = arm/disarm");

    for (;;) {
        uint32_t boot = bsp_buttons() & BOOT_BTN_MASK;

        /* Rising edge on BOOT: toggle arm (not while ALARM banner holds). */
        if (boot != 0u && boot_was == 0u) {
            if (st == ALARM_ST_IDLE) {
                st = ALARM_ST_ARMED;
                alarm_set_state(st);
                alarm_set_detail("ARMED - shake me");
            } else if (st == ALARM_ST_ARMED) {
                st = ALARM_ST_IDLE;
                alarm_set_state(st);
                alarm_set_detail("IDLE - BOOT to arm");
            }
            /* If ALARM, ignore BOOT until hold ends. */
        }
        boot_was = boot;

        real = (bsp_imu_read(acc, gyro) == 0);
        if (!real) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += SAMPLE_DT_S;
        }
        if (real != was_real) {
            was_real = real;
            alarm_set_live(real);
        }

        horiz = sqrtf(acc[0] * acc[0] + acc[1] * acc[1]);
        shake = (horiz >= SHAKE_THRESH_MPS2);

        if (st == ALARM_ST_ARMED && shake && !was_shake) {
            st = ALARM_ST_ALARM;
            alarm_set_state(st);
            alarm_set_detail("tripped!");
            alarm_until = xTaskGetTickCount() + pdMS_TO_TICKS(ALARM_HOLD_MS);
            evt.kind = 0;
            (void)xQueueSend(g_evt, &evt, 0);
        }
        was_shake = shake;

        if (st == ALARM_ST_ALARM && alarm_until != 0 &&
            xTaskGetTickCount() >= alarm_until) {
            alarm_until = 0;
            st = ALARM_ST_ARMED;
            alarm_set_state(st);
            alarm_set_detail("ARMED - shake me");
        }

        if (st == ALARM_ST_ARMED) {
            alarm_set_detail("horiz %.1f", (double)horiz);
        }

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_MS));
    }
}

void ex02_step05_start(void)
{
g_evt = xQueueCreate(EVT_QUEUE_LEN, sizeof(alarm_evt_t));
    if (g_evt == NULL) {
        return;
    }

    (void)xTaskCreate(audio_task, "audio", 4096, NULL, 5, NULL);
    (void)xTaskCreate(detect_task, "detect", 4096, NULL, 5, NULL);
}
