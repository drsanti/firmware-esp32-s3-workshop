# Demo 09 — Form (composition)

One screen mixes **radio**, **switch**, **checkbox**, **slider**, and
**button**. No new leaf — teaches nested `rb_layout` + callbacks.

Apply copies the live controls into a status `rb_label`.

## Build / flash

```bash
cd demos/demo-labs
.\run 09
# Ternion: .\rb config --board es3c28p then .\run 09
```

## Success

Taps update widgets; **Apply** refreshes the status line from current
values.

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex09.h"`
2. `ex09_start();`

Or: `.\run 09` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
