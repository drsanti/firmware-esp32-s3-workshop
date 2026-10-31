/*
 * Demo 17 — mic-viz
 *
 * WHAT THIS CODE DOES
 *   rb_vu dBFS + mic HISTORY (mic_sim if no mic)
 *
 * HOW IT WORKS
 *   main/main.c calls ex17_start() after bsp_init().
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
#include "ex17.h"

#include "bsp.h"
#include "mic_sim.h"
#include "rb_chart.h"
#include "rb_label.h"
#include "rb_screen.h"
#include "rb_slider.h"
#include "rb_style.h"
#include "rb_vu.h"

#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define FS_HZ 16000
#define BLOCK_FRAMES 64
#define BLOCKS_PER_TICK 1
#define COLS_PER_TICK 16
/* Soft post-gain after PGA. Keep small — slider owns most of the range. */
#define DIG_GAIN 4
#define GAIN_DEFAULT 45
#define LAYOUT_GAP 4
#define READOUT_H 20
#define GAIN_W_MIN 52
#define GAIN_W_MAX 72
#define DBFS_FLOOR (-60)

static rb_display_t g_disp;
static rb_vu_t g_vu;
static rb_slider_t g_gain;
static rb_chart_t g_wave;
static rb_label_t g_lbl_dbfs;
static rb_label_t g_lbl_pcm;
static char g_prev_dbfs[RB_LABEL_TEXT_MAX];
static char g_prev_pcm[RB_LABEL_TEXT_MAX];

static int g_have_mic;
static int g_gain_pct = GAIN_DEFAULT;
static int g_last_clip;
/* Display AGC for the strip: map recent peak → ~0.8 of ±1 plot range.
 * Without this, speech at PCM ~1k is ~3% FS and looks like a flat line. */
static float g_plot_env = 1500.0f;

static void mono_from_stereo(const int16_t *stereo, int frames, int16_t *mono)
{
    int i;

    for (i = 0; i < frames; i++) {
        int16_t l = stereo[2 * i];
        int16_t r = stereo[2 * i + 1];
        int al = l < 0 ? -l : l;
        int ar = r < 0 ? -r : r;
        mono[i] = (al >= ar) ? l : r;
    }
}

static int raw_peak_of(const int16_t *mono, int n)
{
    int peak = 0;
    int i;

    for (i = 0; i < n; i++) {
        int a = mono[i] < 0 ? -mono[i] : mono[i];
        if (a > peak) {
            peak = a;
        }
    }
    return peak;
}

static int raw_rms_of(const int16_t *mono, int n)
{
    int64_t acc = 0;
    int i;
    int rms;

    if (n < 1) {
        return 0;
    }
    for (i = 0; i < n; i++) {
        int32_t s = mono[i];
        acc += (int64_t)s * (int64_t)s;
    }
    rms = (int)sqrtf((float)acc / (float)n);
    if (rms > 32767) {
        rms = 32767;
    }
    return rms;
}

static void apply_dig_gain(int16_t *mono, int n)
{
    int i;

    for (i = 0; i < n; i++) {
        int32_t v = (int32_t)mono[i] * DIG_GAIN;
        if (v > 32767) {
            v = 32767;
        }
        if (v < -32768) {
            v = -32768;
        }
        mono[i] = (int16_t)v;
    }
}

static int peak_to_dbfs(int peak)
{
    float db;

    if (peak < 1) {
        return DBFS_FLOOR;
    }
    db = 20.0f * log10f((float)peak / 32767.0f);
    if (db < (float)DBFS_FLOOR) {
        return DBFS_FLOOR;
    }
    if (db > 0.0f) {
        return 0;
    }
    return (int)(db + (db >= 0.0f ? 0.5f : -0.5f));
}

static void update_plot_env(int block_peak)
{
    float pk = (float)block_peak;

    if (pk > g_plot_env) {
        g_plot_env = g_plot_env * 0.6f + pk * 0.4f; /* fast rise */
    } else {
        g_plot_env = g_plot_env * 0.97f + pk * 0.03f; /* slow fall */
    }
    if (g_plot_env < 800.0f) {
        g_plot_env = 800.0f; /* avoid noise blow-up */
    }
    if (g_plot_env > 32767.0f) {
        g_plot_env = 32767.0f;
    }
}

static void push_waveform(const int16_t *mono, int n)
{
    int cols = COLS_PER_TICK;
    int c;
    int i;
    int block_pk;

    if (n < 1) {
        return;
    }
    if (cols > n) {
        cols = n;
    }

    block_pk = raw_peak_of(mono, n);
    update_plot_env(block_pk);

    for (c = 0; c < cols; c++) {
        int i0 = (c * n) / cols;
        int i1 = ((c + 1) * n) / cols;
        int pk = 0;
        int signed_pk = 0;
        float y;

        if (i1 <= i0) {
            i1 = i0 + 1;
        }
        for (i = i0; i < i1 && i < n; i++) {
            int a = mono[i] < 0 ? -mono[i] : mono[i];
            if (a > pk) {
                pk = a;
                signed_pk = mono[i];
            }
        }
        /* Scale to recent envelope so normal speech fills the pane. */
        y = (float)signed_pk / g_plot_env;
        if (y > 1.0f) {
            y = 1.0f;
        }
        if (y < -1.0f) {
            y = -1.0f;
        }
        rb_chart_push_frame(&g_wave, &y, 1);
    }
}

static void push_silence_cols(int cols)
{
    int c;
    float z = 0.0f;

    for (c = 0; c < cols; c++) {
        rb_chart_push_frame(&g_wave, &z, 1);
    }
}

static void set_label_if_changed(rb_label_t *lab, char *prev, size_t prev_sz,
                                 const char *text)
{
    if (strncmp(text, prev, prev_sz) == 0) {
        return;
    }
    (void)snprintf(prev, prev_sz, "%s", text);
    rb_label_set_text(lab, text);
}

static void refresh_readouts(int dbfs, int pcm)
{
    char buf[RB_LABEL_TEXT_MAX];

    (void)snprintf(buf, sizeof(buf), "%d dBFS", dbfs);
    set_label_if_changed(&g_lbl_dbfs, g_prev_dbfs, sizeof(g_prev_dbfs), buf);

    (void)snprintf(buf, sizeof(buf), "PCM %d", pcm);
    set_label_if_changed(&g_lbl_pcm, g_prev_pcm, sizeof(g_prev_pcm), buf);
}

static void on_gain(rb_slider_t *s, int32_t value, void *user)
{
    (void)user;
    g_gain_pct = (int)value;
    if (g_have_mic) {
        bsp_mic_gain((uint8_t)g_gain_pct);
    }
}

static int capture_block(int real, int16_t *stereo, int16_t *mono, float *t,
                         uint32_t *rng)
{
    size_t got;

    if (real) {
        got = bsp_audio_read(stereo, BLOCK_FRAMES);
        if (got == 0) {
            return 0;
        }
        mono_from_stereo(stereo, (int)got, mono);
        return (int)got;
    }
    mic_sim_fill(mono, BLOCK_FRAMES, *t, rng);
    *t += (float)BLOCK_FRAMES / MIC_SIM_FS_HZ;
    return BLOCK_FRAMES;
}

static void pace_after_capture(int real, int n)
{
    uint32_t ms;

    if (real || n < 1) {
        return;
    }
    ms = (uint32_t)((n * 1000 + FS_HZ / 2) / FS_HZ);
    if (ms < 1u) {
        ms = 1u;
    }
    vTaskDelay(pdMS_TO_TICKS(ms));
}

/* Wireframe: left stack ~80%, narrow full-height gain (layout has no weights). */
static void place_dashboard(void)
{
    uint16_t W = g_disp.gfx.width;
    uint16_t H = g_disp.gfx.height;
    uint16_t gap = LAYOUT_GAP;
    uint16_t gain_w = (uint16_t)(W / 5);
    uint16_t left_w;
    uint16_t vu_h;
    uint16_t plot_h;
    uint16_t half;
    int16_t y;

    if (gain_w < GAIN_W_MIN) {
        gain_w = GAIN_W_MIN;
    }
    if (gain_w > GAIN_W_MAX) {
        gain_w = GAIN_W_MAX;
    }
    if (gain_w + gap >= W) {
        gain_w = (uint16_t)(W / 4);
    }
    left_w = (uint16_t)(W - gap - gain_w);

    /* VU + readout share the top half; plot takes the bottom half. */
    half = (uint16_t)((H - gap) / 2);
    if (half > READOUT_H + gap + 40) {
        vu_h = (uint16_t)(half - READOUT_H - gap);
    } else {
        vu_h = (uint16_t)(half * 3 / 4);
    }
    plot_h = (uint16_t)(H - vu_h - READOUT_H - 2 * gap);

    g_vu.base.disp = &g_disp;
    g_vu.base.rect.x = 0;
    g_vu.base.rect.y = 0;
    g_vu.base.rect.w = left_w;
    g_vu.base.rect.h = vu_h;
    (void)rb_vu_apply_geometry(&g_vu);

    y = (int16_t)(vu_h + gap);
    g_lbl_dbfs.base.disp = &g_disp;
    g_lbl_dbfs.base.rect.x = 0;
    g_lbl_dbfs.base.rect.y = y;
    g_lbl_dbfs.base.rect.w = (uint16_t)(left_w / 2);
    g_lbl_dbfs.base.rect.h = READOUT_H;
    (void)rb_label_apply_geometry(&g_lbl_dbfs);

    g_lbl_pcm.base.disp = &g_disp;
    g_lbl_pcm.base.rect.x = (int16_t)(left_w / 2);
    g_lbl_pcm.base.rect.y = y;
    g_lbl_pcm.base.rect.w = (uint16_t)(left_w - left_w / 2);
    g_lbl_pcm.base.rect.h = READOUT_H;
    (void)rb_label_apply_geometry(&g_lbl_pcm);

    y = (int16_t)(vu_h + gap + READOUT_H + gap);
    g_wave.base.disp = &g_disp;
    g_wave.base.rect.x = 0;
    g_wave.base.rect.y = y;
    g_wave.base.rect.w = left_w;
    g_wave.base.rect.h = plot_h;
    (void)rb_chart_apply_geometry(&g_wave);

    g_gain.base.disp = &g_disp;
    g_gain.base.rect.x = (int16_t)(left_w + gap);
    g_gain.base.rect.y = 0;
    g_gain.base.rect.w = gain_w;
    g_gain.base.rect.h = H;
    (void)rb_slider_apply_geometry(&g_gain);
}

static void touch_task(void *arg)
{
    (void)arg;
    for (;;) {
        bsp_touch_sample_t smp;
        if (bsp_touch_read(&smp) == 0) {
            (void)rb_slider_feed_touch(&g_gain, &smp);
        }
        vTaskDelay(pdMS_TO_TICKS(16));
    }
}

static void feed_task(void *arg)
{
    int16_t stereo[BLOCK_FRAMES * 2];
    int16_t mono[BLOCK_FRAMES * BLOCKS_PER_TICK];
    int was_real = -1;
    float t = 0.0f;
    uint32_t rng = 1u;
    int n;
    int b;
    int got;
    int pk;
    int rms;
    int dbfs;

    (void)arg;

    g_have_mic = (bsp_caps() & BOARD_CAP_MIC) != 0;
    if (g_have_mic) {
        if (bsp_audio_init() != 0 || bsp_mic_enable(1) != 0) {
            g_have_mic = 0;
        } else {
            bsp_mic_gain((uint8_t)g_gain_pct);
            bsp_mic_mute(0);
        }
    }

    for (;;) {
        n = 0;
        for (b = 0; b < BLOCKS_PER_TICK; b++) {
            got = capture_block(g_have_mic, stereo, mono + n, &t, &rng);
            if (got <= 0) {
                break;
            }
            n += got;
        }
        if (n <= 0) {
            rb_vu_set_level(&g_vu, 0, 0);
            rb_vu_tick(&g_vu);
            refresh_readouts(DBFS_FLOOR, 0);
            push_silence_cols(COLS_PER_TICK);
            vTaskDelay(pdMS_TO_TICKS(4));
            continue;
        }

        if (g_have_mic != was_real) {
            was_real = g_have_mic;
            rb_chart_set_title(&g_wave,
                               g_have_mic ? "PLOT" : "PLOT (sim)");
            rb_vu_set_label(&g_vu, g_have_mic ? "VU" : "VU sim");
        }

        apply_dig_gain(mono, n);
        pk = raw_peak_of(mono, n);
        rms = raw_rms_of(mono, n);
        dbfs = peak_to_dbfs(pk);

        rb_vu_set_level(&g_vu, (uint16_t)rms, (uint16_t)pk);
        {
            int clip = pk >= 30000 ? 1 : 0;
            if (clip != g_last_clip) {
                g_last_clip = clip;
                rb_vu_set_clipped(&g_vu, clip);
            }
        }
        refresh_readouts(dbfs, pk);
        rb_vu_tick(&g_vu);
        push_waveform(mono, n);
        pace_after_capture(g_have_mic, n);
    }
}

void ex17_start(void)
{
    (void)rb_display_open(&g_disp);

    rb_vu_init(&g_vu);
    rb_vu_set_scale(&g_vu, RB_VU_SCALE_DBFS);
    rb_vu_set_db_range(&g_vu, DBFS_FLOOR);
    rb_vu_set_segments(&g_vu, 20);
    rb_vu_set_zones(&g_vu, 12, 16);
    rb_vu_set_label(&g_vu, "VU");
    rb_vu_set_show_value(&g_vu, 1);
    rb_vu_set_show_peak(&g_vu, 1);
    rb_vu_set_hold_ms(&g_vu, 80);
    rb_vu_set_decay_ms(&g_vu, 25);
    rb_vu_set_thickness_px(&g_vu, 22);

    rb_label_init(&g_lbl_dbfs);
    rb_label_init(&g_lbl_pcm);
    rb_label_set_align(&g_lbl_dbfs, RB_LABEL_ALIGN_LEFT);
    rb_label_set_align(&g_lbl_pcm, RB_LABEL_ALIGN_RIGHT);
    (void)rb_label_set_scale(&g_lbl_dbfs, 1);
    (void)rb_label_set_scale(&g_lbl_pcm, 1);
    rb_label_set_fg_rgb(&g_lbl_dbfs, 200, 200, 210);
    rb_label_set_fg_rgb(&g_lbl_pcm, 200, 200, 210);
    rb_label_set_bg_rgb(&g_lbl_dbfs, 0, 0, 0);
    rb_label_set_bg_rgb(&g_lbl_pcm, 0, 0, 0);
    rb_label_set_padding(&g_lbl_dbfs, 4, 0, 2, 0);
    rb_label_set_padding(&g_lbl_pcm, 2, 0, 4, 0);
    g_prev_dbfs[0] = g_prev_pcm[0] = '\0';
    rb_label_set_text(&g_lbl_dbfs, "-60 dBFS");
    rb_label_set_text(&g_lbl_pcm, "PCM 0");

    rb_slider_init(&g_gain);
    rb_slider_set_orient(&g_gain, RB_SLIDER_ORIENT_VERTICAL);
    rb_slider_set_mode(&g_gain, RB_SLIDER_MODE_INPUT);
    rb_slider_set_style(&g_gain, RB_STYLE_PRIMARY);
    rb_slider_set_range(&g_gain, 0, 100);
    rb_slider_set_value(&g_gain, GAIN_DEFAULT);
    rb_slider_set_label(&g_gain, "GAIN");
    rb_slider_set_show_value(&g_gain, 1);
    rb_slider_set_on_change(&g_gain, on_gain, 0);

    rb_chart_init(&g_wave);
    rb_chart_set_channels(&g_wave, 1);
    rb_chart_set_mode(&g_wave, RB_CHART_MODE_SCROLL);
    rb_chart_set_title(&g_wave, "PLOT");
    rb_chart_set_range(&g_wave, -1.0f, 1.0f);
    rb_chart_set_show_grid(&g_wave, 1);
    rb_chart_set_show_zero(&g_wave, 1);
    rb_chart_set_channel_label(&g_wave, 0, "pcm");

    place_dashboard();

    (void)xTaskCreate(feed_task, "mic", 6144, 0, 5, 0);
    (void)xTaskCreate(touch_task, "touch", 4096, 0, 5, 0);

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
