# Lab 01 / Step 04 — Split tasks + overwrite queue

**Device lab:** [Spirit Level](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 03](../step-03-busy-loop/) (felt the freeze)

---

## 1. What this step must deliver

- Bubble still tracks IMU/sim tilt; LEVEL near center.
- Motion stays **smooth** again — even though step 03 taught you a chime can
  freeze a single loop.
- **No beep in this step** — we fix the **data path** first (sense ↔ UI).
  Audio returns in step 05 without putting `beep_play` in the UI task.

**Success:** tilt freely; the bubble keeps up. You can explain *why* a length-1
overwrite queue fits “latest tilt.”

---

## 2. Problems this step faces

- Step 03: sense + draw + long work in one loop → UI dies while blocked.
- Sensor and UI often want **different rates** (e.g. sample ~50 Hz, draw ~30 Hz).
- UI needs the **newest** tilt, not a backlog of old samples.

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| `sensor_task` | Owns IMU/sim only; may run on its own period |
| `ui_task` | Owns `spirit_*` only; never calls `beep_play` |
| Queue length **1** + `xQueueOverwrite` | Always keep **latest** tilt; sensor never blocks on “queue full” |
| `xQueueReceive` in UI | Wait for data; draw when a sample arrives |

### Architecture

Sensor and UI decouple; length-1 overwrite always holds the latest tilt.

```
┌─────────────┐  Overwrite   ┌─────────┐  Receive   ┌─────────┐
│sensor_task  │ ───────────► │ tilt x1 │ ─────────► │ ui_task │
│ IMU / sim   │              │  queue  │            │ spirit  │
└─────────────┘              └─────────┘            └─────────┘
```

### Sequence

UI blocks on Receive; sensor never waits on a full queue.

```
sensor_task        tilt x1         ui_task
      |               |               |
      |-- Overwrite ->|               |
      |               |-- Receive --->|
      |               |               |-- spirit_set_accel
      |-- Overwrite ->|               |
      |               |  (drops stale if unread)
      |               |               |
```

**Rejected for continuous tilt:**

| Option | Why not (here) |
|--------|----------------|
| Task notify | “Event happened” — no ax/ay payload |
| Shared `volatile float` | Works for one float but weak ownership / easy races later |
| Long queue | Backlog of **old** tilts — bubble lags behind the real world |

---

## 4. Activate ESP-IDF, then build

```powershell
. <IDF_PATH>\export.ps1
```

```bash
cd labs/device-labs
# Lab: .\run 01  |  Step: ex01.c

.\run 01   # from labs/device-labs (step still in ex01.c)
# or
idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx monitor
```

---

## 5. Student sketch (`main/main.c`)

```c
typedef struct { float ax, ay; } tilt_sample_t;
static QueueHandle_t g_tilt;

static void sensor_task(void *arg)
{
    tilt_sample_t sample;
    /* ... read imu or sim into acc ... */
    sample.ax = acc[0];
    sample.ay = acc[1];
    (void)xQueueOverwrite(g_tilt, &sample);
    vTaskDelay(pdMS_TO_TICKS(20));
}

static void ui_task(void *arg)
{
    tilt_sample_t sample;
    spirit_init();
    spirit_start();
    spirit_set_live(1);
    for (;;) {
        if (xQueueReceive(g_tilt, &sample, portMAX_DELAY) == pdTRUE) {
            spirit_set_accel_mps2(sample.ax, sample.ay);
        }
    }
}

void app_main(void)
{
    bsp_init();
    g_tilt = xQueueCreate(1, sizeof(tilt_sample_t));
    (void)xTaskCreate(sensor_task, "sensor", 4096, NULL, 5, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
```

### How the code works

**`app_main`.**  
Creates a queue that holds **one** `tilt_sample_t`, then starts sensor and UI.
The queue must exist before either task runs.

**`sensor_task`.**  
Reads IMU (or sim), packs `ax`/`ay`, and `xQueueOverwrite`s into `g_tilt`.
Overwrite on a length-1 queue replaces any unread sample — the UI will never
draw a stale backlog. Then `vTaskDelay(20)` (~50 Hz).

**`ui_task`.**  
Sets up the spirit face once, then blocks on `xQueueReceive` until a sample
arrives, updates the bubble, and waits again. This task never plays sound.

---

## 6. FreeRTOS in depth — queues for a data stream

### `xQueueCreate(1, sizeof(tilt_sample_t))`

| Argument | Meaning |
|----------|---------|
| `1` | Capacity: one slot (required for `xQueueOverwrite`) |
| `sizeof(...)` | Bytes copied per send/receive |

The queue **copies** the struct (by value). No shared pointer lifetime issues
for this small sample.

### `xQueueOverwrite` vs `xQueueSend`

| API | On full queue | Fit for spirit level |
|-----|--------------|----------------------|
| `xQueueSend` | Block or fail — keeps older items if length > 1 | Bad for “latest tilt” |
| `xQueueOverwrite` | Replaces the single item; never waits for space | Perfect for newest sample |

Use overwrite when **new data makes old data useless** (live instruments).

### `xQueueReceive(..., portMAX_DELAY)`

| Piece | Meaning |
|-------|---------|
| `&sample` | Where to copy the item |
| `portMAX_DELAY` | Wait forever for the next sample |

UI sleeps with almost no CPU until the sensor posts. That is better than
polling a shared variable in a tight loop.

### Why notify is the wrong tool for tilt

Lab 00 notify = “Beep now” / “BOOT pressed” — a **count**, not `{ax, ay}`.
You could notify and then read a global, but then you reinvent a racy queue.
A real queue makes the data path explicit.

### Why this fixes step 03’s freeze (for the UI path)

Even if a **future** audio task blocks on `beep_play`, `ui_task` can keep
Receiving and drawing — as long as audio is not *this* task. Step 05 adds
that audio path with notify; the queue stays for tilt.

### Mental model

> **Stream of measurements** → length-1 overwrite queue → UI.  
> **Discrete events** (LEVEL entered) → notify → audio (next step).  
> Different jobs, different FreeRTOS tools.

---

## 7. Checkpoint

- [ ] Can name what each task owns (sensor vs UI)  
- [ ] Can explain why queue length is 1 and why Overwrite  
- [ ] Can say why notify alone is a poor fit for ax/ay  
- [ ] Bubble stays smooth while tilting  
- [ ] Works on your board (`ws28p` or `es3c28p`)  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Blank LCD | `ui_task` must call `spirit_start`; create queue before tasks |
| Bubble stuck at center | Sensor not posting; check IMU/sim path |
| Build error on `xQueueOverwrite` | Include `freertos/queue.h`; queue length must be 1 |

---

## 9. Next

[Step 05 — LEVEL cue](../step-05-level-cue/) (notify + audio; queue stays for tilt).
