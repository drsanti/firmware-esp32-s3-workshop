# ex02 — Motion Alarm

**Lab switch:** `.\run 02` (or `main/main.c` → `ex02_start()`)  
**Step switch:** `ex02.c` → one `ex02_stepMM_start()`

| Start | Folder |
|-------|--------|
| `ex02_step01_start()` | `steps/step-01-armed-face/` |
| `ex02_step02_start()` | `steps/step-02-shake-detect/` |
| `ex02_step03_start()` | `steps/step-03-busy-alarm/` |
| `ex02_step04_start()` | `steps/step-04-queue-audio/` |
| `ex02_step05_start()` | `steps/step-05-arm-disarm/` |

Default in `ex02.c` is the **last** step (finished device).

```powershell
# from labs/device-labs (after .\rb config --port COMx):
.\run 02
# step still selected in ex02-motion-alarm/ex02.c
```

---

**Build a shake alarm** → hit a blocking-beep freeze → learn an **event queue**
to audio (and arm/disarm).

Work **one step at a time**. Enable it in this lab's `exNN.c`, flash from `labs/device-labs`.

Every step README follows the workshop labsheet standard: **what to deliver →
problems → design → sketch → How the code works → FreeRTOS in depth**. Prefer
plain language. Lab 01 is the model for depth.

## What the finished device must do

- LCD shows **IDLE / ARMED / ALARM**.
- When **ARMED**, a shake (IMU live or sim) trips the alarm.
- Alarm sound plays **without freezing** the status UI.
- **BOOT** arms and disarms (both boards).
- Works on `ws28p` and `es3c28p`.

## Problems along the way

- Need a clear alarm face, not raw accel numbers.
- Need a shake definition (threshold + rising edge).
- Blocking `beep_play` in the detect loop freezes the UI.
- Alarm is an **event** → short queue to an audio task (not a tilt stream).
- Ignore shakes when IDLE; cooldown while sounding.

## FreeRTOS teaching set (3)

| Feature | Role in this lab |
|---------|------------------|
| `vTaskDelay` | Pace UI / sensor loop |
| `xTaskCreate` | Separate detect/UI from audio |
| Queue (`Send` / `Receive`) | Alarm **events** → audio |

Notify is **not** the lesson here (Lab 00/01 already covered it).

## Steps (5)

| Step | Folder | Focus | FreeRTOS |
|------|--------|-------|----------|
| 01 | [`step-01-armed-face`](steps/step-01-armed-face/) | Alarm face + idle pulse | `vTaskDelay` |
| 02 | [`step-02-shake-detect`](steps/step-02-shake-detect/) | IMU/sim → ALARM banner | single-loop contract |
| 03 | [`step-03-busy-alarm`](steps/step-03-busy-alarm/) | Beep in loop (**broken**) | feel the freeze |
| 04 | [`step-04-queue-audio`](steps/step-04-queue-audio/) | Event queue → audio task | restore smooth UI |
| 05 | [`step-05-arm-disarm`](steps/step-05-arm-disarm/) | BOOT arm/disarm + cooldown | product rules |

## Design arc

```
  Step 01       Step 02       Step 03       Step 04       Step 05
┌─────────┐   ┌─────────┐   ┌─────────┐   ┌─────────┐   ┌─────────┐
│  face   │──►│ shake   │──►│ busy    │──►│ queue   │──►│  BOOT   │
│ ARMED   │   │ detect  │   │ beep    │   │ + audio │   │ arm/dis │
└─────────┘   └─────────┘   └─────────┘   └─────────┘   └─────────┘
```

## Finished architecture (step 05)

```
┌─────────────┐  xQueueSend   ┌─────────────┐  Receive  ┌─────────────┐
│   detect    │ ─────────────►│  alarm evt  │──────────►│    audio    │
│    task     │               │ queue (x4)  │           │ beep_play   │
└──────┬──────┘               └─────────────┘           └─────────────┘
       │
       │ alarm_*
       ▼
┌─────────────┐
│     LCD     │
│ IDLE/ARMED/ │
│    ALARM    │
└─────────────┘
```

Shared helper: [`../common/lab_alarm.h`](../common/lab_alarm.h) (`alarm_*` + `lab_lcd`).

## Build / flash

```powershell
cd labs/device-labs
.\run 02
# Lab switch: .\run 02 (or main/main.c -> ex02_start())
# Step: ex02-motion-alarm/ex02.c -> ex02_stepMM_start()
```

## Prior lab

[Lab 01 — Spirit Level](../ex01-spirit-level/)
