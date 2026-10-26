# Demo 07 — Checkboxes

**2×4** style collage. Tap to toggle; top row mirrors to bottom via
`rb_checkbox_set_on_change`.

## Build / flash

```bash
cd demos/07-checkbox
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
# or: idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx
```

## Success

Boxes show a check when on; paired cells stay in sync.
