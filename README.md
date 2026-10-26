# firmware-esp32-s3-workshop

**Public** ESP32-S3 workshop labs for ND03 / robotos courses.

Every lab links the same library archive **`librobotos_fw.a`**.
Application code stays thin — wire protocol, codecs, and middleware
live inside the library (developed in the private repo
`firmware-esp32-s3-dev`).

## Layout

```text
lib/                         # librobotos_fw.a (from a private release)
include/firmware-esp32-s3-dev/   # public headers (from the same release)
ports/boards/<sku>/          # board HAL for class hardware
labs/
  01-blink/                  # first lab — LED via bsp_led_set + rb_spin
```

## Get the library

1. Download a `robotos-fw-*.tar.gz` release produced by
   `firmware-esp32-s3-dev` (`scripts/dist_fw.sh`), **or**
2. Copy `lib/` and `include/` from that tarball into this repo root.

See [lib/README.md](lib/README.md).

## Labs

| Lab | Goal |
|---|---|
| [01-blink](labs/01-blink/) | Blink an LED through `bsp_led_set` on an `rb_node` |

More labs will grow the same `.a` (button, log, topics, cmds, twin).

## Host side

Board labs that publish topics need the robotos host package
(`serial-bridge` or Wi‑Fi). Early blink does not require the host.
