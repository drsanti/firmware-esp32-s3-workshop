# Demo 12 — Params

First **`rb_param_*`** lab. A tiny table (`demo.level`, `demo.name`)
answers host `ParamCmd` (0x8a). Events print on the LCD term, log
on the wire, and publish `ParamEvent` uplinks.

Local buttons bump `demo.level` so the board works without a host.

## Build / flash

```bash
cd demos/demo-labs
.\run 12
```

## Success

Term shows `param demo.level=…` when you tap **+** / **−** or when a
host ParamCmd arrives.

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex12.h"`
2. `ex12_start();`

Or: `.\run 12` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
