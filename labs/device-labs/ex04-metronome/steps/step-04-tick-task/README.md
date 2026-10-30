# Lab 04 / Step 04 — Tick task + `vTaskDelayUntil`

**Device lab:** [Metronome](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 03](../step-03-feel-jitter/)

---

## 1. What this step must deliver

- `tick_task` owns the beat: click + **`vTaskDelayUntil`**.
- `ui_task` only displays BPM / flash / jitter.
- Jitter drops vs step 03 (beat schedule compensates for click time).

**Success:** clicks sound steadier; max jitter much smaller.

---

## 2. Problems this step faces

- UI work must not define the beat schedule.
- Need absolute-period pacing, not “delay from now.”

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| `tick_task` at higher priority | Beat work first |
| `vTaskDelayUntil(&last, period)` | Wake on a fixed schedule |
| Shared `volatile` stats | UI reads beat count / jitter |
| UI never calls `beep_play` | Display only |

### Architecture

The beat timetable lives in `tick_task`; UI only mirrors state.

```
┌─────────────┐   beat++   ┌─────────────┐   poll   ┌─────────────┐
│  tick_task  │ ─────────► │  g_stats    │ ◄─────── │   ui_task   │
│ DelayUntil  │            │ beat/jitter │          │ flash / LCD │
└─────────────┘            └─────────────┘          └─────────────┘
```

### Sequence

```
tick_task              ui_task
    |                      |
    |-- beep_play          |
    |-- DelayUntil         |
    |-- beat++             |
    |                      |-- sees beat change
    |                      |-- flash + jitter
    |-- beep_play          |-- vTaskDelay (UI only)
    |-- DelayUntil         |
    |                      |
```

---

## 4. Build

```bash
cd labs/device-labs
# Lab: .\run 04  |  Step: ex04.c
.\run 04   # from labs/device-labs (step still in ex04.c)
```

---

## 5. Student sketch (`main/main.c`)

```c
static metro_stats_t g_stats;
static unsigned g_period_ms;

static void tick_task(void *arg)
{
    TickType_t last_wake = xTaskGetTickCount();
    TickType_t period_ticks = pdMS_TO_TICKS(g_period_ms);

    for (;;) {
        /* ... measure jitter from prev wake ... */
        g_stats.beat++;
        (void)beep_play(BEEP_CLICK, &(beep_params_t){
            .gain = 16u, .ms = 25u, .echo = 5u, .release = 10u,
        });

        /* next wake = last_wake + period (not "now + period") */
        vTaskDelayUntil(&last_wake, period_ticks);
    }
}

static void ui_task(void *arg)
{
    unsigned seen = 0;
    metro_init();
    metro_start();
    metro_set_bpm(120);
    for (;;) {
        if (g_stats.beat != seen) {
            seen = g_stats.beat;
            metro_flash_beat();
            metro_set_jitter(g_stats.last_j, g_stats.max_j);
            vTaskDelay(pdMS_TO_TICKS(60));
            metro_set_beat_idle();
        } else {
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

void app_main(void)
{
    bsp_init();
    g_period_ms = 60000u / 120u;
    (void)xTaskCreate(tick_task, "tick", 4096, NULL, 6, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
```

### How the code works

**`app_main`.**  
Sets the period, starts **tick** (prio 6) then **ui** (prio 5).

**`tick_task`.**  
On each beat: update jitter stats, bump `g_stats.beat`, play click, then
`vTaskDelayUntil`. The API advances `last_wake` by `period_ticks` each call,
so time spent in `beep_play` is **subtracted** from the sleep (as long as work
< period).

**`ui_task`.**  
Watches `g_stats.beat`. On change, flash and show jitter. UI delays do **not**
move the beat schedule.

---

## 6. FreeRTOS in depth — `vTaskDelayUntil`

### Relative vs absolute

| API | Meaning |
|-----|---------|
| `vTaskDelay(N)` | Sleep **N** ticks from **now** |
| `vTaskDelayUntil(&last, N)` | Sleep until **last + N**; then `last += N` |

### Timing

```
vTaskDelay (relative)          vTaskDelayUntil (absolute)
|-- work --|-- N --|           |-- work --|-- N-work --|
|<--- N + work --->|           |<-------- N ---------->|
```

### Arguments

| Argument | Role |
|----------|------|
| `&last_wake` | In/out: previous wake time (must init once) |
| `period_ticks` | Ideal spacing between wakes |

### Limits

If work **ever** takes longer than one period, you cannot catch up by sleeping
negative time — you will run late / skip. Keep the click short vs BPM period.

### Mental model

> **Delay** = nap from now.  
> **DelayUntil** = meet a timetable.

---

## 7. Checkpoint

- [ ] Can contrast `vTaskDelay` vs `vTaskDelayUntil`  
- [ ] Jitter much lower than step 03  
- [ ] Can say why UI delay no longer shifts the beat  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Still high jitter | Still using `vTaskDelay` in tick — switch to Until |
| No flash | UI not seeing `g_stats.beat` updates |
| Build error on DelayUntil | Include `freertos/task.h`; init `last_wake` |

---

## 9. Next

[Step 05 — BPM control](../step-05-bpm-control/).
