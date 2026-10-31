# Lab 05 / Step 03 — UI @100 Hz (**broken**)

**Device lab:** [Vibration Scope](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 02](../step-02-imu-scope/)

---

## 1. What this step must deliver

- Same IMU path, but loop period **10 ms** (~100 Hz).
- `scope_push` (draw) on **every** sample.
- You should see lag, stutter, or a sluggish UI.

**Success:** you can name “drawing every sample is too much.”

---

## 2. Problems this step faces

- Chart updates are heavier than a label poke.
- 100 draws/s in the sense task starves responsiveness.

---

## 3. Design approach (intentionally bad)

| Choice | Why (for the lesson) |
|--------|----------------------|
| `vTaskDelay(10)` + `scope_push` | Wants 100 Hz data **and** 100 Hz draw |

### Architecture

Sample and full chart draw share one loop at 10 ms — UI work dominates.

```
┌────────────────────────────────┐
│   single loop @100 Hz (BAD)    │
│ IMU + scope_push every 10 ms   │
│      chart draw too heavy      │
└────────────────────────────────┘
```

### Timing

```
~10 ms target:  .sample+draw.sample+draw.sample+draw.
actual CPU:     .long stall.....late....long stall.....
                      chart stutter / sluggish feel
```

---

## 4. Build

```bash
cd labs/device-labs
# Lab: rbdev run 05  |  Step: ex05.c
rbdev run 05   # from labs/device-labs (step still in ex05.c)
```

---

## 5. Student sketch (`main/main.c`)

```c
scope_set_status("100 Hz draw BAD");

for (;;) {
    /* ... bsp_imu_read / imu_sim_fill ... */
    /*
     * INTENTIONAL BUG: draw on every 100 Hz sample in this task.
     */
    scope_push(acc[0]);
    vTaskDelay(pdMS_TO_TICKS(10));
}
```

### How the code works

**Same structure as step 02**, only the period shrinks to 10 ms and status
advertises the bad rate.

**The bug.**  
Each `scope_push` redraws the HISTORY chart. Doing that 100 times per second
in the only task makes the device feel stuck even though FreeRTOS is running
the delays.

**Contrast.**  
You still want ~100 Hz **samples** for vibration detail — but the **UI** should
draw slower. Step 04 splits those rates with a queue.

---

## 6. FreeRTOS in depth — rate mismatch

| Job | Comfortable rate |
|-----|------------------|
| IMU sample | 100 Hz OK |
| Chart draw | ~30 Hz OK |

One loop forcing both rates to 100 Hz is the architecture smell — not a kernel
bug.

---

## 7. Checkpoint

- [ ] Felt lag / stutter vs step 02  
- [ ] Can say sample rate ≠ draw rate  
- [ ] Ready for a sample queue  

---

## 8. Next

[Step 04 — Split + queue](../step-04-split-queue/).
