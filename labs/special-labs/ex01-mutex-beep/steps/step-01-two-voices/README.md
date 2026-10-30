# S1 / Step 01 — Two voices

**Special:** [Mutex Beep](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- Two tasks (`hi` / `lo`) each play a tone and update an LCD line.
- Delays are **staggered** so overlaps are rare.

**Success:** alternating-ish beeps; LCD shows who plays.

---

## 2. Problems

- Need a baseline with two callers of one speaker before the race is obvious.

---

## 3. Design

| Choice | Why |
|--------|-----|
| Equal priority tasks | Fair schedule |
| Stagger start + long gap | Hide the race for now |

```
┌─────────┐     ┌───────────┐
│ hi_task │ ──► │ beep_play │
└─────────┘     └───────────┘

┌─────────┐     ┌───────────┐
│ lo_task │ ──► │ beep_play │
└─────────┘     └───────────┘
     staggered in time
```

---

## 4. Build

```bash
cd labs/special-labs
# Lab: .\run 01  |  Step: ex01.c
.\run 01   # from labs/special-labs (step still in ex01.c)
```

---

## 5. Sketch

```c
xTaskCreate(hi_task, "hi", 4096, NULL, 4, NULL);
xTaskCreate(lo_task, "lo", 4096, NULL, 4, NULL);
/* each: beep_play(...); vTaskDelay(700); */
```

### How it works

Two tasks share `beep_play` with no lock. Long gaps make it *sound* fine —
step 02 removes the gap.

---

## 6. FreeRTOS — tasks only

`xTaskCreate` + `vTaskDelay`. Mutex arrives in step 03.

---

## 7. Next

[Step 02 — Overlap](../step-02-overlap-crash/).
