<div align="center">

# firmware-esp32-s3-workshop

**Public ESP32-S3 workshop** for ND03 / robotos courses — thin apps on
[`librobotos_fw.a`](lib/README.md), FreeRTOS labs, and an in-repo flash CLI.

[![License](https://img.shields.io/badge/license-Apache%202.0%20%7C%20MIT-blue.svg)](LICENSE-APACHE)
[![Platform](https://img.shields.io/badge/platform-ESP32--S3-red.svg)](https://www.espressif.com/en/products/socs/esp32-s3)
[![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v5%2Fv6-brightgreen.svg)](https://docs.espressif.com/projects/esp-idf/)
[![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-orange.svg)](https://www.freertos.org/)
[![Board](https://img.shields.io/badge/board-ws28p%20%7C%20es3c28p-informational.svg)](ports/boards/README.md)
[![GitHub release](https://img.shields.io/github/v/release/drsanti/firmware-esp32-s3-workshop?include_prereleases&sort=semver)](https://github.com/drsanti/firmware-esp32-s3-workshop/releases)
[![Last commit](https://img.shields.io/github/last-commit/drsanti/firmware-esp32-s3-workshop)](https://github.com/drsanti/firmware-esp32-s3-workshop/commits/main)

[Quick start](#quick-start) ·
[Tracks](#workshop-tracks) ·
[Boards](#supported-boards) ·
[Docs](#documentation) ·
[License](#license)

</div>

---

## Table of contents

- [Overview](#overview)
- [Features](#features)
- [Repository layout](#repository-layout)
- [Supported boards](#supported-boards)
- [Prerequisites](#prerequisites)
- [Quick start](#quick-start)
- [Workshop tracks](#workshop-tracks)
- [Documentation](#documentation)
- [Host integration](#host-integration)
- [Naming](#naming)
- [License](#license)

---

## Overview

Application code in this repo stays **thin**. Wire protocol, codecs, UI
widgets, and middleware live in the prebuilt archive **`librobotos_fw.a`**
(developed in the private repo `firmware-esp32-s3-dev`).

Students clone this workshop, open ESP-IDF, then flash with **`rb`** +
**`run NN`** — one IDF project per track, many exercises switched from
`main/main.c`.

| Track | Role |
|-------|------|
| [`demos/demo-labs`](demos/demo-labs/) | UI / widget / wire catalog (`ex01`…`ex17`) |
| [`labs/device-labs`](labs/device-labs/) | Device curriculum (`ex00`…`ex06`, stepped) |
| [`labs/special-labs`](labs/special-labs/) | Optional FreeRTOS specials (`ex01`…`ex06`) |
| [`labs/rtos-labs`](labs/rtos-labs/) | FreeRTOS deep-dives + game capstones (`ex00`…`ex15`) |

---

## Features

- **One-project switchers** — select an exercise with `.\run NN` (rewrites
  `main/main.c`), then build + flash
- **In-repo CLI** — [`rbdev/`](rbdev/README.md) binaries for Windows / macOS /
  Linux; sticky port + board via `rb config`
- **Two boards** — Waveshare **ws28p** (default) and Ternion **es3c28p**
- **Shared firmware package** — `lib/` + `include/` from a `robotos-fw` release
- **Stepped device labs** — lab in `main.c`, step in `exNN.c`
- **Dual license** — Apache-2.0 or MIT

---

## Repository layout

```text
firmware-esp32-s3-workshop/
├── lib/                          # librobotos_fw.a
├── include/firmware-esp32-s3-dev/  # public headers (same release)
├── ports/boards/<sku>/           # board HAL — ws28p | es3c28p
├── rbdev/                        # student CLI (windows | mac | linux)
├── demos/demo-labs/              # ONE IDF project — UI catalog
├── labs/
│   ├── device-labs/              # ONE IDF project — devices
│   ├── special-labs/             # ONE IDF project — specials
│   └── rtos-labs/                # ONE IDF project — FreeRTOS + games
├── NAMING.md
└── README.md
```

---

## Supported boards

| `--board` / `-DBOARD=` | Product |
|------------------------|---------|
| **`ws28p`** (default) | Waveshare ESP32-S3-Touch-LCD-2.8 (V1 CST328 / V2 CST3530) |
| **`es3c28p`** | Ternion ESP32-S3 (ILI9341 + FT6336) |

Both targets need **`set-target esp32s3`**. Pin maps and notes:
[ports/boards/README.md](ports/boards/README.md).

---

## Prerequisites

| Need | Notes |
|------|--------|
| This repo | With `lib/` + `include/` present ([lib/README.md](lib/README.md)) |
| ESP-IDF | Course Path A — **ESP-IDF PowerShell** on Windows is easiest |
| USB serial | Port from `.\rb doctor` (no universal COM default) |
| Board | `ws28p` or `es3c28p` |

> **Library package** — install a `robotos-fw-*.tar.gz` from
> `firmware-esp32-s3-dev` (`scripts/dist_fw.sh`), or copy `lib/` + `include/`
> from that tarball into the repo root.

---

## Quick start

Binaries ship under [`rbdev/`](rbdev/README.md). Do **not** permanently add
that folder to `PATH`.

```powershell
git clone https://github.com/drsanti/firmware-esp32-s3-workshop.git
cd firmware-esp32-s3-workshop

# ESP-IDF PowerShell, then (once per window — avoids "not digitally signed"):
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass

cd labs\device-labs
.\rb doctor
.\rb config --port COMx --board ws28p   # once — YOUR port from doctor

.\run 00          # select ex00 + build + flash
.\run 01 monitor  # select + build + flash + monitor
```

```bash
# macOS / Linux — in-repo binaries under rbdev/mac and rbdev/linux
chmod +x rbdev/mac/rbdev rbdev/linux/rbdev rb ./labs/*/rb ./demos/demo-labs/rb 2>/dev/null || true
cd labs/device-labs
./rb doctor
./rb config --port /dev/cu.usbmodemXXXX --board ws28p   # Linux: /dev/ttyACM0 or /dev/ttyUSB0
./run 00
# macOS Gatekeeper: right-click → Open rbdev/mac/rbdev once if blocked
```

Same `rb` / `run` pattern in `labs/special-labs`, `labs/rtos-labs`, and
`demos/demo-labs`.

<details>
<summary><strong>One-shot board / port override</strong></summary>

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\rb build --board es3c28p
.\rb flash --port COM6 --board es3c28p
```

After switching boards: `.\rb reconfigure` (or a clean build).

</details>

<details>
<summary><strong>Plain <code>idf.py</code></strong></summary>

```powershell
. <IDF_PATH>\export.ps1
cd labs/device-labs
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
```

</details>

Full student guide: **[rbdev/README.md](rbdev/README.md)**.

### Troubleshooting (Windows)

| Symptom | Fix |
|---------|-----|
| `not digitally signed` / `.ps1` blocked | `Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass` once per PowerShell window |
| `No module named 'rich_click'` (or `click`) | ESP-IDF Python venv incomplete — see below |
| `Unexpected token` / parse error in `_run.ps1` | Re-download / `git pull` (old scripts used Unicode dashes) |

**`rich_click` repair** (most common after a fresh or partial IDF install):

```powershell
cd C:\esp\v6.1\esp-idf          # your IDF path — `.\rb doctor` prints it
.\install.bat                   # several minutes; installs Python deps
.\export.ps1
idf.py --version

# From workshop root — should say "rich_click OK":
.\rbdev\windows\rbdev.exe doctor
```

Then retry `.\run 01` from the lab folder. Details:
[rbdev/README.md](rbdev/README.md#no-module-named-rich_click-or-click--rich).

---

## Workshop tracks

### Device labs — build a device, learn FreeRTOS

| | |
|--|--|
| Path | [`labs/device-labs`](labs/device-labs/) |
| Flash | `.\run 00` … `.\run 06` (step still selected in `exNN.c`) |
| Index | [labs/README.md](labs/README.md) |

```
ex00 Hello → ex01 Spirit → ex02 Alarm → ex03 React → ex04 Metro → ex05 Scope → ex06 Monitor
```

### Special labs — optional FreeRTOS topics

| | |
|--|--|
| Path | [`labs/special-labs`](labs/special-labs/) |
| Flash | `.\run 01` … `.\run 06` |
| Topics | Mutex, BOOT ISR, priority, counting sem, event group, reload timer |

### RTOS labs — deep-dives + capstones

| | |
|--|--|
| Path | [`labs/rtos-labs`](labs/rtos-labs/) |
| Flash | `.\run 00` … `.\run 15` |
| Highlights | Affinity, **`ex02-os-tick`**, queues, sync, timers, ISR, IMU games |

### Demo labs — UI / widget / wire catalog

| | |
|--|--|
| Path | [`demos/demo-labs`](demos/demo-labs/) |
| Flash | `.\run 01` … `.\run 17` |
| Labsheets | Each `exNN-*/README.md` — theory, ASCII architecture, Mermaid sequence |

<details>
<summary><strong>Full demo catalog (ex01–ex17)</strong></summary>

| Ex | Goal |
|----|------|
| [ex01-label](demos/demo-labs/ex01-label/) | 4×4 `rb_label` grid via `rb_screen` / `rb_layout` |
| [ex02-led](demos/demo-labs/ex02-led/) | 4×4 `rb_led` grid with intensity breathe |
| [ex03-term](demos/demo-labs/ex03-term/) | 3×2 `rb_term` panes — six logging FreeRTOS tasks |
| [ex04-switch](demos/demo-labs/ex04-switch/) | 2×2 `rb_switch` collage (pill/rect × ON/OFF) |
| [ex05-button](demos/demo-labs/ex05-button/) | 2×2 `rb_button` — normal + toggle w/ indicator |
| [ex06-slider](demos/demo-labs/ex06-slider/) | Nested `rb_slider` — V inputs drive H progress |
| [ex07-checkbox](demos/demo-labs/ex07-checkbox/) | 2×4 `rb_checkbox` — styles + paired `on_change` |
| [ex08-radio](demos/demo-labs/ex08-radio/) | Two exclusive `rb_radio` groups (Mode / Speed) |
| [ex09-form](demos/demo-labs/ex09-form/) | Composed form — radio + switch + checkbox + slider + button |
| [ex10-log](demos/demo-labs/ex10-log/) | First wire: `rb_node` + `rb_log` + topic pub + `rb_cmd_*` |
| [ex11-gauge](demos/demo-labs/ex11-gauge/) | 2×3 `rb_gauge` — styles + bands + breathe |
| [ex12-param](demos/demo-labs/ex12-param/) | `rb_param_*` table — local +/- and host ParamCmd |
| [ex13-topics-ui](demos/demo-labs/ex13-topics-ui/) | Slider → gauge + `rb_publish` |
| [ex14-progress](demos/demo-labs/ex14-progress/) | H/V `rb_progress` pills — bands + breathe |
| [ex15-chart](demos/demo-labs/ex15-chart/) | Multi-pane `rb_chart` — SCROLL + HISTORY |
| [ex16-imu-viz](demos/demo-labs/ex16-imu-viz/) | `rb_level` + `rb_imu_readout` + Accel HISTORY |
| [ex17-mic-viz](demos/demo-labs/ex17-mic-viz/) | `rb_vu` dBFS + mic HISTORY (`mic_sim` if no mic) |

</details>

---

## Documentation

| Doc | Audience |
|-----|----------|
| [rbdev/README.md](rbdev/README.md) | Student CLI — doctor, config, build, flash, `run NN` |
| [labs/README.md](labs/README.md) | Device / special / rtos curriculum index |
| [demos/README.md](demos/README.md) | Demo-labs index |
| [NAMING.md](NAMING.md) | `rb_*` layers (Board · Runtime · Wire · Features · demo) |
| [lib/README.md](lib/README.md) | Installing `librobotos_fw.a` + headers |
| [ports/boards/README.md](ports/boards/README.md) | Board SKUs and HAL |

---

## Host integration

Board demos that publish topics need the robotos host package
(`serial-bridge` or Wi‑Fi).

| Demos | Host required? |
|-------|----------------|
| 01–09, 11, 14–17 | No — UI / sensors on-device |
| 10, 12, 13 | Yes — COBS on the USB-JTAG port |

---

## Naming

Read **[NAMING.md](NAMING.md)** before Demo 01. On the MCU, **`rb_*` means
robotos** (not ROS): five layers — Board · Runtime · Wire · Features · your
demo.

---

## License

Dual-licensed under **[Apache License 2.0](LICENSE-APACHE)** or
**[MIT](LICENSE-MIT)**, at your option.

```text
Copyright contributors to firmware-esp32-s3-workshop
```

---

<div align="center">

**Clone → `rb doctor` → `rb config` → `run NN`**

[Open on GitHub](https://github.com/drsanti/firmware-esp32-s3-workshop) ·
[Issues](https://github.com/drsanti/firmware-esp32-s3-workshop/issues)

</div>
