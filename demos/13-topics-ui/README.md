# Demo 13 — Topics + UI

Slider (INPUT) drives a horseshoe gauge (DISPLAY) and publishes the
value on the wire (`RSP_MSG_IMU` payload = `int32`). Gauge bands turn
warn/danger colors above 70 / 90.

Teaches: Runtime pub + UI callback in one screen.

## Build / flash

```bash
cd demos/13-topics-ui
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
```

## Success

Drag the slider; the gauge follows and `rb_log` prints the published
value. Past 70 the fill goes yellow; past 90 red.
