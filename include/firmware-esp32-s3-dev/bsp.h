/*
 * bsp.h — the ONLY contract between the portable serial-bridge
 * application code and a specific MCU/toolchain.
 *
 * Implement per board (see ports/). Which functions are required
 * depends on the modules linked — serial-only needs the 5-fn core
 * (uart_write/uart_read/mono_ns/critical_enter/critical_exit, +
 * bsp_yield on RTOS); params adds bsp_param_*; zlink adds the
 * bsp_wifi_* group; sound/mic adds bsp_mic_* and bsp_audio_read.
 * Reference ports:
 *   ports/esp_idf.c        — ESP-IDF (ESP32-S3 etc.), uart driver +
 *                            esp_timer
 *   ports/psoc_mtb.c       — PSoC (ModusToolbox), SCB UART + HAL
 *   ports/bare_metal.c     — template for NXP/Microchip/Zephyr/etc.
 *
 * Rules that come from the spec (docs/embedded/serial-bridge-spec.md):
 *   - The link is a hardware UART (8N1). USB CDC is out of scope —
 *     not every target MCU has a USB peripheral. On USB-capable
 *     parts (e.g. ESP32-S3), keep USB for the debug console and put
 *     protocol on a real UART.
 *   - bsp_uart_write must be atomic relative to itself and to any
 *     debug output sharing the UART (use a mutex/critical section if
 *     more than one task/context can emit bytes). Text landing inside
 *     a frame corrupts it.
 *   - The protocol UART must not echo RX bytes.
 *   - bsp_mono_ns is a monotonic clock — never wall time. Wrap is
 *     acceptable; nanosecond units.
 */
#ifndef ROBOTOS_BSP_H
#define ROBOTOS_BSP_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* One-time board init: clocks, UART, GPIO. Called before app_run(). */
void bsp_init(void);

/* Blocking TX of `len` bytes onto the protocol UART. */
void bsp_uart_write(const uint8_t *data, size_t len);

/*
 * Non-blocking RX: fetch one byte into `*byte`, return 1 on success,
 * 0 when the FIFO is empty. Called in a polling loop; must never block.
 */
int bsp_uart_read(uint8_t *byte);

/* Monotonic nanoseconds since boot (microsecond resolution is fine). */
uint64_t bsp_mono_ns(void);

/* Short critical section for cross-task producer/consumer claims.
 * Task context only, must be nest-safe, keep the section short
 * (FreeRTOS: taskENTER_CRITICAL; bare metal: interrupt mask). The
 * section also acts as the memory barrier between slot writes and
 * index publishes on SMP targets. */
void bsp_critical_enter(void);
void bsp_critical_exit(void);

/*
 * Yield/sleep inside the main loop — REQUIRED on RTOS ports to keep
 * the idle task and watchdogs alive (the app loop is a tight poll).
 * On FreeRTOS/ESP-IDF: vTaskDelay(1). Bare metal: may be empty.
 */
void bsp_yield(void);

/* Example actuator hook — replace/extend per your hardware.
 * Called from command dispatch; keep it cheap and never block. */
void bsp_led_set(int on);

/*
 * RGB variant for addressable/PWM LEDs (0-255 per channel).
 * Boards without a color LED may implement this as a no-op or map
 * it onto bsp_led_set. The app keeps a shadow copy of the last
 * commanded color — addressable LEDs (WS2812 etc.) are write-only,
 * so "read" always means "last commanded value", never hardware
 * readback.
 */
void bsp_led_rgb(uint8_t r, uint8_t g, uint8_t b);

/* Optional: display device for boards that have one (ui/display.h).
 * Ports without a panel leave this NULL. Call after bsp_init. */
struct display_dev;
struct display_dev *bsp_display(void);

/* Optional: large framebuffer heap (LCD full-frame RGB565). Prefer
 * PSRAM when present. Ports without a suitable heap return NULL.
 * Used by rb_term / status UI — not required for serial-only builds. */
void *bsp_fb_alloc(size_t bytes);
void  bsp_fb_free(void *p);

/* Optional: recursive mutex for the LCD framebuffer / SPI flush.
 * rb_term takes these around every draw. Empty stubs on ports
 * without an RTOS. */
void bsp_display_lock(void);
void bsp_display_unlock(void);

/* Optional: audio output (codec + amp). Ports without audio return
 * -1 from init / 0 from write. pcm = interleaved L,R int16 frames. */
int bsp_audio_init(void);
size_t bsp_audio_write(const int16_t *pcm, size_t frames);
void bsp_audio_volume(uint8_t percent);

/* Optional: audio input (codec ADC mic). Ports return -1/0 without
 * it. pcm = interleaved L,R int16 frames — same format as write. */
int    bsp_mic_enable(int on);          /* 0=off 1=on; -1 unsupported */
size_t bsp_audio_read(int16_t *pcm, size_t frames); /* frames read  */
void   bsp_mic_gain(uint8_t percent);   /* 0..100 -> PGA map        */
void   bsp_mic_mute(int on);            /* ADC digital mute          */

/* Required: board SKU / hardware model tag, e.g. "es3c28p",
 * "ws28p". Published on BoardInfo.hw_model — the no-parse
 * contract for hosts that need board-aware behavior. */
const char *bsp_hw_model(void);

/* Required: peripheral inventory — the app gates UI pages and
 * feature enablement on this mask. */
#define BOARD_CAP_LED  0x01 /* user-controllable RGB/status LED   */
#define BOARD_CAP_MIC  0x02 /* microphone capture path            */
#define BOARD_CAP_IMU  0x04 /* real 6-axis IMU                    */
#define BOARD_CAP_RTC  0x08 /* battery-backed RTC                 */
#define BOARD_CAP_BATT 0x10 /* battery voltage sense              */
#define BOARD_CAP_BTN  0x20 /* physical button(s)                 */
uint32_t bsp_caps(void);

/* Optional: 6-axis IMU. Fills accel (m/s^2) + gyro (rad/s); returns
 * 0 on a fresh sample, -1 when the board has no IMU. Poll-paced by
 * the app's sensor tick — no buffering expected. */
int bsp_imu_read(float accel_mps2[3], float gyro_rads[3]);

/* Optional: battery sense. Pack voltage in millivolts, -1 when the
 * board has no battery ADC. PCT mapping is app-side (LiPo curve). */
int bsp_battery_mv(void);

/* Optional: battery-backed RTC. Datetime is civil UTC, full year
 * (e.g. 2026), month 1-12, day 1-31, wday 0-6 (port-defined base).
 * -1 = no RTC / unreadable. The app seeds its wall clock from
 * bsp_rtc_read at boot and calls bsp_rtc_set on TimeSync. */
typedef struct {
    int year, month, day, hour, minute, second, wday;
} bsp_datetime_t;
int bsp_rtc_read(bsp_datetime_t *t);
int bsp_rtc_set(const bsp_datetime_t *t);

/* Optional: physical buttons — bitmask of currently-pressed
 * buttons (bit n = button n, active-low). 0 = none pressed or no
 * buttons. The key middleware debounces and classifies presses;
 * the ordinal mapping is board-defined (ws28p: bit0=PWR, bit1=BOOT). */
uint32_t bsp_buttons(void);

/* Optional: power-hold latch (ws28p SYS_EN style) — en=1 keeps the
 * battery rail alive; en=0 cuts power immediately. Ports latch it
 * high early in bsp_init/misc_init before calling it from app
 * code. -1 = no latch. */
int bsp_pwr_hold(int en);

/* Optional: WiFi station + persistent credentials (ADR-0017).
 * load/save hit NVS (or equivalent) so cmd 0x87 survives reboot.
 * connect() blocks ~10 s for DHCP; ready() is 1 once an IP exists. */
int  bsp_wifi_load_creds(char *ssid, size_t ssid_n, char *pass,
                           size_t pass_n, char *connect, size_t cn_n);
int  bsp_wifi_save_creds(const char *ssid, const char *pass,
                           const char *connect);
int  bsp_wifi_connect(const char *ssid, const char *pass);
int  bsp_wifi_ready(void);
int  bsp_wifi_last_reason(void); /* last disconnect reason code    */
int  bsp_wifi_last_rc(void);     /* last driver return code    */
int  bsp_wifi_scan_ssid(const char *ssid); /* rssi dBm, 0 not seen */
int  bsp_wifi_disconnect(void);  /* stop driver; connect() restarts */
/* pause=1: power the Wi-Fi radio down for the BLE session.
 * pause=0: allow a later reconnect. Returns 1 if the radio was
 * stopped and should be brought back when BLE drops. */
int bsp_wifi_pause_for_ble(int pause);
int bsp_wifi_paused_for_ble(void);
int  bsp_wifi_ip(char *buf, size_t n); /* "a.b.c.d" or -1 if no IP */
int  bsp_wifi_rssi(void);        /* live dBm, 0 when not connected */
int  bsp_wifi_set_enabled(int en); /* NVS: persist on/off           */
int  bsp_wifi_enabled(void);       /* 1 unless explicitly disabled  */
unsigned long bsp_free_heap(void);         /* 0 when not reported   */

/* Board identity — stable per-chip UUID like "ternion-esp32s3-a4cf12"
 * (suffix = last 3 MAC bytes). Returns strlen written, or -1 when the
 * port has no unique hardware id (callers should fall back). */
int bsp_identity(char *buf, size_t n);

/* MAC in "aa:bb:cc:dd:ee:ff" form, or -1 when no hw id. */
int bsp_mac_str(char *buf, size_t n);

/* Generic NVS-backed string param store — persisted user config
 * (param names are NVS keys — the esp32s3 port namespaces them
 * under "wifi"; other ports pick their own, max ~14
 * chars on ESP32). -1 = unsupported / missing / flash error. */
int bsp_param_get(const char *name, char *buf, size_t n);
int bsp_param_set(const char *name, const char *value);

/* Soft reset. Never returns on real ports; stub ports do nothing. */
void bsp_reboot(void);

/* Optional: capacitive touch, multi-point. One sample per call —
 * `n` contacts in pts[] (raw controller order, P1 then P2), plus the
 * controller's gesture id (0 = none). Points carry raw controller
 * event flags (0=down 1=up 2=contact); edge detection / id stability
 * / coords mapping live in middleware/touch.c — ports just translate
 * the chip registers. Returns 0 ok, <0 = no touch hw or bus error. */
#define RB_TOUCH_MAX_POINTS 2 /* frozen — sizes HAL structs AND the
                                 wire budget (serial.proto points
                                 max_count = 4, HAL stays 2) */

typedef struct {
    int16_t x, y;   /* DISPLAY coords (landscape), port-mapped;
                       clamped non-negative (>=0) by the port    */
    uint8_t event;  /* RAW controller flag (FT6336: 0 down 1 up
                       2 contact) — NOT the wire TOUCH_EV_*      */
} bsp_touch_pt_t;

/* Normalized gesture ids — ports translate chip-specific codes to
 * these (FT6336: 0x10..0x49). 1:1 with proto TouchGesture ordinals. */
#define BOARD_GEST_NONE   0
#define BOARD_GEST_UP     1
#define BOARD_GEST_DOWN   2
#define BOARD_GEST_LEFT   3
#define BOARD_GEST_RIGHT  4
#define BOARD_GEST_CLICK  5
#define BOARD_GEST_DBL    6
#define BOARD_GEST_ZIN    7
#define BOARD_GEST_ZOUT   8

typedef struct {
    uint8_t n;        /* contacts valid in pts[]             */
    uint8_t gesture;  /* BOARD_GEST_* (0 = none)             */
    bsp_touch_pt_t pts[RB_TOUCH_MAX_POINTS];
} bsp_touch_sample_t;

int bsp_touch_read(bsp_touch_sample_t *s);

#ifdef __cplusplus
}
#endif

#endif /* ROBOTOS_BSP_H */
