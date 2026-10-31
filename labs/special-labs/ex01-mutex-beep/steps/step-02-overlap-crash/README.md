# S1 / Step 02 — Overlap (broken)

**Special:** [Mutex Beep](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- Same two tasks, **short** delays, longer tones.
- Overlap on purpose — audio sounds wrong / stepped on.

**Success:** you can hear (and name) the shared-resource bug.

---

## 2. Problems

- `beep_play` is blocking and owns the speaker path — two callers corrupt it.

---

## 3. Design

| Choice | Why |
|--------|-----|
| Short `vTaskDelay(120)` | Force concurrent entry |
| No lock | Make the failure obvious |

```
┌─────────┐          ┌───────────┐
│ hi_task │ ────────►│ beep_play │
└─────────┘          │ UNSAFE    │
┌─────────┐          │ overlap   │
│ lo_task │ ────────►│           │
└─────────┘          └───────────┘
```

---

## 4. Build

```bash
cd labs/special-labs
# Lab: rbdev run 01  |  Step: ex01.c
rbdev run 01   # from labs/special-labs (step still in ex01.c)
```

---

## 5. Sketch

Same as step 01, but delays ~120 ms and tones ~180 ms — they collide.

### How it works

Scheduler can switch while `beep_play` runs (or start the other task’s beep
while the first still owns the hardware path). Result: garbled audio.

---

## 6. FreeRTOS — why a lock

| Tool | Fit? |
|------|------|
| Notify | “Event happened” — no mutual exclusion |
| Queue | Can serialize *requests* (Lab 02) — heavier here |
| **Mutex** | “Only one task in the critical section” |

---

## 7. Next

[Step 03 — Mutex guard](../step-03-mutex-guard/).
