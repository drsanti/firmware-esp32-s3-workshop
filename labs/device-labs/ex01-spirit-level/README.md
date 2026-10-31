# ex01 — Spirit Level

**Lab switch:** `rbdev run 01` (or `main/main.c` → `ex01_start()`)  
**Step switch:** `ex01.c` → one `ex01_stepMM_start()`

| Start | Folder |
|-------|--------|
| `ex01_step01_start()` | `steps/step-01-draw-tube/` |

| `ex01_step02_start()` | `steps/step-02-imu-bubble/` |

| `ex01_step03_start()` | `steps/step-03-busy-loop/` |

| `ex01_step04_start()` | `steps/step-04-split-queue/` |

| `ex01_step05_start()` | `steps/step-05-level-cue/` |

Default in `ex01.c` is the **last** step (finished device).

```powershell
# from labs/device-labs (after rbdev config --port COMx):
rbdev run 01
# step still selected in ex01-spirit-level/ex01.c
```

---

**Build a digital spirit level** → hit a real firmware problem → learn FreeRTOS
design (queue for tilt samples, notify for LEVEL events).

Work **one step at a time**. Enable it in this lab's `exNN.c`, flash from `labs/device-labs`.

Every step README follows the workshop labsheet standard: **what to deliver →
problems → design → sketch → How the code works → FreeRTOS in depth**. Prefer
plain language over opaque codes. See step 01 §6 for the model.

## What the finished device must do

- Graphical spirit-level face on the LCD (large bubble).
- Bubble tracks tilt; **LEVEL** near center.
- Motion stays **smooth** (no long freezes).
- Works on `ws28p` and `es3c28p` (IMU live or sim).
- Soft cue when **entering** LEVEL — without freezing the UI.

## Problems along the way

- Need an instrument UI, not raw ax/ay text only.
- Need a stable accel → bubble mapping.
- Blocking work in one loop starves drawing.
- Sensor rate ≠ UI rate; need the **latest** tilt between tasks.
- LEVEL is an **event**; tilt is a **stream** → different FreeRTOS tools.

## Steps (5)

| Step | Folder | Focus | FreeRTOS |
|------|--------|-------|----------|
| 01 | [`step-01-draw-tube`](steps/step-01-draw-tube/) | Instrument face + fake tilt | `vTaskDelay` |
| 02 | [`step-02-imu-bubble`](steps/step-02-imu-bubble/) | Real/sim IMU → bubble | single-loop contract |
| 03 | [`step-03-busy-loop`](steps/step-03-busy-loop/) | Blocking beep in loop (**broken**) | feel the freeze |
| 04 | [`step-04-split-queue`](steps/step-04-split-queue/) | sensor + UI + overwrite queue | restore smooth tilt |
| 05 | [`step-05-level-cue`](steps/step-05-level-cue/) | LEVEL edge → notify → audio | cue without blocking |

## Design arc

Steps build the instrument, break it on purpose, then split stream vs event:

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Step 01    │ ──► │  Step 02    │ ──► │  Step 03    │ ──► │  Step 04    │ ──► │  Step 05    │
│ fake+face   │     │ IMU bubble  │     │ busy beep   │     │ queue split │     │ LEVEL notify│
└─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘
```

Finished lab (step 05) — tilt stream on a length-1 queue; LEVEL cue on notify:

```
┌─────────────┐  Overwrite   ┌─────────┐  Receive   ┌─────────┐  NotifyGive  ┌─────────────┐
│sensor_task  │ ───────────► │ tilt x1 │ ─────────► │ ui_task │ ───────────► │ audio_task  │
│ IMU / sim   │              │  queue  │            │ spirit  │   on edge    │ BEEP_CHIME  │
└─────────────┘              └─────────┘            └─────────┘              └─────────────┘
```

Shared helper: [`../common/lab_spirit.h`](../common/lab_spirit.h) (`spirit_*` + `rb_level`).

## Build / flash

```powershell
cd labs/device-labs
rbdev run 01
# Lab switch: rbdev run 01 (or main/main.c -> ex01_start())
# Step: ex01-spirit-level/ex01.c -> ex01_stepMM_start()
```

## Prior lab

[Lab 00 — Hello Device](../ex00-hello-device/)
