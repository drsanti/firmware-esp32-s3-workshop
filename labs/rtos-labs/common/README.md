# rtos-labs / common

Shared helpers for **this** exercise app (not workshop `labs/common/`).

| Header | Role |
|--------|------|
| [`ex_terms.h`](ex_terms.h) | Multi-pane `rb_term` grid (`ex_terms_begin` / `ex_terms_get` / `ex_terms_log`) |
| [`ex_imu_panel.h`](ex_imu_panel.h) | Instrument UI: `rb_imu_readout` + `|a|` gauge + footer |

CMake GLOBs `common/*.c` and adds this folder to include paths.
