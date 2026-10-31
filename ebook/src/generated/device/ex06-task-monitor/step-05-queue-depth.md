# Lab 06 / Step 05 — Queue depth + page

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

<span class="ebook-badge ok">finished device</span>

```text
rbdev run 06
# step switch in ex06-task-monitor.c:  ex06_step05_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex06-task-monitor/steps/step-05-queue-depth/README.md</code></p>

</div>


**Device lab:** [Task Monitor](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 04](../step-04-stack-hwm/)

---

## 1. What this step must deliver

- Demo queue length 8; `fill_task` sends often; UI drains slowly.
- Title shows **`Q waiting/len`** and page number.
- **BOOT** advances through pages of 3 task rows.
- Rows still show name / state / prio / HWM.

**Success:** queue count moves; BOOT flips pages of the task list.

---

## 2. Problems this step faces

- Only three LCD rows — need paging for longer lists.
- Queue fill is invisible without `MessagesWaiting` / spaces.

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| `uxQueueMessagesWaiting` | How many items queued |
| `uxQueueSpacesAvailable` | Free slots; `waiting + spaces == length` |
| BOOT edge | Same button pattern as Labs 00 / 04 |
| Snapshot then page slice | Draw rows `[page*3 .. page*3+2]` |

### Architecture

```
┌─────────────┐  Send   ┌──────────┐  depth   ┌─────────────┐
│ fill_task   │ ──────► │ demo Q   │ ───────► │ title Qn/L  │
└─────────────┘         │ len 8    │          └─────────────┘
                        └────┬─────┘
                             │ drain 1 / refresh
┌─────────────┐  snapshot    ▼
│ load / ui   │ ──────► ui_task ──► mon rows (page)
└─────────────┘              ▲
                             │ BOOT edge
                        ┌────┴────┐
                        │  BOOT   │
                        └─────────┘
```

### Sequence

```
fill           queue            ui               boot
 |              |              |                |
 |-- Send ----->|              |                |
 |              |<- waiting ---|                |
 |              |<- Receive ---|                |
 |              |              |<- edge --------|
 |              |              |-- page++       |
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
waiting = uxQueueMessagesWaiting(g_demo_q);
spaces  = uxQueueSpacesAvailable(g_demo_q);
mon_set_title("Q%u/%u p%u", waiting, waiting + spaces, page + 1);

start = page * MON_ROWS;
for (row = 0; row < MON_ROWS; row++) {
    i = start + row;
    if (i >= snap_n) break;
    /* draw snap[i] ... */
}
```

### How the code works

**Producer.**  
`fill_task` uses non-blocking `xQueueSend(..., 0)` every 100 ms so a full
queue does not stall the filler forever.

**Consumer.**  
UI takes **one** item per refresh — slower than fill — so `waiting` climbs
toward 8, then wobbles as drain catches bursts.

**Paging.**  
`g_page` indexes groups of three. BOOT rising edge increments; wrap when past
the last page.

---

## 6. FreeRTOS in depth — queue depth

| API | Role |
|-----|------|
| `uxQueueMessagesWaiting` | Items currently in the queue |
| `uxQueueSpacesAvailable` | Empty slots left |
| Sum of both | Queue length (when no concurrent change mid-read) |

Contrast Lab 05: there the queue carried **samples**. Here the queue exists so
you can **see** depth on the monitor.

Mental model: **depth is a gauge; the task list is a paged table.**

---

## 7. Checkpoint

- [ ] Title `Q…/…` moves over time  
- [ ] BOOT changes which three tasks show  
- [ ] Can explain waiting vs spaces  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Q always 0 | `fill_task` not created / Send failing |
| BOOT does nothing | Edge detect / `BOOT_BTN_MASK` bit1 |
| Page empty | `snap_n` small — still create workers |

---

## 9. Next

Workshop complete for device labs 00–06 — see [labs/README.md](../../README.md).
