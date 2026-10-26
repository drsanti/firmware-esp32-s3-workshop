# firmware-esp32-s3-workshop

**Public** ESP32-S3 workshop demos for ND03 / robotos courses.

Every demo links the same library archive **`librobotos_fw.a`**.
Application code stays thin — wire protocol, codecs, and middleware
live inside the library (developed in the private repo
`firmware-esp32-s3-dev`).

## Layout

```text
lib/                         # librobotos_fw.a (from a private release)
include/firmware-esp32-s3-dev/   # public headers (from the same release)
ports/boards/<sku>/          # board HAL — ws28p | es3c28p (see boards/README.md)
demos/
  01-label/                  # first demo — screen → layout → labels
  02-led/                    # framed LED indicators (intensity breathe)
  03-term/                   # 3×2 rb_term panes (six logging tasks)
  04-switch/                 # 2×2 flat slide switches
  05-button/                 # 2×2 normal + toggle buttons
  06-slider/                 # V|V left, H/H right + on_change
  07-checkbox/               # 2×4 checkboxes + on_change
```

Structured `labs/` exercises can land later; this tree ships runnable demos.

## Get the library

1. Download a `robotos-fw-*.tar.gz` release produced by
   `firmware-esp32-s3-dev` (`scripts/dist_fw.sh`), **or**
2. Copy `lib/` and `include/` from that tarball into this repo root.

See [lib/README.md](lib/README.md).

## Naming

Read **[NAMING.md](NAMING.md)** before Demo 01 — five layers
(Board · Runtime · Wire · Features · Your demo). **`rb_*` = robotos**
on the MCU (not ROS).

## Boards

| `-DBOARD=` | Product |
|---|---|
| **`ws28p`** (default) | Waveshare ESP32-S3-Touch-LCD-2.8 (V1 CST328 / V2 CST3530) |
| **`es3c28p`** | Ternion ESP32-S3 (ILI9341 + FT6336) |

Both need **`idf.py set-target esp32s3`**. Details: [ports/boards/README.md](ports/boards/README.md).

## Demos

| Demo | Goal |
|---|---|
| [01-label](demos/01-label/) | 4×4 `rb_label` grid via `rb_screen` / `rb_layout` |
| [02-led](demos/02-led/) | 4×4 `rb_led` grid with intensity breathe |
| [03-term](demos/03-term/) | 3×2 `rb_term` panes — six logging FreeRTOS tasks |
| [04-switch](demos/04-switch/) | 2×2 `rb_switch` collage (pill/rect × ON/OFF) |
| [05-button](demos/05-button/) | 2×2 `rb_button` — normal + toggle w/ indicator |
| [06-slider](demos/06-slider/) | Nested layout `rb_slider` — V inputs drive H progress via `on_change` |
| [07-checkbox](demos/07-checkbox/) | 2×4 `rb_checkbox` — styles + paired `on_change` |

More demos will grow the same `.a` (log, topics, cmds, twin).

## Host side

Board demos that publish topics need the robotos host package
(`serial-bridge` or Wi‑Fi). Early UI demos do not require the host.
