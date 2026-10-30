#include "ex08_ui.h"

#include "bsp.h"
#include "rb_button.h"
#include "rb_screen.h"
#include "rb_style.h"
#include "rb_term.h"

#include <stdio.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define UI_MS 40u

enum { BTN_A = 0, BTN_B = 1, BTN_MODE = 2, BTN_COUNT = 3 };

static rb_display_t s_disp;
static rb_screen_t s_screen;
static rb_layout_t s_layout;
static rb_term_t s_term;
static rb_button_t s_btn[BTN_COUNT];

static ex08_bit_toggle_fn s_on_bit;
static ex08_mode_toggle_fn s_on_mode;
static void *s_user;

static void on_button(rb_button_t *btn, rb_button_event_t ev, void *user)
{
    unsigned which = (unsigned)(uintptr_t)user;

    (void)btn;
    if (ev != RB_BUTTON_EV_CLICK) {
        return;
    }

    if (which == BTN_MODE) {
        if (s_on_mode != NULL) {
            /* ON label = AND (wait all); OFF = OR */
            s_on_mode(rb_button_is_on(&s_btn[BTN_MODE]) ? 1 : 0, s_user);
        }
        return;
    }

    if (which < EX08_BIT_COUNT && s_on_bit != NULL) {
        s_on_bit(which, rb_button_is_on(&s_btn[which]) ? 1 : 0, s_user);
    }
}

int ex08_ui_begin(ex08_bit_toggle_fn on_bit, ex08_mode_toggle_fn on_mode,
                  void *user)
{
    static const rb_style_t styles[BTN_COUNT] = {
        RB_STYLE_INFO, RB_STYLE_WARNING, RB_STYLE_SUCCESS};
    static const char *on_lab[BTN_COUNT] = {"A1", "B1", "AND"};
    static const char *off_lab[BTN_COUNT] = {"A0", "B0", "OR"};
    unsigned i;

    s_on_bit = on_bit;
    s_on_mode = on_mode;
    s_user = user;

    if (rb_display_open(&s_disp) != 0) {
        return -1;
    }
    if (rb_screen_init(&s_screen, &s_disp) != 0) {
        return -1;
    }
    /* term | MODE / A | B */
    if (rb_layout_init(&s_layout, &s_screen, RB_LAYOUT_GRID, 2, 2, 4) != 0) {
        return -1;
    }

    rb_term_init(&s_term);
    (void)rb_term_set_scale(&s_term, 1);
    (void)rb_layout_add(&s_layout, &s_term.base);

    rb_button_init(&s_btn[BTN_MODE]);
    rb_button_set_mode(&s_btn[BTN_MODE], RB_BUTTON_MODE_TOGGLE);
    rb_button_set_labels(&s_btn[BTN_MODE], on_lab[BTN_MODE], off_lab[BTN_MODE]);
    rb_button_set_style(&s_btn[BTN_MODE], styles[BTN_MODE]);
    rb_button_set_indicator_size(&s_btn[BTN_MODE], 80, 2);
    rb_button_set_on(&s_btn[BTN_MODE], 1); /* boot AND */
    rb_button_set_on_event(&s_btn[BTN_MODE], on_button,
                           (void *)(uintptr_t)BTN_MODE);
    (void)rb_layout_add(&s_layout, &s_btn[BTN_MODE].base);

    for (i = 0; i < EX08_BIT_COUNT; i++) {
        rb_button_init(&s_btn[i]);
        rb_button_set_mode(&s_btn[i], RB_BUTTON_MODE_TOGGLE);
        rb_button_set_labels(&s_btn[i], on_lab[i], off_lab[i]);
        rb_button_set_style(&s_btn[i], styles[i]);
        rb_button_set_indicator_size(&s_btn[i], 80, 2);
        rb_button_set_on(&s_btn[i], 0);
        rb_button_set_on_event(&s_btn[i], on_button, (void *)(uintptr_t)i);
        (void)rb_layout_add(&s_layout, &s_btn[i].base);
    }

    rb_screen_set_root(&s_screen, &s_layout.base);
    rb_screen_reflow(&s_screen);

    rb_term_set_padding(&s_term, 4, 4, 4, 4);
    (void)rb_term_apply_geometry(&s_term);
    return 0;
}

void ex08_ui_reset_bit_buttons(void)
{
    unsigned i;

    for (i = 0; i < EX08_BIT_COUNT; i++) {
        rb_button_set_on(&s_btn[i], 0);
    }
}

void ex08_ui_tick(int wait_all, int phase, uint32_t bits, uint32_t fire_count)
{
    char line[RB_TERM_LOG_MAX_COLS];
    bsp_touch_sample_t smp;
    uint16_t phase_fg;
    unsigned i;

    if (bsp_touch_read(&smp) == 0) {
        for (i = 0; i < BTN_COUNT; i++) {
            (void)rb_button_feed_touch(&s_btn[i], &smp);
        }
    }

    (void)snprintf(line, sizeof(line), "ex08 %s", wait_all ? "AND" : "OR ");
    rb_term_set_line_ex(&s_term, 0, wait_all ? RB_TERM_GREEN : RB_TERM_YELLOW,
                        RB_TERM_BLACK, line);

    (void)snprintf(line, sizeof(line), "A=%u  B=%u",
                   (bits & EX08_BIT_A) ? 1u : 0u,
                   (bits & EX08_BIT_B) ? 1u : 0u);
    rb_term_set_line_ex(&s_term, 1, RB_TERM_CYAN, RB_TERM_BLACK, line);

    phase_fg = (phase == EX08_PHASE_FIRE) ? RB_TERM_GREEN : RB_TERM_WHITE;
    (void)snprintf(line, sizeof(line), "%s  #%lu",
                   (phase == EX08_PHASE_FIRE) ? "FIRED" : "WAIT ",
                   (unsigned long)fire_count);
    rb_term_set_line_ex(&s_term, 2, phase_fg, RB_TERM_BLACK, line);

    vTaskDelay(pdMS_TO_TICKS(UI_MS));
}
