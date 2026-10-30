# Demo 10 — Log / topics / cmds (first wire)

First **Runtime + Wire** demo. LCD shows an `rb_term` mirror; USB-JTAG
carries the same COBS frames a host bridge would see.

| Layer | What this demo uses |
|-------|---------------------|
| Runtime | `rb_node_*`, `rb_spin_once`, `rb_log`, `rb_pub_*` |
| Wire | `RSP_MSG_LOG`, heartbeat, `RSP_MSG_IMU` topic, `RSP_MSG_CMD_LED` |
| Features | `rb_cmd_*` (timesync + reboot) |
| UI | `rb_term` (local mirror only) |

## Build / flash

```bash
cd demos/demo-labs
.\run 10
```

`idf.py monitor` shows raw UART/JTAG text mixed with framed traffic.
For clean COBS decode use the robotos serial-bridge host tool.

## Success

Term scrolls `tick N` lines; every second a log + IMU-sized topic
publish leave the board. Host `LedCmd` (0x80) prints on the term.

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex10.h"`
2. `ex10_start();`

Or: `.\run 10` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
