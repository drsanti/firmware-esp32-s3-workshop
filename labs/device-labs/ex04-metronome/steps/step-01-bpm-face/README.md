# Lab 04 / Step 01 — BPM face

**Device lab:** [Metronome](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. What this step must deliver

- LCD shows **Metronome** and **120 BPM**.
- Beat row flashes on each period, then returns to idle.
- No speaker yet — visual only.

**Success:** steady visual pulse at ~120 BPM.

---

## 2. Problems this step faces

- Need a clear metronome face before audio/timing lessons.
- Need paced redraw + yield (`vTaskDelay`).

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| `metro_*` helper | Owns layout |
| Period = `60000 / bpm` ms | Standard BPM → ms |
| Flash then delay remainder | Simple visual beat |

### Architecture

One task owns the metronome face; each period is flash, short hold, idle, then
sleep the remainder.

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│ metro_flash │ ──► │ vTaskDelay  │ ──► │ metro idle  │
│    _beat    │     │  (flash ms) │     │ + delay rest│
└─────────────┘     └─────────────┘     └─────────────┘
```

Repeat that chain each BPM period.

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
#define BPM 120u
#define FLASH_MS 80u

static unsigned period_ms(unsigned bpm)
{
    if (bpm < 40u) bpm = 40u;
    return 60000u / bpm;   /* 120 BPM → 500 ms */
}

void app_main(void)
{
    unsigned period = period_ms(BPM);
    bsp_init();
    metro_init();
    metro_start();
    metro_set_bpm(BPM);

    for (;;) {
        metro_flash_beat();
        vTaskDelay(pdMS_TO_TICKS(FLASH_MS));
        metro_set_beat_idle();
        vTaskDelay(pdMS_TO_TICKS(period - FLASH_MS));
    }
}
```

### How the code works

**`period_ms`.**  
Converts BPM to milliseconds between beats (`60000 / bpm`). Clamp avoids
divide-by-zero / insane tempos.

**`metro_start` / `metro_set_bpm`.**  
Draw the face once; show the tempo on the big line.

**Loop.**  
Flash the beat row, hold briefly, clear it, then sleep the rest of the period.
`vTaskDelay` paces and yields — no audio yet.

---

## 6. FreeRTOS in depth — `vTaskDelay` for a period

Here delay is used as a crude period generator. That is fine for a visual demo.
Later steps put **work before** a relative delay and the real period grows —
that is the jitter lesson.

---

## 7. Checkpoint

- [ ] Can compute ms from BPM  
- [ ] Beat flash looks steady  
- [ ] Can say what `vTaskDelay` is doing  

---

## 8. Next

[Step 02 — UI click](../step-02-ui-click/).
