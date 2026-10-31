# Lab 03 / Step 04 — Timer GO

**Device lab:** [Reaction Game](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 03](../step-03-busy-wait/)

---

## 1. What this step must deliver

- Random WAIT started with a **one-shot software timer**.
- During WAIT the hint line keeps pulsing (`live N`) — UI stays alive.
- Timer callback only `xTaskNotifyGive`s the UI task (short!).
- GO → BOOT → RESULT ms as before.

**Success:** watch `live` count up during WAIT, then GO appears.

---

## 2. Problems this step faces

- Need “fire GO later” without blocking the UI task.
- Timer callbacks run in the **timer service** task — must stay short.

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| `xTimerCreate(..., pdFALSE, …)` | One-shot (does not auto-reload) |
| `xTimerChangePeriod` + `xTimerStart` | Set random wait, then arm |
| Callback → `xTaskNotifyGive(g_ui)` | Wake UI; no LCD / beep in callback |
| UI polls notify with short timeout | Can update hint while waiting |

### Architecture

```
┌──────────┐   Start    ┌──────────┐   Notify   ┌──────────┐
│ ui task  │ ─────────► │ go timer │ ─────────► │ ui task  │
└──────────┘             └──────────┘             └──────────┘
```

The ui task keeps pulsing `react_*` hints on the LCD during WAIT.

### Sequence

```
ui                 go timer           tmr svc
 |                     |                    |
 |-- ChangePeriod ---->|                    |
 |-- Start ----------->|                    |
 |-- pulse hint        |                    |
 |                     |-- fire ----------->|
 |                     |                    |-- NotifyGive
 |<- GO wake ----------|                    |
 |-- show GO           |                    |
```

---

## 4. Build

```bash
cd labs/device-labs
# Lab: rbdev run 03  |  Step: ex03.c
rbdev run 03   # from labs/device-labs (step still in ex03.c)
```

---

## 5. Student sketch (`main/main.c`)

```c
static TaskHandle_t g_ui;
static TimerHandle_t g_go_timer;
static TickType_t g_go_tick;

static void go_timer_cb(TimerHandle_t t)
{
    (void)t;
    g_go_tick = xTaskGetTickCount();
    if (g_ui != NULL) {
        (void)xTaskNotifyGive(g_ui);  /* SHORT — no LCD / beep */
    }
}

static void ui_task(void *arg)
{
    /* ... READY + BOOT to start ... */
    wait_ms = 1000u + (rnd_u32() % 2000u);
    react_set_state(REACT_ST_WAIT);
    (void)xTimerChangePeriod(g_go_timer, pdMS_TO_TICKS(wait_ms), 0);
    (void)xTimerStart(g_go_timer, 0);

    (void)ulTaskNotifyTake(pdTRUE, 0); /* clear stale */
    for (;;) {
        if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(20)) > 0u) {
            break; /* timer fired → GO */
        }
        react_set_hint("live %u", ++pulse);
    }

    react_set_state(REACT_ST_GO);
    /* ... BOOT edge + RESULT ms ... */
}

void app_main(void)
{
    bsp_init();
    g_go_timer = xTimerCreate("go", pdMS_TO_TICKS(1000), pdFALSE, NULL, go_timer_cb);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, &g_ui);
}
```

### How the code works

**`app_main`.**  
Creates a **one-shot** timer (`pdFALSE` = no auto-reload) and the UI task.
Save the UI handle in `g_ui` so the callback can notify it.

**`go_timer_cb`.**  
Runs in the timer service task. Only stamps `g_go_tick` and
`xTaskNotifyGive(g_ui)`. Never touches the LCD or `beep_play`.

**Starting WAIT.**  
`xTimerChangePeriod` sets the random delay; `xTimerStart` arms it. UI does
**not** call `vTaskDelay(wait_ms)`.

**WAIT loop.**  
`ulTaskNotifyTake` with a short timeout: if the count is > 0, GO time arrived;
if it times out, refresh `live N` and poll again. That is why the hint keeps
moving during WAIT.

**GO / RESULT.**  
Same tick math as step 02 once the notify wakes the UI.

---

## 6. FreeRTOS in depth — software timers

### Why a timer (not delay in UI)

| | `vTaskDelay` in UI | One-shot timer |
|--|--------------------|----------------|
| Who waits | The UI task | Timer service |
| UI during wait | Blocked | Can poll / draw |
| “Do X later” | Awkward if UI must stay live | Natural |

### Key APIs

| API | Role |
|-----|------|
| `xTimerCreate(name, period, pdFALSE, id, cb)` | Create one-shot timer |
| `xTimerChangePeriod` | Set next wait length |
| `xTimerStart` | Arm it |
| Callback | Runs in timer daemon — **NotifyGive only** |

### Callback rules

Allowed: notify, give semaphore, set a flag carefully.  
Not allowed here: `beep_play`, long loops, complex UI.

### Mental model

> **Delay** = this task sleeps.  
> **Timer** = ask the system to wake someone later.

---

## 7. Checkpoint

- [ ] `live` increments during WAIT  
- [ ] Can explain one-shot + short callback  
- [ ] Can point at ChangePeriod / Start / NotifyTake in the sketch  
- [ ] Reaction ms still works  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Never GO | `xTimerStart` / `ChangePeriod` missing |
| Crash in wait | Beep/LCD moved into callback — undo |
| No pulse | NotifyTake timeout too long / loop broken |

---

## 9. Next

[Step 05 — Score + audio](../step-05-score-audio/).
