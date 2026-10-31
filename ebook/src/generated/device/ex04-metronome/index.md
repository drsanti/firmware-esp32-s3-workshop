# ex04 — Metronome

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

```text
rbdev run 04
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex04-metronome/README.md</code></p>

</div>


**Lab switch:** `rbdev run 04` (or `main/main.c` → `ex04_start()`)  
**Step switch:** `ex04.c` → one `ex04_stepMM_start()`

| Start | Folder |
|-------|--------|
| `ex04_step01_start()` | `steps/step-01-bpm-face/` |

| `ex04_step02_start()` | `steps/step-02-ui-click/` |

| `ex04_step03_start()` | `steps/step-03-feel-jitter/` |

| `ex04_step04_start()` | `steps/step-04-tick-task/` |

| `ex04_step05_start()` | `steps/step-05-bpm-control/` |

Default in `ex04.c` is the **last** step (finished device).

```powershell
# from labs/device-labs (after rbdev config --port COMx):
rbdev run 04
# step still selected in ex04-metronome/ex04.c
```

---

**Build a BPM clicker** → feel timing jitter from `vTaskDelay` after work →
learn **`vTaskDelayUntil`** in a dedicated tick task.

Work **one step at a time**. Enable it in this lab's `exNN.c`, flash from `labs/device-labs`.

Every step README follows: **deliver → problems → design → Student sketch →
How the code works → FreeRTOS in depth**.

## What the finished device must do

- LCD shows **BPM**, a beat flash, and **jitter** (last / max ms).
- Steady speaker click on the beat.
- **BOOT** steps BPM (both boards; ignore PWR).
- Works on `ws28p` and `es3c28p`.

## Problems along the way

- Need a metronome face (BPM + beat).
- Need a beat period from BPM (`60000 / bpm` ms).
- `vTaskDelay` *after* work makes period = work + delay → jitter.
- Need absolute-period pacing (`vTaskDelayUntil`).
- Need to measure and show jitter; allow live BPM change.

## FreeRTOS teaching set (3)

| Feature | Role |
|---------|------|
| `vTaskDelay` | Early steps; show relative-delay drift |
| `xTaskCreate` | Split tick vs UI |
| `vTaskDelayUntil` | Headline — absolute period |

## Steps (5)

| Step | Folder | Focus | FreeRTOS |
|------|--------|-------|----------|
| 01 | [`step-01-bpm-face`](steps/step-01-bpm-face/) | BPM + beat flash | `vTaskDelay` |
| 02 | [`step-02-ui-click`](steps/step-02-ui-click/) | Beep in UI loop | relative delay |
| 03 | [`step-03-feel-jitter`](steps/step-03-feel-jitter/) | Show jitter ms | measure the slip |
| 04 | [`step-04-tick-task`](steps/step-04-tick-task/) | tick + `DelayUntil` | steady beat |
| 05 | [`step-05-bpm-control`](steps/step-05-bpm-control/) | BOOT ±BPM + max jitter | polish |

## Design arc

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Step 01    │ ──► │  Step 02    │ ──► │  Step 03    │ ──► │  Step 04    │ ──► │  Step 05    │
│  BPM face   │     │ UI + click  │     │   jitter    │     │ tick+Until  │     │  BOOT BPM   │
│ flash only  │     │ vTaskDelay  │     │  measure    │     │ split tasks │     │  + stats    │
└─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘
```

Helper: [`../common/lab_metro.h`](../common/lab_metro.h).

## Prior lab

[Lab 03 — Reaction Game](../ex03-reaction-game/)
