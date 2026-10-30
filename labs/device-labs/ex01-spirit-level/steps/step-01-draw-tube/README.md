# Lab 01 / Step 01 — Draw the spirit level

**Device lab:** [Spirit Level](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** Lab 00 LCD / FreeRTOS delay basics

---

## 1. What this step must deliver

By the end of this step, the board should:

- Show a **spirit-level face** on the LCD: large bubble, thin top bar with title and status.
- **Move the bubble** using fake tilt (no IMU yet).
- Show **LEVEL** near center and **TILT** otherwise.
- Refresh the UI smoothly (~30 times per second) **without** spinning the CPU in a tight empty loop.

**Success:** the bubble walks left/right on its own; the status flips LEVEL ↔ TILT as it crosses center.

---

## 2. Problems this step faces

- We need an **instrument UI**, not a screen full of raw numbers.
- We must redraw often, but a tight `for (;;)` with no pause would **burn CPU** and fight the scheduler.
- The IMU comes in a later step — for now we still need to prove the **face** of the device.

---

## 3. Design approach (why this FreeRTOS feature)

| Choice | Why |
|--------|-----|
| Single loop in `app_main` | One job only: drive fake tilt → UI. No concurrency yet |
| `vTaskDelay(33 ms)` | Sets ~30 Hz frame rate **and** yields so FreeRTOS idle / other work can run |
| `spirit_*` helper + `rb_level` | Library draws the bubble; you own the tilt values |

**Not yet:** tasks, queues, notify, IMU. Those appear when later requirements force them.

### Architecture

One task drives fake tilt into the spirit helper; delay sets frame rate and yield.

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  app_main   │ ──► │  spirit_*   │ ──► │ bubble+text │
│ fake ax     │     │  rb_level   │     │ LEVEL/TILT  │
└──────┬──────┘     └─────────────┘     └─────────────┘
       │
       ▼
┌─────────────┐
│ vTaskDelay  │
└─────────────┘
```

---

## 4. Activate ESP-IDF, then build

```powershell
. <IDF_PATH>\export.ps1
```

```bash
cd labs/device-labs
# Lab: .\run 01  |  Step: ex01.c

.\run 01   # from labs/device-labs (step still in ex01.c)
# or
idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx monitor
```

---

## 5. Student sketch (`main/main.c`)

```c
void app_main(void)
{
    float ax = 0.0f;
    int dir = 1;

    bsp_init();
    spirit_init();
    spirit_start();
    spirit_set_live(1);

    for (;;) {
        ax += (float)dir * 0.35f;
        if (ax > 8.0f) { ax = 8.0f; dir = -1; }
        else if (ax < -8.0f) { ax = -8.0f; dir = 1; }

        spirit_set_accel_mps2(ax, 0.0f);
        vTaskDelay(pdMS_TO_TICKS(33));
    }
}
```

### How the code works

**Drawing the face and moving the bubble.**  
`spirit_start` lays out the LCD (title, status, large bubble). Each
`spirit_set_accel_mps2` moves the bubble and updates LEVEL / TILT.

**Why the loop ends with a delay.**  
We want a smooth ~30 Hz refresh **without** a busy-spin that burns the CPU.
That is what `vTaskDelay(pdMS_TO_TICKS(33))` is for — details in the FreeRTOS
section below.

Hold this model: *UI first, sensor later.* Step 02 will replace the fake `ax`
sweep with real (or sim) IMU samples — same `spirit_*` calls.

---

## 6. FreeRTOS in this step — `vTaskDelay` in depth

This step’s only FreeRTOS call is the delay. Treat it as a **design choice**,
not magic sleep.

### Why we need a delay at all

```text
  without delay                    with vTaskDelay(33 ms)
  --------------------             -------------------------
  for (;;) {                       for (;;) {
    update bubble;                   update bubble;   // ~1 ms work
    /* spin again NOW */             vTaskDelay(...); // sleep ~32 ms
  }                                }
  CPU ~100% busy                   CPU free most of the time
  frame rate = "as fast as         frame rate ~= 30 Hz (stable)
    silicon allows" (unstable)
  other tasks starve               scheduler can run idle / peers
```

Two jobs in one call:

1. **Pace** the UI (~30 Hz ≈ every 33 ms) so the bubble motion looks even.  
2. **Yield** so FreeRTOS can run other ready work (idle hook, later tasks).

A busy `for (;;)` with no delay fails the smooth-refresh goal: it burns
power/CPU and does not play nicely with a multitasking kernel — even when you
only have one loop today.

### What is a *tick*?

FreeRTOS does not schedule in milliseconds internally. It counts **ticks** —
short time slices from a hardware timer (often 1 ms or 10 ms per tick,
depending on `configTICK_RATE_HZ`).

| Idea | Meaning |
|------|---------|
| Tick | Smallest time unit the scheduler counts |
| `configTICK_RATE_HZ` | How many ticks per second (e.g. 1000 → 1 ms/tick) |
| Delay argument | Always in **ticks**, not ms |

### What `pdMS_TO_TICKS(33)` does

```c
vTaskDelay(pdMS_TO_TICKS(33));
```

`pdMS_TO_TICKS(ms)` converts **human milliseconds → ticks** for your build:

- If 1 tick = 1 ms → `pdMS_TO_TICKS(33)` → `33` ticks.  
- If 1 tick = 10 ms → `pdMS_TO_TICKS(33)` → about `4` ticks (~40 ms real time).

You write delays in **ms** so the code stays readable; the macro adapts to
the board’s tick rate. Prefer `pdMS_TO_TICKS(...)` over hard-coding tick
counts like `vTaskDelay(33)`.

Rounding note: conversion rounds to a whole number of ticks, so the real
sleep is “at least about 33 ms,” not a perfect crystal period.

### What `vTaskDelay` does (relative delay)

`vTaskDelay(n)` means: **block this task for at least `n` ticks from now**,
then wake it again.

- The task leaves the Running state (Blocked).  
- The scheduler can run something else.  
- After ≥ `n` ticks, the task becomes Ready and may run again.

It is a **relative** delay: the countdown starts when you call it. Time spent
updating the bubble *before* the delay is **not** subtracted. So one full
loop is roughly:

```text
  loop period ≈ (work time) + (delay time)
             ≈ 1 ms + 33 ms  ≈ 34 ms   // slightly slower than 30 Hz
```

For a soft UI animation that is fine. Precise periods need a different API.

### Why not `vTaskDelayUntil` (yet)?

| API | Behavior | Best when |
|-----|----------|-----------|
| `vTaskDelay` | Sleep **N ticks from now** | Simple pacing; work time is tiny or jitter OK |
| `vTaskDelayUntil` | Wake at an absolute tick deadline; next wake = last + period | Fixed-rate loops (control, sample clocks) |

`vTaskDelayUntil` keeps a `TickType_t last_wake` and aims for a **constant
period** even if work time varies:

```c
TickType_t last = xTaskGetTickCount();
for (;;) {
    /* work */
    vTaskDelayUntil(&last, pdMS_TO_TICKS(33));
}
```

**Why this step uses `vTaskDelay` instead:**

1. Work here is tiny (fake `ax` + one UI update) — relative vs absolute barely matters.  
2. Fewer concepts for the first Spirit Level step (no `last_wake` handle).  
3. Lab 01 later cares about **architecture** (queue / notify), not servo-grade timing.

**When you would switch to `vTaskDelayUntil`:** a sensor or control loop that
must stay close to 50 Hz / 100 Hz even if drawing sometimes takes longer.
That shows up in later device labs (e.g. sampling clocks), not here.

### Mental model for the checkpoint

> `vTaskDelay` is how this firmware keeps a **smooth UI refresh** and still
> **yields** to the scheduler. `pdMS_TO_TICKS` is only the ms→ticks translator.
> `vTaskDelayUntil` is the next tool when period accuracy becomes a product
> need — we are not there yet.

---

## 7. Checkpoint

- [ ] Can describe what this step must show on the LCD  
- [ ] Can explain why a delay is required (pace + yield, not “just sleep”)  
- [ ] Can say what `pdMS_TO_TICKS` converts  
- [ ] Can say why `vTaskDelay` is enough here vs `vTaskDelayUntil`  
- [ ] Bubble moves; LEVEL appears near center  

---

## 8. Next

[Step 02 — IMU → bubble](../step-02-imu-bubble/).
