# ex07 — counting semaphore (resource pool)

**Goal:** see a FreeRTOS **counting semaphore** limit how many tasks hold a
shared resource at once.

**4** workers, **2** slots. Each worker Takes a slot, HOLDs ~450 ms, then
Gives it back.

| Mode | What happens | What you see |
|------|----------------|--------------|
| **OFF** | No Take/Give | `in use` can hit **4**; many HOLD at once |
| **ON** | `Take` / `Give` on counting sem | `in use` never above **2**; extras stay WAIT |

Boot **OFF**. Toggle **ON** to enforce the pool.

Builds on [ex06](../ex06-mutex-canvas/) (mutex = 1 owner). Here max holders = N.

```
┌──────────────────────────────────────┐
│ ex07 OFF  free --/2  in 3            │
│ W0 HOLD #..  W1 HOLD #..             │
│ W2 WAIT #..  W3 HOLD #..             │
├──────────────────────────────────────┤
│ [ OFF / ON ]                         │
└──────────────────────────────────────┘
```

Four workers (`W0`…`W3`), two pool slots.

Enable in `main/main.c` with `ex07_start()` (only one exercise at a time).

| File | Holds |
|------|--------|
| `ex07.c` | Counting sem, workers, `ex07_start` |
| `ex07_ui.*` | LCD layout, touch, status redraw |

After adding this folder **or** a new `.c` (e.g. `ex07_ui.c`), reconfigure once:

```powershell
idf.py -DBOARD=ws28p reconfigure
# or
idf.py -DBOARD=es3c28p reconfigure
```

See [rtos-labs README](../README.md#build--flash). Skip reconfigure → linker
`undefined reference` to `ex07_ui_*`.

---

## 1. Learning objectives

| Idea | In this exercise |
|------|------------------|
| Counting sem | `xSemaphoreCreateCounting(max, init)` — pool of tokens |
| Take / Give | Acquire a slot / return it |
| Free count | `uxSemaphoreGetCount` — tokens left |
| Oversubscribe | OFF path skips Take → `in use` > slots |
| vs mutex | Mutex max holders = 1; counting max = N |

**Success:** OFF shows 3–4 HOLD at once; ON caps HOLD at 2 with others WAIT.

---

## 2. What you should see

1. Boot **OFF**: several yellow **HOLD** lines; `in use` climbs past 2.  
2. Tap toggle → **ON**: at most two HOLD; others cyan **WAIT**; `free` moves 0…2.  
3. `#` counters keep climbing for every worker (blocked ones catch up after Take).

Workers on **core 0**; touch/UI on **core 1**.

---

## 3. Mental model

```
  workers (x4)          counting sem (max 2)
       │                      │
       │── Take (if ON) ─────►│  token--
       │   HOLD 450 ms        │
       │── Give (if took) ───►│  token++
       │   REST 80 ms         │
```

```c
int took = 0;
if (pool_on) {
    xSemaphoreTake(slots, portMAX_DELAY);
    took = 1;
}
/* critical "slot" work */
if (took) {
    xSemaphoreGive(slots);
}
```

Remember `took` so a mid-HOLD mode flip does not Give without a matching Take.

---

## 4. How the code is wired

| Task | Prio | Pin | Role |
|------|------|-----|------|
| `w0`…`w3` | 5 | core 0 | Compete for slots |
| `ui` | 4 | core 1 | Touch + status |

| API | Role |
|-----|------|
| `xSemaphoreCreateCounting(2, 2)` | Full pool at start |
| `xSemaphoreTake` / `Give` | Enter / leave pool |
| `uxSemaphoreGetCount` | `free` on the LCD |

---

## 5. Tips and tricks

### Tip — max vs init

`CreateCounting(maxCount, initialCount)`. Here both are `SLOTS` so the pool
starts full. Init `0` would mean every worker blocks until something Gives
(producer/consumer style).

### Tip — not a mutex

No priority inheritance; no “owner.” Any task may Give. Use a mutex when
you need exclusive ownership of one object.

### Tip — leak = stuck WAIT

Forget Give after Take → tokens drain to 0 and everyone waits forever. Try
commenting out Give as an experiment.

---

## 6. Checkpoint

- [ ] Can name CreateCounting / Take / Give / GetCount  
- [ ] Can explain why OFF allows `in use` > 2  
- [ ] Can contrast counting sem (N) vs mutex (1)  
- [ ] Can say why `took` is recorded before HOLD  

---

## 7. Experiments

1. Change `SLOTS` to 1 — ON looks almost like a mutex.  
2. Change `SLOTS` to 4 — ON and OFF look similar (no contention).  
3. Comment out `xSemaphoreGive` — eventually all WAIT.  
4. Raise `HOLD_MS` — WAIT pile-up more obvious on ON.

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Always `in use` ≤ 2 even OFF | Toggle updating `s_pool_on`? Boot button OFF? |
| ON still shows 4 HOLD | Confirm Take path runs (`s_pool_on` and `took`) |
| Everyone stuck WAIT | Matching Give after every successful Take? |
| Linker miss after add | `idf.py -DBOARD=… reconfigure` then build |

---

## 9. Next

[ex08 — event group (WaitBits)](../ex08-event-group/) — from token pools to a
bitfield checklist (AND vs OR).
