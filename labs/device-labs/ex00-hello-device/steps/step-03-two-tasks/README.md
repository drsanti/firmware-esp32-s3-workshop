# Lab 00 / Step 03 — Two tasks + notify

**Device lab:** [Hello Device](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 02](../step-02-sound-pad/) (buttons + callbacks)

---

## 1. What this step must deliver

| Step 02 | Step 03 (new) |
|---------|----------------|
| Callback **plays** `beep_play` | Callback **only** `xTaskNotifyGive` |
| UI path does the audio | Dedicated `audio_task` plays the tone |
| — | `ulTaskNotifyTake` / `xTaskNotifyGive` |

**Success:** tap **Beep** → you hear a tone from `audio_task`, not from the click callback.

---

## 2. Problems this step faces

Playing audio inside the touch/click path **blocks** that path for the whole tone.  
Notify wakes another task that is allowed to block on `bsp_audio_write`.

```text
  tap  -->  on_beep_click  --NotifyGive-->  audio_task  -->  beep_play
                 |                              |
            (returns fast)              (waits + plays)
```

## 3. Design approach

| Choice | Why |
|--------|-----|
| `audio_task` + notify | Click returns fast; audio may block |
| `xTaskNotifyGive` in callback | Event bridge — no payload needed |
| Create audio before UI | `g_audio` ready before first tap |

### Architecture

Two tasks share one notify bridge; the pad click path never calls `beep_*`.

```
┌─────────────┐  NotifyGive  ┌─────────────┐
│  ui_task    │ ───────────► │ audio_task  │
│ sound_pad   │              │ beep_play   │
└──────┬──────┘              └─────────────┘
       │
       ▼
┌─────────────┐
│ on_beep_    │
│ click       │
└─────────────┘
```

### Sequence

One Beep tap: UI returns immediately; audio blocks on the speaker.

```
pad_touch      on_beep_click      audio_task
      |                |                 |
      |-- CLICK ------>|                 |
      |                |-- NotifyGive -->|
      |<- returns -----|                 |-- Take wakes
      |                |                 |-- beep_play
      |                |                 |
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
| `audio_task` | Blocks on `ulTaskNotifyTake`; then `beep_play(BEEP_TONE, …)` |
| `ui_task` | init / knobs / **Beep** button / `start` |
| `on_beep_click` | `xTaskNotifyGive(g_audio)` only |
| `on_gain` | store percent into `g_gain` |
| `app_main` | Create **audio first**, then ui (so `g_audio` is ready) |

UI scaffold: [`lab_sound_pad`](../../common/lab_sound_pad.h) — **Beep** + knobs labeled **G** / **E** / **R**
(Gain / Echo / Release).  
`on_gain` updates `g_gain` (audio applies it); E/R use `NULL` → `beep_set_echo` / `beep_set_release`.  
Tones use library `rb_sfx` under `lab_beep` (same voice as the firmware SFX engine).

---

## 5. Student sketch (`main/main.c`)

```c
static TaskHandle_t g_audio;
static uint8_t g_gain = 40;

static void on_beep_click(void *user)
{
    (void)user;
    if (g_audio != NULL) {
        (void)xTaskNotifyGive(g_audio);  /* wake audio — do not beep here */
    }
}

static void on_gain(uint8_t percent, void *user)
{
    (void)user;
    g_gain = percent;  /* store only — audio applies later */
}

static void audio_task(void *arg)
{
    (void)arg;
    for (;;) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        (void)beep_play(BEEP_TONE, &(beep_params_t){
                                       .hz = 880u,
                                       .ms = 150u,
                                       .gain = g_gain,
                                   });
    }
}

static void ui_task(void *arg)
{
    (void)arg;
    sound_pad_init();
    (void)sound_pad_add_gain_slider(on_gain, NULL);
    (void)sound_pad_add_echo_slider(NULL, NULL);
    (void)sound_pad_add_release_slider(NULL, NULL);
    (void)sound_pad_add_button("Beep", RB_STYLE_SUCCESS, on_beep_click, NULL);
    sound_pad_start();
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
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

Read this once with the sketch open. Step 02 played sound *inside* the click
callback. Here the click path only wakes another task — that is the whole
lesson.

**Two tasks, one handle.**  
`app_main` creates `audio_task` first (saving its handle in `g_audio`), then
`ui_task`. Create order matters: a tap must never see a null `g_audio`.

**`on_beep_click` — notify only.**  
On finger lift, the pad calls this function. It must **not** call `beep_*`.
It only `xTaskNotifyGive(g_audio)`, which increments the audio task’s notify
count and returns quickly so touch stays responsive.

**`on_gain` — store, don’t play.**  
Dragging **G** calls `on_gain` with 0..100. You write `g_gain` and return.
Echo (**E**) and Release (**R**) use `NULL` callbacks, so the helper calls
`beep_set_echo` / `beep_set_release` directly. Gain is stored in a global so
*audio* applies it at play time — same decoupling idea as notify.

**`audio_task` — take, then tone.**  
The loop blocks on `ulTaskNotifyTake(pdTRUE, portMAX_DELAY)` (“sleep until
notified”). When a Give arrives, it plays
`beep_play(BEEP_TONE, …)` with `g_gain`, then waits again. `pdTRUE` clears the count so each
tap produces one beep.

**`ui_task` — pad only.**  
Init the sound pad, add knobs and the Beep button, call `sound_pad_start`,
then idle. Touch and slider work continue in the helper’s touch task; UI’s
own loop just keeps the task alive.

| Piece | Job in one sentence |
|-------|---------------------|
| `g_audio` | Handle used to wake `audio_task` |
| `g_gain` | Loudness written by UI, read by audio |
| `on_beep_click` | Notify only — never `beep_*` |
| `on_gain` | Store percent into `g_gain` |
| `audio_task` | Wait on notify; set gain; play tone |
| `ui_task` | Build Beep + G/E/R pad; idle |

Hold this model in mind: *UI signals; audio performs.* If you put `beep_play`
back in the click callback, you feel the old coupling again — then undo it.

---

## 6. FreeRTOS in depth — tasks + notify

### Why two tasks?

| Problem (step 02) | Design (this step) |
|-------------------|--------------------|
| `beep_play` in the click path blocks touch | Click only signals; `audio_task` may block on speaker I/O |
| UI and audio share one call stack | Separate stacks / schedules |

`xTaskCreate` starts a new concurrent loop. Priority `5` matches UI here —
scheduling fairness is enough; we are not tuning real-time ranks yet.

Create **audio first** and keep `g_audio`: Give needs a valid `TaskHandle_t`.
If UI ran first and somehow notified before create finished, Give would be
skipped (`g_audio == NULL`).

### Why notify (not a queue) for Beep?

| Tool | Carries | Fits |
|------|---------|------|
| **Task notify (Give/Take)** | “something happened” (count) | One-shot events: tap Beep |
| **Queue** | Typed payloads | Streams / structured messages (Lab 01+) |
| Call `beep_play` in UI | — | Rejected — blocks the wrong task |

Beep is an **event**, not a stream of samples. Notify is the lightest
“wake that task” tool FreeRTOS offers for this product moment.

### `xTaskNotifyGive` / `ulTaskNotifyTake` arguments

```c
xTaskNotifyGive(g_audio);
ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
```

| Piece | Meaning |
|-------|---------|
| `Give(handle)` | Add one to that task’s notification count; may unblock it |
| `Take(clear, ticks)` | Block until count > 0 (or timeout) |
| `pdTRUE` (clear on take) | After wake, set count to 0 → **one Give → one beep** |
| `pdFALSE` | Decrement by one (counting semaphore style) — not used here |
| `portMAX_DELAY` | Wait forever; no timeout (audio has nothing else to do) |

If you used `pdFALSE` and tapped Beep three times before audio finished one
tone, three Gives could queue up as a count of 3 — three beeps in a row after
catch-up. Clearing with `pdTRUE` keeps “each finished Take pairs with the
wakes you care about” simple for this lab (still one beep per Take; rapid
taps during a long tone can coalesce depending on timing — good discussion).

### Why not `vTaskDelay` inside `audio_task` to wait for taps?

Delay would wake on a timer whether or not anyone tapped. Notify wakes
**only** when UI has work for audio — zero CPU while idle, no polling.

### Mental model

> UI owns input and may only **Give**. Audio owns the speaker and only
> **Takes** then plays. Notify is the bridge for events; queues come later
> for data streams.

---

## 7. Exercises

1. Tap **Beep** several times — each tap should produce a tone.  
2. Drag **G** / **E** / **R**, tap again — loudness, echo, and release change.  
3. Put `beep_play` back inside `on_beep_click` temporarily — feel the UI/audio coupling (then undo).  
4. (Stretch) Add Lo/Mid/Hi buttons; pass Hz with `xTaskNotify` **value** instead of plain Give (advanced).  

---

## 8. Checkpoint

- [ ] Can name the two tasks and who waits / who gives  
- [ ] Can explain why notify fits Beep better than a queue here  
- [ ] Can explain `pdTRUE` and `portMAX_DELAY` on `Take`  
- [ ] Click callback does **not** call `beep_*`  
- [ ] `on_gain` only stores `g_gain`; `audio_task` applies it  
- [ ] Creating `audio` before `ui` makes sense to you  
- [ ] Tap Beep works on your board (`ws28p` or `es3c28p`)  

---

## 9. Troubleshooting

| Symptom | Try |
|---------|-----|
| `idf.py` not found | Source `<IDF_PATH>\export.ps1` / `export.sh` |
| Wrong board | `-DBOARD=ws28p` or `es3c28p` |
| Tap does nothing | `sound_pad_start()` after `add_button`; `g_audio` non-NULL |
| No sound | Gain not zero; rebuild with current `lab_beep.h` |

---

## 10. Next

[`../step-04-imu-status`](../step-04-imu-status/) — IMU live / sim status on the LCD.
