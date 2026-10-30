# Demo 14 — Progress bars

**H + V** `rb_progress` pills. Left column matches the design snap
(values 20 / 50 / 100 with warn@50 / danger@90). Right column has two
vertical bars that breathe.

## Build / flash

```bash
cd demos/demo-labs
.\run 14
# Ternion: .\rb config --board es3c28p then .\run 14
```

## Success

Three horizontal capsules: green / yellow / red fills. Two vertical
bars on the right sweep 0↔100 with band color changes.

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex14.h"`
2. `ex14_start();`

Or: `.\run 14` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
