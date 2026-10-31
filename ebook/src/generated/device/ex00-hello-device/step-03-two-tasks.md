# Lab 00 / Step 03 — Two tasks + notify

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

```text
rbdev run 00
# step switch in ex00-hello-device.c:  ex00_step03_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex00-hello-device/steps/step-03-two-tasks/README.md</code></p>

</div>


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

## 4. Build / flash
Once per PC (workshop root): `rbdev init`, then a **new** terminal.
From the lab folder use `rbdev run NN` (no `export.ps1` / `export.sh`).


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
| `idf.py` / flash fails | Re-run `rbdev init`; use `rbdev run NN` |
| Wrong board | `-DBOARD=ws28p` or `es3c28p` |
| Tap does nothing | `sound_pad_start()` after `add_button`; `g_audio` non-NULL |
| No sound | Gain not zero; rebuild with current `lab_beep.h` |

---

## 10. Next

[`../step-04-imu-status`](../step-04-imu-status/) — IMU live / sim status on the LCD.
