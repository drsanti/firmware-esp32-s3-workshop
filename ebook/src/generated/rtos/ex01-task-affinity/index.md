# ex01 — task affinity

<div class="ebook-run">

**Track:** `rtos-labs` · **Project:** `labs/rtos-labs`

```text
rbdev run 01
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/rtos-labs/ex01-task-affinity/README.md</code></p>

</div>


**Goal:** understand how to **pin a task to a CPU core**, how **two cores
run two tasks at once**, and what affinity does *not* give you for free.

Builds on [ex00](../ex00-task-creation/) (`xTaskCreate` + `vTaskDelay`).
Here both panes use the **same period**; the new evidence is the **core id**
printed on every line.

```
┌────────────────┐  ┌────────────────┐
│ core0 (cyan)   │  │ core1 (yellow) │
│ pin=0 on=0     │  │ pin=1 on=1     │
│ core0 #0 c0    │  │ core1 #0 c1    │
│ every 500 ms   │  │ every 500 ms   │
└────────────────┘  └────────────────┘
```

Enable in `main/main.c`:

```c
#include "ex01.h"
/* ... */
ex01_start();
```

(Comment out the previous `exNN_start` — only one exercise at a time.)

---

## 1. Learning objectives

| Idea | In this exercise |
|------|------------------|
| Affinity | `xTaskCreatePinnedToCore(..., coreID)` |
| Prove the pin | `xPortGetCoreID()` inside the task |
| Dual scheduler | ESP32-S3 has two cores → two Ready/Running sets |
| Shared world | Heap, FreeRTOS objects, and globals are still shared |

**Success:** left pane always shows `c0`, right pane always shows `c1`.
Counters advance together at ~500 ms.

**What this exercise is not:** IPC between cores, spinlocks, or “make my
app twice as fast.” Pinning is placement, not a speed cheat code.

---

## 2. Mental model — two cores, one firmware image

ESP32-S3 is a dual-core Xtensa chip. FreeRTOS (ESP-IDF SMP) runs a
scheduler on **each** core. A pinned task may only be **Running** on its
chosen core.

```
┌─────────────────────┐     ┌─────────────────────┐
│       core 0        │     │       core 1        │
│  scheduler + tasks  │     │  scheduler + tasks  │
│  (this lab: core0)  │     │  (this lab: core1)  │
└─────────────────────┘     └─────────────────────┘
            \                     /
             \   shared RAM      /
              \  heap / TCBs    /
               \_______________/
```

**Analogy:** two kitchens in one restaurant. Affinity says which kitchen a
cook works in. The pantry (RAM) is still shared — so you still need
discipline when two cooks touch the same shelf (later: mutex / queue).

**Unpinned tasks:** `xTaskCreate` (or pin with `tskNO_AFFINITY`) lets the
scheduler migrate the task between cores. That is fine for many jobs; pin
when you care about locality, ISR pairing, or isolating heavy work.

---

## 3. How to pin a task

```c
(void)xTaskCreatePinnedToCore(pane_task, "core0", 3072, &core0, 4, NULL, 0);
/*                            |          |       |      |      |  |     |
 *                            fn         name  stack  arg  prio  h  core
 */
```

| Argument | Meaning here |
|----------|----------------|
| First five | Same idea as `xTaskCreate` (see ex00) |
| `handle` | `NULL` here; keep one if you need delete / notify |
| `coreID` | `0` or `1` on ESP32-S3; or `tskNO_AFFINITY` |

### Read the live core

```c
int core = (int)xPortGetCoreID();
```

Call this **inside** the task. Logging the pin you *requested* is not enough —
`on=cN` proves where the scheduler actually ran you.

### Return value

Same rule as ex00: real firmware checks `pdPASS`. Creation can still fail
(heap). Affinity does not create extra RAM.

---

## 4. How this differs from ex00

```
┌────────────┐  pin 0   ┌────────────┐
│ ex01_start │ ───────► │ core0 task │  always on CPU0
└────────────┘          └────────────┘
┌────────────┐  pin 1   ┌────────────┐
│ ex01_start │ ───────► │ core1 task │  always on CPU1
└────────────┘          └────────────┘
```

| ex00 | ex01 |
|------|------|
| `xTaskCreate` (no affinity) | `xTaskCreatePinnedToCore` |
| Different periods | Same period (fair visual) |
| Lesson: multitasking + delay | Lesson: which core runs you |

Both still use `vTaskDelay` so each core’s idle task can run when the
worker is Blocked.

---

## 5. Scheduler picture (dual core)

On each core, the familiar state machine still holds:

```
  Running              Blocked               Ready             Running
┌──────────┐ vTaskDelay ┌──────────┐  tick  ┌──────────┐ pick ┌──────────┐
│  core0   │ ─────────► │  core0   │ ─────► │  core0   │ ───► │  core0   │
│  logs    │            │  waiting │        │ wants CPU│      │  logs    │
└──────────┘            └──────────┘        └──────────┘      └──────────┘
```

The new fact: **core1 can be Running at the same wall-clock time** as core0.
That is true parallelism for two Ready tasks pinned apart — something a
single core can only fake by switching.

**ESP-IDF default:** protocol / Wi-Fi often live on a specific core. Your
app tasks share those cores with system work. Pinning everything to core 1
“to leave core 0 alone” is a strategy — measure before you believe it.

---

## 6. Read the code (`ex01.c`)

1. `ex_terms_begin(1, 2)` — UI only.  
2. Two `pane_cfg_t` with `pin_core` 0 and 1 (`static` lifetime — same tip as ex00).  
3. Two `xTaskCreatePinnedToCore` calls.  
4. Each loop logs `cN` from `xPortGetCoreID()`.

---

## 7. Tips and tricks (from the lab bench)

### Tip — pin for a reason, not for decoration

Good reasons: keep a fast control loop away from Wi-Fi; run UI on one core
and DSP on the other; match a peripheral ISR’s core. Bad reason: “dual core
means I must pin everything.” Unpinned can be simpler and still correct.

### Tip — shared data is still shared

Two cores writing the same global without sync is a data race — often worse
than on one core because true overlap is more likely. Affinity is **not** a
mutex. This lab avoids sharing mutable state on purpose.

### Tip — LCD / drivers may live on one core

Logging from both cores into a display helper that takes a lock is OK if
the lock is correct. If a driver is “core 0 only” by design, calling it from
core 1 can deadlock or corrupt. Know your BSP. Our `ex_terms_log` path uses
the display lock inside `rb_term` — fine for demos; still treat UI as a
shared resource.

### Tip — same stack / prio rules as ex00

Stack size is still in **words**. `arg` must still outlive the task.
`pdPASS` still matters. Pinning does not fix a too-small stack.

### Tip — `tskNO_AFFINITY` vs hard pin

| Choice | Use when |
|--------|----------|
| Pin `0` / `1` | You need a stable home core |
| `tskNO_AFFINITY` | Soft real-time work that can migrate |
| Only `xTaskCreate` | Same as no affinity on ESP-IDF |

### Tip — proving affinity in reviews

In code review we ask: “Where do you print / assert `xPortGetCoreID()`?”
If the answer is nowhere, the pin is a hope, not a verified property.

### Tip — load balance is not automatic magic

Pinning two light tasks to two cores looks great on this LCD. Pinning ten
heavy tasks all to core 1 makes core 0 idle and core 1 late. Design the
map; then measure (task monitor / runtime stats — later labs).

---

## 8. Focus / avoid / consider

### Focus on

- `xTaskCreatePinnedToCore`’s last argument  
- Reading `xPortGetCoreID()` every loop  
- “Two Running tasks at once” vs ex00’s time-slicing story  

### Avoid

- Assuming pin doubles performance  
- Sharing writable globals across cores with no sync  
- Pinning every task “because the chip has two cores”  
- Forgetting system tasks already occupy both cores  

### Consider

- What if both tasks pin to core 0? (Back to single-core contention.)  
- What if one task is unpinned? (May migrate; `cN` can change.)  
- Where should a future queue producer / consumer live?  

---

## 9. Common mistakes

| Symptom | Likely cause | Check first |
|---------|--------------|-------------|
| Both panes show `c0` | Both pinned to 0 / create failed | Create calls; `pdPASS` |
| `cN` changes over time | Unpinned or wrong API | Last arg of create |
| Crash in log from core 1 | Driver / lock assumptions | BSP thread safety |
| One pane dead | Create failed; bad `arg` lifetime | Heap; `static` cfg |

---

## 10. Checkpoint

- [ ] Can write `xTaskCreatePinnedToCore` from memory  
- [ ] Can explain pin vs `xPortGetCoreID` proof  
- [ ] Can say why shared RAM still needs care  
- [ ] Can contrast this lab with ex00 (affinity vs multitasking intro)  

---

## 11. Experiments

1. Pin **both** tasks to core 0 — both panes show `c0`; they take turns.  
2. Pin both to core 1 — same story on the other CPU.  
3. Use `tskNO_AFFINITY` for one task — watch whether `cN` ever flips.  
4. Drop one task’s `vTaskDelay` — that core stays busy; the other core’s
   pane still advances (true parallel contrast to ex00 starvation demos).  

---

## 12. Troubleshooting

| Symptom | Try |
|---------|-----|
| Blank LCD | `bsp_init`; `-DBOARD=`; only one `exNN_start` |
| Link error on `xTaskCreatePinnedToCore` | ESP-IDF FreeRTOS / SMP target `esp32s3` |
| `ex01.h` not found | Rebuild from `labs/rtos-labs` after adding sources |

---

## 13. Next

You can place work on a core. Next skills: **communicate safely** between
tasks (and cores) with queues / notifications, and **design priorities**
so the important work wins without starving the rest.
