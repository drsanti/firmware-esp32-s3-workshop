# Demo 13 — Topics + UI

Slider (INPUT) drives a horseshoe gauge (DISPLAY) and publishes the
value on the wire (`RSP_MSG_IMU` payload = `int32`). Gauge bands turn
warn/danger colors above 70 / 90.

Teaches: Runtime pub + UI callback in one screen.

## Build / flash

```bash
cd demos/demo-labs
.\run 13
```

## Success

Drag the slider; the gauge follows and `rb_log` prints the published
value. Past 70 the fill goes yellow; past 90 red.

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex13.h"`
2. `ex13_start();`

Or: `.\run 13` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
