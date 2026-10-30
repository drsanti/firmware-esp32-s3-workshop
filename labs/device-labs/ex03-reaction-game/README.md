# ex03 — Reaction Game

**Lab switch:** `.\run 03` (or `main/main.c` → `ex03_start()`)  
**Step switch:** `ex03.c` → one `ex03_stepMM_start()`

| Start | Folder |
|-------|--------|
| `ex03_step01_start()` | `steps/step-01-game-face/` |
| `ex03_step02_start()` | `steps/step-02-go-and-tap/` |
| `ex03_step03_start()` | `steps/step-03-busy-wait/` |
| `ex03_step04_start()` | `steps/step-04-timer-go/` |
| `ex03_step05_start()` | `steps/step-05-score-audio/` |

Default in `ex03.c` is the **last** step (finished device).

```powershell
# from labs/device-labs (after .\rb config --port COMx):
.\run 03
# step still selected in ex03-reaction-game/ex03.c
```

---

**Build a reaction timer** → hit a busy-wait freeze → learn a **one-shot
software timer** (plus notify for the GO beep).

Work **one step at a time**. Enable it in this lab's `exNN.c`, flash from `labs/device-labs`.

Every step README follows the workshop labsheet standard: **what to deliver →
problems → design → sketch → How the code works → FreeRTOS in depth**.

## What the finished device must do

- LCD states: **READY / WAIT / GO / RESULT**.
- Random delay before GO; **BOOT** to react (both boards; ignore PWR).
- Show reaction time in ms; track **best**; detect **false start**.
- Soft beep on GO **without** freezing the UI during WAIT.
- Works on `ws28p` and `es3c28p`.

## Problems along the way

- Need a clear game face and state machine.
- Need a fair random wait before GO.
- Busy-spin during WAIT freezes the banner / burns CPU.
- “Fire GO later” needs a one-shot timer, not a blocking delay in the UI path.
- Score / false-start / audio are product rules on top of the timer.

## FreeRTOS teaching set (3)

| Feature | Role in this lab |
|---------|------------------|
| `vTaskDelay` | Pace UI; crude WAIT in step 02 |
| Software timer (`xTimer*`) | One-shot “GO later” without blocking |
| Task notify | GO → audio (reuse from Lab 00/01) |

Queue is **not** the lesson here (Labs 01–02).

## Steps (5)

| Step | Folder | Focus | FreeRTOS |
|------|--------|-------|----------|
| 01 | [`step-01-game-face`](steps/step-01-game-face/) | State banner demo | `vTaskDelay` |
| 02 | [`step-02-go-and-tap`](steps/step-02-go-and-tap/) | Delay-wait + BOOT → ms | tick capture |
| 03 | [`step-03-busy-wait`](steps/step-03-busy-wait/) | Busy-spin WAIT (**broken**) | feel the freeze |
| 04 | [`step-04-timer-go`](steps/step-04-timer-go/) | One-shot timer → GO | **timer** |
| 05 | [`step-05-score-audio`](steps/step-05-score-audio/) | Best / false-start / beep | notify + polish |

## Design arc

```
  Step 01       Step 02       Step 03       Step 04       Step 05
┌─────────┐   ┌─────────┐   ┌─────────┐   ┌─────────┐   ┌─────────┐
│  face   │──►│ delay   │──►│ busy    │──►│ timer   │──►│ score / │
│ states  │   │ + BOOT  │   │ spin    │   │ GO      │   │ audio   │
└─────────┘   └─────────┘   └─────────┘   └─────────┘   └─────────┘
```

## Finished architecture (step 05)

```
┌─────────────┐  NotifyGive   ┌─────────────┐  NotifyGive ┌─────────────┐
│  go timer   │ ─────────────►│   ui task   │────────────►│ audio task  │
│  (one-shot) │               │ game states │             │ BEEP_CLICK  │
└─────────────┘               └──────┬──────┘             └─────────────┘
                                     │
                                     │ react_*
                                     ▼
                              ┌─────────────┐
                              │     LCD     │
                              └─────────────┘
```

Shared helper: [`../common/lab_react.h`](../common/lab_react.h).

## Build / flash

```powershell
cd labs/device-labs
.\run 03
# Lab switch: .\run 03 (or main/main.c -> ex03_start())
# Step: ex03-reaction-game/ex03.c -> ex03_stepMM_start()
```

## Prior lab

[Lab 02 — Motion Alarm](../ex02-motion-alarm/)
