# Lab 05 / Step 01 — Scope face

**Device lab:** [Vibration Scope](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. What this step must deliver

- Full-screen **HISTORY** chart titled for vibration / `ax`.
- A **fake sine** fills left→right at ~30 Hz, then shifts (no IMU yet).
- Smooth updates via `vTaskDelay`.

**Success:** a wave builds across the plot, then slides with newest on the right.

---

## 2. Problems this step faces

- Need a scope face before sensors.
- Need paced push + yield.

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| `scope_*` + `rb_chart` HISTORY | Ring buffer; full plot redraw |
| Fake sine via `imu_sim_sin` | No IMU yet; proves push path |
| `vTaskDelay(33)` | ~30 Hz UI |

### Architecture

Fake sine feeds the HISTORY ring; delay sets frame rate.

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  imu_sim    │ ──► │ scope_push  │ ──► │  rb_chart   │
│ fake sine   │     │ ring sample │     │  HISTORY    │
└──────┬──────┘     └─────────────┘     └─────────────┘
       │
       ▼
┌─────────────┐
│ vTaskDelay  │
│ 33 ms ~30Hz │
└─────────────┘
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
void app_main(void)
{
    float t = 0.0f;
    float ax;

    bsp_init();
    scope_init();
    scope_start();
    scope_set_status("fake wave");

    for (;;) {
        ax = 8.0f * imu_sim_sin(2.0f * IMU_SIM_PI * 0.7f * t);
        scope_push(ax);
        t += 0.033f;
        vTaskDelay(pdMS_TO_TICKS(33));
    }
}
```

### How the code works

**`scope_start`.**  
Opens the display and mounts a one-channel **HISTORY** chart (ring buffer
inside `lab_scope`, capacity sized for a full-width pane).

**Fake wave.**  
Each loop computes a soft sine (helper from `imu_sim.h`, no `libm`) and
`scope_push`es it into the history ring. The plot grows left→right; once
full, it shifts so the newest sample stays at the right edge.

**`vTaskDelay(33)`.**  
~30 Hz pace + yield. Step 02 swaps the fake value for IMU/`imu_sim_fill`.

---

## 6. FreeRTOS in depth — `vTaskDelay`

Same pace + yield lesson. Later steps stress what happens when you push/draw
much faster than the panel can comfortably update.

---

## 7. Checkpoint

- [ ] Wave fills then shifts on the LCD (HISTORY)  
- [ ] Can name `scope_push`  
- [ ] Knows delay sets ~30 Hz  

---

## 8. Next

[Step 02 — IMU scope](../step-02-imu-scope/).
