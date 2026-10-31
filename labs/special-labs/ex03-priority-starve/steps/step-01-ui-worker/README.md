# S3 / Step 01 — UI + worker

**Special:** [Priority Starve](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- `ui` ticks every 100 ms.
- `worker` increments a counter but **delays** between work.

**Success:** both `ui` and `work` numbers climb.

---

## 2. Problems

- Need a healthy baseline before the starvation demo.

---

## 3. Design

| Choice | Why |
|--------|-----|
| Worker `vTaskDelay(50)` | Always blocks → UI can run |
| UI prio 4 > worker 3 | UI preferred when both ready |

```
┌──────────┐  prio4   ┌──────────┐  prio3
│ ui_task  │          │ worker   │
│ Delay100 │          │ Delay50  │
└──────────┘          └──────────┘
```

---

## 4. Build

```bash
cd labs/special-labs
# Lab: rbdev run 03  |  Step: ex03.c
rbdev run 03   # from labs/special-labs (step still in ex03.c)
```

---

## 5. Next

[Step 02 — High-prio hog](../step-02-high-prio-hog/).
