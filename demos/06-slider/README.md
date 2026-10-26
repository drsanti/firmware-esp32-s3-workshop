# Demo 06 — Horizontal / vertical sliders

Snapshot layout: **left** two verticals (INPUT), **right** two
horizontals (PROGRESS). Dragging a vertical fires `on_change` and
mirrors the value onto its horizontal peer.

## Build / flash

```bash
cd demos/06-slider
# Always esp32s3. GPIO_NUM_45 undeclared ⇒ wrong target (not S3).
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
# or: idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx
```

## Success

Left thumbs drag; right bars follow via `rb_slider_set_on_change`.
