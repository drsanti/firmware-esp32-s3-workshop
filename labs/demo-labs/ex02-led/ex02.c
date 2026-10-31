/*
 * Demo 02 — led
 *
 * WHAT THIS CODE DOES
 *   4x4 rb_led grid with intensity breathe
 *
 * HOW IT WORKS
 *   main/main.c calls ex02_start() after bsp_init().
 *   This file builds the rb_* widget tree and starts any demo tasks.
 *
 * PROBLEM WITHOUT THIS PATTERN
 *   Seventeen separate IDF apps make flash/reconfigure slow for a widget catalog.
 *
 * PROBLEM THIS FILE SOLVES
 *   One flashable demo-labs binary; switch demos in main/main.c.
 *
 * WHAT YOU LEARN HERE
 *   - rb_screen / rb_layout composition for this widget
 *   - See README.md in this folder for the full labsheet
 *
 * Labsheet: README.md in this folder.
 */
#include "ex02.h"

#include "bsp.h"
#include "rb_led.h"
#include "rb_screen.h"

#include <stdint.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

typedef TickType_t lab_tick_t;

#define LAB_ROWS 4
#define LAB_COLS 4
#define LAB_N (LAB_ROWS * LAB_COLS)

static rb_display_t g_disp;
static rb_screen_t g_screen;
static rb_layout_t g_layout;
static rb_led_t g_leds[LAB_N];

typedef struct {
    rb_led_t *led;
    uint8_t phase;
    uint32_t period_ms;
} breathe_cfg_t;

static const uint8_t k_rgb[LAB_N][3] = {
    {255, 40, 40},  {255, 160, 0},  {255, 220, 40}, {40, 220, 80},
    {40, 200, 255}, {40, 80, 255},  {180, 40, 255}, {255, 40, 180},
    {255, 80, 40},  {200, 200, 40}, {80, 255, 160}, {40, 160, 255},
    {120, 80, 255}, {255, 40, 120}, {200, 200, 200}, {255, 120, 40},
};

static void breathe_task(void *arg)
{
    breathe_cfg_t *cfg = (breathe_cfg_t *)arg;
    uint8_t t = cfg->phase;

    for (;;) {
        /* triangle 0..255..0 */
        uint8_t intens = (t < 128) ? (uint8_t)(t * 2) : (uint8_t)((255 - t) * 2);
        rb_led_set_intensity(cfg->led, intens);
        t++;
        {
            lab_tick_t delay = pdMS_TO_TICKS(cfg->period_ms);
            vTaskDelay(delay);
        }
    }
}

void ex02_start(void)
{
    static breathe_cfg_t cfgs[LAB_N];
    static char task_names[LAB_N][8];
    int i;
    (void)rb_display_open(&g_disp);
    (void)rb_screen_init(&g_screen, &g_disp);
    (void)rb_layout_init(&g_layout, &g_screen, RB_LAYOUT_GRID, LAB_ROWS,
                         LAB_COLS, 2);
    for (i = 0; i < LAB_N; i++) {
        rb_led_init(&g_leds[i]);
        rb_led_set_shape(&g_leds[i], (i & 1) ? RB_LED_SHAPE_ROUND_RECT
                                             : RB_LED_SHAPE_CIRCLE);
        rb_led_set_rgb(&g_leds[i], k_rgb[i][0], k_rgb[i][1], k_rgb[i][2]);
        rb_led_set_intensity(&g_leds[i], 0);
        (void)rb_layout_add(&g_layout, &g_leds[i].base);
    }
    rb_screen_set_root(&g_screen, &g_layout.base);
    rb_screen_reflow(&g_screen);

    for (i = 0; i < LAB_N; i++) {
        cfgs[i].led = &g_leds[i];
        cfgs[i].phase = (uint8_t)(i * 16);
        cfgs[i].period_ms = 20u + (uint32_t)(i % 4) * 4u;
        (void)snprintf(task_names[i], sizeof(task_names[i]), "led%02d", i);
        (void)xTaskCreate(breathe_task, task_names[i], 3072, &cfgs[i], 5,
                          NULL);
    }

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
