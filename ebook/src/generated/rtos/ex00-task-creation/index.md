# ex00 — task creation

<div class="ebook-run">

**Track:** `rtos-labs` · **Project:** `labs/rtos-labs`

```text
rbdev run 00
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/rtos-labs/ex00-task-creation/README.md</code></p>

</div>


**Goal:** understand how to **create tasks**, how **two tasks run together**,
and how the **scheduler** switches them.

This is the first FreeRTOS exercise on purpose. Before queues, mutexes, or
priorities, you need a solid picture of *what a task is* and *who decides
which one runs*. The LCD is only a window. Each task owns one `rb_term` pane
so you can *see* both loops advancing at different rates.

```
┌────────────────┐  ┌────────────────┐
│ blink (cyan)   │  │ count (yellow) │
│ blink #0 c0    │  │ count #0 c1    │
│ blink #1 c0    │  │ count #1 c0    │
│ every 500 ms   │  │ every 1000 ms  │
└────────────────┘  └────────────────┘
```

`cN` is `xPortGetCoreID()` — which CPU ran that wake. With plain
`xTaskCreate` there is **no pin**, so `c0` / `c1` may appear on either pane
and can change over time.

Enable in `main/main.c` with `ex00_start()`.

---

## 1. Learning objectives

| Idea | In this exercise |
|------|------------------|
| Create a task | `xTaskCreate(fn, name, stack, arg, prio, handle)` |
| Multitasking | Two infinite loops — each looks like it has the CPU |
| Scheduler | Picks who runs; `vTaskDelay` blocks so a peer can run |
| Task states | **Running** → `vTaskDelay` → **Blocked** → wake → **Ready** → **Running** |
| Which core? | `xPortGetCoreID()` — no affinity → may be 0 or 1 |

**Success:** both panes keep scrolling; blink advances about twice as fast as
count. Each line shows `cN`. Cores may match or differ; either is correct
without pinning.

**What this exercise is not:** forcing a core (`xTaskCreatePinnedToCore` —
see [ex01](../ex01-task-affinity/)), priority design, or LCD widgets as the
topic. The UI is evidence.

---

## 2. Mental model — what is a task?

A FreeRTOS **task** is a function that never returns (normally), plus:

- its own **stack**
- its own saved **registers / program counter** when it is not Running
- a **priority** the scheduler uses when choosing who runs
- an optional **name** and **handle** for debugging

```
┌──────────────────────────────────────────┐
│                 task TCB                 │
│  state, priority, name, stack pointer    │
└───────────────────┬──────────────────────┘
                    │
                    ▼
┌──────────────────────────────────────────┐
│              private stack               │
│  locals, call frames, ISR nesting room   │
└──────────────────────────────────────────┘
```

**Analogy we use in reviews:** a task is a thread that the RTOS owns. It is
not a C++ thread object, not a Linux process, and not “a function you call
when you feel like it.” Once created, it lives until you delete it (or the
system resets).

**ESP-IDF note:** `app_main` itself runs in a task created by the startup
code. When `ex00_start` returns, that does **not** stop blink/count — they
are separate tasks. The idle task (and other system tasks) keep running too.

---

## 3. How to create a task

```c
(void)xTaskCreate(pane_task, "blink", 3072, &blink, 4, NULL);
/*                 |          |       |      |      |  |
 *                 fn         name    stack  arg  prio handle
 */
```

| Argument | Meaning here | Field tip |
|----------|----------------|-----------|
| `pane_task` | Function that becomes the task body (`void fn(void *)`) | One function can back many tasks if `arg` differs |
| `"blink"` | Name (debugger / `vTaskList` / monitor) | Keep short; FreeRTOS truncates long names |
| `3072` | Stack depth in **words** (`StackType_t`), not bytes | On Xtensa, words are 4 bytes → ~12 KiB here |
| `&blink` | Passed as `pvParameters` into `pane_task` | Must outlive the task (see Tips) |
| `4` | Priority (same for both tasks in this exercise) | Higher number = higher priority in FreeRTOS |
| `NULL` | We do not keep a `TaskHandle_t` | Pass a handle when you need delete / notify later |

`ex00_start` returns after creating both tasks. The tasks keep running under
the scheduler; creation is the last FreeRTOS step in this function.

### Return value — check it in real firmware

```c
BaseType_t ok = xTaskCreate(...);
if (ok != pdPASS) {
    /* out of heap for TCB/stack — do not pretend the task exists */
}
```

In classroom demos we often cast to `void`. In shipping code, **always**
check `pdPASS`. Silent failure looks like “my task never runs” for hours.

### Where does the task run? (`xPortGetCoreID`)

`xTaskCreate` does **not** take a core id. On ESP-IDF SMP that means
**no affinity** (`tskNO_AFFINITY`): the task may Run on **core 0 or core 1**,
and the scheduler may **migrate** it later.

```c
ex_terms_log(cfg->term, cfg->fg, "%s #%lu c%d", cfg->name,
             (unsigned long)n, (int)xPortGetCoreID());
```

| What you see | Meaning |
|--------------|---------|
| Always `c0` | Fine — scheduler kept you on core 0 for now |
| Mix of `c0` / `c1` | Fine — no pin; migration is allowed |
| Want a fixed core | Use `xTaskCreatePinnedToCore` (ex01) |

**Field tip:** never assume “I created it from `app_main` on core 0, so the
task lives on core 0 forever.” Print `xPortGetCoreID()` when core matters.
Hope is not affinity.

---

## 4. How multitasking looks

Both tasks share the **same function** (`pane_task`) but different **args**
(pane, name, color, period). That is still two tasks — two stacks, two
program counters, two independent `n` locals.

```
┌────────────┐  create   ┌────────────┐
│ ex00_start │ ────────► │ blink task │  left pane, 500 ms
└────────────┘           └────────────┘
┌────────────┐  create   ┌────────────┐
│ ex00_start │ ────────► │ count task │  right pane, 1000 ms
└────────────┘           └────────────┘
```

On the LCD you watch **two independent counters**. That is the evidence that
multitasking works — not that two CPUs are busy all the time.

### Same function, two tasks — why professionals do this

| Approach | When it is good |
|----------|-----------------|
| One `fn`, many tasks + `arg` | Identical behavior, different config (this lab) |
| Separate `blink_task` / `count_task` | Bodies diverge; clearer names in backtraces |
| One task that does everything | Usually worse — harder timing, harder debugging |

Reuse the function when the **control flow** is the same. Split functions
when the **job** is different (sensor vs UI vs protocol).

---

## 5. How the scheduler works (this exercise)

FreeRTOS on ESP-IDF is **preemptive**: a higher-priority Ready task can
preempt a lower-priority Running task. In *this* exercise both tasks have
the **same** priority, so you mainly see **blocking** make room for the peer.

The important call is `vTaskDelay`:

```
  Running              Blocked               Ready             Running
┌──────────┐ vTaskDelay ┌──────────┐  tick  ┌──────────┐ pick ┌──────────┐
│  blink   │ ─────────► │  blink   │ ─────► │  blink   │ ───► │  blink   │
│  logs #n │            │  waiting │        │ wants CPU│      │  logs    │
└──────────┘            └──────────┘        └──────────┘      └──────────┘
```

While blink is Blocked, the scheduler can Run count (or idle).

| Call | Effect |
|------|--------|
| `pdMS_TO_TICKS(ms)` | Convert milliseconds → ticks for this build |
| `vTaskDelay(ticks)` | This task **Blocked**; scheduler runs someone else |
| No delay in a tight loop | Task stays **Running** and starves same/lower peers |

Blink delays 500 ms; count delays 1000 ms. While one is Blocked, the other
(or idle) can Run. That is **cooperative yielding** on top of a preemptive
kernel.

**Tick rate:** delays are quantized to the RTOS tick (often 100 Hz / 10 ms
on ESP-IDF defaults). Asking for 1 ms may become one tick. Do not design
hard real-time edges with `vTaskDelay` alone — later labs cover
`vTaskDelayUntil` and hardware timers.

**Not yet:** locking a task to one core (`xTaskCreatePinnedToCore` in
[ex01](../ex01-task-affinity/)), priority design, mutexes, notifications.

---

## 6. Read the code (`ex00.c`)

1. `ex_terms_begin(1, 2)` — UI helper only (see `common/ex_terms.h`).
2. Fill two `pane_cfg_t` structs (arg for each task).
3. Two `xTaskCreate` calls — **this is the FreeRTOS lesson** (no core pin).
4. Each task: log with `xPortGetCoreID()` → `vTaskDelay` → forever.

Notice `blink` and `count` are **`static`**. That is deliberate — see Tips.

---

## 7. Tips and tricks (from the lab bench)

### Tip — `arg` lifetime is a classic bug

```c
void bad_start(void)
{
    pane_cfg_t cfg;          /* stack of the creator */
    cfg.period_ms = 500;
    xTaskCreate(pane_task, "blink", 3072, &cfg, 4, NULL);
    /* bad_start returns → cfg is gone → pane_task reads garbage */
}
```

**Do this instead:** `static` storage, file-scope globals, or heap that you
own for the life of the task. Passing `&local` into `xTaskCreate` is one of
the most common student (and junior) bugs.

### Tip — tasks must not “return”

Falling off the end of a task function is undefined / asserts in many
configs. Use `for (;;)` or call `vTaskDelete(NULL)` if the work is truly
finished.

### Tip — stack size is in words, and “enough” is measured

`3072` is not magic. Too small → stack overflow (canary / panic). Too large
→ wasted RAM (many small MCUs die here first). In real projects:

1. Start with a generous size while bringing a feature up.
2. Use high-water mark APIs / Lab 06-style monitoring.
3. Trim with evidence, not vibes.

Leave headroom for printf-style formatting, deep call chains, and ISRs that
nest on the current stack (port-dependent).

### Tip — always leave a blocking point in periodic work

A task that only burns CPU:

```c
for (;;) {
    do_work();   /* no vTaskDelay / queue / semaphore */
}
```

will starve same-priority peers (time-slicing helps only if enabled and only
among Ready tasks of equal prio — do not rely on it for structure). Design
the loop as **work → wait**, not **work forever**.

### Tip — names are free debugging

`"blink"` / `"count"` show up in crashes, `uxTaskGetSystemState`, and
monitors. `"t1"` / `"t2"` waste your future self’s time. Prefer verbs or
subsystem names: `imu_sample`, `ui_draw`, `net_tx`.

### Tip — log the core when placement matters

`xPortGetCoreID()` is cheap evidence. In bring-up we often log it once at
task start *and* periodically (as here). If a bug is “works on my desk,
fails with Wi-Fi,” compare cores before you rewrite the algorithm.

### Tip — same priority is a teaching tool, not a product default

Equal priority + delays is the clearest way to *see* multitasking. In
products you usually separate:

| Kind of work | Typical priority idea |
|--------------|------------------------|
| Hard deadlines / control | Higher |
| UI / logging | Lower |
| Background housekeeping | Lowest (or idle hooks) |

Do not invent a unique priority for every task “just in case.” Fewer levels,
clear rules.

### Tip — check creation; log failures early

Out-of-memory at `xTaskCreate` is common when someone copies a demo and
spawns ten fat stacks. Fail loud at boot, not with a blank pane three
minutes later.

### Tip — one responsibility per task (scale this habit now)

This exercise is tiny, but the habit matters: blink does not also parse
UART; count does not also drive I2C. When a task owns two unrelated jobs,
timing bugs become undebuggable. Split early.

---

## 8. Focus / avoid / consider

### Focus on

- Exact meaning of each `xTaskCreate` argument  
- Task states: Running / Ready / Blocked  
- Why `vTaskDelay` is both a **timer** and a **yield**  
- Evidence on the LCD: two rates + `cN` from `xPortGetCoreID()`  
- No pin ⇒ core is **observed**, not promised  

### Avoid

- Passing pointers to creator-stack locals as `arg`  
- Tight loops with no blocking API  
- Returning from a task body  
- Assuming stack size is in bytes  
- Assuming “created on core 0” means “always runs on core 0”  
- Treating “it works on my board” as proof of stack safety  

### Consider

- What happens if blink’s period becomes `1` and count’s stays `1000`?  
- What happens if you delete the delay in one task only?  
- Where does the idle task fit when both are Blocked?  
- How would you stop one task later (`TaskHandle_t` + `vTaskDelete`)?  
- How would you make `cN` never change? (ex01 — pin the task)  

---

## 9. Common mistakes (field notes)

| Symptom | Likely cause | What we check first |
|---------|--------------|---------------------|
| One pane never moves | Peer stuck in tight loop / higher prio hog | Delays and priorities |
| Crash right after create | Stack too small / bad `arg` pointer | Stack size; `static` cfg |
| Both rates look identical | Same `period_ms` by copy-paste | Config structs |
| Task “never started” | `xTaskCreate` failed (`!= pdPASS`) | Heap; return value |
| Weird strings / colors | `arg` pointed at dead stack | Lifetime of `pane_cfg_t` |
| Works in debug, dies in release | Stack closer to the edge | High-water mark |

---

## 10. Checkpoint

- [ ] Can name every `xTaskCreate` argument and its unit for stack  
- [ ] Can explain why both panes move without a second CPU busy forever  
- [ ] Can say what `vTaskDelay` does to task state  
- [ ] Can explain why `blink` / `count` configs are `static`  
- [ ] Can predict what happens if you remove the delay  
- [ ] Can say why checking `pdPASS` matters in production  
- [ ] Can explain what `cN` means and why it may change without a pin  

---

## 11. Experiments (try on the board)

1. Change blink to `100` ms — relative rates should jump.  
2. Remove `vTaskDelay` from one task only — watch the other pane starve or
   stutter.  
3. Give count priority `5` and blink `4` — with both still delaying, both
   run; remove count’s delay and blink suffers (preview of priority).  
4. Watch `cN` for a minute — note whether either pane ever flips core.  
5. Add a third pane: `ex_terms_begin(1, 3)` + a third `xTaskCreate`.  
6. Intentionally pass a stack-local cfg (then fix it) — feel the bug once so
   you recognize it forever.  
7. Move to [ex01](../ex01-task-affinity/) — pin cores and see `cN` freeze.

---

## 12. Troubleshooting

| Symptom | Try |
|---------|-----|
| Blank LCD | `bsp_init` in `app_main`; correct `-DBOARD=` |
| Only one pane logs | Check both `xTaskCreate` calls and both periods |
| Build cannot find `ex_terms.h` | Build from `labs/rtos-labs`; after adding `common/` or any new `.c`, run `idf.py -DBOARD=ws28p reconfigure` (or `-DBOARD=es3c28p`) then build |
| Linker `undefined reference` to a new `.c` | Same: reconfigure for your board, then build — see [rtos-labs README](../README.md#build--flash) |
| Linker / heap assert at create | Lower stack sizes or free heap; check `pdPASS` |

---

## 13. Next

Keep this picture in your head: **create → Ready → Run → Block → Ready**.
Every later FreeRTOS object (queue, semaphore, notification) is another way
to move a task between those states on purpose.
