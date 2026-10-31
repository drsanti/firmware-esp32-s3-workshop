# Lab 01 / Step 02 — IMU → bubble

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

```text
rbdev run 01
# step switch in ex01-spirit-level.c:  ex01_step02_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex01-spirit-level/steps/step-02-imu-bubble/README.md</code></p>

</div>


**Device lab:** [Spirit Level](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 01](../step-01-draw-tube/) (spirit face) and Lab 00 IMU idea

---

## 1. What this step must deliver

By the end of this step, the board should:

- Keep the **spirit-level face** from step 01.
- Move the bubble from **real tilt** when the IMU works, or from **sim** when it does not.
- Still refresh smoothly (~30 Hz) in **one short loop** — no long blocking calls.

**Success:** tilt the board (or watch sim motion); the bubble follows; LEVEL appears when centered. Both boards count as success (live or sim).

---

## 2. Problems this step faces

- Fake tilt was only a demo — a real instrument needs **sensor data**.
- Not every workshop board has a working IMU → need a **sim fallback** (same idea as Lab 00 step 04).
- One loop is still fine **only if** each pass stays short. Blocking audio here would freeze the bubble (that failure is step 03 on purpose).

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| Same `spirit_*` UI | Face already proven; only the tilt source changes |
| `bsp_imu_read`, else `imu_sim_fill` | Live when possible; sim keeps the lab runnable |
| Single loop + `vTaskDelay(33)` | Sensor + draw together while work stays tiny |
| **No** `beep_play` in this loop | Protects smooth motion; documents the single-loop **contract** |

### Architecture

Same single loop as step 01; IMU or sim feeds the bubble each frame.

```
┌─────────────┐
│  app_main   │
│ loop+delay  │
└──────┬──────┘
       │
       ▼
┌─────────────┐     ok      ┌─────────────┐
│bsp_imu_read │ ──────────► │  acc ax ay  │
└──────┬──────┘             └──────┬──────┘
       │ fail                      │
       ▼                           ▼
┌─────────────┐             ┌─────────────┐
│ imu_sim_fill│ ──────────► │  spirit_*   │
└─────────────┘             └─────────────┘
```

---

## 4. Build / flash
Once per PC (workshop root): `rbdev init`, then a **new** terminal.
From the lab folder use `rbdev run NN` (no `export.ps1` / `export.sh`).


```bash
cd labs/device-labs
# Lab: rbdev run 01  |  Step: ex01.c

rbdev run 01   # from labs/device-labs (step still in ex01.c)
# or
idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx monitor
```

---

## 5. Student sketch (`main/main.c`)

```c
void app_main(void)
{
    float acc[3], gyro[3], t = 0.0f;
    uint32_t rng = 1u;
    int real, was_real = -1;

    bsp_init();
    spirit_init();
    spirit_start();

    for (;;) {
        real = (bsp_imu_read(acc, gyro) == 0);
        if (!real) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += 0.033f;
        }
        if (real != was_real) {
            was_real = real;
            spirit_set_live(1);
        }
        spirit_set_accel_mps2(acc[0], acc[1]);
        vTaskDelay(pdMS_TO_TICKS(33));
    }
}
```

### How the code works

**Sensor choice.**  
Each pass tries `bsp_imu_read`. Return `0` means hardware answered. Any failure
fills `acc` / `gyro` with `imu_sim_fill` and advances sim time `t` so motion
keeps changing.

**Bubble update.**  
`spirit_set_accel_mps2(acc[0], acc[1])` uses X/Y accel (m/s²), same units as
Lab 00. The helper maps that into the circular level and LEVEL / TILT.

**Single-loop contract.**  
This architecture is valid **while** the loop body stays short: read (or sim),
update UI, delay. Do **not** put `beep_play` (or other long work) here — that
breaks smooth refresh. Step 03 will break that contract on purpose so you can
feel the failure.

---

## 6. FreeRTOS in depth — same delay, new contract

### What is new vs step 01?

| Step 01 | Step 02 |
|---------|---------|
| Fake `ax` triangle | Real/sim IMU samples |
| Delay for animation | Delay for **sample + draw** pacing |

FreeRTOS API is still mainly `vTaskDelay` + `pdMS_TO_TICKS`. The **design**
lesson is the contract: one loop is allowed only if it does not block for long.

### Why ~33 ms again?

- ~30 Hz is enough for a bubble the eye can follow.  
- Yields every frame so the scheduler stays healthy.  
- Matches sim `SAMPLE_DT_S` so synthetic motion does not jump oddly.

Ticks and `pdMS_TO_TICKS` work as in step 01 (ms → ticks for your
`configTICK_RATE_HZ`). Relative `vTaskDelay` is still fine: IMU read + UI
update are short.

### Why not a second task yet?

| Option | When |
|--------|------|
| One short loop (this step) | Sense + draw both cheap; no audio |
| Split tasks + queue (step 04) | After you see blocking destroy smoothness |
| Notify + audio (step 05) | LEVEL cue without freezing the bubble |

Introducing a queue before students feel the failure would skip the “why.”

### Why not `vTaskDelayUntil` yet?

Same as step 01: work is small; soft ~30 Hz is enough. Absolute periods matter
more for hard sample clocks later.

### Mental model

> One loop may own sensor **and** UI **only while every pass stays short**.
> Delay paces and yields. The moment you add long blocking work, this design
> fails — that is the next step’s lesson.

---

## 7. Checkpoint

- [ ] Can explain live vs sim and why both are success  
- [ ] Can state the single-loop contract (no long blocking in the loop)  
- [ ] Bubble follows tilt (or sim); LEVEL near center  
- [ ] Works on your board (`ws28p` or `es3c28p`)  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| `idf.py` / flash fails | Re-run `rbdev init`; use `rbdev run NN` |
| Bubble never moves on `es3c28p` | Sim should still animate; check rebuild with `-DBOARD=es3c28p` |
| Always still on `ws28p` | Wrong `BOARD`; `fullclean` and rebuild |
| Blank LCD | `bsp_init` / `spirit_start`; match board SKU |

---

## 9. Next

[Step 03 — Busy loop (broken)](../step-03-busy-loop/).
