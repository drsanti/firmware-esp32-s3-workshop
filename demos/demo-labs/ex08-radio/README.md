# Demo 08 — Radios

Two exclusive groups (Mode / Speed), **2×4** collage. Tap selects;
re-tap does not clear — exclusivity is the lesson.

## Build / flash

```bash
cd demos/demo-labs
# Must be esp32s3 (GPIO_NUM_45 undeclared ⇒ wrong target / plain esp32).
.\run 08
# Ternion: .\rb config --board es3c28p then .\run 08
```

## Success

Only one radio per column group is filled; switching picks a new
dot and clears the previous.

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex08.h"`
2. `ex08_start();`

Or: `.\run 08` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
