# Demo 06 — Horizontal / vertical sliders

Snapshot layout: **left** two verticals (INPUT), **right** two
horizontals (PROGRESS). Dragging a vertical fires `on_change` and
mirrors the value onto its horizontal peer.

## Build / flash

```bash
cd demos/demo-labs
# Always esp32s3. GPIO_NUM_45 undeclared ⇒ wrong target (not S3).
.\run 06
# Ternion: .\rb config --board es3c28p then .\run 06
```

## Success

Left thumbs drag; right bars follow via `rb_slider_set_on_change`.

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex06.h"`
2. `ex06_start();`

Or: `.\run 06` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
