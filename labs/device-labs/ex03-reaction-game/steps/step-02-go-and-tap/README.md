# Lab 03 / Step 02 — GO and tap

**Device lab:** [Reaction Game](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 01](../step-01-game-face/)

---

## 1. What this step must deliver

- **BOOT** starts a round (bit1; ignore PWR on ws28p).
- Random WAIT (1–3 s) via `vTaskDelay`.
- Then **GO** — next BOOT measures reaction ms with tick timestamps.
- Show RESULT, then next round.

**Success:** plausible times (tens–hundreds of ms) after GO.

---

## 2. Problems this step faces

- Need BOOT edge detection (same as Lab 00/02).
- Need to capture time at GO and at press.
- `vTaskDelay` for WAIT **blocks this task** — works, but UI cannot do other
  work during the wait (timer comes in step 04).

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| `vTaskDelay(wait_ms)` | Simple fair wait for a first working game |
| `xTaskGetTickCount` | Measure interval in ticks → ms |
| Rising edge on BOOT | One start / one react per press |

### Architecture

```
┌─────────┐  edge    ┌──────────┐  delay   ┌──────────┐  edge   ┌─────────┐
│  BOOT   │ ───────► │   ui     │ ───────► │   WAIT   │ ──────► │   GO    │
│ button  │          │  loop    │          │  task    │         │  score  │
└─────────┘          └──────────┘          └──────────┘         └─────────┘
```

### Sequence

```
player            ui task
 |                   |
 |-- BOOT start ---->|
 |                   |-- vTaskDelay (random WAIT)
 |                   |-- show GO
 |-- BOOT react ---->|
 |<- RESULT ms ------|
```

---

## 4. Build

```bash
cd labs/device-labs
# Lab: .\run 03  |  Step: ex03.c
.\run 03   # from labs/device-labs (step still in ex03.c)
```

---

## 5. Student sketch (`main/main.c`)

```c
#define BOOT_BTN_MASK 2u  /* bit1 = BOOT on both boards */

static int boot_pressed(uint32_t *was)
{
    uint32_t now = bsp_buttons() & BOOT_BTN_MASK;
    int edge = (now != 0u && *was == 0u) ? 1 : 0;
    *was = now;
    return edge;
}

/* one round inside app_main's for(;;) */
react_set_state(REACT_ST_READY);
react_set_detail("BOOT to start");
while (!boot_pressed(&boot_was)) {
    vTaskDelay(pdMS_TO_TICKS(20));
}

wait_ms = 1000u + (rnd_u32() % 2000u);
react_set_state(REACT_ST_WAIT);
react_set_detail("wait %u ms", (unsigned)wait_ms);
vTaskDelay(pdMS_TO_TICKS(wait_ms));   /* blocks this task for the whole wait */

go_tick = xTaskGetTickCount();
react_set_state(REACT_ST_GO);
react_set_detail("BOOT now!");
while (!boot_pressed(&boot_was)) {
    vTaskDelay(pdMS_TO_TICKS(20));
}

ms = (uint32_t)((xTaskGetTickCount() - go_tick) * portTICK_PERIOD_MS);
react_set_state(REACT_ST_RESULT);
react_set_detail("%u ms", (unsigned)ms);
```

### How the code works

**`boot_pressed`.**  
Reads `bsp_buttons` bit1 and returns 1 only on a **rising edge** (was up, now
down). Same pattern as Lab 00 BOOT LEDs / Lab 02 arm toggle.

**READY.**  
Poll BOOT with a short `vTaskDelay` so the UI yields while waiting to start.

**WAIT.**  
Pick a random 1–3 s delay and call `vTaskDelay(wait_ms)`. This task sleeps for
the whole wait — polite to the CPU, but the banner cannot show a live pulse
(contrast step 04’s timer).

**GO + score.**  
Stamp `go_tick` when GO appears. On the next BOOT edge, convert
`(now - go_tick) * portTICK_PERIOD_MS` to milliseconds and show RESULT.

---

## 6. FreeRTOS in depth — ticks for measurement

| Piece | Meaning |
|-------|---------|
| `xTaskGetTickCount()` | Current tick |
| `portTICK_PERIOD_MS` | ms per tick (often 1 or 10) |
| Difference × period | Elapsed ms (coarse but good enough here) |

`vTaskDelay` for WAIT is a **blocking** wait: this task does nothing else until
it ends. Step 03 shows a worse busy-spin; step 04 uses a timer so WAIT can
still update the UI.

---

## 7. Checkpoint

- [ ] BOOT starts / reacts (not PWR)  
- [ ] Can explain `boot_pressed` rising edge  
- [ ] Can explain go_tick → ms  
- [ ] Knows WAIT delay blocks this task  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| No start | Press **BOOT**, not PWR on ws28p |
| Always ~0 ms | Pressed before GO / edge not cleared |
| Crazy large ms | Tick math / forgot to stamp `go_tick` at GO |

---

## 9. Next

[Step 03 — Busy wait](../step-03-busy-wait/) (**broken**).
