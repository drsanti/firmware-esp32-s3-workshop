/*
 * ports/esp32s3_audio.c — ES3C28P speaker path (bsp.h audio hooks).
 *
 * ES8311 codec (I2C 0x18 on shared bus SDA16/SCL15) -> I2S std TX
 * (MCLK4 BCLK5 DOUT8 WS7, MCLK=256fs=4.096MHz) -> FM8002E amp
 * (GPIO1, low-active) -> speaker header.
 *
 * 16 kHz / 16-bit stereo frames; mono samples duplicated L/R.
 * Register values follow the vendor demo's es8311.cpp init for the
 * 4.096 MHz / 16 kHz coefficient row.
 */
#ifdef ESP_PLATFORM

#include <string.h>

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/i2s_std.h"
#include "../../esp32s3_bsp.h"
#include "../../bsp.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ES8311_ADDR 0x18
#define PA_EN       GPIO_NUM_1 /* low-active */

#define I2S_MCLK GPIO_NUM_4
#define I2S_BCLK GPIO_NUM_5
#define I2S_DOUT GPIO_NUM_8
#define I2S_WS   GPIO_NUM_7
#define I2S_DIN  GPIO_NUM_6 /* codec ADC -> MCU (microphone) */

#define SFX_FS_HZ 16000

static i2c_master_dev_handle_t s_codec;
static i2s_chan_handle_t s_i2s_tx;
static i2s_chan_handle_t s_i2s_rx;
static uint8_t s_mic_on;

static int codec_write(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = {reg, val};
    return i2c_master_transmit(s_codec, buf, 2, 50) == ESP_OK ? 0 : -1;
}

static int codec_read(uint8_t reg, uint8_t *val) {
    return i2c_master_transmit_receive(s_codec, &reg, 1, val, 1, 50) ==
                   ESP_OK
               ? 0
               : -1;
}

/* minimal ES8311 bring-up: reset -> clocks -> i2s slave -> power.
 * coeff row: MCLK 4096000, fs 16000: pre_div 1, pre_multi 0,
 * adc_div 1, dac_div 1, fs_mode 0, lrck 0x00FF, bclk_div 4,
 * osr 0x10/0x10 (from the vendor es8311.cpp table). */
static int codec_init(void) {
    if (codec_write(0x00, 0x1F)) return -1; /* reset */
    vTaskDelay(pdMS_TO_TICKS(20));
    if (codec_write(0x00, 0x00)) return -1;
    if (codec_write(0x00, 0x80)) return -1; /* power on */

    uint8_t v;
    if (codec_write(0x01, 0x3F)) return -1; /* all clocks, MCLK pin */

    if (codec_read(0x06, &v)) return -1;
    v &= ~0x20; /* sclk not inverted */
    if (codec_write(0x06, v)) return -1;

    if (codec_read(0x02, &v)) return -1;
    v = (v & 0x07) | (0 << 5) | (0 << 3); /* pre_div 1, pre_multi 0 */
    if (codec_write(0x02, v)) return -1;
    if (codec_write(0x03, 0x10)) return -1; /* fs_mode 0, adc_osr 0x10 */
    if (codec_write(0x04, 0x10)) return -1; /* dac_osr 0x10 */
    if (codec_write(0x05, 0x00)) return -1; /* adc/dac div 1 */

    if (codec_read(0x06, &v)) return -1;
    v = (v & 0xE0) | 3; /* bclk_div 4 -> stored -1 */
    if (codec_write(0x06, v)) return -1;

    if (codec_read(0x07, &v)) return -1;
    v = (v & 0xC0) | 0x00; /* lrck_h */
    if (codec_write(0x07, v)) return -1;
    if (codec_write(0x08, 0xFF)) return -1; /* lrck_l */

    if (codec_read(0x00, &v)) return -1;
    v &= 0xBF; /* codec = I2S slave */
    if (codec_write(0x00, v)) return -1;
    if (codec_write(0x09, 0x0C)) return -1; /* SDP in 16-bit i2s */
    if (codec_write(0x0A, 0x0C)) return -1; /* SDP out 16-bit i2s */

    if (codec_write(0x0D, 0x01)) return -1; /* analog power up */
    if (codec_write(0x0E, 0x02)) return -1; /* PGA + ADC modulator */
    if (codec_write(0x12, 0x00)) return -1; /* DAC power up */
    if (codec_write(0x13, 0x10)) return -1; /* HP drive enable */
    if (codec_write(0x14, 0x1A)) return -1; /* analog mic, PGA 30dB */
    if (codec_write(0x17, 0xBF)) return -1; /* ADC digital vol 0dB */
    if (codec_write(0x1B, 0x0C)) return -1; /* ADC HPF-S1 on (DC block) */
    if (codec_write(0x1C, 0x6A)) return -1; /* ADC eq bypass */
    if (codec_write(0x37, 0x08)) return -1; /* DAC eq bypass */
    codec_write(0x32, 0);
    bsp_audio_volume(60); /* taper-mapped; boot at -24 dB */
    return 0;
}

static int i2s_init(void) {
    i2s_chan_config_t ch = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0,
                                                    I2S_ROLE_MASTER);
    if (i2s_new_channel(&ch, &s_i2s_tx, &s_i2s_rx) != ESP_OK) {
        return -1;
    }
    i2s_std_config_t std = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SFX_FS_HZ),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
            I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_MCLK,
            .bclk = I2S_BCLK,
            .ws = I2S_WS,
            .dout = I2S_DOUT,
            .din = I2S_DIN,
            .invert_flags = {.mclk_inv = 0, .bclk_inv = 0,
                             .ws_inv = 0},
        },
    };
    std.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256; /* 4.096 MHz */
    if (i2s_channel_init_std_mode(s_i2s_tx, &std) != ESP_OK ||
        i2s_channel_init_std_mode(s_i2s_rx, &std) != ESP_OK) {
        return -1;
    }
    if (i2s_channel_enable(s_i2s_tx) != ESP_OK) {
        return -1;
    }
    /* RX stays disabled until bsp_mic_enable(1) — the ADC path is
     * powered by codec_init but capture is on-demand. */
    return 0;
}

int bsp_audio_init(void) {
    if (esp32s3_i2c0() == NULL) {
        return -1;
    }
    i2c_device_config_t dev = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ES8311_ADDR,
        .scl_speed_hz = 100000,
    };
    if (i2c_master_bus_add_device(esp32s3_i2c0(), &dev, &s_codec) !=
        ESP_OK) {
        return -1;
    }
    if (codec_init() != 0) {
        return -1;
    }
    if (i2s_init() != 0) {
        return -1;
    }
    gpio_reset_pin(PA_EN);
    gpio_set_direction(PA_EN, GPIO_MODE_OUTPUT);
    gpio_set_level(PA_EN, 0); /* amp on (low-active) */
    return 0;
}

/* stereo frames: pcm[] is L,R interleaved int16; blocks until the
 * DMA buffer drains — natural pacing for the render task */
size_t bsp_audio_write(const int16_t *pcm, size_t frames) {
    if (s_i2s_tx == NULL) {
        return 0;
    }
    size_t written = 0;
    i2s_channel_write(s_i2s_tx, pcm, frames * 4, &written,
                      portMAX_DELAY);
    return written / 4;
}

/* DAC volume reg 0x32 is 0.5 dB/LSB (0 = -95.5 dB mute, 255 = 0 dB):
 * a linear percent map parks the bottom ~45% of the range below
 * -70 dB — inaudible. Taper to a -40 dB floor instead: 1% -> reg 176
 * (-39.5 dB, soft but present), 50% -> -20 dB, 100% -> 0 dB. */
void bsp_audio_volume(uint8_t percent) {
    if (percent > 100) percent = 100;
    if (s_codec) {
        codec_write(0x32, percent ? (uint8_t)(175 + percent * 80 / 100)
                                  : 0);
    }
}

/* ---- mic input (ES8311 ADC -> I2S DIN) -------------------------- */

int bsp_mic_enable(int on) {
    if (s_i2s_rx == NULL) {
        return -1;
    }
    bsp_critical_enter();   /* check-set: app + UI tasks both call */
    if (s_mic_on == (uint8_t)(on != 0)) {
        bsp_critical_exit();
        return 0; /* idempotent — disable on a stopped channel errs */
    }
    bsp_critical_exit();
    /* i2s_channel_* take the channel lock and can block — they must
     * run outside the spinlock critical section */
    esp_err_t e = on ? i2s_channel_enable(s_i2s_rx)
                     : i2s_channel_disable(s_i2s_rx);
    if (e == ESP_OK) {
        s_mic_on = on ? 1 : 0;
    }
    return e == ESP_OK ? 0 : -1;
}

/* stereo frames: pcm[] is L,R interleaved int16 — mic is mono so L
 * and R carry the same sample (codec duplicates on the wire). */
size_t bsp_audio_read(int16_t *pcm, size_t frames) {
    if (!s_mic_on || s_i2s_rx == NULL) {
        return 0;
    }
    size_t got = 0;
    /* short timeout — the caller runs on a periodic task and must
     * never stall; a slow read is a dropped block, not a hang */
    if (i2s_channel_read(s_i2s_rx, pcm, frames * 4, &got,
                         pdMS_TO_TICKS(4)) != ESP_OK) {
        return 0;
    }
    return got / 4;
}

/* percent 0..100 -> ES8311 ADC gain scale (0x16, 3-bit 0..7 =
 * 0..+42dB in +6dB steps). 0% maps to scale 0, 100% to 7 — writing
 * 8 would set bit3 (outside the field) and read back as scale 0. */
void bsp_mic_gain(uint8_t percent) {
    if (percent > 100) percent = 100;
    if (s_codec) {
        codec_write(0x16, (uint8_t)(percent * 7 / 100));
    }
}

/* ADC digital volume 0x17: 0 = mute, 0xBF = 0 dB. */
void bsp_mic_mute(int on) {
    if (s_codec) {
        codec_write(0x17, on ? 0x00 : 0xBF);
    }
}

#endif /* ESP_PLATFORM */
