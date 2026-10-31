/*
 * ex08 — event group (WaitBits AND vs OR).
 *
 * Teaching focus (kept here):
 *   - xEventGroupCreate / SetBits / ClearBits
 *   - WaitBits(waitAll=pdTRUE)  → need A and B (AND)
 *   - WaitBits(waitAll=pdFALSE) → need A or  B (OR)
 *   - clearOnExit clears matched bits after a successful wait
 *
 * LCD / touch live in ex08_ui.*.
 */
#include "ex08.h"

#include "ex08_ui.h"

#include <stdint.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/event_groups.h"
#include "freertos/task.h"

#define FIRE_HOLD_MS 600u
#define BIT_MASK (EX08_BIT_A | EX08_BIT_B)

static EventGroupHandle_t s_bits;
static volatile uint8_t s_wait_all; /* 1 = AND, 0 = OR */
static volatile uint8_t s_phase;
static volatile uint32_t s_fire_count;
static volatile uint8_t s_go;
static volatile uint8_t s_reset_btns;

static void on_bit_toggle(unsigned bit_idx, int is_on, void *user)
{
    EventBits_t mask;

    (void)user;
    if (s_bits == NULL || bit_idx >= EX08_BIT_COUNT) {
        return;
    }
    mask = (bit_idx == 0u) ? EX08_BIT_A : EX08_BIT_B;
    if (is_on) {
        (void)xEventGroupSetBits(s_bits, mask);
    } else {
        (void)xEventGroupClearBits(s_bits, mask);
    }
}

static void on_mode_toggle(int wait_all, void *user)
{
    (void)user;
    s_wait_all = wait_all ? 1u : 0u;
}

static void worker_task(void *arg)
{
    (void)arg;

    while (!s_go) {
        vTaskDelay(1);
    }

    for (;;) {
        EventBits_t got;
        BaseType_t wait_all = (s_wait_all != 0u) ? pdTRUE : pdFALSE;

        s_phase = EX08_PHASE_WAIT;
        got = xEventGroupWaitBits(s_bits, BIT_MASK, pdTRUE /* clearOnExit */,
                                  wait_all, portMAX_DELAY);

        if (wait_all) {
            if ((got & BIT_MASK) != BIT_MASK) {
                continue;
            }
        } else if ((got & BIT_MASK) == 0u) {
            continue;
        }

        s_phase = EX08_PHASE_FIRE;
        s_fire_count++;
        s_reset_btns = 1;
        vTaskDelay(pdMS_TO_TICKS(FIRE_HOLD_MS));
    }
}

static void ui_task(void *arg)
{
    (void)arg;

    while (!s_go) {
        vTaskDelay(1);
    }

    for (;;) {
        EventBits_t snap = 0;

        if (s_reset_btns) {
            s_reset_btns = 0;
            ex08_ui_reset_bit_buttons();
        }
        if (s_bits != NULL) {
            snap = xEventGroupGetBits(s_bits);
        }
        ex08_ui_tick((int)s_wait_all, (int)s_phase, (uint32_t)snap,
                     s_fire_count);
    }
}

void ex08_start(void)
{
    s_wait_all = 1;
    s_phase = EX08_PHASE_WAIT;
    s_fire_count = 0;
    s_go = 0;
    s_reset_btns = 0;

    s_bits = xEventGroupCreate();
    if (s_bits == NULL) {
        return;
    }

    if (ex08_ui_begin(on_bit_toggle, on_mode_toggle, NULL) != 0) {
        return;
    }

    (void)xTaskCreatePinnedToCore(worker_task, "wait", 3072, NULL, 5, NULL, 0);
    (void)xTaskCreatePinnedToCore(ui_task, "ui", 4096, NULL, 4, NULL, 1);

    s_go = 1;
}
