# Lab 02 / Step 01 — Armed face

**Device lab:** [Motion Alarm](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** Lab 00 LCD / `vTaskDelay`

---

## 1. What this step must deliver

- LCD shows title **Motion Alarm** and big status **ARMED**.
- A detail line pulses a counter (~2 Hz).
- Smooth updates **without** a busy-spin CPU loop.

**Success:** green ARMED banner; counter ticks.

---

## 2. Problems this step faces

- Need a clear **alarm face**, not raw numbers.
- Need paced redraw + yield (same delay lesson as Lab 00/01).

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| `alarm_*` helper | Owns layout; you own state text |
| Single loop + `vTaskDelay(500)` | Pace pulse; yield |
| No IMU / beep yet | Prove the face first |

### Architecture

```
┌──────────┐  vTaskDelay   ┌──────────┐  redraw   ┌─────────┐
│ app_main │ ────────────► │ alarm_*  │ ────────► │   LCD   │
│   loop   │ ◄──────────── │  helper  │           │ banner  │
└──────────┘  pulse++      └──────────┘           └─────────┘
```

### State machine

This step holds **ARMED** only; the detail line pulses on each loop.

```
┌──────────────┐
│    ARMED     │
│ pulse detail │
└──────────────┘
```

---

## 4. Build / flash
Once per PC (workshop root): `rbdev init`, then a **new** terminal.
From the lab folder use `rbdev run NN` (no `export.ps1` / `export.sh`).


```bash
cd labs/device-labs
# Lab: rbdev run 02  |  Step: ex02.c
rbdev run 02   # from labs/device-labs (step still in ex02.c)
# or -DBOARD=es3c28p
```

---

## 5. Student sketch (`main/main.c`)

```c
void app_main(void)
{
    unsigned pulse = 0;
    bsp_init();
    alarm_init();
    alarm_start();
    alarm_set_state(ALARM_ST_ARMED);
    alarm_set_live(1);
    for (;;) {
        pulse++;
        alarm_set_detail("watching... %u", pulse);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```

### How the code works

`alarm_start` lays out four rows (title, big state, detail, IMU hint).
The loop only bumps a counter and delays — no sensor, no sound.

---

## 6. FreeRTOS in depth — `vTaskDelay` (pace + yield)

Same idea as Lab 00/01: delay sets the pulse rate **and** blocks this task so
the scheduler can run idle (and later peers). A tight `for (;;)` with no delay
would burn CPU for no UI gain.

| Call | Meaning |
|------|---------|
| `pdMS_TO_TICKS(500)` | Convert 500 ms → ticks for this build |
| `vTaskDelay(...)` | Block at least that many ticks, then Ready again |

**Not yet:** tasks, queues. Step 02 adds IMU.

---

## 7. Checkpoint

- [ ] Can name what `alarm_set_state` vs `alarm_set_detail` own  
- [ ] Can say why the loop needs `vTaskDelay`  
- [ ] ARMED banner + ticking counter on your board  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Blank LCD | `bsp_init` + `alarm_start`; correct `-DBOARD=` |
| Counter stuck | Delay missing or crash before loop |

---

## 9. Next

[Step 02 — Shake detect](../step-02-shake-detect/).
