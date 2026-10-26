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

Demos 01–09 and 11 use **Board + Features (UI) + Your demo**. Demos
10 / 12 / 13 add **Runtime + Wire** (`rb_node`, `RSP_MSG_*`,
`rb_cmd_*` / `rb_param_*`).

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
| **`rb_display_*` / `rb_screen_*` / `rb_layout_*` / `rb_label_*` / `rb_led_*` / `rb_switch_*` / `rb_button_*` / `rb_slider_*` / `rb_checkbox_*` / `rb_radio_*` / `rb_gauge_*` / `rb_term_*`** | LCD UI tree leaves. |
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
| 08-radio | yes | — | — | `rb_screen` + `rb_radio_*` (2 groups) | yes |
| 09-form | yes | — | — | mixed UI leaves (composition) | yes |
| 10-log | yes | `rb_node` / `rb_log` / pub | `RSP_MSG_*` | `rb_cmd_*` + `rb_term` | yes |
| 11-gauge | yes | — | — | `rb_screen` + `rb_gauge_*` (2×3 breathe) | yes |
| 12-param | yes | `rb_node` | `RSP_MSG_CMD_PARAM` | `rb_param_*` + buttons + term | yes |
| 13-topics-ui | yes | `rb_node` / pub | `RSP_MSG_IMU` | slider + gauge bands | yes |

Full maintainer prefix map: private repo `firmware-esp32-s3-dev`
[`docs/NAMING.md`](https://github.com/drsanti/firmware-esp32-s3-dev) /
`API.md` (when you have access).
