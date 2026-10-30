# Demo 10 — Log, node, topics, and commands (first wire)

**Goal.** Bring up the **robotos runtime on serial**: an **`rb_node`** named **`demo10`**, framed **log** lines, a once-per-second **topic publish**, and **standard command** subscribers — while mirroring human-readable text on a full-screen **`rb_term`**. This is the workshop’s first **wire** demo after the UI-only chain (ex01–ex09).

## Learning outcomes

1. Open **`rb_serial_transport`**, configure **`rb_node_cfg_t`** (transport, name, **heartbeat_ms = 1000**), and call **`rb_node_init`**.
2. Run **`rb_spin_once`** and **`rb_cmd_poll`** from a dedicated **`spin_task`** so RX/TX and command dispatch stay off the UI loop.
3. Mirror firmware activity locally with **`rb_term_println`** and on the wire with **`rb_log`** via **`rb_cmd_bind_log`**.
4. **Publish** a periodic uplink with **`rb_pub_init`** + **`rb_publish`** (topic id **`RSP_MSG_IMU`**, payload = incrementing **`uint32_t`** tick — a stand-in payload for lab traffic).
5. **Subscribe** to **`RSP_MSG_CMD_LED`** (custom handler), **`RSP_MSG_CMD_REBOOT`** (`rb_cmd_reboot`), and **`RSP_MSG_CMD_TIME`** (`rb_cmd_timesync`).

## Theory — deep detail

**Why a node.** UI demos drew widgets only. Host tools (serial-bridge, class scripts) speak **COBS-framed messages** with topic ids from **`protocol.h`**. An **`rb_node_t`** binds a **transport** (here **`rb_transport_serial_init`**) to pub/sub tables, heartbeats, and logging hooks. Without **`rb_spin_once`**, incoming frames stall; without **`rb_cmd_poll`**, deferred command work never runs.

**Dual observability.** Every interesting event is duplicated on purpose:

| Sink | API | Audience |
|------|-----|----------|
| LCD terminal | `term_log` → `rb_term_println` | You at the bench |
| Wire log topic | `rb_log(&g_node, …)` | Host decoder / bridge |

At boot: **`robotos-fw {version} up`**, **`node=demo10 wire ready`**, then each second **`tick N`** on both sinks.

**Publish path.** `g_tick` increments in the main loop (1 s period). **`rb_publish(&g_tick_pub, &g_tick, sizeof(g_tick))`** emits binary uplink **`RSP_MSG_IMU` (0x20)**. The id name says IMU because the protocol slot is shared across labs; **this demo’s payload is only a 4-byte counter**, not an IMU struct.

**Subscribe path.**

- **`on_led_cmd`**: logs first payload byte and length; prints **`cmd LED 0x.. NB`** on the term.
- **`rb_cmd_reboot`**: standard soft-reset handler (ack then reset — use carefully).
- **`rb_cmd_timesync`**: applies host time sync when a bridge sends **`RSP_MSG_CMD_TIME`**.

**Host interaction.**

- **`idf.py monitor`** / **`.\run 10 monitor`** shows **mixed** human text and binary framing on the debug UART — fine for seeing **`tick`** lines, poor for parsing COBS.
- For **clean decode**, use the workshop **serial-bridge** host tool (see **`docs/embedded/serial-bridge-spec.md`** and workshop [README.md](../../README.md)). Send **`LedCmd` (`RSP_MSG_CMD_LED`, 0x80)** with at least one payload byte to exercise **`on_led_cmd`**.

**UI.** Single **`rb_term`**, scale **1**, cyan on black, full layout — no touch task in this file.

## Architecture

```
┌────────────────────── app_main ──────────────────────┐
│  bsp_init()  →  ex10_start()                         │
└──────────────────────────┬───────────────────────────┘
                           │
    ┌──────────────────────┼──────────────────────┐
    │                      │                      │
    ▼                      ▼                      ▼
┌─────────┐        ┌───────────────┐      ┌──────────────┐
│ rb_term │        │ rb_node       │      │ main loop    │
│ on LCD  │        │ demo10        │      │ 1 s: tick++  │
└────▲────┘        │ serial xport  │      │ publish+log  │
     │              │ heartbeat 1s  │      └──────┬───────┘
     │ term_log     │ pub IMU tick  │             │
     └──────────────│ sub LED/TIME/ │◄────────────┘
                    │     REBOOT    │
                    └───────┬───────┘
                            │
                    ┌───────▼───────┐
                    │ spin_task     │
                    │ spin_once +   │
                    │ cmd_poll 10ms │
                    └───────┬───────┘
                            │
                    ┌───────▼───────┐
                    │ Host / UART   │
                    │ serial-bridge │
                    └───────────────┘
```

## Sequence

```mermaid
sequenceDiagram
    participant Loop as main loop (1 s)
    participant Node as rb_node
    participant Term as rb_term
    participant Spin as spin_task
    participant Host

    Loop->>Node: rb_publish(tick) RSP_MSG_IMU
    Loop->>Node: rb_log("tick N")
    Loop->>Term: term_log("tick N")

    Spin->>Node: rb_spin_once()
    Spin->>Node: rb_cmd_poll()

    Host->>Node: LedCmd (0x80)
    Node->>Node: on_led_cmd
    Node->>Node: rb_log(...)
    Node->>Term: term_log("cmd LED ...")

    Host->>Node: TimeSync / RebootCmd
    Node->>Node: rb_cmd_timesync / rb_cmd_reboot
```

## APIs this demo uses

| Layer | You will call |
|-------|----------------|
| Board | `bsp_init` |
| UI | `rb_term_init`, `rb_term_set_scale`, `rb_term_set_color`, `rb_term_println`, `rb_screen_*`, `rb_layout_*` |
| Transport | `rb_transport_serial_init`, `rb_serial_transport_t` |
| Runtime | `rb_node_init`, `rb_spin_once`, `rb_log`, `rb_fw_version` |
| Pub/sub | `rb_pub_init`, `rb_publish`, `rb_sub_init` |
| Commands | `rb_cmd_bind_log`, `rb_cmd_poll`, `rb_cmd_reboot`, `rb_cmd_timesync` |
| Protocol ids | `RSP_MSG_IMU`, `RSP_MSG_CMD_LED`, `RSP_MSG_CMD_REBOOT`, `RSP_MSG_CMD_TIME` |
| RTOS | `xTaskCreate`, `vTaskDelay` |

Wire ids and framing: **`protocol.h`** (header comment references `docs/embedded/serial-bridge-spec.md` in the full robotos tree). Host setup: workshop [README.md](../../README.md).

## Prerequisites

- Completed [Demo 09 — Form](../ex09-form/README.md) (UI composition)
- `librobotos_fw.a` with wire stack enabled
- USB cable to the board’s **USB-JTAG / serial** port
- Optional but recommended: **serial-bridge** host tool for framed RX/TX and **LedCmd** injection

## Build / flash

```bash
cd demos/demo-labs
.\run 10
# Optional: .\run 10 monitor
```

## Success + checklist

- [ ] LCD **`rb_term`** scrolls **`robotos-fw …`**, **`node=demo10 wire ready`**, then **`tick 1`**, **`tick 2`**, … every second.
- [ ] With serial-bridge attached, decoded **LOG** lines and **0x20** uplinks appear once per second.
- [ ] Host **LedCmd** → term line **`cmd LED 0x.. NB`** and a matching **`rb_log`** on the wire.
- [ ] **`spin_task`** running: link stays responsive while ticks continue (no stuck RX).
- [ ] Understand: **`RSP_MSG_IMU`** here carries the **tick counter**, not real IMU samples (see Demo 16+ for IMU).

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex10.h"`
2. `ex10_start();`

Or: `.\run 10` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.

## Previous / Next

- **Previous:** [Demo 09 — Form](../ex09-form/README.md)
- **Next:** [Demo 11 — Gauges](../ex11-gauge/README.md)
