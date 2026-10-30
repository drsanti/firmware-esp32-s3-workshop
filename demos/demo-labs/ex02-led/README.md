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
cd demos/demo-labs
.\run 02
# Ternion: .\rb config --board es3c28p then .\run 02
```

## Success

A 4×4 grid of colored LEDs pulses; alternating shapes are visible.
No host bridge required.

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex02.h"`
2. `ex02_start();`

Or: `.\run 02` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
