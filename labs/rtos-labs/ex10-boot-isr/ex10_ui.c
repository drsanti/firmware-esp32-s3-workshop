#include "ex10_ui.h"

#include "bsp.h"
#include "rb_button.h"
#include "rb_screen.h"
#include "rb_style.h"
#include "rb_term.h"

#include <stdio.h>

#include "freertos/FreeRTOS.h" /* IWYU pragma: keep */
#include "freertos/task.h"

#define UI_MS 40u

static rb_display_t s_disp;
static rb_screen_t s_screen;
static rb_layout_t s_layout;
static rb_term_t s_term;
static rb_button_t s_btn;

static ex10_mode_toggle_fn s_on_toggle;
static void *s_toggle_user;

static void on_button(rb_button_t *btn, rb_button_event_t ev, void *user)
{
    (void)user;
    if (ev != RB_BUTTON_EV_CLICK || s_on_toggle == NULL) {
        return;
    }
    s_on_toggle(rb_button_is_on(btn) ? 1 : 0, s_toggle_user);
}

int ex10_ui_begin(ex10_mode_toggle_fn on_toggle, void *user)
{
    s_on_toggle = on_toggle;
    s_toggle_user = user;

    if (rb_display_open(&s_disp) != 0) {
        return -1;
    }
    if (rb_screen_init(&s_screen, &s_disp) != 0) {
        return -1;
    }
    if (rb_layout_init(&s_layout, &s_screen, RB_LAYOUT_GRID, 2, 1, 4) != 0) {
        return -1;
    }

    rb_term_init(&s_term);
    (void)rb_term_set_scale(&s_term, 1);
    (void)rb_layout_add(&s_layout, &s_term.base);

    rb_button_init(&s_btn);
    rb_button_set_mode(&s_btn, RB_BUTTON_MODE_TOGGLE);
    rb_button_set_labels(&s_btn, "ISR", "POLL");
    rb_button_set_style(&s_btn, RB_STYLE_SUCCESS);
    rb_button_set_indicator_size(&s_btn, 100, 2);
    rb_button_set_on(&s_btn, 0); /* boot POLL */
    rb_button_set_on_event(&s_btn, on_button, NULL);
    (void)rb_layout_add(&s_layout, &s_btn.base);

    rb_screen_set_root(&s_screen, &s_layout.base);
    rb_screen_reflow(&s_screen);

    rb_term_set_padding(&s_term, 4, 4, 4, 4);
    (void)rb_term_apply_geometry(&s_term);
    return 0;
}

void ex10_ui_tick(int use_isr, uint32_t presses, uint32_t ui_ticks,
                  int flash_on)
{
    char line[RB_TERM_LOG_MAX_COLS];
    bsp_touch_sample_t smp;
    uint16_t mode_fg;

    if (bsp_touch_read(&smp) == 0) {
        (void)rb_button_feed_touch(&s_btn, &smp);
    }

    mode_fg = use_isr ? RB_TERM_GREEN : RB_TERM_YELLOW;
    (void)snprintf(line, sizeof(line), "ex10 %s",
                   use_isr ? "ISR " : "POLL");
    rb_term_set_line_ex(&s_term, 0, mode_fg, RB_TERM_BLACK, line);

    (void)snprintf(line, sizeof(line), "press #%lu  %s",
                   (unsigned long)presses, flash_on ? "****" : "    ");
    rb_term_set_line_ex(&s_term, 1, flash_on ? RB_TERM_CYAN : RB_TERM_WHITE,
                        RB_TERM_BLACK, line);

    (void)snprintf(line, sizeof(line), "ui ticks %lu",
                   (unsigned long)ui_ticks);
    rb_term_set_line_ex(&s_term, 2, RB_TERM_WHITE, RB_TERM_BLACK, line);

    (void)snprintf(line, sizeof(line), "use BOOT btn");
    rb_term_set_line_ex(&s_term, 3, RB_TERM_CYAN, RB_TERM_BLACK, line);

    vTaskDelay(pdMS_TO_TICKS(UI_MS));
}
