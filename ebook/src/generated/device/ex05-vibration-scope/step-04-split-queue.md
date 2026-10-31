# Lab 05 / Step 04 — Split + sample queue

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

```text
rbdev run 05
# step switch in ex05-vibration-scope.c:  ex05_step04_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex05-vibration-scope/steps/step-04-split-queue/README.md</code></p>

</div>


**Device lab:** [Vibration Scope](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 03](../step-03-ui-100hz/)

---

## 1. What this step must deliver

- `sensor_task` samples ~100 Hz and `xQueueSend`s `ax`.
- `ui_task` runs ~30 Hz: **drains** the queue and `scope_push`es.
- Chart looks smooth again while sensing stays fast.

**Success:** live strip without the step-03 slog.

---

## 2. Problems this step faces

- Need different rates for produce vs draw.
- Need a short backlog (not Lab 01’s “latest only”).

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| Queue length **64** | Hold ~0.6 s at 100 Hz if UI pauses briefly |
| `Send(..., 0)` | Drop rather than block sensor |
| UI drain with `Receive(..., 0)` | Batch-draw then delay ~33 ms |
| Sensor prio ≥ UI | Prefer filling the buffer |

### Architecture

```
┌─────────────┐   Send    ┌─────────────┐  drain   ┌─────────────┐
│sensor_task  │ ────────► │ sample Q    │ ───────► │  ui_task    │
│ IMU @100Hz  │           │  ax x64     │ @30Hz    │ rb_chart    │
└─────────────┘           └─────────────┘          └─────────────┘
```

### Sequence

```
sensor_task         sample Q          ui_task
      |                |                 |
      |-- read IMU     |                 |
      |-- Send ------->|                 |
      |-- delay 10ms   |                 |
      |-- Send ------->|                 |
      |                |-- Receive batch>|
      |                |                 |-- scope_push x N
      |                |                 |-- delay 33ms
      |                |                 |
```

**Rejected:** len-1 overwrite (Lab 01) — UI would see only the newest sample
per frame and throw away vibration detail between draws.

---

## 4. Build

```bash
cd labs/device-labs
# Lab: rbdev run 05  |  Step: ex05.c
rbdev run 05   # from labs/device-labs (step still in ex05.c)
```

---

## 5. Student sketch (`main/main.c`)

```c
static QueueHandle_t g_samples;

static void sensor_task(void *arg)
{
    float ax;
    for (;;) {
        /* ... read imu/sim into acc ... */
        ax = acc[0];
        (void)xQueueSend(g_samples, &ax, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void ui_task(void *arg)
{
    float ax;
    scope_init();
    scope_start();
    for (;;) {
        while (xQueueReceive(g_samples, &ax, 0) == pdTRUE) {
            scope_push(ax);
        }
        vTaskDelay(pdMS_TO_TICKS(33));
    }
}

void app_main(void)
{
    bsp_init();
    g_samples = xQueueCreate(64, sizeof(float));
    (void)xTaskCreate(sensor_task, "sensor", 4096, NULL, 6, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
```

### How the code works

**`app_main`.**  
Creates a float queue (64 slots), then sensor (prio 6) and UI (prio 5).

**`sensor_task`.**  
~100 Hz: read IMU/sim, `Send` `ax` with timeout 0 (never block on a full
queue).

**`ui_task`.**  
Each frame: drain all pending samples into `scope_push`, then
`vTaskDelay(33)` (~30 Hz). Drawing happens in bursts of several columns, not
100 separate full-loop iterations with long stalls.

---

## 6. FreeRTOS in depth — stream queue vs Lab 01/02

| | Lab 01 tilt | Lab 02 alarm | Lab 05 scope |
|--|-------------|--------------|--------------|
| Item | `{ax,ay}` | event | `float ax` |
| Length | **1** overwrite | **4** events | **64** samples |
| Goal | Latest only | A few cues | Short backlog |

### Drain pattern

```c
while (xQueueReceive(q, &ax, 0) == pdTRUE) {
    scope_push(ax);
}
```

Timeout `0` = non-blocking. Empty → stop draining. That batches work for one
UI frame.

### Mental model

> **Fast producer** → queue → **slower consumer**.  
> Different rates, one buffer.

---

## 7. Checkpoint

- [ ] Smooth strip vs step 03  
- [ ] Can explain why length > 1  
- [ ] Can explain drain + 30 Hz delay  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Empty chart | Queue create / Send path |
| Still laggy | UI still pushing inside sensor — split tasks |
| Build target wrong | `set-target esp32s3` |

---

## 9. Next

[Step 05 — Drop stats](../step-05-drop-stats/).
