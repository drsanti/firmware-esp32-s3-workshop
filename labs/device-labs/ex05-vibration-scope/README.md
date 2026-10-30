# ex05 — Vibration Scope

**Lab switch:** `.\run 05` (or `main/main.c` → `ex05_start()`)  
**Step switch:** `ex05.c` → one `ex05_stepMM_start()`

| Start | Folder |
|-------|--------|
| `ex05_step01_start()` | `steps/step-01-scope-face/` |
| `ex05_step02_start()` | `steps/step-02-imu-scope/` |
| `ex05_step03_start()` | `steps/step-03-ui-100hz/` |
| `ex05_step04_start()` | `steps/step-04-split-queue/` |
| `ex05_step05_start()` | `steps/step-05-drop-stats/` |

Default in `ex05.c` is the **last** step (finished device).

```powershell
# from labs/device-labs (after .\rb config --port COMx):
.\run 05
# step still selected in ex05-vibration-scope/ex05.c
```

---

**Build a live accel stripchart** → hit “draw every sample @100 Hz” lag →
learn a **sample queue** (producer @100 Hz / consumer UI @30 Hz).

Work **one step at a time**. Enable it in this lab's `exNN.c`, flash from `labs/device-labs`.

Labsheet standard: **deliver → problems → design → Student sketch → How the
code works → FreeRTOS in depth**.

## What the finished device must do

- HISTORY chart of horizontal accel (`ax`).
- Smooth UI (~30 Hz draw) while sensing faster (~100 Hz).
- Show rate / dropped-sample stats when the queue fills.
- Works on `ws28p` and `es3c28p` (IMU live or sim).

## Problems along the way

- Need a scope face (`rb_chart` HISTORY).
- Need real/sim IMU into the strip.
- Drawing **every** sample at 100 Hz freezes / lags the UI.
- Sensor rate ≠ UI rate → buffer samples between tasks.
- Need visibility when the buffer overflows (drops).

## FreeRTOS teaching set (3)

| Feature | Role |
|---------|------|
| `vTaskDelay` | Pace sensor / UI loops |
| `xTaskCreate` | Split sensor vs UI |
| Queue (`Send` / `Receive`) | Sample **stream** buffer (not overwrite-1) |

Contrast Lab 01 (len-1 overwrite = latest tilt) and Lab 02 (short event queue).

## Steps (5)

| Step | Folder | Focus | FreeRTOS |
|------|--------|-------|----------|
| 01 | [`step-01-scope-face`](steps/step-01-scope-face/) | Fake sine strip | `vTaskDelay` |
| 02 | [`step-02-imu-scope`](steps/step-02-imu-scope/) | IMU → chart ~50 Hz | single-loop OK |
| 03 | [`step-03-ui-100hz`](steps/step-03-ui-100hz/) | Sample+draw @100 Hz (**broken**) | feel the lag |
| 04 | [`step-04-split-queue`](steps/step-04-split-queue/) | sensor@100 / UI@30 + queue | producer/consumer |
| 05 | [`step-05-drop-stats`](steps/step-05-drop-stats/) | Show Hz + drops | polish |

## Design arc

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Step 01    │ ──► │  Step 02    │ ──► │  Step 03    │ ──► │  Step 04    │ ──► │  Step 05    │
│ fake sine   │     │ IMU loop    │     │ 100Hz BAD   │     │ queue split │     │ drop stats  │
│ HISTORY     │     │  ~50 Hz OK  │     │ sample+draw │     │ 100 to 30   │     │ on overflow │
└─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘
```

Helper: [`../common/lab_scope.h`](../common/lab_scope.h).

## Prior lab

[Lab 04 — Metronome](../ex04-metronome/)

## Next lab

[Lab 06 — Task Monitor](../ex06-task-monitor/)
