/*
 * lab_led_row.c - display / 4x rb_led chase row / presses label.
 */
#include "lab_led_row.h"

#include "rb_label.h"
#include "rb_led.h"
#include "rb_screen.h"
#include "rb_style.h"

#define LED_ROW_GAP 8
#define LED_ON_INTENSITY 255u
#define LED_OFF_INTENSITY 0u

static rb_display_t s_disp;
static rb_screen_t s_screen;
static rb_layout_t s_root;
static rb_layout_t s_leds;
static rb_label_t s_title;
static rb_label_t s_presses;
static rb_led_t s_led[LED_ROW_COUNT];
static int s_inited;
static int s_started;
static unsigned s_active;

/* Distinct colors so the chase is easy to see. */
static const rb_style_t s_styles[LED_ROW_COUNT] = {
    RB_STYLE_DANGER,
    RB_STYLE_WARNING,
    RB_STYLE_SUCCESS,
    RB_STYLE_INFO,
};

void led_row_init(void)
{
    unsigned i;

    if (s_inited) {
        return;
    }

    (void)rb_display_open(&s_disp);
    (void)rb_screen_init(&s_screen, &s_disp);

    rb_label_init(&s_title);
    (void)rb_label_set_scale(&s_title, 2);
    rb_label_set_color(&s_title, RB_LABEL_CYAN, RB_LABEL_BLACK);
    rb_label_printf(&s_title, "BOOT LEDs");

    rb_label_init(&s_presses);
    (void)rb_label_set_scale(&s_presses, 2);
    rb_label_set_color(&s_presses, RB_LABEL_GRAY, RB_LABEL_BLACK);
    rb_label_printf(&s_presses, "presses: 0");

    for (i = 0; i < (unsigned)LED_ROW_COUNT; i++) {
        rb_led_init(&s_led[i]);
        rb_led_set_shape(&s_led[i], RB_LED_SHAPE_CIRCLE);
        rb_led_set_style(&s_led[i], s_styles[i]);
        rb_led_set_intensity(&s_led[i], LED_OFF_INTENSITY);
    }

    s_active = 0;
    s_inited = 1;
    s_started = 0;
}

void led_row_start(void)
{
    unsigned i;

    if (!s_inited || s_started) {
        return;
    }

    (void)rb_layout_init(&s_leds, &s_screen, RB_LAYOUT_COLS, 1,
                         (unsigned)LED_ROW_COUNT, LED_ROW_GAP);
    for (i = 0; i < (unsigned)LED_ROW_COUNT; i++) {
        (void)rb_layout_add(&s_leds, &s_led[i].base);
    }

    (void)rb_layout_init(&s_root, &s_screen, RB_LAYOUT_ROWS, 3, 1, LED_ROW_GAP);
    (void)rb_layout_add(&s_root, &s_title.base);
    (void)rb_layout_add(&s_root, &s_leds.base);
    (void)rb_layout_add(&s_root, &s_presses.base);

    rb_screen_set_root(&s_screen, &s_root.base);
    rb_screen_reflow(&s_screen);

    s_started = 1;
    led_row_set_active(0);
    led_row_set_presses(0);
}

void led_row_set_active(unsigned index)
{
    unsigned i;

    if (!s_started) {
        return;
    }
    if (index >= (unsigned)LED_ROW_COUNT) {
        index = 0;
    }

    s_active = index;
    for (i = 0; i < (unsigned)LED_ROW_COUNT; i++) {
        rb_led_set_intensity(&s_led[i], (i == s_active) ? LED_ON_INTENSITY
                                                        : LED_OFF_INTENSITY);
    }
}

void led_row_set_presses(unsigned n)
{
    if (!s_started) {
        return;
    }
    rb_label_printf(&s_presses, "presses: %u", n);
}
