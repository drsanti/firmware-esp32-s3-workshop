# Demo 05 — Normal + toggle buttons

**2×4** `rb_style` collage. Top = normal (DOWN thickens border, CLICK
counts presses). Bottom = toggle (CLICK mirrors peer ON state).

## APIs this demo uses

| Layer | You will call |
|-------|----------------|
| Board | `bsp_init`, `bsp_touch_read` |
| UI tree | `rb_display_open`, `rb_screen_*`, `rb_layout_*` |
| Style | `rb_button_set_style` (`RB_STYLE_*`) |
| Button | `set_on_event` (`DOWN`/`UP`/`CLICK`), `feed_touch` |

## Build / flash

```bash
cd demos/demo-labs
.\run 05
# Ternion: .\rb config --board es3c28p then .\run 05
```

## Success

Normal labels become press counts; toggles stay in sync pairwise.

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex05.h"`
2. `ex05_start();`

Or: `.\run 05` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
