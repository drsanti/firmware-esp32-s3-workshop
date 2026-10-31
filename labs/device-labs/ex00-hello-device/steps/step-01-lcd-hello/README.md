# Lab 00 / Step 01 — LCD hello

**Device lab:** [Hello Device](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** none (first step)

---

## 1. What this step must deliver

- Build and flash an ESP-IDF app from a lab `step-*` folder.
- Initialize the board (`bsp_init`) and draw text with `lcd_*`.
- Update a line from a FreeRTOS loop (`vTaskDelay`).

**Success:** LCD shows a title, a subtitle, and a yellow counter that increments
about once per second.

---

## 2. Problems this step faces

- Need a proven display path before sound or sensors.
- A tight empty loop would burn CPU and fight the scheduler.
- Prefer `rbdev run NN` — it loads ESP-IDF for you (no `export` activate).

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| Single loop in `app_main` | One job: bring-up + paced LCD |
| `vTaskDelay(1000 ms)` | ~1 Hz counter **and** yield |
| `lcd_*` helper | Hide `rb_screen` tree wiring |

**Not yet:** second task, notify, speaker, IMU.

### LCD helper (`labs/common/lab_lcd.h`)

| Call | Meaning |
|------|---------|
| `lcd_begin(n)` | Open LCD; create **n** text rows (max 4) |
| `lcd_printf(row, fmt, …)` | Print to that row |
| `lcd_set_color` / `lcd_set_scale` | Style a row |

Rows are 0-based. ASCII only. Scale is integer `1`–`3`.

### Architecture

```
┌──────────┐     ┌──────────┐     ┌──────────┐
│ bsp_init │ ──► │ app_main │ ──► │ lab_lcd  │ ──► LCD
└──────────┘     │loop+delay│     └──────────┘
                 └──────────┘
```

---

## 4. Build / flash
Once per PC (workshop root): `rbdev init`, then a **new** terminal.
From the lab folder use `rbdev run NN` (no `export.ps1` / `export.sh`).

```bash
cd labs/device-labs
# Lab: rbdev run 00  |  Step: ex00.c
rbdev run 00   # from labs/device-labs (step still in ex00.c)
# or
idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx monitor
```

Exit monitor with `Ctrl+]`.

**Observe:**

```text
Hello Device          (large, cyan)
step 01 - LCD         (smaller, gray)
0                     (yellow; then 1, 2, 3, …)
```

---

## 5. Student sketch (`main/main.c`)

```c
void app_main(void)
{
    int cnt = 0;

    bsp_init();
    (void)lcd_begin(3);
    lcd_printf(0, "Hello Device");
    lcd_printf(1, "step 01 - LCD");
    lcd_set_color(2, RB_LABEL_YELLOW);
    lcd_set_scale(2, 2);

    for (;;) {
        lcd_printf(2, "%d", cnt++);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

### How the code works

**`bsp_init`.**  
Once, first — clocks, buses, panel for `-DBOARD=`.

**`lcd_begin(3)`.**  
Three rows `0..2`. Title/subtitle written once; row 2 is the live counter.

**Forever loop.**  
Print, increment, `vTaskDelay` ~1 s. One task only (`app_main`).

---

## 6. FreeRTOS in depth — `vTaskDelay`

### Why a delay

| Without delay | With `vTaskDelay(pdMS_TO_TICKS(1000))` |
|---------------|----------------------------------------|
| CPU ~100% busy | Task blocked most of the time |
| Unstable update | ~1 Hz, readable |
| Other work starves | Scheduler can run peers / idle |

### Ticks

Prefer `pdMS_TO_TICKS(ms)` over raw tick counts.

### `vTaskDelay` vs `vTaskDelayUntil`

| API | Meaning | This step |
|-----|---------|-----------|
| `vTaskDelay` | Sleep N ticks **from now** | Used |
| `vTaskDelayUntil` | Absolute period | Later (Lab 04) |

Mental model: **pace + yield**, not busy-spin.

---

## 7. Checkpoint

- [ ] `rbdev doctor` is green (or `rbdev run` flashes this lab)  
- [ ] Title, subtitle, counter visible  
- [ ] Counter ~1 Hz  
- [ ] Can name `bsp_init`, `lcd_begin`, `vTaskDelay`  

Optional: rename subtitle (ASCII); try `2000` ms; try `RB_LABEL_GREEN`.

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| `idf.py` / flash fails | Re-run `rbdev init`; use `rbdev run NN` (not bare `run`) |
| Missing `librobotos_fw.a` | Workshop `lib/README.md` |
| Blank LCD | Correct `-DBOARD=`; keep `sdkconfig.defaults` |
| Counter stuck | Valid row `< n` from `lcd_begin(n)` |
| Weird glyphs | ASCII only on LCD |

---

## 9. Next

[Step 02 — Sound pad](../step-02-sound-pad/).
