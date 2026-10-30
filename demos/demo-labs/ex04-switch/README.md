# Demo 04 — Realistic slide switches

Build a **screen → layout → rb_switch** tree: **2×2** collage matching
the reference (pill/rect × ON/OFF). Flat track, ON/OFF label, white
knob. Tap a cell to toggle.

## APIs this demo uses

| Layer | You will call |
|-------|----------------|
| Board | `bsp_init`, `bsp_touch_read` |
| UI tree | `rb_display_open`, `rb_screen_*`, `rb_layout_*` |
| Switch | `rb_switch_init`, `rb_switch_set_shape`, `rb_switch_set_on`, `rb_switch_feed_touch` |

## Build / flash

```bash
cd demos/demo-labs
.\run 04
# Ternion: .\rb config --board es3c28p then .\run 04
```

## Success

Four panel switches (two shapes). Tap a quadrant to flip ON/OFF.
No host bridge required.

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex04.h"`
2. `ex04_start();`

Or: `.\run 04` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
