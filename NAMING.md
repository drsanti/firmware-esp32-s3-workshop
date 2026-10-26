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
| 5 | **Your demo** | `main/main.c` | The code you write |

Demos 01–02 use **Board + Features (UI) + Your demo**. Wire and Runtime
appear when a demo README lists them.

## Glossary

| Term | One-liner |
|------|-----------|
| **robotos** | Product / middleware name (host tools + MCU library). |
| **`rb_*`** | Runtime (and Feature) C API — short for robotos. |
| **`bsp_*`** | Board HAL — init MCU, drive LED, UART, time. |
| **board SKU** | Which PCB via `-DBOARD=`: **`ws28p`** or **`es3c28p`**. |
| **`librobotos_fw.a`** | Prebuilt library; demos link it, they do not rebuild it early on. |
| **`rb_spin_once`** | One runtime tick (drain RX, dispatch, heartbeat). |
| **`rb_style_*` + `rb_*_set_style`** | Shared semantic colors (`PRIMARY`…`LIGHT`) for all leaves. |
| **`rb_display_*` / `rb_screen_*` / `rb_layout_*` / `rb_label_*` / `rb_led_*` / `rb_switch_*` / `rb_button_*` / `rb_slider_*` / `rb_checkbox_*` / `rb_term_*`** | LCD UI tree leaves. |
| **Wire** | Serial/BLE framing + `RSP_MSG_*` types (`protocol.h`). |

## Symbols by demo

| Demo | Board | Runtime | Wire | Features | Your demo |
|------|-------|---------|------|----------|-----------|
| 01-label | yes | — | — | `rb_screen` + `rb_label_*` (4×4) | yes |
| 02-led | yes | — | — | `rb_screen` + `rb_led_*` (4×4 breathe) | yes |
| 03-term | yes | — | — | `rb_screen` + `rb_term_*` (3×2 log) | yes |
| 04-switch | yes | — | — | `rb_screen` + `rb_switch_*` (2×2) | yes |
| 05-button | yes | — | — | `rb_screen` + `rb_button_*` (2×2) | yes |
| 06-slider | yes | — | — | `rb_screen` + `rb_slider_*` (nested H/V) | yes |
| 07-checkbox | yes | — | — | `rb_screen` + `rb_checkbox_*` (2×4) | yes |

Full maintainer prefix map: private repo `firmware-esp32-s3-dev`
[`docs/NAMING.md`](https://github.com/drsanti/firmware-esp32-s3-dev) /
`API.md` (when you have access).
