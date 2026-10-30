#include "ex07_ui.h"

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

static ex07_pool_toggle_fn s_on_toggle;
static void *s_toggle_user;

static void on_button(rb_button_t *btn, rb_button_event_t ev, void *user)
{
    (void)user;
    if (ev != RB_BUTTON_EV_CLICK || s_on_toggle == NULL) {
        return;
    }
    s_on_toggle(rb_button_is_on(btn) ? 1 : 0, s_toggle_user);
}

int ex07_ui_begin(ex07_pool_toggle_fn on_toggle, void *user)
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
    rb_button_set_labels(&s_btn, "ON", "OFF");
    rb_button_set_style(&s_btn, RB_STYLE_SUCCESS);
    rb_button_set_indicator_size(&s_btn, 100, 2);
    rb_button_set_on(&s_btn, 0); /* boot OFF = oversubscribe */
    rb_button_set_on_event(&s_btn, on_button, NULL);
    (void)rb_layout_add(&s_layout, &s_btn.base);

    rb_screen_set_root(&s_screen, &s_layout.base);
    rb_screen_reflow(&s_screen);

    rb_term_set_padding(&s_term, 4, 4, 4, 4);
    (void)rb_term_apply_geometry(&s_term);
    return 0;
}

void ex07_ui_tick(int pool_on, int free_n, unsigned in_use,
                  const volatile uint8_t *state,
                  const volatile uint32_t *done)
{
    static const char *names[EX07_WORKER_COUNT] = {"W0", "W1", "W2", "W3"};
    char line[RB_TERM_LOG_MAX_COLS];
    bsp_touch_sample_t smp;
    uint16_t mode_fg;
    unsigned i;

    if (bsp_touch_read(&smp) == 0) {
        (void)rb_button_feed_touch(&s_btn, &smp);
    }

    mode_fg = pool_on ? RB_TERM_GREEN : RB_TERM_RED;
    if (free_n >= 0) {
        (void)snprintf(line, sizeof(line), "ex07 %s  free %d/%u  in %u",
                       pool_on ? "ON " : "OFF", free_n, (unsigned)EX07_SLOTS,
                       in_use);
    } else {
        (void)snprintf(line, sizeof(line), "ex07 %s  free --/%u  in %u",
                       pool_on ? "ON " : "OFF", (unsigned)EX07_SLOTS, in_use);
    }
    rb_term_set_line_ex(&s_term, 0, mode_fg, RB_TERM_BLACK, line);

    for (i = 0; i < EX07_WORKER_COUNT; i += 2u) {
        const char *st0 = (state[i] == EX07_ST_HOLD) ? "HOLD" : "WAIT";
        const char *st1 = (state[i + 1u] == EX07_ST_HOLD) ? "HOLD" : "WAIT";
        uint16_t fg = (state[i] == EX07_ST_HOLD || state[i + 1u] == EX07_ST_HOLD)
                          ? RB_TERM_YELLOW
                          : RB_TERM_CYAN;

        (void)snprintf(line, sizeof(line), "%s %s #%lu  %s %s #%lu", names[i],
                       st0, (unsigned long)done[i], names[i + 1u], st1,
                       (unsigned long)done[i + 1u]);
        rb_term_set_line_ex(&s_term, (int)(1u + i / 2u), fg, RB_TERM_BLACK,
                            line);
    }

    vTaskDelay(pdMS_TO_TICKS(UI_MS));
}
