# Demo 12 — Params

First **`rb_param_*`** lab. A tiny table (`demo.level`, `demo.name`)
answers host `ParamCmd` (0x8a). Events print on the LCD term, log
on the wire, and publish `ParamEvent` uplinks.

Local buttons bump `demo.level` so the board works without a host.

## Build / flash

```bash
cd demos/12-param
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
```

## Success

Term shows `param demo.level=…` when you tap **+** / **−** or when a
host ParamCmd arrives.
