# Lab 06 / Step 02 — Live names

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

```text
rbdev run 06
# step switch in ex06-task-monitor.c:  ex06_step02_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex06-task-monitor/steps/step-02-live-names/README.md</code></p>

</div>


**Device lab:** [Task Monitor](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 01](../step-01-name-face/)

---

## 1. What this step must deliver

- Create worker tasks `load_a` / `load_b`.
- UI task snapshots the system ~2 Hz and shows **live** task names.
- Title shows how many tasks the snapshot returned.

**Success:** names change from fake strings to real kernel names (including IDLE / ui).

---

## 2. Problems this step faces

- Hard-coded names lie when you add tasks.
- Need a **snapshot** API, not a continuous trace.

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| `uxTaskGetSystemState` | Fills a `TaskStatus_t` array once |
| Array length 16 | Enough for this lab; teach sizing |
| Refresh every 500 ms | Readable; avoids hammering the kernel |
| Extra `load_*` tasks | Make the list interesting |

### Architecture

```
┌─────────────┐                 ┌─────────────┐  draw  ┌──────────┐
│ load_a/b    │                 │  ui_task    │ ─────► │  mon_*   │
│ Blocked     │                 │ SystemState │  2 Hz  │  names   │
└─────────────┘                 └─────────────┘        └──────────┘
```

### Sequence

```
ui_task                    kernel
 |                           |
 |-- GetSystemState -------->|
 |<- TaskStatus array -------|
 |-- mon_set_row             |
 |-- vTaskDelay 500ms ------>|
```

---

## 4. Build

```bash
cd labs/device-labs
# Lab: rbdev run 06  |  Step: ex06.c
rbdev run 06   # from labs/device-labs (step still in ex06.c)
```

Needs `CONFIG_FREERTOS_USE_TRACE_FACILITY=y` (already in `sdkconfig.defaults`).

---

## 5. Student sketch (`main/main.c`)

```c
TaskStatus_t snap[16];
UBaseType_t n = uxTaskGetSystemState(snap, 16, NULL);
mon_clear_rows();
for (i = 0; i < n && row < MON_ROWS; i++) {
    mon_set_row(row++, "%.8s", snap[i].pcTaskName);
}
mon_set_title("names %u", (unsigned)n);
```

### How the code works

**Workers.**  
`load_task` only delays — it stays **Blocked** most of the time, but still appears in the snapshot.

**`uxTaskGetSystemState`.**  
Writes up to `SNAP_MAX` entries. Return value = how many tasks were copied. If the system has more tasks than the array, the list is **truncated** (size the array for your product).

**UI pacing.**  
Snapshot + draw, then `vTaskDelay(500)`. A snapshot every loop with no delay makes the UI feel busy for no teaching gain.

---

## 6. FreeRTOS in depth — system state snapshot

| API | Role |
|-----|------|
| `uxTaskGetSystemState(buf, len, total_runtime)` | Fill `TaskStatus_t` array |
| `pcTaskName` | Name string (truncated on LCD) |
| `total_runtime` | Optional; pass `NULL` here |

**Rejected here:** parsing `vTaskList` text — awkward for LCD columns. Prefer the structured array.

Mental model: **photo of the task list**, not a video of the scheduler.

---

## 7. Checkpoint

- [ ] Live names include `ui` and `load_*`  
- [ ] Can explain truncated array risk  
- [ ] Can point at GetSystemState in the sketch  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Link / compile error on GetSystemState | Trace facility in sdkconfig; `fullclean` + `set-target` |
| Only 1–2 names | Workers not created |

---

## 9. Next

[Step 03 — State + priority](../step-03-state-prio/).
