/*
 * ports/boards/ws28p/pcm5101.c — Waveshare ESP32-S3-Touch-LCD-2.8
 * (SKU 27690) audio output (bsp_audio_* impl).
 *
 * PCM5101A is a dumb I2S DAC — no control interface, no ADC, no mic
 * (unlike the ES3C28P's ES8311 codec). I2S pins: BCK48 LRCK38 DIN47,
 * no MCLK (the PCM5101 has an internal PLL off BCK). 8Ω 2W speaker
 * via the onboard amp.
 *
 * Volume: the DAC has no register surface, so bsp_audio_volume()
 * applies a software gain on the samples in bsp_audio_write() —
 * cheap and keeps the SFX loudness knob working on this board.
 * bsp_mic_* are graceful stubs — mic_apply_cmd sees -1 and marks
 * capture unsupported instead of a hard failure.
 */
#ifdef ESP_PLATFORM

#include "../../bsp.h"
#include "../../esp32s3_bsp.h"
#include "bsp_pins.h"

#include <string.h>

#include "driver/gpio.h"
#include "driver/i2s_std.h"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"

#define I2S_BCK  GPIO_NUM_48
#define I2S_WS   GPIO_NUM_38 /* LRCK */
#define I2S_DOUT GPIO_NUM_47

#define SFX_FS_HZ 16000

static i2s_chan_handle_t s_i2s_tx;
static volatile uint8_t s_vol = 100; /* software gain, 0..100 */

static int i2s_init(void) {
    i2s_chan_config_t ch = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0,
                                                    I2S_ROLE_MASTER);
    if (i2s_new_channel(&ch, &s_i2s_tx, NULL) != ESP_OK) {
        return -1;
    }
    i2s_std_config_t std = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SFX_FS_HZ),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
            I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = GPIO_NUM_NC, /* PCM5101 derives its clocks from BCK */
            .bclk = I2S_BCK,
            .ws = I2S_WS,
            .dout = I2S_DOUT,
            .din = GPIO_NUM_NC,
            .invert_flags = {0},
        },
    };
    if (i2s_channel_init_std_mode(s_i2s_tx, &std) != ESP_OK) {
        return -1;
    }
    return i2s_channel_enable(s_i2s_tx) == ESP_OK ? 0 : -1;
}

int bsp_audio_init(void) {
    if (s_i2s_tx != NULL) {
        return 0;
    }
    return i2s_init();
}

/* stereo frames: pcm[] is L,R interleaved int16; blocks until the
 * DMA buffer drains — natural pacing for the render task. Software
 * gain applied in place when s_vol < 100. */
size_t bsp_audio_write(const int16_t *pcm, size_t frames) {
    if (s_i2s_tx == NULL) {
        return 0;
    }
    size_t written = 0;
    if (s_vol >= 100) {
        i2s_channel_write(s_i2s_tx, pcm, frames * 4, &written,
                          portMAX_DELAY);
    } else {
        int16_t buf[128]; /* chunk-wise: cap the scratch buffer */
        size_t done = 0;
        while (done < frames) {
            size_t n = frames - done;
            if (n > 64) n = 64; /* 64 stereo frames per chunk */
            for (size_t i = 0; i < n * 2; i++) {
                buf[i] = (int16_t)((int32_t)pcm[done * 2 + i] *
                                   s_vol / 100);
            }
            size_t w = 0;
            i2s_channel_write(s_i2s_tx, buf, n * 4, &w, portMAX_DELAY);
            done += w / 4;
            if (w == 0) break;
        }
        return done;
    }
    return written / 4;
}

void bsp_audio_volume(uint8_t percent) {
    if (percent > 100) percent = 100;
    s_vol = percent; /* software gain — PCM5101 has no ctrl iface */
}

/* ---- mic input: no ADC on this board ---------------------------- */

int bsp_mic_enable(int on) {
    (void)on;
    return -1; /* PCM5101 is output-only — no capture path */
}

size_t bsp_audio_read(int16_t *pcm, size_t frames) {
    (void)pcm; (void)frames;
    return 0;
}

void bsp_mic_gain(uint8_t percent) { (void)percent; }
void bsp_mic_mute(int on) { (void)on; }

#endif /* ESP_PLATFORM */
