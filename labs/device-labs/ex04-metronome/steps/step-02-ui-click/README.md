# Lab 04 / Step 02 — UI click

**Device lab:** [Metronome](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 01](../step-01-bpm-face/)

---

## 1. What this step must deliver

- Same BPM face.
- Each beat: flash + `beep_play(BEEP_CLICK)` then `vTaskDelay(period)`.
- Click is in the **same** loop as the UI.

**Success:** you hear a click roughly at 120 BPM (exact timing comes next).

---

## 2. Problems this step faces

- `beep_play` **blocks** for the click duration.
- Relative delay starts *after* that work → real period = work + delay.

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| Beep in UI loop | Simple; sets up the jitter lesson |
| `vTaskDelay(period)` after beep | Looks like “wait one period” — incomplete |

### Architecture

The same UI loop now plays audio; blocking work sits **before** the relative
delay.

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│ metro_flash │ ──► │  beep_play  │ ──► │ vTaskDelay  │
│             │     │  (blocks)   │     │  (period)   │
└─────────────┘     └─────────────┘     └─────────────┘
```

### Timing

Beat-to-beat spacing grows because the delay starts after the click finishes.

```
beat n                 beat n+1 (late)

  .--flash+beep---------.-- delay(period) --.
  .<----- target period ------------------->.
  .<-------- actual exceeds period -------->.
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
void app_main(void)
{
    unsigned period = period_ms(120);
    bsp_init();
    metro_init();
    metro_start();
    metro_set_bpm(120);

    for (;;) {
        metro_flash_beat();
        (void)beep_play(BEEP_CLICK, &(beep_params_t){
            .gain = 16u, .ms = 25u, /* quiet; still blocks — timing lesson */
        });
        metro_set_beat_idle();
        vTaskDelay(pdMS_TO_TICKS(period));
    }
}
```

### How the code works

**Beat body.**  
Flash, play a short click, clear the flash.

**`beep_play`.**  
Does not return until the click finishes. That time is **not** subtracted from
the following delay.

**`vTaskDelay(period)`.**  
Sleeps `period` ms *from now* — after the beep — so the gap between beat
*starts* is longer than `period`. Step 03 measures that slip.

---

## 6. FreeRTOS in depth — relative delay after work

| Call | Starts counting from |
|------|----------------------|
| `vTaskDelay(N)` | **Now** (after previous work) |

So:

```text
  loop period ≈ time(beep + draw) + N
```

not `N` alone. That is acceptable for a first click, wrong for a metronome.

---

## 7. Checkpoint

- [ ] Hear click + see flash  
- [ ] Can explain why period ≥ beep + delay  
- [ ] Ready to measure jitter  

---

## 8. Next

[Step 03 — Feel jitter](../step-03-feel-jitter/).
