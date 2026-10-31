/*
 * ex07 — counting semaphore as a resource pool.
 *
 * Teaching focus (kept here):
 *   - xSemaphoreCreateCounting(max, init) = M tokens
 *   - Take / Give; POOL OFF skips them (oversubscribe)
 *   - POOL ON caps in_use at EX07_SLOTS
 *
 * LCD / touch live in ex07_ui.*.
 */
#include "ex07.h"

#include "ex07_ui.h"

#include <stdint.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/semphr.h"
#include "freertos/task.h"

#define HOLD_MS 450u
#define REST_MS 80u

static SemaphoreHandle_t s_slots;
static volatile uint8_t s_pool_on;
static volatile uint8_t s_go;
static volatile uint8_t s_state[EX07_WORKER_COUNT];
static volatile uint32_t s_done[EX07_WORKER_COUNT];
static volatile uint8_t s_in_use;

static void on_pool_toggle(int is_on, void *user)
{
    (void)user;
    s_pool_on = is_on ? 1u : 0u;
}

static void worker_task(void *arg)
{
    unsigned idx = (unsigned)(uintptr_t)arg;

    while (!s_go) {
        vTaskDelay(1);
    }

    for (;;) {
        int took = 0;

        s_state[idx] = EX07_ST_WAIT;

        if (s_pool_on && s_slots != NULL) {
            (void)xSemaphoreTake(s_slots, portMAX_DELAY);
            took = 1;
        }

        s_state[idx] = EX07_ST_HOLD;
        s_in_use++;
        vTaskDelay(pdMS_TO_TICKS(HOLD_MS));
        s_in_use--;
        s_state[idx] = EX07_ST_WAIT;

        if (took && s_slots != NULL) {
            (void)xSemaphoreGive(s_slots);
        }

        s_done[idx]++;
        vTaskDelay(pdMS_TO_TICKS(REST_MS));
    }
}

static void ui_task(void *arg)
{
    (void)arg;

    while (!s_go) {
        vTaskDelay(1);
    }

    for (;;) {
        int free_n = -1;

        if (s_pool_on && s_slots != NULL) {
            free_n = (int)uxSemaphoreGetCount(s_slots);
        }
        ex07_ui_tick((int)s_pool_on, free_n, (unsigned)s_in_use, s_state,
                     s_done);
    }
}

void ex07_start(void)
{
    unsigned i;

    s_pool_on = 0;
    s_go = 0;
    s_in_use = 0;
    for (i = 0; i < EX07_WORKER_COUNT; i++) {
        s_state[i] = EX07_ST_WAIT;
        s_done[i] = 0;
    }

    s_slots = xSemaphoreCreateCounting(EX07_SLOTS, EX07_SLOTS);
    if (s_slots == NULL) {
        return;
    }

    if (ex07_ui_begin(on_pool_toggle, NULL) != 0) {
        return;
    }

    {
        static const char *const names[EX07_WORKER_COUNT] = {"w0", "w1", "w2",
                                                             "w3"};

        for (i = 0; i < EX07_WORKER_COUNT; i++) {
            (void)xTaskCreatePinnedToCore(worker_task, names[i], 2048,
                                          (void *)(uintptr_t)i, 5, NULL, 0);
        }
    }
    (void)xTaskCreatePinnedToCore(ui_task, "ui", 4096, NULL, 4, NULL, 1);

    s_go = 1;
}
