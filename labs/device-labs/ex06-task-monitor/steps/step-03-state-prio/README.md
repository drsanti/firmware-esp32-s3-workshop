# Lab 06 / Step 03 — State + priority

**Device lab:** [Task Monitor](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 02](../step-02-live-names/)

---

## 1. What this step must deliver

- Each row: **name**, **state letter**, **priority**.
- Example: `load_a B 3`.
- Different priorities on `load_a` / `load_b` so the column is obvious.

**Success:** you can read who is Blocked vs Running and at which prio.

---

## 2. Problems this step faces

- Names alone do not show scheduling state.
- Need compact encoding for a narrow LCD.

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| `eCurrentState` | From the same snapshot |
| `uxCurrentPriority` | Same |
| One-letter state | Fits `name S P` on one row |

### State letters

| Letter | `eTaskState` |
|--------|--------------|
| `r` | Running (this core, now) |
| `R` | Ready |
| `B` | Blocked (delay / wait) |
| `S` | Suspended |
| `D` | Deleted |

### Architecture

```
┌──────────────────┐   snapshot   ┌─────────────┐
│ TaskStatus_t[]   │ ───────────► │  mon row    │
│ name state prio  │              │ name S P    │
└──────────────────┘              └─────────────┘
```

---

## 4. Build

```bash
cd labs/device-labs
# Lab: rbdev run 06  |  Step: ex06.c
rbdev run 06   # from labs/device-labs (step still in ex06.c)
```

---

## 5. Student sketch (`main/main.c`)

```c
mon_set_row(row, "%.6s %c %u",
            snap[i].pcTaskName,
            state_ch(snap[i].eCurrentState),
            (unsigned)snap[i].uxCurrentPriority);
```

### How the code works

**Same snapshot as step 02.**  
Only the **format** grows. No second kernel query.

**Workers at prio 3 and 2.**  
Priority column should differ so students trust the field.

**Running vs Ready.**  
The UI task often shows `r` while it is painting; workers on `vTaskDelay` show `B`.

---

## 6. FreeRTOS in depth — state and priority fields

| Field | Meaning |
|-------|---------|
| `eCurrentState` | Scheduler's view at snapshot time |
| `uxCurrentPriority` | Effective priority (mutex inheritance can raise it later — not in this lab) |

Mental model: **columns are fields of one photo.**

---

## 7. Checkpoint

- [ ] Rows show letter + number  
- [ ] Can decode B vs r  
- [ ] Ready for stack HWM  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| All priorities look equal | Check `xTaskCreate(..., prio, ...)` args |
| Truncated text | Shorten name width (`%.6s`) |

---

## 9. Next

[Step 04 — Stack HWM](../step-04-stack-hwm/).
