#include "ex_imu_panel.h"

#include "rb_gauge.h"
#include "rb_imu_readout.h"
#include "rb_label.h"
#include "rb_screen.h"
#include "ui/gfx.h"

#include <stdio.h>

#define EX_IMU_TITLE_H 24u

static rb_display_t s_disp;
static rb_screen_t s_screen;
static rb_layout_t s_root; /* rows: title / body (heights patched after reflow) */
static rb_layout_t s_body; /* cols: |a| gauge | imu readout */
static rb_label_t s_title;
static rb_imu_readout_t s_readout;
static rb_gauge_t s_gauge;
static int s_ok;

/* Equal-weight ROWS would give the title half the glass — pin a thin strip. */
static void apply_hero_geometry(void)
{
    uint16_t w = rb_display_width(&s_disp);
    uint16_t h = rb_display_height(&s_disp);
    uint16_t th = EX_IMU_TITLE_H;

    if (h <= th + 8u) {
        th = (uint16_t)(h / 8u);
    }

    s_title.base.rect.x = 0;
    s_title.base.rect.y = 0;
    s_title.base.rect.w = w;
    s_title.base.rect.h = th;

    s_body.base.rect.x = 0;
    s_body.base.rect.y = (int16_t)th;
    s_body.base.rect.w = w;
    s_body.base.rect.h = (uint16_t)(h - th);

    (void)rb_label_apply_geometry(&s_title);
    rb_layout_reflow(&s_body);
}

int ex_imu_panel_begin(void)
{
    static const float zero3[3] = {0.0f, 0.0f, 0.0f};

    s_ok = 0;

    if (rb_display_open(&s_disp) != 0) {
        return -1;
    }
    rbgfx_fill(&s_disp.gfx, RBGFX_BLACK);
    rb_display_flush(&s_disp);

    if (rb_screen_init(&s_screen, &s_disp) != 0) {
        return -1;
    }
    if (rb_layout_init(&s_root, &s_screen, RB_LAYOUT_ROWS, 2, 1, 0) != 0) {
        return -1;
    }
    if (rb_layout_init(&s_body, &s_screen, RB_LAYOUT_COLS, 1, 2, 4) != 0) {
        return -1;
    }

    rb_label_init(&s_title);
    (void)rb_label_set_scale(&s_title, 1);
    rb_label_set_color(&s_title, RB_LABEL_CYAN, RB_LABEL_BLACK);
    rb_label_set_align(&s_title, RB_LABEL_ALIGN_LEFT);
    rb_label_set_text(&s_title, "ex03 IMU PIPELINE");

    rb_gauge_init(&s_gauge);
    rb_gauge_set_mode(&s_gauge, RB_GAUGE_MODE_DISPLAY);
    /*
     * Fill uses hundredths (0..2500). Built-in value text is integer-only in
     * the shipped gauge, so hide it and paint "%.2f" via unit (max 6 chars).
     */
    rb_gauge_set_range(&s_gauge, 0, 2500);
    rb_gauge_set_show_value(&s_gauge, 0);
    rb_gauge_set_unit(&s_gauge, "0.00");
    rb_gauge_set_label(&s_gauge, "m/s2");
    rb_gauge_set_bands(&s_gauge, 1500, 2000);
    rb_gauge_set_value(&s_gauge, 0);

    rb_imu_readout_init(&s_readout);

    /* Hero |a| on the left; supporting ax/ay/az on the right. */
    (void)rb_layout_add(&s_body, &s_gauge.base);
    (void)rb_layout_add(&s_body, &s_readout.base);
    (void)rb_layout_add(&s_root, &s_title.base);
    (void)rb_layout_add(&s_root, &s_body.base);

    rb_screen_set_root(&s_screen, &s_root.base);
    rb_screen_reflow(&s_screen);
    apply_hero_geometry();

    /*
     * SUCCESS = green fill. Do NOT use RB_STYLE_DARK — that paints the
     * value arc a charcoal gray (~33,37,41), which reads as a "gray gauge".
     */
    rb_gauge_set_style(&s_gauge, RB_STYLE_SUCCESS);
    rb_gauge_set_fill_rgb(&s_gauge, 0x2e, 0xd5, 0x73);   /* bright green */
    rb_gauge_set_track_rgb(&s_gauge, 0x14, 0x3d, 0x28);  /* dark green rail */
    rb_gauge_set_warn_rgb(&s_gauge, 0xff, 0xc1, 0x07);
    rb_gauge_set_danger_rgb(&s_gauge, 0xdc, 0x35, 0x45);

    rb_imu_readout_set(&s_readout, zero3, zero3, 0);
    rb_imu_readout_set_status(&s_readout, "boot");

    s_ok = 1;
    return 0;
}

void ex_imu_panel_set(const float acc[3], const float gyro[3], float mag_mps2,
                      int live, int c_sample, int c_compute)
{
    static const float zero3[3] = {0.0f, 0.0f, 0.0f};
    float mag;
    int32_t gauge_v;

    if (!s_ok || acc == NULL) {
        return;
    }

    mag = mag_mps2;
    if (mag < 0.0f) {
        mag = 0.0f;
    }
    if (mag > 25.0f) {
        mag = 25.0f;
    }
    gauge_v = (int32_t)(mag * 100.0f + 0.5f); /* e.g. 9.81 → 981 */

    rb_imu_readout_set(&s_readout, acc, gyro != NULL ? gyro : zero3,
                       live ? 1 : 0);
    rb_imu_readout_set_status(&s_readout, live ? NULL : "IMU sim");
    rb_gauge_set_value(&s_gauge, gauge_v);
    {
        char num[RB_GAUGE_UNIT_MAX];
        (void)snprintf(num, sizeof(num), "%.2f", (double)mag);
        rb_gauge_set_unit(&s_gauge, num); /* e.g. "9.81" */
    }

    if (c_sample >= 0 && c_compute >= 0) {
        rb_label_printf(&s_title, "ex03 IMU PIPELINE  c%c->c%c  %s",
                        c_sample == 0 ? '0' : '1',
                        c_compute == 0 ? '0' : '1', live ? "LIVE" : "SIM");
    } else {
        rb_label_printf(&s_title, "ex03 IMU PIPELINE  %s",
                        live ? "LIVE" : "SIM");
    }
}
