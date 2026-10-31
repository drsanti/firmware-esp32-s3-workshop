# ex05 — task suspend / resume

**Goal:** control tasks with **`vTaskSuspend`** / **`vTaskResume`**. A
suspended task is not scheduled; resume continues it with state intact.

Builds on [ex00](../ex00-task-creation/) (create + delay). Same priority for
both workers so this lab is about **control**, not ranking ([ex04](../ex04-task-priority/)).

```
┌──────────────────┬──────────────────┐
│ A  #1234         │ B  #567          │
│ state RUN        │ state SUSP       │
├──────────────────┼──────────────────┤
│ [ RUN / SUSP ]   │ [ RUN / SUSP ]   │
│   toggle A       │   toggle B       │
└──────────────────┴──────────────────┘
```

Enable in `main/main.c` with `ex05_start()` (only one exercise at a time).

---

## 1. Learning objectives

| Idea | In this exercise |
|------|------------------|
| Suspend | `vTaskSuspend(handle)` — leave Ready/Running set |
| Resume | `vTaskResume(handle)` — eligible to run again |
| State kept | Counter does not reset across suspend |
| Independent | Button A only affects worker A (same for B) |
| Not delete | TCB + stack remain until `vTaskDelete` |

**Success:** freeze one counter with its button; the other keeps climbing;
resume restores motion from the same count.

---

## 2. What you should see

1. Both panes show climbing `#` and `state RUN`; both toggles show **RUN**.  
2. Tap **A** → label **SUSP**, A’s `#` freezes, B still moves.  
3. Tap **A** again → **RUN**, A’s `#` continues (not reset to 0).  
4. Same for B, independently.

Touch is polled on **core 1** so a suspended worker on core 0 cannot block
the UI.

---

## 3. Mental model

```
  button A CLICK
       │
       ├─ ON  (RUN)  ──► vTaskResume(A)  ──► Ready / Running
       │
       └─ OFF (SUSP) ──► vTaskSuspend(A) ──► Suspended (not scheduled)

  worker A:  count++ … vTaskDelay(20) …   (stops while Suspended)
  worker B:  same loop, own handle + button
```

Suspend is **not** the same as Blocked (`vTaskDelay` / queue wait). A
delayed task becomes Ready when the time expires; a suspended task stays
out until **resume**.

---

## 4. How the code is wired

| Task | Prio | Pin | Role |
|------|------|-----|------|
| `wkA` | 5 | core 0 | Counter A |
| `wkB` | 5 | core 0 | Counter B |
| `ui` | 4 | core 1 | Touch + status redraw |

```c
if (rb_button_is_on(btn)) {
    vTaskResume(handle);   /* RUN */
} else {
    vTaskSuspend(handle);  /* SUSP */
}
```

Handles come from `xTaskCreatePinnedToCore(..., &handle, ...)`.

---

## 5. Tips and tricks

### Tip — keep the handle

Without `TaskHandle_t` you cannot target that task. Always pass a handle
pointer into create when you plan to suspend/resume/delete later.

### Tip — who calls suspend?

Another task (here: UI callback path) or the task itself
(`vTaskSuspend(NULL)`). Do not suspend a task that still holds a mutex you
need.

### Tip — resume is idempotent enough for demos

Calling `vTaskResume` on an already-running task is safe for this lab.
Still track your own RUN/SUSP flag for the LCD.

### Tip — UI on the other core

If the only painter lived in a worker you suspended, the screen would
freeze too. Status + touch stay on core 1.

---

## 6. Focus / avoid / consider

### Focus on

- Counter freezes on SUSP and continues on RUN  
- Independence of A vs B  
- Suspend ≠ delete ≠ delay  

### Avoid

- Suspending the idle task or the task that feeds the watchdog forever  
- Assuming suspend clears variables  
- Mixing this with priority starvation (that is ex04)  

### Consider

- What if both are suspended? (Only UI / idle / system tasks run.)  
- `eTaskGetState(handle)` as a cross-check for the LCD flag  

---

## 7. Checkpoint

- [ ] Can name the APIs for suspend and resume  
- [ ] Can explain why the count does not reset  
- [ ] Can say why each button needs its own `TaskHandle_t`  
- [ ] Can contrast Suspended vs Blocked  

---

## 8. Experiments

1. Suspend A, wait, resume — confirm `#` jumped only after resume.  
2. Suspend both — UI should still redraw and accept taps.  
3. Call `vTaskSuspend` twice, then one `vTaskResume` — observe nesting
   behavior for your IDF/FreeRTOS version (document what you see).  

---

## 9. Troubleshooting

| Symptom | Try |
|---------|-----|
| Buttons do nothing | Touch task running? `bsp_touch_read` + `rb_button_feed_touch` |
| Both freeze when one tapped | Wrong handle / both buttons wired to same index |
| Count resets | Accidental recreate/delete — this lab only suspends |
| Blank LCD | Layout add order: terms then buttons; `rb_screen_reflow` |

---

## 10. Next

You can pause work without destroying it. Next controls often combine
suspend with notifications or queues so a woken task has a reason to run.
