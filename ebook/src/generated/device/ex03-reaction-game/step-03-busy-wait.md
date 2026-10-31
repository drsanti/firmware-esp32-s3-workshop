# Lab 03 / Step 03 — Busy wait (**broken**)

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

<span class="ebook-badge warn">broken on purpose</span>

```text
rbdev run 03
# step switch in ex03-reaction-game.c:  ex03_step03_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex03-reaction-game/steps/step-03-busy-wait/README.md</code></p>

</div>


**Device lab:** [Reaction Game](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 02](../step-02-go-and-tap/)

---

## 1. What this step must deliver

- Same game flow as step 02.
- WAIT uses a **busy-spin** until the deadline.
- You should notice the UI is stuck / device feels “dead” during WAIT.

**Success:** you can name why spinning is worse than `vTaskDelay`.

---

## 2. Problems this step faces

- Spinning until a tick deadline uses 100% CPU and never yields.
- No chance to refresh a “still waiting…” hint.

---

## 3. Design approach (intentionally bad)

| Choice | Why (for the lesson) |
|--------|----------------------|
| Busy `while` until deadline | Looks like “wait until time T”; starves UI |

### Architecture

```
┌──────────┐  set WAIT  ┌──────────┐  spin      ┌─────────┐
│   ui     │ ─────────► │ deadline │ ──────────►│  tick   │
│  loop    │            │  loop    │  no yield  │ compare │
└──────────┘            └──────────┘            └─────────┘
                               │
                               ▼
                        banner frozen / CPU hot
```

### State machine

```
┌────────┐   BOOT   ┌────────┐   spin done   ┌────────┐
│ READY  │ ───────► │  WAIT  │ ────────────► │   GO   │
└────────┘          │ (busy) │               └────────┘
                    └────────┘
```

---

## 4. Build

```bash
cd labs/device-labs
# Lab: rbdev run 03  |  Step: ex03.c
rbdev run 03   # from labs/device-labs (step still in ex03.c)
```

---

## 5. Student sketch (`main/main.c`)

```c
wait_ms = 1000u + (rnd_u32() % 2000u);
react_set_state(REACT_ST_WAIT);
react_set_detail("BUSY wait %u", (unsigned)wait_ms);
react_set_hint("banner freezes");

deadline = xTaskGetTickCount() + pdMS_TO_TICKS(wait_ms);
/*
 * INTENTIONAL BUG: spin until deadline — no yield.
 * UI cannot refresh; other tasks starve. Do not ship this.
 */
while (xTaskGetTickCount() < deadline) {
    spin++;
}

go_tick = xTaskGetTickCount();
react_set_state(REACT_ST_GO);
/* ... BOOT edge + RESULT ms same as step 02 ... */
```

### How the code works

**Same round shape as step 02** until WAIT.

**The bug.**  
Instead of `vTaskDelay(wait_ms)`, the code computes a future `deadline` and
spins in a tight `while` reading `xTaskGetTickCount()`. That loop never calls
a blocking FreeRTOS API, so this task stays Running and the banner cannot show
a live hint.

**After the spin.**  
GO / BOOT / RESULT are unchanged — the lesson is only the wait strategy.

**Contrast.**  
Step 02’s `vTaskDelay` at least yielded (banner still “frozen” because blocked,
but polite). Step 04’s timer lets the UI keep pulsing during WAIT.

---

## 6. FreeRTOS in depth — delay vs busy-wait

| Approach | Yields? | UI during WAIT | CPU |
|----------|---------|----------------|-----|
| `vTaskDelay` (step 02) | Yes | Frozen *because blocked*, but polite | Low |
| Busy-spin (this step) | No | Frozen + hostile | High |
| Timer (step 04) | Yes | Can stay live | Low |

| Myth | Reality |
|------|---------|
| “I’m just waiting for a tick” | Without block/yield you monopolize the CPU |
| “FreeRTOS failed” | Architecture chose a spin |

---

## 7. Checkpoint

- [ ] Felt / saw freeze during WAIT  
- [ ] Can point at the `while (tick < deadline)` bug in the sketch  
- [ ] Can contrast spin vs `vTaskDelay` vs timer  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| No obvious freeze | Use a long wait (~3 s); watch hint stuck on “banner freezes” |
| Device resets | WDT may trip on long spin — that *is* the lesson |

---

## 9. Next

[Step 04 — Timer GO](../step-04-timer-go/).
