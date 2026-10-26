# Lab 01 — Blink

Blink the board LED using **`librobotos_fw.a`** — not raw ESP-IDF GPIO.

You write a short `main`: init the board and an `rb_node`, then toggle
`board_led_set` in a loop while calling `rb_spin_once`.

## Prerequisites

- ESP-IDF (same major as class notes)
- `lib/librobotos_fw.a` + `include/firmware-esp32-s3-dev/` installed
  at the workshop repo root (see [../../lib/README.md](../../lib/README.md))
- Class board (default **ws28p**)

## Build / flash

```bash
cd labs/01-blink
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
```

## Success

The LED blinks ~2.5 Hz. No host bridge required for this lab.
