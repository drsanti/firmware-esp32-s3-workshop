# Demo 02 — Framed LED indicators

Build a **screen → layout → rb_led** tree. Each cell is a realistic
panel LED (metal frame, dark body, lens, intensity-driven glow).
Odd cells are **round-rect**, even cells are **circle**. FreeRTOS
tasks breathe intensity 0→255→0.

## APIs this demo uses

| Layer | You will call |
|-------|----------------|
| Board | `bsp_init` |
| UI tree | `rb_display_open`, `rb_screen_*`, `rb_layout_*` |
| LED | `rb_led_init`, `rb_led_set_shape`, `rb_led_set_rgb`, `rb_led_set_intensity` |

## Build / flash

```bash
cd demos/02-led
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
# or: idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx
```

## Success

A 4×4 grid of colored LEDs pulses; alternating shapes are visible.
No host bridge required.
