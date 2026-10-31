/*
 * lab_sound_pad.c - display / optional knobs / touch; students own callbacks.
 */
#include "lab_sound_pad.h"

#include "bsp.h"
#include "lab_beep.h"
#include "rb_button.h"
#include "rb_label.h"
#include "rb_screen.h"
#include "rb_slider.h"

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

typedef struct {
    sound_pad_click_fn fn;
    void *user;
} sound_pad_click_slot_t;

typedef struct {
    rb_slider_t slider;
    sound_pad_knob_fn fn;
    void *user;
    int enabled;
    void (*apply)(uint8_t percent); /* default when fn is NULL */
} sound_pad_knob_t;

static rb_display_t s_disp;
static rb_screen_t s_screen;
static rb_layout_t s_root;
static rb_layout_t s_btns;
static rb_label_t s_title;
static rb_button_t s_btn[SOUND_PAD_MAX_BUTTONS];
static sound_pad_click_slot_t s_click[SOUND_PAD_MAX_BUTTONS];
static sound_pad_knob_t s_gain;
static sound_pad_knob_t s_echo;
static sound_pad_knob_t s_release;
static unsigned s_btn_count;
static int s_inited;
static int s_started;

static void on_knob_change(rb_slider_t *s, int32_t value, void *user)
{
    sound_pad_knob_t *k = (sound_pad_knob_t *)user;
    uint8_t percent;

    (void)s;
    if (k == 0) {
        return;
    }
    if (value < 0) {
        value = 0;
    }
    if (value > 100) {
        value = 100;
    }
    percent = (uint8_t)value;
    if (k->fn != 0) {
        k->fn(percent, k->user);
    } else if (k->apply != 0) {
        k->apply(percent);
    }
}

static void on_button_event(rb_button_t *btn, rb_button_event_t ev, void *user)
{
    sound_pad_click_slot_t *slot = (sound_pad_click_slot_t *)user;

    (void)btn;
    if (ev != RB_BUTTON_EV_CLICK || slot == 0 || slot->fn == 0) {
        return;
    }
    slot->fn(slot->user);
}

static void touch_task(void *arg)
{
    (void)arg;
    for (;;) {
        bsp_touch_sample_t smp;
        unsigned i;

        if (bsp_touch_read(&smp) == 0) {
            for (i = 0; i < s_btn_count; i++) {
                (void)rb_button_feed_touch(&s_btn[i], &smp);
            }
            if (s_gain.enabled) {
                (void)rb_slider_feed_touch(&s_gain.slider, &smp);
            }
            if (s_echo.enabled) {
                (void)rb_slider_feed_touch(&s_echo.slider, &smp);
            }
            if (s_release.enabled) {
                (void)rb_slider_feed_touch(&s_release.slider, &smp);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

static int add_knob(sound_pad_knob_t *k, const char *label, rb_style_t style,
                    uint8_t initial, sound_pad_knob_fn on_change, void *user,
                    void (*apply)(uint8_t))
{
    if (!s_inited || s_started || k->enabled) {
        return -1;
    }

    k->fn = on_change;
    k->user = user;
    k->apply = apply;

    rb_slider_init(&k->slider);
    rb_slider_set_orient(&k->slider, RB_SLIDER_ORIENT_HORIZONTAL);
    rb_slider_set_mode(&k->slider, RB_SLIDER_MODE_INPUT);
    rb_slider_set_style(&k->slider, style);
    rb_slider_set_range(&k->slider, 0, 100);
    rb_slider_set_value(&k->slider, (int32_t)initial);
    rb_slider_set_label(&k->slider, label);
    rb_slider_set_show_value(&k->slider, 1);
    rb_slider_set_show_fill(&k->slider, 1);
    rb_slider_set_caption_gap(&k->slider, 10); /* label/value ↔ rail */
    rb_slider_set_on_change(&k->slider, on_knob_change, k);

    k->enabled = 1;
    return 0;
}

void sound_pad_init(void)
{
    if (s_inited) {
        return;
    }

    beep_set_gain(40);
    beep_set_echo(45); /* match firmware page_sfx default */
    beep_set_release(30);

    (void)rb_display_open(&s_disp);
    (void)rb_screen_init(&s_screen, &s_disp);

    rb_label_init(&s_title);
    (void)rb_label_set_scale(&s_title, 2);
    rb_label_set_color(&s_title, RB_LABEL_CYAN, RB_LABEL_BLACK);
    rb_label_printf(&s_title, "Sound pad");

    s_btn_count = 0;
    s_gain.enabled = 0;
    s_echo.enabled = 0;
    s_release.enabled = 0;
    s_inited = 1;
    s_started = 0;
}

int sound_pad_add_gain_slider(sound_pad_knob_fn on_change, void *user)
{
    return add_knob(&s_gain, "G", RB_STYLE_PRIMARY, beep_get_gain(),
                    on_change, user, beep_set_gain);
}

int sound_pad_add_echo_slider(sound_pad_knob_fn on_change, void *user)
{
    return add_knob(&s_echo, "E", RB_STYLE_INFO, beep_get_echo(),
                    on_change, user, beep_set_echo);
}

int sound_pad_add_release_slider(sound_pad_knob_fn on_change, void *user)
{
    return add_knob(&s_release, "R", RB_STYLE_WARNING, beep_get_release(),
                    on_change, user, beep_set_release);
}

int sound_pad_add_button(const char *label, rb_style_t style,
                         sound_pad_click_fn on_click, void *user)
{
    unsigned i;

    if (!s_inited || s_started ||
        s_btn_count >= (unsigned)SOUND_PAD_MAX_BUTTONS) {
        return -1;
    }

    i = s_btn_count;
    s_click[i].fn = on_click;
    s_click[i].user = user;

    rb_button_init(&s_btn[i]);
    rb_button_set_mode(&s_btn[i], RB_BUTTON_MODE_NORMAL);
    rb_button_set_label(&s_btn[i], label != 0 ? label : "?");
    rb_button_set_style(&s_btn[i], style);
    rb_button_set_on_event(&s_btn[i], on_button_event, &s_click[i]);

    s_btn_count++;
    return 0;
}

void sound_pad_start(void)
{
    unsigned rows;
    unsigned cols;
    unsigned knob_n;
    unsigned i;

    if (!s_inited || s_started) {
        return;
    }

    cols = s_btn_count > 0u ? s_btn_count : 1u;
    (void)rb_layout_init(&s_btns, &s_screen, RB_LAYOUT_COLS, 1, cols, 6);
    for (i = 0; i < s_btn_count; i++) {
        (void)rb_layout_add(&s_btns, &s_btn[i].base);
    }

    /* Each knob is a root row (not nested) so cells are tall enough
     * for caption_gap between label/value and the rail. */
    knob_n = 0u;
    if (s_gain.enabled) {
        knob_n++;
    }
    if (s_echo.enabled) {
        knob_n++;
    }
    if (s_release.enabled) {
        knob_n++;
    }

    rows = 2u + knob_n;
    (void)rb_layout_init(&s_root, &s_screen, RB_LAYOUT_ROWS, rows, 1, 4);
    (void)rb_layout_add(&s_root, &s_title.base);
    (void)rb_layout_add(&s_root, &s_btns.base);
    if (s_gain.enabled) {
        (void)rb_layout_add(&s_root, &s_gain.slider.base);
    }
    if (s_echo.enabled) {
        (void)rb_layout_add(&s_root, &s_echo.slider.base);
    }
    if (s_release.enabled) {
        (void)rb_layout_add(&s_root, &s_release.slider.base);
    }

    rb_screen_set_root(&s_screen, &s_root.base);
    rb_screen_reflow(&s_screen);
    (void)xTaskCreate(touch_task, "touch", 3072, NULL, 5, NULL);
    s_started = 1;
}
