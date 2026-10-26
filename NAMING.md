# Naming & symbols (workshop)

**robotos** is the middleware you are learning (ROS 2–like: node,
pub/sub, spin). On the MCU, C symbols use the short prefix **`rb_*`**
(`rb` = robotos). This is not ROS 2 — do not look for `ros_` / `rcl_`.

## Five layers

| # | Layer | Spelling | What it is |
|---|-------|----------|------------|
| 1 | **Board** | `bsp_*` | Pins, UART, LED, clock — hardware port |
| 2 | **Runtime** | `rb_*` | On-device node: init, publish, subscribe, spin |
| 3 | **Wire** | `RSP_*` / `rsp_*` | Message IDs / framing shared with the PC bridge |
| 4 | **Features** | `rb_<name>_` | Library modules (keys, params, cmds, LED FX, …) |
| 5 | **Your lab** | `main/main.c` | The code you write |

Labs 01–03 use **Board + Runtime + Your lab**. Wire and Features
appear when a lab README lists them.

## Glossary

| Term | One-liner |
|------|-----------|
| **robotos** | Product / middleware name (host tools + MCU library). |
| **`rb_*`** | Runtime (and Feature) C API — short for robotos. |
| **`bsp_*`** | Board HAL — init MCU, drive LED, UART, time. |
| **board SKU** | Which PCB (`ws28p`, …) via `-DBOARD=` — not the API name. |
| **`librobotos_fw.a`** | Prebuilt library; labs link it, they do not rebuild it early on. |
| **`rb_spin_once`** | One runtime tick (drain RX, dispatch, heartbeat). |
| **Wire** | Serial/BLE framing + `RSP_MSG_*` types (`protocol.h`). |

## Symbols by lab

| Lab | Board | Runtime | Wire | Features | Your lab |
|-----|-------|---------|------|----------|----------|
| 01-blink | yes | yes | — | — | yes |

Full maintainer prefix map: private repo `firmware-esp32-s3-dev`
[`docs/NAMING.md`](https://github.com/drsanti/firmware-esp32-s3-dev) /
`API.md` (when you have access).
