# ex10 — BOOT ISR (`…FromISR` notify)

<div class="ebook-run">

**Track:** `rtos-labs` · **Project:** `labs/rtos-labs`

```text
rbdev run 10
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/rtos-labs/ex10-boot-isr/README.md</code></p>

</div>


**Goal:** handle a button in an ISR the right way — **notify a task**, do
the work later. Contrast with **polling**.

**BOOT** = GPIO0 (active low) on `ws28p` / `es3c28p`. Press the physical
BOOT button (not the on-screen toggle).

| Mode | Path | What you see |
|------|------|--------------|
| **POLL** (boot) | Task reads `bsp_buttons()` | `press #` on each edge |
| **ISR** | GPIO ISR → `vTaskNotifyGiveFromISR` → task | Same count; ISR stays tiny |

`ui ticks` keeps climbing so you can tell the system is alive.

Builds on [ex09](../ex09-reload-timer/) (deferred work). Here the wake source
is hardware, not a software timer.

```
┌──────────────────────────────────────┐
│ ex10 POLL|ISR                        │
│ press #3  ****                       │
│ ui ticks 1200                        │
│ use BOOT btn                         │
├──────────────────────────────────────┤
│ [ POLL / ISR ]                       │
└──────────────────────────────────────┘
```

Enable in `main/main.c` with `ex10_start()` (only one exercise at a time).

| File | Holds |
|------|--------|
| `ex10.c` | Poll loop, GPIO ISR, notify, `ex10_start` |
| `ex10_ui.*` | LCD status, mode toggle |

After adding this folder **or** a new `.c`, reconfigure once:

```powershell
idf.py -DBOARD=ws28p reconfigure
# or
idf.py -DBOARD=es3c28p reconfigure
```

See [rtos-labs README](../README.md#build--flash).

---

## 1. Learning objectives

| Idea | In this exercise |
|------|------------------|
| Poll | Task samples the button each loop |
| GPIO ISR | Negedge on BOOT wakes software |
| FromISR notify | `vTaskNotifyGiveFromISR` + optional yield |
| Debounce in task | Not in the ISR |
| Keep ISR short | No LCD / long loops in `boot_isr` |

**Success:** both modes count BOOT presses; `ui ticks` never freezes.

---

## 2. What you should see

1. Boot **POLL**: press BOOT → `press #` + `****`; ticks keep moving.  
2. Tap on-screen **ISR**: same BOOT presses still count.  
3. (Experiment) busy-wait inside the ISR → ticks stall while held — bad.

---

## 3. Mental model

```
  POLL:   worker ──bsp_buttons edge──► on_press ──► LCD

  ISR:    BOOT GPIO ──► boot_isr ──NotifyGiveFromISR──► worker
              (short!)                                      │
                                                       on_press ──► LCD
```

```c
static void IRAM_ATTR boot_isr(void *arg)
{
    BaseType_t hp = pdFALSE;
    vTaskNotifyGiveFromISR(worker, &hp);
    if (hp == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}
```

---

## 4. How the code is wired

| Task / ISR | Prio | Pin | Role |
|------------|------|-----|------|
| `boot` | 5 | core 0 | Poll or `ulTaskNotifyTake` → count |
| `ui` | 4 | core 1 | Touch + status + tick counter |
| `boot_isr` | — | GPIO0 | Notify only when mode ISR |

| API | Role |
|-----|------|
| `gpio_isr_handler_add` | Attach BOOT ISR |
| `vTaskNotifyGiveFromISR` | Wake worker |
| `ulTaskNotifyTake` | Worker blocks for presses |
| `bsp_buttons` | POLL-mode edge detect (bit1 = BOOT) |

---

## 5. Tips and tricks

### Tip — never do real work in the ISR

LCD, `printf`, long math, or `vTaskDelay` in an ISR can deadlock or freeze
the chip. Notify (or queue-from-ISR) and return.

### Tip — debounce in the task

Mechanical bounce can fire the ISR many times. Filter with time in the
task after the notify (here 200 ms).

### Tip — `ui ticks` is your freeze detector

If ticks stop while you hold BOOT, the ISR (or something it triggers) is
blocking the system.

---

## 6. Checkpoint

- [ ] Can name `vTaskNotifyGiveFromISR` / `ulTaskNotifyTake`  
- [ ] Can explain why the ISR must stay short  
- [ ] Can contrast poll vs ISR paths  
- [ ] Knows BOOT is the **physical** button (GPIO0)  

---

## 7. Experiments

1. Add a long busy loop in `boot_isr` — watch `ui ticks` stall (then remove).  
2. Remove debounce — counts jump on one press.  
3. Leave mode POLL but keep the ISR attached with `s_use_isr==0` — ISR is a no-op.

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| No presses | Press **BOOT**, not PWR / not the LCD toggle |
| ISR mode silent | `gpio_isr_handler_add` ok? `s_use_isr` set? |
| Double counts | Debounce window; mode switch mid-press |
| Linker miss after add | `idf.py -DBOARD=… reconfigure` then build |

---

## 9. Next

[ex11 — IMU road racer](../ex11-imu-racer/) — capstone game: tilt steer, gfx,
queued SFX.
