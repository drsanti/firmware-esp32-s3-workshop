# Lab 00 / Step 05 — BOOT → LED chase

**Device lab:** [Hello Device](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 03](../step-03-two-tasks/) (notify) and LCD helpers

---

## 1. What this step must deliver

| Prior steps | Step 05 (new) |
|-------------|----------------|
| Notify + beep (steps 03–04) | Physical **BOOT** button via `bsp_buttons` |
| LCD text / sound pad | Four LCD LEDs — **one lit** (chase) |
| — | Press counter + notify chime |

**Success:** pressing **BOOT** advances the lit LED (`0→1→2→3→0`), bumps
`presses: N`, and plays a soft chime.

---

## 2. Problems this step faces

The two workshop boards do **not** share the same buttons — need one control
that works on both:

| Board | Physical buttons | This lab uses |
|-------|------------------|---------------|
| `ws28p` | PWR (bit0) + BOOT (bit1) | **BOOT only** |
| `es3c28p` | BOOT (bit1) only | **BOOT** |

`bsp_buttons()` returns a bitmask of currently pressed buttons. Bit 1 is BOOT
on both SKUs, so one `main.c` works with either `-DBOARD=`. Ignore PWR on
`ws28p` (it is tied to power-hold / battery).

```text
  BOOT GPIO  -->  bsp_buttons() bit1 (mask 2)
                      |
                      v
                 ui_task edge detect
                      |
          +-----------+-----------+
          |                       |
          v                       v
   chase LED + count        NotifyGive --> audio_task --> beep_play(CHIME)
```

### Architecture

Physical BOOT is polled in UI; LED chase and press count stay in the same task.

```
┌─────────────┐  NotifyGive  ┌─────────────┐
│  ui_task    │ ───────────► │ audio_task  │
└─────────────┘              └─────────────┘

┌─────────────┐     ┌─────────────┐
│bsp_buttons  │     │ lab_led_row │
└─────────────┘     └─────────────┘
```

### Sequence

One BOOT press: UI updates LEDs, then wakes audio without playing in the poll loop.

```
ui_task          audio_task
    |                 |
    |-- edge BOOT     |
    |-- led chase     |
    |-- NotifyGive -->|
    |                 |-- Take wakes
    |                 |-- beep_play
    |                 |
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
| `ui_task` | Poll BOOT; on rising edge: chase + count + `NotifyGive` |
| `led_row_*` | Four LCD LEDs + `presses: N` (helper owns geometry) |
| `bsp_buttons` | Raw pressed bitmask (`BOOT_BTN_MASK = 2`) |
| `app_main` | Create **audio first**, then ui |

```text
  BOOT edge  -->  NotifyGive  -->  audio_task  -->  beep_play(CHIME)
  BOOT edge  -->  led_row_set_active / set_presses
```

Display uses [`lab_led_row`](../../common/lab_led_row.h). FreeRTOS here is the
same notify pattern as step 03 — no new queues or semaphores.

---

## 5. Student sketch (`main/main.c`)

```c
#define BOOT_BTN_MASK 2u   /* bit1 = BOOT on both boards */
#define LED_COUNT 4u

static TaskHandle_t g_audio;

static void notify_beep(void)
{
    if (g_audio != NULL) {
        (void)xTaskNotifyGive(g_audio);
    }
}

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
    uint32_t was = 0;
    unsigned active = 0;
    unsigned presses = 0;

    (void)arg;
    led_row_init();
    led_row_start();

    for (;;) {
        uint32_t now = bsp_buttons() & BOOT_BTN_MASK;
        if (now != 0u && was == 0u) {
            presses++;
            active = (active + 1u) % LED_COUNT;
            led_row_set_active(active);
            led_row_set_presses(presses);
            notify_beep();
        }
        was = now;
        vTaskDelay(pdMS_TO_TICKS(20));
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

**`app_main` — start order.**  
Same as steps 03–04: `bsp_init`, then create `audio_task` first so `g_audio` is
valid before UI can notify.

**`audio_task` — wait, then chime.**  
Blocks on `ulTaskNotifyTake`. One Give → one `beep_play(BEEP_CHIME)`. Audio never polls
buttons or touches the LCD.

**`ui_task` — poll, edge, update.**

1. `led_row_init` / `led_row_start` draw title, four LEDs (LED 0 lit), and
   `presses: 0`.
2. Every 20 ms, read `bsp_buttons() & 2`. That is BOOT only.
3. **Rising edge** (`now` pressed, `was` released): bump `presses`, advance
   `active` with `% 4`, update the helper, notify audio.
4. Store `was = now` so a held BOOT does not keep counting.
5. `vTaskDelay(20 ms)` yields and doubles as a light debounce.

| Piece | Job in one sentence |
|-------|---------------------|
| `BOOT_BTN_MASK` | Select bit1 so both boards share one control |
| `was` / `now` | Edge detect — count once per press |
| `led_row_set_active` | Keep exactly one LED lit |
| `notify_beep` | Wake audio without playing sound in UI |

Hold this model: *UI owns button + LEDs; audio owns the speaker and only wakes
when told.*

---

## 6. FreeRTOS in depth — poll period + notify (again)

### Same split as steps 03–04

| Task | May block on | Must not do |
|------|--------------|-------------|
| `ui_task` | short `vTaskDelay` only | `beep_play` (long) |
| `audio_task` | `ulTaskNotifyTake` forever | poll BOOT / touch LCD |

BOOT press is an **event** → `xTaskNotifyGive`. Chime stays in audio.

### Why poll with `vTaskDelay(20)` instead of a GPIO ISR?

| Approach | Pros | Cons / teaching |
|----------|------|-----------------|
| Poll every 20 ms | Simple; works on both boards via `bsp_buttons` | Not microsecond latency |
| GPIO ISR → notify | Faster edge | More setup; ISR rules; easy to get wrong in Lab 00 |

For a human finger, 20 ms is plenty. Lab 00 stays in task context. ISRs appear
when a later device truly needs them.

### Why 20 ms also helps debounce

Mechanical buttons bounce. Sampling at ~50 Hz plus **rising-edge** logic
(`now && !was`) ignores a held BOOT and most bounce bursts without a separate
debounce state machine. Faster polling without edge detect would multi-count.

### `pdMS_TO_TICKS(20)` and relative delay

Same story as step 01 / Lab 01: convert ms → ticks; sleep **from now**. Work
per loop is tiny (read mask, maybe update LEDs), so relative delay ≈ period.

### Why not a queue of “press events”?

A queue could carry press counts or timestamps. Here one bit of information
matters: “please chime once.” Notify is enough. Lab 01 uses a **queue** when
the payload is continuous tilt data, not a one-shot cue.

### Mental model

> Poll for hardware state in UI; **edge** → event; **notify** → audio.
> Delay sets poll rate and debounce. No new primitives — deepen the ones you
> already know.

Not introduced here: queues, semaphores, mutexes, GPIO ISRs.

---

## 7. Exercises

1. Press BOOT four times — lit LED should walk `0→1→2→3→0`.  
2. Confirm `presses: N` matches how many times you pressed.  
3. Hold BOOT — count should not race (edge detect).  
4. On `ws28p`, try PWR: this lab should ignore it (mask is BOOT only).  
5. Put `beep_play` in the poll loop temporarily — LEDs freeze during chime (undo).  

---

## 8. Checkpoint

- [ ] Can name which physical button this lab uses on each board  
- [ ] Can explain poll + edge vs ISR for this lab  
- [ ] Can explain why notify (not queue) for the chime  
- [ ] Exactly one LCD LED is lit at a time  
- [ ] Beep comes from `audio_task` via notify, not from the poll loop body  
- [ ] Works on your board (`ws28p` or `es3c28p`)  

---

## 9. Troubleshooting

| Symptom | Try |
|---------|-----|
| `idf.py` not found | Source `<IDF_PATH>\export.ps1` / `export.sh` |
| No response to button | Confirm you press **BOOT**, not PWR; rebuild with matching `-DBOARD=` |
| Blank LCD | `bsp_init` / `led_row_start`; match board SKU |
| No beep | Audio path / volume; rebuild with current `lab_beep` |
| Multiple counts per press | Poll is 20 ms; release fully between presses |

---

## 10. Next

Lab 00 is done — return to [`../README.md`](../README.md).  
Next: [Lab 01 — Spirit Level](../../ex01-spirit-level/) (start at step 01).
