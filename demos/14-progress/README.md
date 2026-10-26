# Demo 14 — Progress bars

**H + V** `rb_progress` pills. Left column matches the design snap
(values 20 / 50 / 100 with warn@50 / danger@90). Right column has two
vertical bars that breathe.

## Build / flash

```bash
cd demos/14-progress
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
# or: idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx
```

## Success

Three horizontal capsules: green / yellow / red fills. Two vertical
bars on the right sweep 0↔100 with band color changes.
