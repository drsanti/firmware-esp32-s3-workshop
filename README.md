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
demos/demo-labs/             # ONE IDF project — UI/widget/wire catalog
  main/main.c                # switch: include exNN.h + exNN_start()
  ex01-label/ … ex17-mic-viz/
labs/device-labs/            # ONE IDF project — FreeRTOS device curriculum
labs/special-labs/           # ONE IDF project — optional FreeRTOS specials
labs/rtos-labs/              # ONE IDF project — FreeRTOS deep-dives + games
```

**`demos/demo-labs/`** = UI / widget / wire catalog (one project, many `exNN`).  
**`labs/`** = FreeRTOS device + specials + deep-dive workshops. See [labs/README.md](labs/README.md).

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

| `-DBOARD=` / `--board` | Product |
|---|---|
| **`ws28p`** (default) | Waveshare ESP32-S3-Touch-LCD-2.8 (V1 CST328 / V2 CST3530) |
| **`es3c28p`** | Ternion ESP32-S3 (ILI9341 + FT6336) |

With **`rb`** (see [rbdev/README.md](rbdev/README.md)):

```powershell
.\rb config --port COMx --board ws28p   # once — YOUR port from doctor (saves .rbdev.toml)
.\rb build
.\rb flash

# Ternion / one-shot:
.\rb build --board es3c28p
.\rb flash --port COM6 --board es3c28p
```

With plain `idf.py`, pass `-DBOARD=…` the same way. Both SKUs need
**`set-target esp32s3`**. Details: [ports/boards/README.md](ports/boards/README.md).

## Demos ([demo-labs](demos/demo-labs/))

Switch in [`demos/demo-labs/main/main.c`](demos/demo-labs/main/main.c):

```powershell
cd demos/demo-labs
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
```

| Ex | Goal |
|---|---|
| [ex01-label](demos/demo-labs/ex01-label/) | 4×4 `rb_label` grid via `rb_screen` / `rb_layout` |
| [ex02-led](demos/demo-labs/ex02-led/) | 4×4 `rb_led` grid with intensity breathe |
| [ex03-term](demos/demo-labs/ex03-term/) | 3×2 `rb_term` panes — six logging FreeRTOS tasks |
| [ex04-switch](demos/demo-labs/ex04-switch/) | 2×2 `rb_switch` collage (pill/rect × ON/OFF) |
| [ex05-button](demos/demo-labs/ex05-button/) | 2×2 `rb_button` — normal + toggle w/ indicator |
| [ex06-slider](demos/demo-labs/ex06-slider/) | Nested layout `rb_slider` — V inputs drive H progress via `on_change` |
| [ex07-checkbox](demos/demo-labs/ex07-checkbox/) | 2×4 `rb_checkbox` — styles + paired `on_change` |
| [ex08-radio](demos/demo-labs/ex08-radio/) | Two exclusive `rb_radio` groups (Mode / Speed) |
| [ex09-form](demos/demo-labs/ex09-form/) | Composed form — radio + switch + checkbox + slider + button |
| [ex10-log](demos/demo-labs/ex10-log/) | First wire: `rb_node` + `rb_log` + topic pub + `rb_cmd_*` |
| [ex11-gauge](demos/demo-labs/ex11-gauge/) | 2×3 `rb_gauge` — styles + unit/value/label + bands + breathe |
| [ex12-param](demos/demo-labs/ex12-param/) | `rb_param_*` table — local +/- and host ParamCmd (0x8a) |
| [ex13-topics-ui](demos/demo-labs/ex13-topics-ui/) | Slider → gauge + `rb_publish` (topics meet UI) |
| [ex14-progress](demos/demo-labs/ex14-progress/) | H/V `rb_progress` pills — bands + breathe |
| [ex15-chart](demos/demo-labs/ex15-chart/) | Multi-pane `rb_chart` — SCROLL + HISTORY (IMU-style) |
| [ex16-imu-viz](demos/demo-labs/ex16-imu-viz/) | `rb_level` + `rb_imu_readout` + Accel HISTORY |
| [ex17-mic-viz](demos/demo-labs/ex17-mic-viz/) | `rb_vu` dBFS + mic HISTORY (`mic_sim` if no mic) |

More demos will grow the same `.a` (twin, zenoh).

## Build / flash with rbdev

Copy the OS-matched binary from the course slim kit into
[`rbdev/`](rbdev/README.md) (gitignored — do not commit):

```text
rbdev/windows/rbdev.exe
rbdev/mac/rbdev
rbdev/linux/rbdev
```

```powershell
# ESP-IDF PowerShell
cd labs\device-labs
.\rb doctor
.\rb config --port COMx --board ws28p
.\rb build
.\rb flash
```

(`.\rb` wraps `rbdev\windows\rbdev.exe` — see [rbdev/README.md](rbdev/README.md).)
You can still use plain `idf.py` if you prefer.

## Host side

Board demos that publish topics need the robotos host package
(`serial-bridge` or Wi‑Fi). UI-only demos (01–09, 11, 14–17) do not
require the host; demos 10 / 12 / 13 speak COBS on the USB-JTAG port.
