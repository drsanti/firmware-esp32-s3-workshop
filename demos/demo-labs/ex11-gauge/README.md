# Demo 11 — Gauges

**2×3** horseshoe gauges with different `rb_style_*` fills. Each has
unit / value / label; values breathe 0↔100 so the CW arc is obvious.

## Build / flash

```bash
cd demos/demo-labs
.\run 11
# Ternion: .\rb config --board es3c28p then .\run 11
```

## Success

Six rings in different colors; fills and center numbers sweep up and
down out of phase.

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex11.h"`
2. `ex11_start();`

Or: `.\run 11` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
