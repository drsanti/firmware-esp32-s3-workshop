# ex08 — event group (WaitBits)

**Goal:** wait on **several conditions** with an event group — **AND** (all
bits) vs **OR** (any bit).

Two bits (**A**, **B**). A worker blocks on `xEventGroupWaitBits`. Toggle
**AND** / **OR** to change `waitForAll`.

| Mode | WaitBits | Fires when |
|------|----------|------------|
| **AND** (boot) | `waitForAll=pdTRUE` | **A and B** both set |
| **OR** | `waitForAll=pdFALSE` | **A or B** set |

After a successful wait, `clearOnExit` clears the bits and the A/B buttons
reset to OFF so you arm again deliberately.

Builds on [ex07](../ex07-counting-sem/) (tokens). Here the object is a
**bitfield**, not a count.

```
┌──────────────────┬──────────────────┐
│ ex08 AND         │ [ AND / OR ]     │
│ A=0  B=0         │                  │
│ WAIT  #0         │                  │
├──────────────────┼──────────────────┤
│ [ A ]            │ [ B ]            │
└──────────────────┴──────────────────┘
```

Enable in `main/main.c` with `ex08_start()` (only one exercise at a time).

| File | Holds |
|------|--------|
| `ex08.c` | Event group, WaitBits worker, `ex08_start` |
| `ex08_ui.*` | LCD checklist, A/B/mode toggles |

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
| Event group | `xEventGroupCreate` — bitfield of conditions |
| Set / Clear | Publish or drop a condition from a toggle |
| WaitBits AND | Block until **all** bits in the mask are set |
| WaitBits OR | Block until **any** bit in the mask is set |
| clearOnExit | Matched bits cleared when the wait succeeds |

**Success:** AND needs both toggles; OR fires on the first one; `#` climbs.

---

## 2. What you should see

1. Boot **AND**, both bits 0, worker **WAIT**.  
2. Tap **A** only → still WAIT (`A=1 B=0`).  
3. Tap **B** → **FIRED**, `#` +1, buttons return to OFF.  
4. Switch mode to **OR**, tap only **A** → FIRED without B.

---

## 3. Mental model

```
  toggle A/B          event group           worker
       │                   │                   │
       │── Set/Clear ─────►│                   │
       │                   │◄── WaitBits ──────│
       │                   │   (AND or OR)     │
       │                   │── clearOnExit ───►│ FIRED
```

```c
xEventGroupWaitBits(
    bits,
    EX08_BIT_A | EX08_BIT_B,
    pdTRUE,              /* clearOnExit */
    wait_all ? pdTRUE : pdFALSE,
    portMAX_DELAY);
```

---

## 4. How the code is wired

| Task | Prio | Pin | Role |
|------|------|-----|------|
| `wait` | 5 | core 0 | `WaitBits` → FIRED hold |
| `ui` | 4 | core 1 | Touch + checklist |

| API | Role |
|-----|------|
| `xEventGroupCreate` | Bitfield object |
| `xEventGroupSetBits` / `ClearBits` | From A/B toggles |
| `xEventGroupWaitBits` | Worker gate |
| `xEventGroupGetBits` | Snapshot for LCD |

---

## 5. Tips and tricks

### Tip — AND vs counting sem

A counting sem answers “how many tokens?” An event group answers “which
conditions are true?” Use bits for a checklist; use a count for a pool.

### Tip — clearOnExit

Without clear, AND would re-fire forever while both bits stay set. Clearing
makes each FIRED a one-shot; arm again with the toggles.

### Tip — who sets bits?

Any task (here: UI callbacks). ISR variants exist (`…FromISR`) — later lab.

---

## 6. Checkpoint

- [ ] Can name Create / SetBits / ClearBits / WaitBits / GetBits  
- [ ] Can explain `waitForAll` true vs false  
- [ ] Can say why clearOnExit matters for one-shot arm  
- [ ] Can contrast event group vs mutex / counting sem  

---

## 7. Experiments

1. Leave clearOnExit as `pdFALSE` — AND may spin FIRED while bits stay set.  
2. Wait only on `EX08_BIT_A` — B becomes irrelevant.  
3. Add a third bit in code — AND gets harder to arm.

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| AND never fires | Both A and B set? Mode really AND? |
| OR never fires | At least one bit set? |
| Buttons ON but bits 0 | After FIRED, re-tap to SetBits again |
| Linker miss after add | `idf.py -DBOARD=… reconfigure` then build |

---

## 9. Next

[ex09 — software timer (auto-reload)](../ex09-reload-timer/) — time-driven
beats without parking a task on `vTaskDelay`.
