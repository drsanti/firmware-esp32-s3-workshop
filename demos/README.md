# demos — UI / widget / wire catalog

All widget demos live in one ESP-IDF project:

**[`demo-labs/`](demo-labs/)** — switch with **`.\run NN`** (rewrites
`main/main.c`; same pattern as `labs/device-labs` and `labs/rtos-labs`).
Each `exNN-*/README.md` is a textbook labsheet (theory, ASCII architecture,
Mermaid sequence, checklist).

```powershell
cd demos/demo-labs
.\rb doctor
.\rb config --port COMx --board ws28p   # once
.\run 01
.\run 17 monitor
```

| Ex | Demo |
|----|------|
| [`ex01-label`](demo-labs/ex01-label/) | 4×4 `rb_label` grid |
| [`ex02-led`](demo-labs/ex02-led/) | 4×4 `rb_led` breathe |
| [`ex03-term`](demo-labs/ex03-term/) | 3×2 `rb_term` panes |
| [`ex04-switch`](demo-labs/ex04-switch/) | `rb_switch` collage |
| [`ex05-button`](demo-labs/ex05-button/) | `rb_button` normal + toggle |
| [`ex06-slider`](demo-labs/ex06-slider/) | nested `rb_slider` |
| [`ex07-checkbox`](demo-labs/ex07-checkbox/) | `rb_checkbox` |
| [`ex08-radio`](demo-labs/ex08-radio/) | exclusive `rb_radio` groups |
| [`ex09-form`](demo-labs/ex09-form/) | composed form |
| [`ex10-log`](demo-labs/ex10-log/) | wire: node + log + topics |
| [`ex11-gauge`](demo-labs/ex11-gauge/) | `rb_gauge` |
| [`ex12-param`](demo-labs/ex12-param/) | `rb_param` table |
| [`ex13-topics-ui`](demo-labs/ex13-topics-ui/) | slider → gauge + publish |
| [`ex14-progress`](demo-labs/ex14-progress/) | `rb_progress` |
| [`ex15-chart`](demo-labs/ex15-chart/) | `rb_chart` |
| [`ex16-imu-viz`](demo-labs/ex16-imu-viz/) | IMU viz |
| [`ex17-mic-viz`](demo-labs/ex17-mic-viz/) | mic VU + HISTORY |

Details: [demo-labs/README.md](demo-labs/README.md). Student CLI:
[../rbdev/README.md](../rbdev/README.md).
