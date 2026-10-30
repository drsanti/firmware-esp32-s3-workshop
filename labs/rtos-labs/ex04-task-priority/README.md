# ex04 — task priority

**Goal:** see how **priority** steers the scheduler: a higher Ready task
runs first; if it never blocks, lower tasks **starve**.

**Phases on the LCD**

| Label | Means | What HIGH does |
|-------|--------|----------------|
| **YLD** | **Yield** — give the CPU away | Burst, then `vTaskDelay` (Blocks) |
| **HOG** | **Hog** — keep the CPU | Burst, then busy-spin (stays Running) |

Builds on [ex00](../ex00-task-creation/) (create + delay) and
[ex01](../ex01-task-affinity/) (pinning). All workers stay on **core 0**
so dual-core cannot hide the effect.

```
┌────────────────────────────────┐
│ ex04 priority  phase=YLD|HOG   │
│                                │
│ HIGH p=6  #820000              │
│ MID  p=5  #205000              │
│ LOW  p=4  #25600               │
└────────────────────────────────┘
```

One full-screen term (not three panes). Workers only bump counters on
**core 0**; a UI task on **core 1** redraws the three lines so the LCD
still updates while HIGH is hogging.

Enable in `main/main.c` with `ex04_start()` (only one exercise at a time).

---

## 1. Learning objectives

| Idea | In this exercise |
|------|------------------|
| Priority order | FreeRTOS: **larger number = higher priority** |
| Preemption | High Ready → mid/low leave Running |
| Ranking | Same loop; bursts **4096 / 1024 / 128** → **HIGH > MID > LOW** |
| Yield | After each burst, `vTaskDelay(1)` → next-lower can run |
| Starvation | HOG: HIGH spins (no delay) → MID/LOW freeze |
| Same core | All pinned to core 0 |

**Success:** in **YLD**, counters stay **HIGH > MID > LOW**; in **HOG**,
only HIGH keeps climbing while MID/LOW stall.

---

## 2. What you should see

### Phase YIELD (~5 s) — header shows `phase=YLD`

All three use the same pattern: busy burst, then `vTaskDelay(1)`.
Bursts are **4096 (HIGH) / 1024 (MID) / 128 (LOW)**. Priority picks who
runs first each round → counters stay **HIGH > MID > LOW**.

### Phase HOG (~4 s) — header shows `phase=HOG`

HIGH busy-spins after its burst instead of delaying. It stays **Running**,
so MID and LOW freeze. UI still redraws from core 1.

Phases auto-cycle forever (controller task at prio 7).

---

## 3. Mental model

```
  higher prio wins when Ready
┌──────────┐  Ready   ┌──────────┐
│  HIGH 6  │ ───────► │ Running  │── busy or delay
└──────────┘          └──────────┘
┌──────────┐          ┌──────────┐
│  MID  5  │ Ready    │ waiting  │  only runs if HIGH Blocked
└──────────┘          └──────────┘
┌──────────┐          ┌──────────┐
│  LOW  4  │ Ready    │ waiting  │  only if HIGH and MID Blocked
└──────────┘          └──────────┘
```

**YIELD:** each Blocks after its burst → HIGH > MID > LOW counts.  
**HOG:** HIGH spins instead of delaying → MID and LOW freeze.

---

## 4. How the code is wired

| Task | Prio | Pin | Role |
|------|------|-----|------|
| `phase` | 7 | core 0 | Flip `s_hog` YIELD ↔ HOG |
| `hi` | 6 | core 0 | burst 4096; delay (YLD) or spin (HOG) |
| `mid` | 5 | core 0 | burst 1024, then `vTaskDelay(1)` |
| `lo` | 4 | core 0 | burst 128, then `vTaskDelay(1)` |
| `ui` | 3 | core 1 | Redraw three counter lines |

```c
for (i = 0; i < burst; i++) s_count[idx]++;  /* 4096 / 1024 / 128 */
if (high && hog) busy_spin_ms(40);           /* no yield → starve lower */
else             vTaskDelay(1);
```

---

## 5. Tips and tricks

### Tip — priority without blocking is a trap

A high-prio task that spins “just for a moment” can lock out networking, UI,
and logging. Prefer: work → block on queue/delay/notify.

### Tip — pin when demonstrating priority

On dual-core, an unpinned low task might run on the other core and look
“fine” while high hogs core 0. This lab pins all workers to core 0 on
purpose.

### Tip — equal delay hides priority

If MID and LOW both `vTaskDelay(20)`, they stay in lockstep and counters
match. Priority only shows when they **compete while Ready** (no matching
delays).

### Tip — measure, then assign levels

In products: few priority bands (ISR deferred / control / UI / idle). Do not
invent a unique level for every task.

---

## 6. Focus / avoid / consider

### Focus on

- **HIGH > MID > LOW** in YLD  
- What `YLD` vs `HOG` does to the counters  
- Why HIGH must *not* call delay during HOG  

### Avoid

- Busy-wait in production high-prio tasks  
- Matching `vTaskDelay` on every worker if you want to *see* priority  
- Mixing this demo with dual-core migration for the workers  

### Consider

- Give MID a rare `vTaskDelay(1)` — LOW gets a trickle, still MID ≫ LOW  
- What if all three use the same prio? (Back toward ex00.)  

---

## 7. Checkpoint

- [ ] Can state FreeRTOS priority direction (higher number wins)  
- [ ] Can explain YLD vs HOG on the LCD  
- [ ] Can say why tasks are pinned to core 0  
- [ ] Can name one real bug caused by high-prio busy-wait  

---

## 8. Experiments

1. Change HOG spin to `5 ms` — starvation weaker / intermittent.  
2. Unpin MID (`xTaskCreate`) — may keep moving on core 1 during HOG.  
3. Give LOW prio 6 and HIGH prio 4 — roles reverse.  

---

## 9. Troubleshooting

| Symptom | Try |
|---------|-----|
| All always move | HOG phase too short; spin optimized / delayed by mistake |
| Nothing moves | Crash; check `ex_terms_begin`; only one `exNN_start` |
| MID moves in HOG on other core | Must use `xTaskCreatePinnedToCore(..., 0)` |

---

## 10. Next

You can starve on purpose — now design so you **do not**: block, notify, and
keep high-prio work short. Queues (ex03) + priority together are how real
pipelines stay fair.
