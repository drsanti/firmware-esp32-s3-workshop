/*
 * esp32s3_battery.c — LiPo voltage sense on ADC1_CH8 (GPIO9) for the
 * ES3C28P board (TP4054 charger, 3.7 V pack). ADC is behind a ~1/2
 * divider so the pin sees <= ~2.1 V; read with DB_12 attenuation and
 * eFuse calibration when available.
 */
#ifdef ESP_PLATFORM

#include "../../bsp.h"
#include "bsp_pins.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define BAT_ADC_CH ADC_CHANNEL_8 /* BOARD_BATT_GPIO = GPIO9 */
#define BAT_DIVIDER_NUM 2        /* pack = pin x2 (per board docs)   */

static adc_oneshot_unit_handle_t s_adc;
static adc_cali_handle_t s_cali;
static int s_ready;

int bsp_battery_mv(void) {
    if (!s_ready) {
        adc_oneshot_unit_init_cfg_t uc = {
            .unit_id = ADC_UNIT_1,
        };
        if (adc_oneshot_new_unit(&uc, &s_adc) != ESP_OK) {
            return -1;
        }
        adc_oneshot_chan_cfg_t cc = {
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        if (adc_oneshot_config_channel(s_adc, BAT_ADC_CH, &cc) !=
            ESP_OK) {
            adc_oneshot_del_unit(s_adc); /* else every call re-news */
            s_adc = NULL;
            return -1;
        }
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
        adc_cali_curve_fitting_config_t cal = {
            .unit_id = ADC_UNIT_1, .chan = BAT_ADC_CH,
            .atten = ADC_ATTEN_DB_12, .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        if (adc_cali_create_scheme_curve_fitting(&cal, &s_cali) == ESP_OK) {
            s_ready = 2; /* calibrated */
        } else
#endif
        {
            s_ready = 1; /* raw fallback */
        }
    }
    int raw = 0;
    if (adc_oneshot_read(s_adc, BAT_ADC_CH, &raw) != ESP_OK) {
        return -1;
    }
    int pin_mv;
    if (s_ready == 2 &&
        adc_cali_raw_to_voltage(s_cali, raw, &pin_mv) == ESP_OK) {
        return pin_mv * BAT_DIVIDER_NUM;
    }
    /* crude fallback: 3.3 V full-scale @ 12-bit */
    return raw * 3300 / 4095 * BAT_DIVIDER_NUM;
}

#endif /* ESP_PLATFORM */
