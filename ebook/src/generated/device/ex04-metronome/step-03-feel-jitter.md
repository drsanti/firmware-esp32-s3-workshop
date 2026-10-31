# Lab 04 / Step 03 — Feel jitter

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

<span class="ebook-badge warn">broken on purpose</span>

```text
rbdev run 04
# step switch in ex04-metronome.c:  ex04_step03_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex04-metronome/steps/step-03-feel-jitter/README.md</code></p>

</div>


**Device lab:** [Metronome](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 02](../step-02-ui-click/)

---

## 1. What this step must deliver

- Same UI-loop click as step 02.
- Measure time between loop starts vs expected `period`.
- Show **last** and **max** absolute error (ms) on the LCD.

**Success:** jitter numbers are clearly above ~0 (often tens of ms from the click).

---

## 2. Problems this step faces

- Need proof that relative delay after work slips.
- Students should see numbers, not only “it feels off.”

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| Stamp `xTaskGetTickCount` at loop start | Actual period between beats |
| `\|actual − expected\|` | Simple jitter metric |
| Keep beep + `vTaskDelay` | Same broken pacing as step 02 |

### Architecture

Measure at **loop start**; then do work and a relative delay — the slip shows
up in the numbers.

```
┌─────────────┐
│ loop start  │── tick stamp ──► abs(err) ms ──► LCD jitter
└──────┬──────┘
       │
       ▼
┌─────────────┐     ┌─────────────┐
│ flash+beep  │ ──► │ vTaskDelay  │──► next loop start (late)
│   (work)    │     │  (relative) │
└─────────────┘     └─────────────┘
```

### Timing

Each beat adds beep time on top of the nominal period.

```
intended:  .---- period ----.---- period ----.
actual:    .work.-- period --.work.-- period --.
                 ^ error grows vs expected edge
```

---

## 4. Build

```bash
cd labs/device-labs
# Lab: rbdev run 04  |  Step: ex04.c
rbdev run 04   # from labs/device-labs (step still in ex04.c)
```

---

## 5. Student sketch (`main/main.c`)

```c
for (;;) {
    now = xTaskGetTickCount();
    if (!first) {
        actual_ms = (int)((now - prev) * portTICK_PERIOD_MS);
        err = actual_ms - (int)period;
        last_j = abs(err);
        if (last_j > max_j) max_j = last_j;
        metro_set_jitter(last_j, max_j);
    }
    first = 0;
    prev = now;

    metro_flash_beat();
    (void)beep_play(BEEP_CLICK, NULL);
    metro_set_beat_idle();
    vTaskDelay(pdMS_TO_TICKS(period));  /* relative — slips */
}
```

### How the code works

**Stamp at loop start.**  
`prev` / `now` mark when each beat *attempt* begins.

**Error.**  
`actual_ms` should equal `period` for a perfect metronome. With beep +
`vTaskDelay(period)`, actual is larger → `|err|` shows up as jitter.

**Display.**  
`metro_set_jitter(last, max)` keeps a running worst case so the slip is hard
to miss.

**The lesson.**  
FreeRTOS delay did what you asked. You asked for “sleep period *after* work,”
not “wake every period.” Step 04 switches to `vTaskDelayUntil`.

---

## 6. FreeRTOS in depth — measuring with ticks

| Piece | Role |
|-------|------|
| `xTaskGetTickCount()` | Timestamp |
| `(now - prev) * portTICK_PERIOD_MS` | Elapsed ms |
| Expected `period` | Ideal BPM spacing |

Resolution is one tick (often 1 or 10 ms). That is enough to see click-induced
slip.

---

## 7. Checkpoint

- [ ] Max jitter clearly > 0  
- [ ] Can point at beep + relative delay as the cause  
- [ ] Ready for `vTaskDelayUntil`  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Jitter always 0 | Measuring after delay instead of at loop start |
| Huge jitter | First iteration skipped via `first` flag? |

---

## 9. Next

[Step 04 — Tick task](../step-04-tick-task/).
