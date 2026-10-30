# Demo 07 — Checkboxes

**2×4** style collage. Tap to toggle; top row mirrors to bottom via
`rb_checkbox_set_on_change`.

## Build / flash

```bash
cd demos/demo-labs
# Must be esp32s3 (GPIO_NUM_45 undeclared ⇒ wrong target / plain esp32).
.\run 07
# Ternion: .\rb config --board es3c28p then .\run 07
```

## Success

Boxes show a check when on; paired cells stay in sync.

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex07.h"`
2. `ex07_start();`

Or: `.\run 07` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
