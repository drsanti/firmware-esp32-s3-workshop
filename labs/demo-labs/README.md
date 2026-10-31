# demo-labs

Lean **UI / widget / wire catalog** — one ESP-IDF project, many `exNN-*`
demos. Same switch pattern as `labs/device-labs` and `labs/rtos-labs`.

Each `exNN-*/README.md` is a **textbook labsheet**: learning outcomes,
in-depth theory, ASCII architecture, Mermaid sequence diagram, API table,
build/flash, and checklist. Read the labsheet before you flash.

## Switch demo

Use **`rbdev run NN`** (rewrites `main/main.c`), or edit includes / `exNN_start()` by hand.

## Build / flash

Once per PC (workshop root): `rbdev init` — see [CLASSROOM-SETUP.md](../../CLASSROOM-SETUP.md).

From this folder:

```powershell
rbdev doctor
rbdev run 01          # ex01-label — select + build + flash
rbdev run 17          # ex17-mic-viz
rbdev run 01 --monitor
```

After adding a new `exNN-*` folder or `.c` file:

```powershell
rbdev reconfigure --board ws28p
```

If you see `No module named 'rich_click'`, re-run `rbdev init` from the
workshop root — [../../rbdev/README.md](../../rbdev/README.md) ·
[CLASSROOM-SETUP.md](../../CLASSROOM-SETUP.md).

## Demos

| [`ex01-label`](ex01-label/) | 4x4 rb_label grid via rb_screen / rb_layout |
| [`ex02-led`](ex02-led/) | 4x4 rb_led grid with intensity breathe |
| [`ex03-term`](ex03-term/) | 3x2 rb_term panes — six logging FreeRTOS tasks |
| [`ex04-switch`](ex04-switch/) | 2x2 rb_switch collage (pill/rect x ON/OFF) |
| [`ex05-button`](ex05-button/) | 2x2 rb_button — normal + toggle with indicator |
| [`ex06-slider`](ex06-slider/) | Nested layout rb_slider — V inputs drive H progress |
| [`ex07-checkbox`](ex07-checkbox/) | 2x4 rb_checkbox — styles + paired on_change |
| [`ex08-radio`](ex08-radio/) | Two exclusive rb_radio groups (Mode / Speed) |
| [`ex09-form`](ex09-form/) | Composed form — radio + switch + checkbox + slider + button |
| [`ex10-log`](ex10-log/) | First wire: rb_node + rb_log + topic pub + rb_cmd_* |
| [`ex11-gauge`](ex11-gauge/) | 2x3 rb_gauge — styles + bands + breathe |
| [`ex12-param`](ex12-param/) | rb_param table — local +/- and host ParamCmd |
| [`ex13-topics-ui`](ex13-topics-ui/) | Slider to gauge + rb_publish (topics meet UI) |
| [`ex14-progress`](ex14-progress/) | H/V rb_progress pills — bands + breathe |
| [`ex15-chart`](ex15-chart/) | Multi-pane rb_chart — SCROLL + HISTORY |
| [`ex16-imu-viz`](ex16-imu-viz/) | rb_level + rb_imu_readout + Accel HISTORY |
| [`ex17-mic-viz`](ex17-mic-viz/) | rb_vu dBFS + mic HISTORY (mic_sim if no mic) |

Shared board HAL and `librobotos_fw.a` come from the workshop root.
