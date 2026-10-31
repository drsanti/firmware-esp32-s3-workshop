# Lab 05 / Step 05 — Drop stats

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

<span class="ebook-badge ok">finished device</span>

```text
rbdev run 05
# step switch in ex05-vibration-scope.c:  ex05_step05_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex05-vibration-scope/steps/step-05-drop-stats/README.md</code></p>

</div>


**Device lab:** [Vibration Scope](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 04](../step-04-split-queue/)

---

## 1. What this step must deliver

- Same producer/consumer split as step 04.
- Shorter queue (**32**) so overflows are visible under load.
- Status line: approx input Hz + **cumulative drops**.
- Sensor `Send` failure increments `drops`.

**Success:** status like `in~100Hz drop N`; chart still usable.

---

## 2. Problems this step faces

- Silent `Send(..., 0)` failures hide backlog pressure.
- Need a simple way to see produce rate vs loss.

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| Count failed `Send` | Drops are explicit |
| Count successful pushes | Approx Hz over 1 s |
| Queue len 32 | Easier to observe drops than 64 |

### Architecture

Non-blocking Send exposes backlog pressure as counters.

```
┌─────────────┐   Send    ┌─────────────┐  drain   ┌─────────────┐
│sensor_task  │ ────────► │ sample Q    │ ───────► │  ui_task    │
│ IMU @100Hz  │           │    x32      │ @30Hz    │ show stats  │
└─────────────┘           └─────────────┘          └─────────────┘
```

### Sequence

```
sensor_task         sample Q           ui_task
      |                 |                 |
      |-- read IMU      |                 |
      |-- Send -------->|                 |
      |-- pushed++      |                 |
      |-- Send fail     |                 |
      |-- drops++       |                 |
      |                 |-- Receive batch>|
      |                 |                 |-- scope_push
      |                 |                 |-- status line
      |                 |                 |
```

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
typedef struct {
    volatile unsigned drops;
    volatile unsigned pushed;
} scope_stats_t;

static scope_stats_t g_stats;

/* sensor: */
if (xQueueSend(g_samples, &ax, 0) != pdTRUE) {
    g_stats.drops++;
} else {
    g_stats.pushed++;
}

/* ui, once per second: */
hz = g_stats.pushed - last_pushed;
last_pushed = g_stats.pushed;
scope_set_status("%uHz d%u", hz, g_stats.drops);  /* title is short */
```

```c
void app_main(void)
{
    bsp_init();
    g_samples = xQueueCreate(32, sizeof(float));
    (void)xTaskCreate(sensor_task, "sensor", 4096, NULL, 6, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
```

### How the code works

**Sensor.**  
On each sample, try non-blocking `Send`. Failure means the queue was full —
increment `drops` instead of blocking (which would slow sensing).

**UI.**  
Same drain + 30 Hz delay as step 04. About once a second, compute how many
successful pushes happened since last report (`hz`) and show cumulative drops.

**`app_main`.**  
Smaller queue (32) than step 04 so students can provoke / observe loss without
waiting forever.

---

## 6. FreeRTOS in depth — full queues are data

| Policy | Behavior |
|--------|----------|
| `Send(..., portMAX_DELAY)` | Sensor blocks — can distort timing |
| `Send(..., 0)` + count | Keep sensing; **admit** loss |

Drops are not “FreeRTOS failing” — they are backpressure. Fix by faster UI,
larger queue, or accepting loss for live scopes.

### Lab 05 feature recap

| Feature | Role |
|---------|------|
| `vTaskDelay` | 100 Hz sensor / 30 Hz UI |
| Tasks | Producer vs consumer |
| Queue | Sample stream between them |

---

## 7. Checkpoint

- [ ] Status shows Hz and drops  
- [ ] Can explain why `Send(..., 0)` can fail  
- [ ] Can contrast Lab 01 overwrite vs this backlog queue  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Always drop 0 | Queue large / UI keeping up — OK |
| Hz ~0 | Sensor not running / wrong board |
| Wrong chip GPIOs | `set-target esp32s3` |

---

## 9. Next

Lab 05 complete. Return to [Lab 05 overview](../README.md) or continue at
[Lab 06 — Task Monitor](../../ex06-task-monitor/).
