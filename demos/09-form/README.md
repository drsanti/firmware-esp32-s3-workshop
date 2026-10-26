# Demo 09 — Form (composition)

One screen mixes **radio**, **switch**, **checkbox**, **slider**, and
**button**. No new leaf — teaches nested `rb_layout` + callbacks.

Apply copies the live controls into a status `rb_label`.

## Build / flash

```bash
cd demos/09-form
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
# or: idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx
```

## Success

Taps update widgets; **Apply** refreshes the status line from current
values.
