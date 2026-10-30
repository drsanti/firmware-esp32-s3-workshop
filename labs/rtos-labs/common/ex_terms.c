#include "ex_terms.h"

#include "rb_screen.h"

#include <stdarg.h>
#include <stdio.h>

static rb_display_t s_disp;
static rb_screen_t s_screen;
static rb_layout_t s_layout;
static rb_term_t s_terms[EX_TERMS_MAX];
static unsigned s_count;
static int s_ok;

int ex_terms_begin(unsigned rows, unsigned cols)
{
    unsigned n;
    unsigned i;

    if (rows < 1u || cols < 1u) {
        return -1;
    }
    n = rows * cols;
    if (n > (unsigned)EX_TERMS_MAX) {
        return -1;
    }

    s_ok = 0;
    s_count = 0;

    if (rb_display_open(&s_disp) != 0) {
        return -1;
    }
    if (rb_screen_init(&s_screen, &s_disp) != 0) {
        return -1;
    }
    if (rb_layout_init(&s_layout, &s_screen, RB_LAYOUT_GRID, rows, cols, 2) !=
        0) {
        return -1;
    }

    for (i = 0; i < n; i++) {
        rb_term_init(&s_terms[i]);
        (void)rb_term_set_scale(&s_terms[i], 1);
        (void)rb_layout_add(&s_layout, &s_terms[i].base);
    }

    rb_screen_set_root(&s_screen, &s_layout.base);
    rb_screen_reflow(&s_screen);

    s_count = n;
    s_ok = 1;
    return 0;
}

unsigned ex_terms_count(void)
{
    return s_ok ? s_count : 0u;
}

rb_term_t *ex_terms_get(unsigned index)
{
    if (!s_ok || index >= s_count) {
        return NULL;
    }
    return &s_terms[index];
}

void ex_terms_log(rb_term_t *t, uint16_t fg, const char *fmt, ...)
{
    char buf[RB_TERM_LOG_MAX_COLS];
    va_list ap;

    if (t == NULL || fmt == NULL) {
        return;
    }
    va_start(ap, fmt);
    (void)vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    rb_term_log_push_ex(t, fg, RB_TERM_BLACK, buf);
}
