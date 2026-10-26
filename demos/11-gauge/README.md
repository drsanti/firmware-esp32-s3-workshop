# Demo 11 — Gauges

**2×3** horseshoe gauges with different `rb_style_*` fills. Each has
unit / value / label; values breathe 0↔100 so the CW arc is obvious.

## Build / flash

```bash
cd demos/11-gauge
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
# or: idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx
```

## Success

Six rings in different colors; fills and center numbers sweep up and
down out of phase.
