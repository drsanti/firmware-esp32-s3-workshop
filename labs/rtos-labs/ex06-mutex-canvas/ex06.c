/*
 * ex06 — mutex on a shared canvas.
 *
 * Teaching focus (kept here):
 *   - MUTEX ON  → paint_clean under Take/Give (smooth)
 *   - MUTEX OFF → paint_strobe with mid-frame flushes (choppy)
 *
 * Drawing / touch / layout live in ex06_canvas.*.
 */
#include "ex06.h"

#include "ex06_canvas.h"

#include <stdint.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/semphr.h"
#include "freertos/task.h"

#define LOOP_ON_MS 55u
#define LOOP_OFF_MS 15u

static SemaphoreHandle_t s_mtx;
static volatile uint8_t s_mutex_on;
static volatile uint8_t s_need_canvas_reset;
static volatile uint32_t s_frames[EX06_BALL_COUNT];
static volatile uint8_t s_go;

static void on_mutex_toggle(int is_on, void *user)
{
    (void)user;
    s_mutex_on = is_on ? 1u : 0u;
    ex06_canvas_reset();
    s_need_canvas_reset = 1;
}

static void painter_task(void *arg)
{
    unsigned idx = (unsigned)(uintptr_t)arg;

    while (!s_go) {
        vTaskDelay(1);
    }

    for (;;) {
        uint8_t unlocked = (s_mutex_on == 0u) ? 1u : 0u;

        if (!unlocked && s_mtx != NULL) {
            (void)xSemaphoreTake(s_mtx, portMAX_DELAY);
        }

        if (s_need_canvas_reset) {
            s_need_canvas_reset = 0;
            ex06_canvas_reset();
        }

        if (unlocked) {
            ex06_canvas_paint_strobe();
        } else {
            ex06_canvas_paint_clean();
        }
        ex06_canvas_advance(idx);

        if (!unlocked && s_mtx != NULL) {
            (void)xSemaphoreGive(s_mtx);
        }

        s_frames[idx]++;
        vTaskDelay(pdMS_TO_TICKS(unlocked ? LOOP_OFF_MS : LOOP_ON_MS));
    }
}

static void ui_task(void *arg)
{
    (void)arg;

    while (!s_go) {
        vTaskDelay(1);
    }

    for (;;) {
        ex06_canvas_ui_tick((int)s_mutex_on, s_frames[EX06_BALL_A],
                            s_frames[EX06_BALL_B]);
    }
}

void ex06_start(void)
{
    s_mutex_on = 0;
    s_need_canvas_reset = 0;
    s_go = 0;
    s_frames[EX06_BALL_A] = 0;
    s_frames[EX06_BALL_B] = 0;

    s_mtx = xSemaphoreCreateMutex();
    if (s_mtx == NULL) {
        return;
    }

    if (ex06_canvas_begin(on_mutex_toggle, NULL) != 0) {
        return;
    }

    (void)xTaskCreatePinnedToCore(ui_task, "ui", 4096, NULL, 4, NULL, 1);
    (void)xTaskCreatePinnedToCore(painter_task, "paintA", 3072,
                                  (void *)(uintptr_t)EX06_BALL_A, 5, NULL, 0);
    (void)xTaskCreatePinnedToCore(painter_task, "paintB", 3072,
                                  (void *)(uintptr_t)EX06_BALL_B, 5, NULL, 0);

    s_go = 1;
}
