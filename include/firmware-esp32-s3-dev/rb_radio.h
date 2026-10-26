/*
 * rb_radio.h — flat radio leaf (circle + optional label) + exclusive group.
 *
 *   rb_radio_group_init(&grp);
 *   rb_radio_init(&r);
 *   rb_radio_set_label(&r, "A");
 *   rb_radio_group_add(&grp, &r);
 *   rb_radio_group_set_on_change(&grp, on_change, user);
 *   rb_radio_group_select(&grp, &r);   // programmatic select
 *   // poll: bsp_touch_read(&smp); rb_radio_feed_touch(&r, &smp);
 */
#ifndef ROBOTOS_RB_RADIO_H
#define ROBOTOS_RB_RADIO_H

#include <stdint.h>

#include "bsp.h"
#include "rb_screen.h"
#include "rb_style.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RB_RADIO_LABEL_MAX 16
#define RB_RADIO_GROUP_MAX 8

typedef struct rb_radio rb_radio_t;
typedef struct rb_radio_group rb_radio_group_t;

/* Fires from feed_touch / group_select when selection changes
 * (not when re-selecting the already-on item). */
typedef void (*rb_radio_on_change_fn)(rb_radio_group_t *grp, rb_radio_t *sel,
                                      void *user);

struct rb_radio {
    rb_widget_t base; /* must be first */
    rb_radio_group_t *group;
    uint8_t on;
    uint8_t pressed;
    uint8_t ok;
    uint8_t ring_r, ring_g, ring_b; /* outer ring / off */
    uint8_t fill_r, fill_g, fill_b; /* selected fill */
    uint8_t dot_r, dot_g, dot_b;    /* inner dot */
    uint8_t tx_r, tx_g, tx_b;       /* label */
    char label[RB_RADIO_LABEL_MAX];
};

struct rb_radio_group {
    rb_radio_t *items[RB_RADIO_GROUP_MAX];
    unsigned count;
    int selected; /* index into items, or -1 */
    rb_radio_on_change_fn on_change;
    void *user;
};

void rb_radio_init(rb_radio_t *r);
int  rb_radio_apply_geometry(rb_radio_t *r);

void rb_radio_set_label(rb_radio_t *r, const char *text); /* NULL/"" */
void rb_radio_set_style(rb_radio_t *r, rb_style_t style);
void rb_radio_set_ring_rgb(rb_radio_t *r, uint8_t rr, uint8_t g, uint8_t b);
void rb_radio_set_fill_rgb(rb_radio_t *r, uint8_t rr, uint8_t g, uint8_t b);
void rb_radio_set_dot_rgb(rb_radio_t *r, uint8_t rr, uint8_t g, uint8_t b);
void rb_radio_set_text_rgb(rb_radio_t *r, uint8_t rr, uint8_t g, uint8_t b);
int  rb_radio_is_on(const rb_radio_t *r);
void rb_radio_redraw(rb_radio_t *r);

int rb_radio_contains(const rb_radio_t *r, int16_t x, int16_t y);
/* Rising-edge press selects this radio in its group. Returns 1 if
 * selection changed. */
int rb_radio_feed_touch(rb_radio_t *r, const bsp_touch_sample_t *s);

void rb_radio_group_init(rb_radio_group_t *grp);
int  rb_radio_group_add(rb_radio_group_t *grp, rb_radio_t *r);
/* Select by pointer (NULL = clear). Fires on_change if selection moves.
 * Returns 0 ok, -1 bad args. */
int  rb_radio_group_select(rb_radio_group_t *grp, rb_radio_t *r);
int  rb_radio_group_selected_index(const rb_radio_group_t *grp);
rb_radio_t *rb_radio_group_selected(const rb_radio_group_t *grp);
/* fn may be NULL to clear. Invoked outside the display lock. */
void rb_radio_group_set_on_change(rb_radio_group_t *grp,
                                  rb_radio_on_change_fn fn, void *user);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_RB_RADIO_H */
