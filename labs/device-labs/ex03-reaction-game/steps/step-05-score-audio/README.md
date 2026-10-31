# Lab 03 / Step 05 — Score + audio

**Device lab:** [Reaction Game](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 04](../step-04-timer-go/)

---

## 1. What this step must deliver

- Same timer-based WAIT / GO as step 04.
- **False start** if BOOT during WAIT (stop timer; show message).
- Track **best** reaction time.
- Soft **click** on GO via notify → `audio_task` (not in UI / not in timer cb).

**Success:** early press → FALSE START; good round → ms + best; hear click on GO.

---

## 2. Problems this step faces

- Product rules on top of the timer path.
- Audio must not block UI (Lab 00/01 pattern).

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| Keep one-shot timer | WAIT stays live |
| BOOT in WAIT → `xTimerStop` | Cancel pending GO |
| `audio_task` + notify | GO cue without blocking UI |
| `g_best_ms` | Simple high score |

### Architecture

```
┌─────────────┐  NotifyGive   ┌─────────────┐  NotifyGive ┌─────────────┐
│  go timer   │ ─────────────►│   ui task   │────────────►│ audio task  │
│  (one-shot) │               │ score/rules │             │ BEEP_CLICK  │
└─────────────┘               └──────┬──────┘             └─────────────┘
                                     │
                                     │ react_*
                                     ▼
                              ┌─────────────┐
                              │     LCD     │
                              └─────────────┘
```

### Sequence

```
ui task           go timer           audio task
 |                    |                   |
 |-- Start timer ---->|                   |
 |-- poll BOOT        |                   |
 |                    |-- NotifyGive ---->|
 |-- show GO          |                   |
 |-- NotifyGive ------------------------->|
 |                    |                   |-- BEEP_CLICK
 |-- BOOT score       |                   |
```

### State machine

```
┌───────┐  BOOT  ┌───────┐  timer  ┌───────┐  BOOT  ┌────────┐
│ READY │ ─────► │  WAIT │ ──────► │  GO   │ ─────► │ RESULT │
└───────┘         └───────┘         └───────┘         └────────┘
```

False start (BOOT during WAIT):

```
┌────────┐
│  WAIT  │
└────┬───┘
     │ BOOT early
     ▼
┌────────────┐
│   RESULT   │
│ FALSE START│
└────────────┘
```

---

## 4. Build

```bash
cd labs/device-labs
# Lab: rbdev run 03  |  Step: ex03.c
rbdev run 03   # from labs/device-labs (step still in ex03.c)
```

---

## 5. Student sketch (`main/main.c`)

```c
static TaskHandle_t g_ui;
static TaskHandle_t g_audio;
static TimerHandle_t g_go_timer;
static uint32_t g_best_ms = 0xffffffffu;

static void go_timer_cb(TimerHandle_t t)
{
    g_go_tick = xTaskGetTickCount();
    (void)xTaskNotifyGive(g_ui);  /* SHORT — no beep here */
}

static void audio_task(void *arg)
{
    for (;;) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        (void)beep_play(BEEP_CLICK, NULL);
    }
}

/* inside ui WAIT loop: */
for (;;) {
    if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(20)) > 0u) {
        break; /* real GO */
    }
    if (boot_pressed(&boot_was)) {
        false_start = 1;
        (void)xTimerStop(g_go_timer, 0);
        break;
    }
    react_set_hint("live %u", ++pulse);
}

if (false_start) {
    react_set_state(REACT_ST_RESULT);
    react_set_detail("FALSE START");
    continue;
}

react_set_state(REACT_ST_GO);
(void)xTaskNotifyGive(g_audio);  /* wake audio — do not beep in UI */

/* after BOOT on GO: */
ms = (xTaskGetTickCount() - g_go_tick) * portTICK_PERIOD_MS;
if (ms < g_best_ms) {
    g_best_ms = ms;
}
```

```c
void app_main(void)
{
    bsp_init();
    g_go_timer = xTimerCreate("go", pdMS_TO_TICKS(1000), pdFALSE, NULL, go_timer_cb);
    (void)xTaskCreate(audio_task, "audio", 4096, NULL, 5, &g_audio);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, &g_ui);
}
```

### How the code works

**Timer callback.** Still only stamps `g_go_tick` and `NotifyGive`s the UI —
never plays sound.

**WAIT loop.** Each poll either gets the GO notify, sees an early BOOT (false
start → `xTimerStop`), or refreshes the live hint.

**GO.** UI notifies `audio_task`; audio alone calls `beep_play(BEEP_CLICK)`.
Then BOOT scores ms and maybe updates `g_best_ms`.

**`app_main`.** Create the one-shot timer, then audio first (so `g_audio` is
valid), then UI.

---

## 6. FreeRTOS in depth — timer + notify together

| Tool | Job |
|------|-----|
| Software timer | Schedule GO |
| Notify (timer→UI) | “GO time” event |
| Notify (UI→audio) | “Play click” event |

No new primitive beyond step 04’s timer — step 05 is **composition** and game
rules. Queue still unused on purpose.

---

## 7. Checkpoint

- [ ] False start works  
- [ ] Best time updates  
- [ ] Click on GO; UI stays live in WAIT  
- [ ] Can name Lab 03’s 3 FreeRTOS features  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| No sound | Audio task / `g_audio` / volume |
| GO after false start | Must `xTimerStop` on early press |
| PWR does nothing useful | Use **BOOT** |

---

## 9. Next

Lab 03 complete. Continue with [Lab 04 — Metronome](../../ex04-metronome/) or
return to [labs/README.md](../../README.md).
