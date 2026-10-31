# Lab 06 / Step 04 — Stack HWM

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

```text
rbdev run 06
# step switch in ex06-task-monitor.c:  ex06_step04_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex06-task-monitor/steps/step-04-stack-hwm/README.md</code></p>

</div>


**Device lab:** [Task Monitor](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 03](../step-03-state-prio/)

---

## 1. What this step must deliver

- Each row adds **stack high-water mark** (minimum free stack, in **words**).
- Format: `name SP HWM` e.g. `load_a B3 380`.
- Workers use different stack sizes / a small pad so HWM differs.

**Success:** HWM numbers are visible and make sense vs create stack depth.

---

## 2. Problems this step faces

- Stack overflow is silent until crash — need a **margin** metric.
- Students confuse HWM with “stack used right now.”

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| `uxTaskGetStackHighWaterMark(handle)` | Classic FreeRTOS teaching API |
| Also show snapshot `usStackHighWaterMark` | Same photo; either works |
| Larger stack on `load_b` | HWM should usually be higher |

### Architecture

```
┌─────────────┐  handle   ┌─────────────┐
│ TaskStatus  │ ────────► │ Stack HWM   │
│  xHandle    │           │ free words  │
└─────────────┘           └──────┬──────┘
                                 │
                                 ▼
                          ┌─────────────┐
                          │ mon row HWM │
                          └─────────────┘
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
hwm = uxTaskGetStackHighWaterMark(snap[i].xHandle);
mon_set_row(row, "%.5s %c%u %u",
            snap[i].pcTaskName,
            state_ch(snap[i].eCurrentState),
            (unsigned)snap[i].uxCurrentPriority,
            (unsigned)hwm);
```

### How the code works

**HWM meaning.**  
High-water mark = the **smallest** amount of free stack the task has ever had
(in words). Lower HWM → closer to overflow historically.

**Not “current free.”**  
It does not climb back up after a deep call returns. It is a **worst-case
minimum free** watermark.

**Worker pad.**  
`volatile uint32_t pad[32]` burns some stack once so numbers are not identical
noise across tasks.

---

## 6. FreeRTOS in depth — stack high-water mark

| API | Role |
|-----|------|
| `uxTaskGetStackHighWaterMark(task)` | Min free words left for that task |
| `TaskStatus_t.usStackHighWaterMark` | Same idea inside a system snapshot |

**Units:** words (on Xtensa, typically 4 bytes/word — confirm in docs if you
convert to bytes).

Mental model: **how close did this task ever get to the cliff?**

---

## 7. Checkpoint

- [ ] HWM column visible  
- [ ] Can explain “minimum free,” not current free  
- [ ] Ready for queue depth + paging  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| HWM 0 | Stack nearly exhausted — enlarge create size |
| Same HWM everywhere | Different create sizes / pad array |

---

## 9. Next

[Step 05 — Queue depth](../step-05-queue-depth/).
