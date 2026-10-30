# Lab 00 / Step 04 — IMU status

**Device lab:** [Hello Device](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 03](../step-03-two-tasks/) (notify) and [Step 01](../step-01-lcd-hello/) (`lcd_*`)

---

## 1. What this step must deliver

| Prior steps | Step 04 (new) |
|-------------|----------------|
| LCD / sound pad / notify | `bsp_imu_read` vs `imu_sim_fill` |
| — | Status **IMU live** (green) or **IMU sim** (magenta) |
| Notify + beep (step 03) | Boot / ay>5 `NotifyGive` → `beep_play(BEEP_CHIME)` |

**Success:** LCD shows title + live/sim status + updating `ax ay az`; beep at boot
and again when **ay** rises above `5` (tilt the board).

---

## 2. Problems this step faces

Not every board has a working IMU in the workshop kit — live and sim must
both count as success.

```text
  ui loop (~5 Hz)
       |
       v
  bsp_imu_read(acc, gyro) ----ok---->  "IMU live" + real numbers
       |
     fail
       |
       v
  imu_sim_fill(...)  -------------->  "IMU sim" + synthetic numbers
```

`ws28p` often shows **IMU live** (QMI8658).  
`es3c28p` may show **IMU sim** if no sensor path — that is still a successful lab.

### Architecture

UI owns the IMU loop and LCD; discrete cues use the same notify bridge as step 03.

```
┌─────────────┐  NotifyGive  ┌─────────────┐
│  ui_task    │ ───────────► │ audio_task  │
└─────────────┘              └─────────────┘
```

```
┌─────────────┐     fail      ┌─────────────┐
│bsp_imu_read │ ────────────► │ imu_sim_fill│
└─────────────┘               └─────────────┘
       │
       ▼
┌─────────────┐
│  lab_lcd    │
└─────────────┘
```

---

## 3. Activate ESP-IDF, then build

```powershell
. <IDF_PATH>\export.ps1
# example: . D:\dev\esp\v6.1\esp-idf\export.ps1
```

```bash
cd labs/device-labs
# Lab: .\run 00  |  Step: ex00.c

.\run 00   # from labs/device-labs (step still in ex00.c)
# or
idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx monitor
```

---

## 4. Runtime pieces

| Piece | Role |
|-------|------|
| `audio_task` | Blocks on `ulTaskNotifyTake`; then `beep_play(BEEP_CHIME, NULL)` |
| `ui_task` | LCD status lines; one boot `NotifyGive`; IMU loop |
| `bsp_imu_read` | Hardware 6-axis when present (returns 0 on ok) |
| `imu_sim_fill` | Synthetic accel/gyro when read fails |
| `app_main` | Create **audio first**, then ui |

```text
  boot / ay>5  -->  NotifyGive  -->  audio_task  -->  beep_play(CHIME)
  loop  -->  bsp_imu_read / imu_sim  -->  lcd lines
```

Display uses [`lab_lcd`](../../common/lab_lcd.h) (same as step 01).  
No sound pad here — keep the focus on IMU.

---

## 5. Student sketch (`main/main.c`)

```c
static TaskHandle_t g_audio;

static void audio_task(void *arg)
{
    (void)arg;
    for (;;) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        (void)beep_play(BEEP_CHIME, NULL);
    }
}

static void ui_task(void *arg)
{
    float acc[3], gyro[3], t = 0.0f;
    uint32_t rng = 1u;
    int real, was_real = -1, ay_hi = 0;

    (void)arg;
    (void)lcd_begin(3);
    lcd_printf(0, "IMU status");
    lcd_set_color(1, RB_LABEL_YELLOW);
    lcd_printf(1, "IMU ...");
    lcd_printf(2, "ax ay az");

    vTaskDelay(pdMS_TO_TICKS(50));
    if (g_audio != NULL) {
        (void)xTaskNotifyGive(g_audio);  /* boot ready cue */
    }

    for (;;) {
        real = (bsp_imu_read(acc, gyro) == 0);
        if (!real) {
            imu_sim_fill(acc, gyro, t, &rng);
            t += 0.2f;
        }
        if (real != was_real) {
            was_real = real;
            lcd_set_color(1, real ? RB_LABEL_GREEN : RB_LABEL_MAGENTA);
            lcd_printf(1, real ? "IMU live" : "IMU sim");
        }
        lcd_printf(2, "%.1f %.1f %.1f", (double)acc[0], (double)acc[1],
                   (double)acc[2]);

        /* ay = acc[1]: notify once each time it crosses above 5 */
        if (acc[1] > 5.0f) {
            if (!ay_hi) {
                ay_hi = 1;
                if (g_audio != NULL) {
                    (void)xTaskNotifyGive(g_audio);
                }
            }
        } else {
            ay_hi = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void app_main(void)
{
    bsp_init();
    (void)xTaskCreate(audio_task, "audio", 4096, NULL, 5, &g_audio);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
```

### How the code works

Read this once with the sketch open. The program is two FreeRTOS tasks plus a
shared handle. Nothing plays sound or reads the IMU from `app_main` itself.

**`app_main` — start order.**  
`bsp_init()` brings up clocks, display, and board drivers. Then two tasks are
created. **Audio is created first** so `&g_audio` is filled before `ui_task`
might call `xTaskNotifyGive`. If UI started first and notified immediately,
`g_audio` could still be `NULL` and the ready beep would be skipped.

**`g_audio` — the bridge.**  
This is a `TaskHandle_t`: an ID FreeRTOS uses to wake a specific task. The UI
side only *gives* a notify to that handle; the audio side only *takes* it.
Neither task calls the other’s functions directly.

**`audio_task` — wait, then beep.**  
The infinite loop blocks on `ulTaskNotifyTake(pdTRUE, portMAX_DELAY)`. That
means “sleep until someone notifies me.” When the boot notify arrives, the
task wakes, calls `beep_play(BEEP_CHIME, NULL)` (C-E-G rising triad), then goes back to waiting. `pdTRUE` clears
the notify count so one Give produces one beep (same idea as step 03).

**`ui_task` — screen, one notify, then the sensor loop.**

1. **Status setup.** `lcd_begin(3)` opens three text rows. Row 0 is the title.
   Row 1 starts yellow with `"IMU ..."`. Row 2 is a placeholder for numbers.
2. **Ready cue.** After a short delay (so the panel and audio path settle), UI
   sends `xTaskNotifyGive(g_audio)`. That is *not* the IMU lesson — it only
   proves the notify path still works and gives an audible “app started.”
3. **Sample loop (~5 Hz).** Each pass tries `bsp_imu_read(acc, gyro)`.
   - Return `0` means hardware answered: set `real = 1` and keep those values.
   - Any failure means fall back to `imu_sim_fill(...)`, which writes synthetic
     motion using time `t` and a small RNG. Advance `t` so the sim keeps moving.
4. **Status line only when mode changes.** `was_real` remembers the previous
   mode. Updating color/text only when `real != was_real` avoids rewriting the
   same `"IMU live"` / `"IMU sim"` string every 200 ms.
5. **Always refresh numbers.** Row 2 prints `ax ay az` every cycle so you see
   motion whether the source is live or sim.
6. **ay threshold beep.** `acc[1]` is Y. When it rises above `5`, UI notifies
   audio once (`ay_hi` edge flag). When ay falls back to ≤5, the flag clears
   so the next rise can beep again. Without the edge flag, a held tilt would
   notify every 200 ms.
7. **`vTaskDelay(200 ms)`** yields so audio (and the rest of FreeRTOS) can run.

| Piece | Job in one sentence |
|-------|---------------------|
| `app_main` | Init board; start audio, then UI |
| `g_audio` | Handle used to wake `audio_task` |
| `audio_task` | Block on notify; play one ready beep |
| `ui_task` setup | Draw three LCD lines; notify audio once |
| `bsp_imu_read` | Prefer real sensor data |
| `imu_sim_fill` | Fill the same arrays when hardware is missing |
| `was_real` | Detect live↔sim transitions for the status color |

Hold this model in mind: *UI owns the screen and the sensor decision; audio
owns the speaker and only wakes when told.* That separation is why a slow beep
cannot freeze the IMU numbers, and why a missing IMU cannot block the ready
tone forever.

---

## 6. IMU API (what to remember)

| Call | Meaning |
|------|---------|
| `bsp_imu_read(acc, gyro)` | Fill `float[3]` accel (m/s^2) and gyro (rad/s); `0` = ok |
| `imu_sim_fill(acc, gyro, t, &rng)` | Demo motion when hardware is missing |
| `lcd_set_color` / `lcd_printf` | Status line color + text |

Sim helper: [`ports/imu_sim.h`](../../../ports/imu_sim.h) (soft sin, no `libm`).

---

## FreeRTOS in depth — notify reuse + sample pacing

### Same notify bridge as step 03

Boot ready and **ay** threshold both use `xTaskNotifyGive` → `audio_task` →
`beep_play(BEEP_CHIME)`. Why reuse notify?

| Need | Tool |
|------|------|
| Discrete cues (boot, cross threshold) | Notify — “event happened” |
| Continuous ax/ay/az for the LCD | Stay in `ui_task` — no queue yet |

IMU numbers are sampled and printed in UI. Sound is still an **event**. Lab 01
will introduce a **queue** when tilt must move as a stream between tasks
without freezing the UI.

### Why `vTaskDelay(200 ms)` (~5 Hz)?

| Choice | Effect |
|--------|--------|
| No delay | Busy spin; LCD thrash; audio may starve |
| 200 ms | Readable numbers; UI yields every loop |
| Much faster (e.g. 10 ms) | Heavier CPU; little teaching gain here |

`pdMS_TO_TICKS(200)` converts ms → ticks. This is a **relative** delay again:
fine for a status panel. A hard 50 Hz control loop would prefer
`vTaskDelayUntil` (see Lab 01 step 01 write-up).

### Why an edge flag for ay (`ay_hi`)?

Without it, every 200 ms while ay stays high would NotifyGive again — a beep
machine. Edge detect = one event per crossing, matching notify’s “event”
semantics.

### Why not play the chime inside the IMU loop?

Same reason as step 03: `beep_play` blocks. During a chime the LCD would stop
updating ax/ay/az. Notify keeps sensing/UI responsive.

### Mental model

> UI samples and decides **when** to cue audio. Audio only wakes on notify.
> Delay paces the sensor panel. Queues wait until a later lab needs a data
> stream between tasks.

---

## 7. Exercises

1. Confirm the middle line is **IMU live** or **IMU sim** (and the color matches).  
2. Watch `ax ay az` update (~5 Hz). Tilt the board if live — numbers should change.  
3. Tilt until **ay** goes above `5` — you should hear one beep per crossing.  
4. Comment out the boot notify — LCD still works; no ready beep at start.  
5. Temporarily call `beep_play` in the IMU loop — feel the numbers freeze (then undo).  

---

## 8. Checkpoint

- [ ] Can explain when sim is used vs live  
- [ ] Status color: green = live, magenta = sim  
- [ ] Can explain why notify + edge flag fit ay beeps  
- [ ] Can explain why 200 ms delay is a design choice  
- [ ] Boot beep comes from `audio_task`, not from the IMU loop  
- [ ] Works on your board (`ws28p` or `es3c28p`)  

---

## 9. Troubleshooting

| Symptom | Try |
|---------|-----|
| `idf.py` not found | Source `<IDF_PATH>\export.ps1` / `export.sh` |
| Always **IMU sim** on `ws28p` | Wrong `-DBOARD=`; `fullclean` and rebuild |
| Blank LCD | `bsp_init` / `lcd_begin`; match board SKU |
| No boot beep | Audio path / volume; rebuild with current `lab_beep` |

---

## 10. Next

Continue to [Step 05 — BOOT → LED chase](../step-05-boot-leds/).
