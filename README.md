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

Students clone this workshop, run **`rbdev init`** once, then flash with
**`rbdev run NN`** — one IDF project per track, many exercises switched from
`main/main.c`. No ESP-IDF activate; no `rb` / `run` wrappers.

| Track | Role |
|-------|------|
| [`labs/demo-labs`](labs/demo-labs/) | UI / widget / wire catalog (`ex01`…`ex17`) |
| [`labs/device-labs`](labs/device-labs/) | Device curriculum (`ex00`…`ex06`, stepped) |
| [`labs/special-labs`](labs/special-labs/) | Optional FreeRTOS specials (`ex01`…`ex06`) |
| [`labs/rtos-labs`](labs/rtos-labs/) | FreeRTOS deep-dives + game capstones (`ex00`…`ex15`) |

---

## Features

- **One-project switchers** — select an exercise with `rbdev run NN` (rewrites
  `main/main.c`), then build + flash
- **In-repo CLI** — [`rbdev/`](rbdev/README.md) binaries for Windows / macOS /
  Linux; `rbdev init` writes sticky `.rbdev.toml` and adds PATH
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
├── start-here.cmd                # Windows optional PATH helper (outside Cursor)
├── .vscode/settings.json         # Cursor/VS Code: rbdev on terminal PATH
├── rbdev/                        # student CLI (windows | mac | linux)
├── labs/demo-labs/              # ONE IDF project — UI catalog
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
| ESP-IDF | Course Path A on the PC — `rbdev init` discovers it (no `export` activate) |
| USB serial | Port from `rbdev init` / `doctor` (`COMx` / `/dev/cu.*` / `/dev/ttyACM*`) |
| Board | `ws28p` or `es3c28p` |

> **Library package** — install a `robotos-fw-*.tar.gz` from
> `firmware-esp32-s3-dev` (`scripts/dist_fw.sh`), or copy `lib/` + `include/`
> from that tarball into the repo root.

---

## Quick start

Binaries ship under [`rbdev/`](rbdev/README.md). `rbdev init` adds that folder
to your **user PATH** (then open a new terminal).

```powershell
git clone https://github.com/drsanti/firmware-esp32-s3-workshop.git
cd firmware-esp32-s3-workshop

# Once per PC / clone (workshop root; pick device-labs):
rbdev init
# fallback: .\rbdev\windows\rbdev.exe init
# outside Cursor only: double-click start-here.cmd, then rbdev init

# NEW terminal, then every lab day:
rbdev run 00              # sticky project; or cd labs\device-labs
rbdev run 00 --monitor
```

```bash
# macOS / Linux
chmod +x rbdev/mac/rbdev rbdev/linux/rbdev 2>/dev/null || true
./rbdev/mac/rbdev init          # Linux: ./rbdev/linux/rbdev init
# NEW terminal:
cd labs/device-labs
rbdev run 00
# macOS Gatekeeper: right-click → Open rbdev/mac/rbdev once if blocked
```

Same `rbdev run NN` pattern in `labs/special-labs`, `labs/rtos-labs`, and
`labs/demo-labs`. Do **not** run `export.ps1` / bare `run`.

<details>
<summary><strong>One-shot board / port override</strong></summary>

```powershell
rbdev build --board es3c28p
rbdev flash --port COM6 --board es3c28p
```

After switching boards: `rbdev reconfigure` (or a clean build).

</details>

Full student guide: **[rbdev/README.md](rbdev/README.md)**.  
**Class go-live checklist:** **[CLASSROOM-SETUP.md](CLASSROOM-SETUP.md)**.

### Troubleshooting

| Symptom | Fix |
|---------|-----|
| `No module named 'rich_click'` (or `click`) | Re-run `rbdev init` (repairs Espressif venv) |
| `rbdev: command not found` | New terminal after `init`; call `./rbdev/<os>/rbdev …`; Windows outside Cursor: [`start-here.cmd`](start-here.cmd) |
| Bare `run 00` → nvm / OneDrive | Use `rbdev run 00` only |
| macOS Gatekeeper blocks `rbdev` | Right-click → Open `rbdev/mac/rbdev` once |
| Linux: permission denied on `/dev/tty*` | `sudo usermod -aG dialout $USER`, then re-login |

**`rich_click` repair** if `init` did not fix it:

```powershell
# Windows — YOUR IDF path from `rbdev doctor`
cd C:\esp\v6.1\esp-idf
.\install.bat
# workshop root:
.\rbdev\windows\rbdev.exe doctor   # expect "rich_click OK"
```

```bash
# macOS / Linux
cd ~/esp/esp-idf
./install.sh
./rbdev/mac/rbdev doctor        # Linux: ./rbdev/linux/rbdev doctor
```

Then retry `rbdev run 00`. Details: [rbdev/README.md](rbdev/README.md) ·
[CLASSROOM-SETUP.md](CLASSROOM-SETUP.md).

---

## Workshop tracks

### Device labs — build a device, learn FreeRTOS

| | |
|--|--|
| Path | [`labs/device-labs`](labs/device-labs/) |
| Flash | `rbdev run 00` … `06` (step still selected in `exNN.c`) |
| Index | [labs/README.md](labs/README.md) |

```
ex00 Hello → ex01 Spirit → ex02 Alarm → ex03 React → ex04 Metro → ex05 Scope → ex06 Monitor
```

### Special labs — optional FreeRTOS topics

| | |
|--|--|
| Path | [`labs/special-labs`](labs/special-labs/) |
| Flash | `rbdev run 01` … `06` |
| Topics | Mutex, BOOT ISR, priority, counting sem, event group, reload timer |

### RTOS labs — deep-dives + capstones

| | |
|--|--|
| Path | [`labs/rtos-labs`](labs/rtos-labs/) |
| Flash | `rbdev run 00` … `15` |
| Highlights | Affinity, **`ex02-os-tick`**, queues, sync, timers, ISR, IMU games |

### Demo labs — UI / widget / wire catalog

| | |
|--|--|
| Path | [`labs/demo-labs`](labs/demo-labs/) |
| Flash | `rbdev run 01` … `17` |
| Labsheets | Each `exNN-*/README.md` — theory, ASCII architecture, Mermaid sequence |

<details>
<summary><strong>Full demo catalog (ex01–ex17)</strong></summary>

| Ex | Goal |
|----|------|
| [ex01-label](labs/demo-labs/ex01-label/) | 4×4 `rb_label` grid via `rb_screen` / `rb_layout` |
| [ex02-led](labs/demo-labs/ex02-led/) | 4×4 `rb_led` grid with intensity breathe |
| [ex03-term](labs/demo-labs/ex03-term/) | 3×2 `rb_term` panes — six logging FreeRTOS tasks |
| [ex04-switch](labs/demo-labs/ex04-switch/) | 2×2 `rb_switch` collage (pill/rect × ON/OFF) |
| [ex05-button](labs/demo-labs/ex05-button/) | 2×2 `rb_button` — normal + toggle w/ indicator |
| [ex06-slider](labs/demo-labs/ex06-slider/) | Nested `rb_slider` — V inputs drive H progress |
| [ex07-checkbox](labs/demo-labs/ex07-checkbox/) | 2×4 `rb_checkbox` — styles + paired `on_change` |
| [ex08-radio](labs/demo-labs/ex08-radio/) | Two exclusive `rb_radio` groups (Mode / Speed) |
| [ex09-form](labs/demo-labs/ex09-form/) | Composed form — radio + switch + checkbox + slider + button |
| [ex10-log](labs/demo-labs/ex10-log/) | First wire: `rb_node` + `rb_log` + topic pub + `rb_cmd_*` |
| [ex11-gauge](labs/demo-labs/ex11-gauge/) | 2×3 `rb_gauge` — styles + bands + breathe |
| [ex12-param](labs/demo-labs/ex12-param/) | `rb_param_*` table — local +/- and host ParamCmd |
| [ex13-topics-ui](labs/demo-labs/ex13-topics-ui/) | Slider → gauge + `rb_publish` |
| [ex14-progress](labs/demo-labs/ex14-progress/) | H/V `rb_progress` pills — bands + breathe |
| [ex15-chart](labs/demo-labs/ex15-chart/) | Multi-pane `rb_chart` — SCROLL + HISTORY |
| [ex16-imu-viz](labs/demo-labs/ex16-imu-viz/) | `rb_level` + `rb_imu_readout` + Accel HISTORY |
| [ex17-mic-viz](labs/demo-labs/ex17-mic-viz/) | `rb_vu` dBFS + mic HISTORY (`mic_sim` if no mic) |

</details>

---

## Documentation

| Doc | Audience |
|-----|----------|
| [CLASSROOM-SETUP.md](CLASSROOM-SETUP.md) | **Start here in class** — doctor checklist (Win/macOS/Linux), TA playbook |
| [start-here.cmd](start-here.cmd) | Windows optional — one cmd window with `rbdev` on PATH (outside Cursor) |
| [`.vscode/settings.json`](.vscode/settings.json) | Cursor/VS Code terminal PATH for `rbdev` (keep committed) |
| [rbdev/README.md](rbdev/README.md) | Student CLI — `init`, doctor, `run NN`, build, flash |
| [labs/README.md](labs/README.md) | Device / special / rtos curriculum index |
| [labs/demo-labs/README.md](labs/demo-labs/README.md) | Demo-labs index |
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

**Clone → `rbdev init` → new terminal → `rbdev run NN`**

[Open on GitHub](https://github.com/drsanti/firmware-esp32-s3-workshop) ·
[Issues](https://github.com/drsanti/firmware-esp32-s3-workshop/issues)

</div>
