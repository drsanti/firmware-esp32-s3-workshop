/*
 * ex10 — BOOT GPIO: poll vs ISR + vTaskNotifyGiveFromISR.
 *
 * Teaching focus (kept here):
 *   - POLL: task reads GPIO0 for a press edge
 *   - ISR:  GPIO negedge ISR only notifies; task does debounce + count
 *   - ISR must stay short — no LCD / long work (see experiments)
 *
 * LCD / touch live in ex10_ui.*.
 * Boards: BOOT = GPIO0 (ws28p / es3c28p).
 */
#include "ex10.h"

#include "ex10_ui.h"

#include <stdint.h>

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define BOOT_GPIO ((gpio_num_t)0)
#define DEBOUNCE_MS 200u
#define FLASH_MS 150u
#define POLL_MS 20u
#define NOTIFY_WAIT_MS 50u

static TaskHandle_t s_worker;
static volatile uint8_t s_use_isr;
static volatile uint8_t s_go;
static volatile uint32_t s_presses;
static volatile uint8_t s_flash;
static volatile uint32_t s_flash_until_ms;
static volatile uint32_t s_ui_ticks;

static uint32_t now_ms(void)
{
    return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

static void on_press(void)
{
    s_presses++;
    s_flash = 1;
    s_flash_until_ms = now_ms() + FLASH_MS;
}

static void IRAM_ATTR boot_isr(void *arg)
{
    BaseType_t hp = pdFALSE;

    (void)arg;
    if (!s_use_isr || s_worker == NULL) {
        return;
    }
    vTaskNotifyGiveFromISR(s_worker, &hp);
    if (hp == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void apply_mode(int use_isr)
{
    s_use_isr = use_isr ? 1u : 0u;

    /* Keep the pin quiet while polling so the two paths do not fight. */
    if (s_use_isr) {
        (void)gpio_intr_enable(BOOT_GPIO);
    } else {
        (void)gpio_intr_disable(BOOT_GPIO);
        if (s_worker != NULL) {
            /* Wake worker if it was blocked in ulTaskNotifyTake. */
            (void)xTaskNotifyGive(s_worker);
        }
    }
}

static void on_mode_toggle(int use_isr, void *user)
{
    (void)user;
    apply_mode(use_isr);
}

/* Active-low BOOT: 1 = pressed. */
static int boot_pressed(void)
{
    return gpio_get_level(BOOT_GPIO) == 0 ? 1 : 0;
}

static int boot_edge(int *was_pressed)
{
    int now = boot_pressed();
    int edge = (now != 0 && *was_pressed == 0) ? 1 : 0;

    *was_pressed = now;
    return edge;
}

static void worker_task(void *arg)
{
    int was_pressed = 0;
    TickType_t last =
        xTaskGetTickCount() - pdMS_TO_TICKS(DEBOUNCE_MS) - (TickType_t)1;

    (void)arg;

    while (!s_go) {
        vTaskDelay(1);
    }

    for (;;) {
        if (s_use_isr) {
            /* Timed wait so toggling back to POLL can resume polling. */
            if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(NOTIFY_WAIT_MS)) ==
                0u) {
                continue;
            }
            /* Mode may have flipped while we waited. */
            if (!s_use_isr) {
                continue;
            }
        } else {
            if (!boot_edge(&was_pressed)) {
                vTaskDelay(pdMS_TO_TICKS(POLL_MS));
                continue;
            }
        }

        {
            TickType_t t = xTaskGetTickCount();

            if ((t - last) < pdMS_TO_TICKS(DEBOUNCE_MS)) {
                continue;
            }
            last = t;
        }
        on_press();
    }
}

static void ui_task(void *arg)
{
    (void)arg;

    while (!s_go) {
        vTaskDelay(1);
    }

    for (;;) {
        s_ui_ticks++;
        if (s_flash && (int32_t)(now_ms() - s_flash_until_ms) >= 0) {
            s_flash = 0;
        }
        ex10_ui_tick((int)s_use_isr, s_presses, s_ui_ticks, (int)s_flash);
    }
}

void ex10_start(void)
{
    s_use_isr = 0;
    s_go = 0;
    s_presses = 0;
    s_flash = 0;
    s_flash_until_ms = 0;
    s_ui_ticks = 0;
    s_worker = NULL;

    if (ex10_ui_begin(on_mode_toggle, NULL) != 0) {
        return;
    }

    (void)xTaskCreatePinnedToCore(worker_task, "boot", 3072, NULL, 5, &s_worker,
                                  0);
    (void)xTaskCreatePinnedToCore(ui_task, "ui", 4096, NULL, 4, NULL, 1);

    (void)gpio_install_isr_service(0);
    (void)gpio_set_intr_type(BOOT_GPIO, GPIO_INTR_NEGEDGE);
    (void)gpio_isr_handler_add(BOOT_GPIO, boot_isr, NULL);
    (void)gpio_intr_disable(BOOT_GPIO); /* POLL boots with IRQ off */

    s_go = 1;
}
