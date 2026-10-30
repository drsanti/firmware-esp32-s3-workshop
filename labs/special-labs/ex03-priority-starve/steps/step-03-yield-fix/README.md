# S3 / Step 03 — Yield fix

**Special:** [Priority Starve](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- Hog runs below UI priority **and** `vTaskDelay(1)` after a work burst.
- UI ticks smoothly; work counter still climbs.

**Success:** both lines move again.

---

## 2. Problems

- Need CPU-bound work that still cooperates with the scheduler.

---

## 3. Design

| Choice | Why |
|--------|-----|
| Hog prio 3 < UI 5 | UI wins when both ready |
| `vTaskDelay(1)` in hog | Even if prios were equal/wrong, blocking lets others run |

```
┌──────────┐  prio5
│ ui_task  │ ─────────► LCD alive
└──────────┘
┌──────────┐  prio3  burst + Delay(1)
│ hog_task │ ─────────► work climbs
└──────────┘
```

### Sequence

```
hog                 scheduler           ui
 |                     |                 |
 |-- busy burst ------>|                 |
 |-- Delay(1) -------->|                 |
 |                     |-- run ui ------>|
 |                     |<- Delay 100 ----|
 |<- wake -------------|                 |
```

---

## 4. Build

```bash
cd labs/special-labs
# Lab: .\run 03  |  Step: ex03.c
.\run 03   # from labs/special-labs (step still in ex03.c)
```

---

## 5. Sketch

```c
xTaskCreate(hog_task, "hog", 2048, NULL, 3, NULL);
xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
/* hog: work burst; vTaskDelay(1); */
```

### FreeRTOS in depth

| Idea | Meaning |
|------|---------|
| Priority | Higher ready task always wins |
| Blocking | `Delay` / queue / notify Take → not ready → others run |
| Starvation | Ready high-prio task never blocks → lower prio never runs |

Mental model: **priority picks the winner; blocking gives others a turn.**

---

## 6. Next

[S4 — Counting semaphore](../../ex04-counting-sem/).
