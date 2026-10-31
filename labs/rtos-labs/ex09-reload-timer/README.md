# ex09 — software timer (auto-reload)

**Goal:** compare a **task-owned** beat (`vTaskDelay`) with a FreeRTOS
**auto-reload software timer** (`xTimerCreate` + `pdTRUE`).

Same period (**500 ms**). Toggle who produces the beat.

| Mode | Who owns the period | What you see |
|------|---------------------|--------------|
| **DLY** (boot) | `delay_beat` task + `vTaskDelay` | Steady `#` + `****` flash |
| **TMR** | Timer daemon → `timer_cb` | Same beat look; task idles |

Callback only bumps counters — **no LCD** in the timer callback.

Builds on [ex08](../ex08-event-group/) (sync objects). Here the lesson is
**time-driven** work without parking a task on Delay for the period.

```
┌──────────────────────────────────────┐
│ ex09 DELAY|TIMER                     │
│ period 500 ms                        │
│ beat #12  ****                       │
├──────────────────────────────────────┤
│ [ DLY / TMR ]                        │
└──────────────────────────────────────┘
```

Enable in `main/main.c` with `ex09_start()` (only one exercise at a time).

| File | Holds |
|------|--------|
| `ex09.c` | Timer create/start/stop, delay task, `ex09_start` |
| `ex09_ui.*` | LCD status, mode toggle |

After adding this folder **or** a new `.c`, reconfigure once:

```powershell
idf.py -DBOARD=ws28p reconfigure
# or
idf.py -DBOARD=es3c28p reconfigure
```

See [rtos-labs README](../README.md#build--flash).

---

## 1. Learning objectives

| Idea | In this exercise |
|------|------------------|
| Auto-reload timer | `xTimerCreate(..., pdTRUE, cb)` |
| Start / Stop | Arm or pause the periodic timer |
| Timer callback | Runs in timer daemon — keep it short |
| vs Delay | Task blocks on period vs daemon owns period |
| UI stays live | Touch works in both modes |

**Success:** both modes pulse at ~500 ms; switching mode keeps `#` climbing.

---

## 2. What you should see

1. Boot **DELAY**: `****` flashes on a steady beat; `#` climbs.  
2. Tap **TMR**: label **TIMER**; beat continues from the callback path.  
3. Tap **DLY** again: back to the delay-task path.

---

## 3. Mental model

```
  DELAY:   delay_task ──vTaskDelay(500)──► beat_pulse ──► LCD

  TIMER:   xTimer (pdTRUE) ──► timer_cb ──► beat_pulse ──► LCD
              (daemon)         (no LCD!)
```

```c
s_timer = xTimerCreate("beat", pdMS_TO_TICKS(500), pdTRUE, NULL, timer_cb);
xTimerStart(s_timer, 0);
/* … later … */
xTimerStop(s_timer, 0);
```

---

## 4. How the code is wired

| Task / object | Prio | Pin | Role |
|---------------|------|-----|------|
| `dly` | 5 | core 0 | Delay-loop beat when mode DLY |
| Timer daemon | — | — | Calls `timer_cb` when mode TMR |
| `ui` | 4 | core 1 | Touch + status |

| API | Role |
|-----|------|
| `xTimerCreate` | Period + auto-reload (`pdTRUE`) |
| `xTimerStart` / `Stop` | Mode toggle |
| `vTaskDelay` | DELAY-mode period |

---

## 5. Tips and tricks

### Tip — keep the callback tiny

Do not draw, take mutexes you might hold long, or block in `timer_cb`.
Bump a counter / notify a task (S6 / notify labs) and return.

### Tip — one-shot vs reload

`pdFALSE` = one-shot (fires once). `pdTRUE` = auto-reload (this lab).

### Tip — who “owns” the beat?

Delay mode: if the delay task is starved, the beat jitters. Timer mode: the
daemon keeps the period even if your UI task is busy (until the daemon
itself is overloaded).

---

## 6. Checkpoint

- [ ] Can name Create / Start / Stop and the `pdTRUE` reload flag  
- [ ] Can explain why the callback must stay short  
- [ ] Can contrast task Delay vs software timer  
- [ ] Can say which mode boots and what the toggle does  

---

## 7. Experiments

1. Change `PERIOD_MS` to 200 — both modes should speed up together.  
2. Put a long `vTaskDelay` inside `timer_cb` — see why docs forbid it.  
3. Leave the timer running and also let the delay task pulse — double beat.

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| No beat on TMR | `xTimerStart` on toggle? `s_use_timer` set? |
| Double beat | Delay task still pulsing while timer runs? |
| Freeze in TMR | LCD or blocking call inside `timer_cb`? |
| Linker miss after add | `idf.py -DBOARD=… reconfigure` then build |

---

## 9. Next

[ex10 — BOOT ISR notify](../ex10-boot-isr/) — wake a task from a GPIO ISR
instead of polling.
