# Demo 01 — Labels in a grid

Drive the board with **`librobotos_fw.a`**: open the display, build a
**screen → layout → label** tree, then update a **4×4** grid of labels
from FreeRTOS tasks. First UI widget demo — no robotos node or serial wire yet.

## APIs this demo uses

| Layer | You will call |
|-------|----------------|
| Board | `bsp_init` |
| UI tree (`rb_screen.h`) | `rb_display_open`, `rb_screen_init`, `rb_layout_init` / `add`, `rb_screen_set_root` / `reflow` |
| Label | `rb_label_init`, `rb_label_set_color`, `rb_label_printf` |
| Your demo | `main/main.c` + FreeRTOS tasks |

See [NAMING.md](../../NAMING.md) for the five-layer map (`rb` = robotos).

## Prerequisites

- ESP-IDF (same major as class notes)
- `lib/librobotos_fw.a` + `include/firmware-esp32-s3-dev/` installed
  at the workshop repo root (see [../../lib/README.md](../../lib/README.md))
- Class board: **ws28p** (default) or **es3c28p** — see [ports/boards/README.md](../../ports/boards/README.md)

## Build / flash

```bash
cd demos/demo-labs
.\run 01
# Ternion: .\rb config --board es3c28p then .\run 01
```

## Success

Sixteen colored labels in a 4×4 grid show `id n` counting up at staggered
rates. No host bridge required.

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex01.h"`
2. `ex01_start();`

Or: `.\run 01` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
