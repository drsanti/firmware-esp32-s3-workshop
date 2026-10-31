# Lab 05 / Step 02 — IMU scope

**Device lab:** [Vibration Scope](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 01](../step-01-scope-face/)

---

## 1. What this step must deliver

- Read IMU live or `imu_sim`.
- Push `acc[0]` (`ax`) into the scope at ~50 Hz.
- Single short loop — **no** 100 Hz draw yet.

**Success:** shake the board (or watch sim) and the strip follows.

---

## 2. Problems this step faces

- Wire sensor → chart without overloading the UI.
- Document the single-loop contract before breaking it.

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| ~50 Hz (`vTaskDelay(20)`) | Comfortable sample+draw rate |
| One loop | Simple; OK while work stays short |
| **No** 10 ms period yet | That stress is step 03 |

### Architecture

One short loop: read IMU or sim, push one sample, yield ~50 Hz.

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│ IMU / sim   │ ──► │ scope_push  │ ──► │  rb_chart   │
│  read ax    │     │             │     │             │
└──────┬──────┘     └─────────────┘     └─────────────┘
       │
       ▼
┌─────────────┐
│ vTaskDelay  │
│ 20 ms ~50Hz │
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
    float acc[3], gyro[3];
    float t = 0.0f;
    uint32_t rng = 1u;
    int real, was_real = -1;

    bsp_init();
    scope_init();
    scope_start();

    for (;;) {
        real = (bsp_imu_read(acc, gyro) == 0);
        if (!real) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += 0.02f;
        }
        if (real != was_real) {
            was_real = real;
            scope_set_live(real);
        }

        scope_push(acc[0]);
        /* Contract: keep short. 100 Hz draw is the break in step 03. */
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
```

### How the code works

**IMU / sim.**  
Same live-vs-sim pattern as Labs 00–02. Status shows which path.

**`scope_push(acc[0])`.**  
One horizontal accel sample per loop → one strip column.

**Delay 20 ms.**  
~50 Hz is enough to look live without saturating draw work.

---

## 6. FreeRTOS in depth — single-loop contract

> Sense + draw may share one loop **only if** every pass stays short.

| Allowed | Not yet |
|---------|---------|
| IMU + one chart push @50 Hz | Sample+draw every 10 ms |

---

## 7. Checkpoint

- [ ] Strip tracks motion / sim  
- [ ] Can name the short-loop contract  
- [ ] Ready to break it at 100 Hz  

---

## 8. Next

[Step 03 — UI @100 Hz](../step-03-ui-100hz/) (**broken**).
