# ex06 — Task Monitor

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

```text
rbdev run 06
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex06-task-monitor/README.md</code></p>

</div>


**Lab switch:** `rbdev run 06` (or `main/main.c` → `ex06_start()`)  
**Step switch:** `ex06.c` → one `ex06_stepMM_start()`

| Start | Folder |
|-------|--------|
| `ex06_step01_start()` | `steps/step-01-name-face/` |

| `ex06_step02_start()` | `steps/step-02-live-names/` |

| `ex06_step03_start()` | `steps/step-03-state-prio/` |

| `ex06_step04_start()` | `steps/step-04-stack-hwm/` |

| `ex06_step05_start()` | `steps/step-05-queue-depth/` |

Default in `ex06.c` is the **last** step (finished device).

```powershell
# from labs/device-labs (after rbdev config --port COMx):
rbdev run 06
# step still selected in ex06-task-monitor/ex06.c
```

---

**Build a live task snapshot screen** → grow columns screen by screen →
learn FreeRTOS **introspection** (`uxTaskGetSystemState`, stack HWM, queue
depth).

Work **one step at a time**. Enable it in this lab's `exNN.c`, flash from `labs/device-labs`.

Labsheet standard: **deliver → problems → design → Student sketch → How the
code works → FreeRTOS in depth**.

## What the finished device must do

- LCD lists running tasks (name, state, priority, stack high-water mark).
- Refresh is a **snapshot**, not a continuous scheduler trace.
- Show a demo queue's **waiting / free** slots.
- **BOOT** pages through the task list (both boards; ignore PWR).
- Works on `ws28p` and `es3c28p`.

## Problems along the way

- Need a compact monitor face (3 rows fit the LCD).
- Need live names from the kernel, not hard-coded strings.
- State + priority make the list readable.
- Stack HWM shows leftover stack (words) per task.
- Queue depth + paging make a longer system observable.

## FreeRTOS teaching set (3)

| Feature | Role |
|---------|------|
| `uxTaskGetSystemState` | Snapshot of tasks (name / state / prio / …) |
| `uxTaskGetStackHighWaterMark` | Minimum free stack left (words) |
| `uxQueueMessagesWaiting` / spaces | Queue fill level |

`vTaskDelay` paces the UI refresh. This lab is about **seeing** the system,
not about a new IPC primitive.

## Steps (5)

| Step | Folder | Focus | FreeRTOS |
|------|--------|-------|----------|
| 01 | [`step-01-name-face`](steps/step-01-name-face/) | Static name rows | `vTaskDelay` |
| 02 | [`step-02-live-names`](steps/step-02-live-names/) | Live names from kernel | `uxTaskGetSystemState` |
| 03 | [`step-03-state-prio`](steps/step-03-state-prio/) | State letter + priority | snapshot fields |
| 04 | [`step-04-stack-hwm`](steps/step-04-stack-hwm/) | Stack high-water mark | `uxTaskGetStackHighWaterMark` |
| 05 | [`step-05-queue-depth`](steps/step-05-queue-depth/) | Queue depth + BOOT page | queue depth APIs |

## Design arc

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Step 01    │ ──► │  Step 02    │ ──► │  Step 03    │ ──► │  Step 04    │ ──► │  Step 05    │
│ name face   │     │ live names  │     │ state+prio  │     │ stack HWM   │     │ Q depth +   │
│ (static)    │     │ SystemState │     │ columns     │     │ per task    │     │ BOOT page   │
└─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘
```

### Finished architecture

```
┌─────────────┐  snapshot   ┌─────────────┐  draw   ┌─────────────┐
│ worker /    │ ──────────► │  ui_task    │ ──────► │  mon_* LCD  │
│ idle tasks  │             │ SystemState │  2 Hz   │  3 rows     │
└─────────────┘             └──────┬──────┘         └─────────────┘
┌─────────────┐  Send              │
│ fill_task   │ ──────────► ┌──────┴──────┐
└─────────────┘             │  demo queue │
                            │ depth title │
                            └─────────────┘
```

Helper: [`../common/lab_mon.h`](../common/lab_mon.h).

## Prior lab

[Lab 05 — Vibration Scope](../ex05-vibration-scope/)
