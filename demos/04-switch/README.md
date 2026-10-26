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
cd demos/04-switch
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
# or: idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx
```

## Success

Four panel switches (two shapes). Tap a quadrant to flip ON/OFF.
No host bridge required.
