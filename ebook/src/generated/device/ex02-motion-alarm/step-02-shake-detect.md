# Lab 02 / Step 02 — Shake detect

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

```text
rbdev run 02
# step switch in ex02-motion-alarm.c:  ex02_step02_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex02-motion-alarm/steps/step-02-shake-detect/README.md</code></p>

</div>


**Device lab:** [Motion Alarm](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 01](../step-01-armed-face/)

---

## 1. What this step must deliver

- Read IMU (live) or `imu_sim` (fallback).
- Compute horizontal accel `sqrt(ax² + ay²)`.
- On a **rising edge** above threshold → show **ALARM** briefly, then ARMED.
- Detail line shows live `horiz` value.
- **No beep** — keep the sense/draw loop short.

**Success:** shake (or wait for sim) → red ALARM → back to ARMED; numbers move.

---

## 2. Problems this step faces

- Need a **shake definition** (threshold + edge, not “always high”).
- Must not put long work in this loop (contract for step 03).

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| Horiz magnitude | Shake moves ax/ay; az is mostly gravity |
| Rising edge `shake && !was_shake` | One trip per entry, not spam every sample |
| Short hold timer | ALARM visible without blocking on audio |
| **No** `beep_play` | Protects smooth banner updates |

### Architecture

```
┌─────────┐  sample   ┌─────────┐  edge?   ┌─────────┐  text   ┌─────────┐
│ IMU or  │ ────────► │  horiz  │ ───────► │  alarm  │ ──────► │   LCD   │
│  sim    │           │  math   │          │  banner │         │         │
└─────────┘           └─────────┘          └─────────┘         └─────────┘
                              (no beep in this loop)
```

### State machine

```
┌──────────────┐   shake edge   ┌──────────────┐   hold done   ┌──────────────┐
│    ARMED     │ ─────────────► │    ALARM     │ ────────────► │    ARMED     │
└──────────────┘                └──────────────┘               └──────────────┘
```

---

## 4. Build / flash
```bash
cd labs/device-labs
# Lab: rbdev run 02  |  Step: ex02.c
rbdev run 02   # from labs/device-labs (step still in ex02.c)
```

---

## 5. Student sketch (`main/main.c`)

```c
void app_main(void)
{
    /* ... locals: acc, gyro, was_shake, alarm_until ... */
    bsp_init();
    alarm_init();
    alarm_start();
    alarm_set_state(ALARM_ST_ARMED);

    for (;;) {
        real = (bsp_imu_read(acc, gyro) == 0);
        if (!real) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += 0.04f;
        }
        if (real != was_real) {
            was_real = real;
            alarm_set_live(real);
        }

        horiz = sqrtf(acc[0]*acc[0] + acc[1]*acc[1]);
        shake = (horiz >= 2.5f);
        if (shake && !was_shake) {
            alarm_set_state(ALARM_ST_ALARM);
            alarm_until = xTaskGetTickCount() + pdMS_TO_TICKS(800);
        }
        was_shake = shake;

        if (alarm_until != 0 && xTaskGetTickCount() >= alarm_until) {
            alarm_until = 0;
            alarm_set_state(ALARM_ST_ARMED);
        }

        alarm_set_detail("horiz %.1f", (double)horiz);
        /*
         * Contract: this loop stays short. Do not call beep_play here —
         * that is the intentional break in step 03.
         */
        vTaskDelay(pdMS_TO_TICKS(40));
    }
}
```

### How the code works

**IMU / sim.**  
`bsp_imu_read` succeeds → live path; otherwise `imu_sim_fill`. `alarm_set_live`
updates the footer when the source changes.

**Shake edge.**  
`horiz = sqrt(ax²+ay²)`. Rising edge (`shake && !was_shake`) flips the banner
to ALARM and starts a short hold timer — visual only, no sound.

**Hold → ARMED.**  
When `alarm_until` expires, return to ARMED so the next shake can trip again.

**`vTaskDelay(40)`.**  
~25 Hz sample/UI pace and a yield. Keep every pass short.

---

## 6. FreeRTOS in depth — single-loop contract

Still only `vTaskDelay` for pacing. The **design** lesson is the contract:

> Sense + draw may share one loop **only if** every pass stays short.

| Allowed | Not allowed (yet) |
|---------|-------------------|
| IMU read, math, LCD text | `beep_play` (hundreds of ms) |

Step 03 breaks the contract on purpose so you feel the freeze.

---

## 7. Checkpoint

- [ ] Can explain threshold + rising edge  
- [ ] Can name the single-loop contract  
- [ ] ALARM flashes on shake / sim without audio  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Never ALARM on sim | Wait ~1–2 s; thresh 2.5 matches sim swing |
| Never ALARM on live | Shake harder; check `IMU live` line |
| Always ALARM | Edge latch broken — need `was_shake` |

---

## 9. Next

[Step 03 — Busy alarm](../step-03-busy-alarm/) (**broken** on purpose).
