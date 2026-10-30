# Lab 04 / Step 05 — BPM control

**Device lab:** [Metronome](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 04](../step-04-tick-task/)

---

## 1. What this step must deliver

- Same `tick_task` + `vTaskDelayUntil` as step 04.
- **BOOT** rising edge adds +10 BPM (wrap 60→180→60).
- Show last / max jitter; reset max when BPM changes.
- Ignore PWR on ws28p.

**Success:** change tempo with BOOT; beat stays on DelayUntil schedule.

---

## 2. Problems this step faces

- Live BPM change must update period without putting timing back in the UI.
- One odd interval after a change is OK; timetable continues.

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| `g_stats.bpm` shared | Tick reads period each loop |
| BOOT edge in UI | Same bit1 pattern as Labs 00/02/03 |
| Keep DelayUntil in tick | Tempo change does not revive relative delay |

### Architecture

BOOT is polled in UI; tick re-reads shared BPM every beat cycle.

```
┌─────────────┐  write bpm  ┌─────────────┐  read bpm  ┌─────────────┐
│  ui_task    │ ──────────► │  g_stats    │ ─────────► │  tick_task  │
│ BOOT edge   │             │    .bpm     │            │ DelayUntil  │
└──────┬──────┘             └─────────────┘            └─────────────┘
       │
       ▼
┌─────────────┐
│bsp_buttons  │
│ BOOT bit1   │
└─────────────┘
```

### Sequence

```
ui_task              tick_task
    |                      |
    |-- BOOT edge          |
    |-- bpm += 10 -------->|
    |-- metro_set_bpm      |-- read g_stats.bpm
    |                      |-- beep_play
    |                      |-- DelayUntil
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
#define BOOT_BTN_MASK 2u
#define BPM_MIN 60u
#define BPM_MAX 180u
#define BPM_STEP 10u

static void tick_task(void *arg)
{
    TickType_t last_wake = xTaskGetTickCount();
    for (;;) {
        unsigned period = period_ms(g_stats.bpm);
        TickType_t period_ticks = pdMS_TO_TICKS(period);
        /* ... jitter measure ... */
        g_stats.beat++;
        (void)beep_play(BEEP_CLICK, &(beep_params_t){
            .gain = 16u, .ms = 25u, .echo = 5u, .release = 10u,
        });
        vTaskDelayUntil(&last_wake, period_ticks);
    }
}

static void ui_task(void *arg)
{
    uint32_t boot_was = 0;
    for (;;) {
        if (boot_pressed(&boot_was)) {
            unsigned bpm = g_stats.bpm + BPM_STEP;
            if (bpm > BPM_MAX) bpm = BPM_MIN;
            g_stats.bpm = bpm;
            g_stats.max_j = 0;
            metro_set_bpm(bpm);
        }
        /* ... flash on g_stats.beat change + show jitter ... */
    }
}

void app_main(void)
{
    bsp_init();
    g_stats.bpm = 120u;
    (void)xTaskCreate(tick_task, "tick", 4096, NULL, 6, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
```

### How the code works

**`tick_task`.**  
Each iteration re-reads `g_stats.bpm`, converts to `period_ticks`, clicks, then
`vTaskDelayUntil`. New tempos apply on the following cycles.

**`ui_task`.**  
BOOT edge bumps BPM (wrap), clears max jitter, updates the banner. Still never
calls `beep_play`. Beat flash follows `g_stats.beat` as in step 04.

**`app_main`.**  
Seed BPM at 120; create tick then UI.

---

## 6. FreeRTOS in depth — reuse + timetable

No new API beyond step 04. The lesson is **product control** on top of an
absolute tick:

| Piece | Role |
|-------|------|
| `vTaskDelayUntil` | Steady beat |
| Shared `bpm` | UI writes, tick reads |
| BOOT edge | Human tempo control |

Relative `vTaskDelay` stays out of the beat path on purpose.

---

## 7. Checkpoint

- [ ] BOOT steps BPM (not PWR)  
- [ ] Jitter stays low after tempo change  
- [ ] Can name Lab 04’s 3 FreeRTOS features  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| No BPM change | Press **BOOT** |
| Wild jitter once | Expected for one interval after change |
| No click | Tick task / volume / `BOARD` |

---

## 9. Next

Lab 04 complete. Continue with [Lab 05 — Vibration Scope](../../ex05-vibration-scope/)
or return to [labs/README.md](../../README.md).
