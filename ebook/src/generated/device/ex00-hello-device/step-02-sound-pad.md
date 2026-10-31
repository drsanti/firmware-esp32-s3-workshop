# Lab 00 / Step 02 — Sound pad (buttons, callbacks, tones)

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

```text
rbdev run 00
# step switch in ex00-hello-device.c:  ex00_step02_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex00-hello-device/steps/step-02-sound-pad/README.md</code></p>

</div>


**Device lab:** [Hello Device](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 01](../step-01-lcd-hello/) (LCD bring-up)

---

## 1. What this step must deliver

| # | Skill | In this lab |
|---|--------|-------------|
| 1 | **Play sound** | `beep_play(BEEP_TONE, &params)` (+ Gain / Echo / Release) |
| 2 | **Create a button** | `sound_pad_add_button(...)` |
| 3 | **Callback functions** | Your `on_tone_click` runs when the button is tapped |
| 4 | **`void *user`** | Pass different data (here: frequency) into the **same** callback |

**Success:** tap Lo / Mid / Hi → three different pitches; drag Gain / Echo / Release to refine the tone.

The helper (`lab_sound_pad`) draws the title and touch polling.  
**You** add knobs (`add_gain_slider` / `add_echo_slider` / `add_release_slider`) and three buttons + callbacks.  
Tones use library `rb_sfx` (soft envelope + echo) under `lab_beep`.

---

## 2. Build / flash
Once per PC (workshop root): `rbdev init`, then a **new** terminal.
From the lab folder use `rbdev run NN` (no `export.ps1` / `export.sh`).


Why keep the task alive with a delay instead of returning?

- `app_main` is a FreeRTOS task. Returning ends that task; the system keeps
  running other tasks (including the pad’s touch poller), but an idle
  `app_main` with delay is a clear “setup done, nothing else to do here.”
- `pdMS_TO_TICKS(1000)` converts ms → ticks (same idea as step 01).
- A busy spin here would waste CPU without helping audio or touch.

### Why playing sound in the click callback is a temporary design

`on_tone_click` calls `beep_play`, which **blocks** until the tone’s release
and echo finish. While it blocks:

- That call stack cannot return quickly to the touch path.
- A long tone can make the UI feel sticky.

For a short teaching pad that is acceptable. It is **not** the architecture
you want when UI, sensors, and audio must stay responsive together.

| Design | When it fits |
|--------|----------------|
| Beep in callback (this step) | Tiny tones; prove `beep_play` + buttons |
| Beep in a dedicated task (step 03) | UI must return immediately; audio may block |

### What we are not using yet

| Feature | Why not here |
|---------|----------------|
| `xTaskCreate` (your own tasks) | Helper owns the touch task; you only idle |
| `xTaskNotifyGive` / `Take` | Step 03 — wake audio without playing in the click path |
| Queue | Later labs (streaming data, not one-shot beeps) |

### Mental model

> Step 02 proves sound and callbacks. FreeRTOS delay only keeps `app_main`
> polite. Step 03 redesigns **who** is allowed to block on audio.

---

## 9. Exercises

1. **Hear the path** — flash, tap Lo / Mid / Hi; confirm pitch order.  
2. **Change a frequency** — set Hi to `2000`; rebuild/flash.  
3. **Three callbacks** — write `on_lo` / `on_mid` / `on_hi` instead of one shared handler (`user` can be `NULL`).  
4. **Knobs** — try Gain, Echo, and Release; tap again.  
5. **(Challenge)** Pass a `struct { unsigned hz; unsigned ms; }` via `user` (must be `static` or global).

---

## 10. Checkpoint

- [ ] Can explain what a callback is in one sentence  
- [ ] Can explain why `user` exists and how 440 becomes `(void *)`  
- [ ] Can say why `beep_play` in the click path can feel sticky  
- [ ] Can say what step 03 will change (notify → audio task)  
- [ ] Three buttons work with different tones  
- [ ] Gain / Echo / Release change the tone  

---

## 11. Troubleshooting

| Symptom | Try |
|---------|-----|
| `idf.py` / flash fails | Re-run `rbdev init`; use `rbdev run NN` |
| Wrong board / blank UI | Match `-DBOARD=` to the hardware (`ws28p` or `es3c28p`) |
| No touch response | Call `sound_pad_start()` after all `add_button`s; confirm touch panel |
| No sound (`es3c28p`) | Gain not zero; amp path is ES8311 — rebuild after latest `lab_beep` / I2S fixes |
| All buttons same pitch | Check each `user` cast / value |
| Continuous tone / reboot | Rebuild with current `lab_beep.h` (static PCM + silence flush) |

---

## 12. Next

[`../step-03-two-tasks`](../step-03-two-tasks/) — move beeps to a dedicated `audio_task` woken with **task notify** (cleaner than playing from the click path).
