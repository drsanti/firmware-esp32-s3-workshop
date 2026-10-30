# Lab 00 / Step 02 — Sound pad (buttons, callbacks, tones)

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

## 2. Activate ESP-IDF, then build / flash

```powershell
. <IDF_PATH>\export.ps1
# example: . D:\dev\esp\v6.1\esp-idf\export.ps1
```

Pick **your** board SKU, then:

```bash
cd labs/device-labs
# Lab: .\run 00  |  Step: ex00.c

# Waveshare ESP32-S3-Touch-LCD-2.8
.\run 00   # from labs/device-labs (step still in ex00.c)

# Ternion ESP32-S3 (ILI9341 + FT6336 + ES8311)
idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx monitor
```

| `BOARD` | Product | Audio | Touch |
|---------|---------|-------|-------|
| `ws28p` | Waveshare 2.8" | PCM5101 I2S DAC | capacitive |
| `es3c28p` | Ternion | ES8311 codec + amp | FT6336 |

Same `main.c` for both — only `-DBOARD=...` changes which `ports/boards/<sku>/` is linked.  
If you switch board, re-run with the new `-DBOARD=` (fullclean if the previous build used the other SKU).

---

## 3. What appears on the LCD

```text
+---------------------------+
| Sound pad                 |
+--------+--------+---------+
|  Lo    |  Mid   |   Hi    |   your three buttons
+--------+--------+---------+
| Gain ############  70     |   helper (beep_set_gain)
+---------------------------+
```

| Button | Frequency (Hz) passed as `user` |
|--------|----------------------------------|
| Lo | 440 |
| Mid | 880 |
| Hi | 1760 |

---

## 4. Play sound — `lab_beep.h`

```c
(void)beep_play(BEEP_TONE, &(beep_params_t){
    .hz = 880,
    .ms = 150,   /* gate ms (capped ~200) */
});
```

| Call | Meaning |
|------|---------|
| `beep_play(kind, params)` | Play a sound (`BEEP_TONE` / `CHIME` / `ALARM` / `CLICK`) |
| `beep_params_t` | Optional fine-tune: `gain` / `echo` / `release` / `hz` / `ms` |
| `beep_set_gain(0..100)` | Live loudness knob (sound pad) |
| `beep_set_echo(0..100)` | Live echo knob |
| `beep_set_release(0..100)` | Live release knob |

`params == NULL` uses kind defaults + current knobs. A field of `0` means
“keep current / default.” Call `beep_play` **inside** your click callback,
not in a tight loop.

---

## 5. Create a button — `sound_pad_add_button`

```c
sound_pad_add_button(label, style, on_click, user);
```

| Argument | Type | Meaning |
|----------|------|---------|
| `label` | `const char *` | Text on the button (ASCII, short) |
| `style` | `rb_style_t` | Look: e.g. `RB_STYLE_INFO` |
| `on_click` | function pointer | Called on tap (or `NULL`) |
| `user` | `void *` | Extra data for that button (see §7) |

Setup order:

1. `sound_pad_init()` — LCD + title  
2. `sound_pad_add_*_slider(NULL, NULL)` — Gain / Echo / Release (`NULL` → `beep_set_*`)  
3. `sound_pad_add_button(...)` — up to 3 times  
4. `sound_pad_start()` — show UI + start touch  

---

## 6. How a callback works

A **callback** is a function **you** write; the framework **calls it later** when an event happens (here: finger lift = CLICK).

```text
  you register                          later, on tap
  ----------------                      ----------------
  add_button(..., on_tone_click, ...)
          |                                    |
          +-------- stored by helper ----------+
                                               v
                                        on_tone_click(user)
                                               |
                                               v
                                          beep_play(BEEP_TONE, ...)
```

Rules of thumb:

- The callback signature is fixed: `void my_fn(void *user)`.
- Do **not** call the callback yourself from `app_main` for the tap — the helper does when touch arrives.
- Keep the body short (play a tone is fine).

### Architecture

Touch events stay inside the sound-pad helper; your callback is the only
student path to audio.

```
┌────────────┐
│ sound_pad  │
│  helper    │
└─────┬──────┘
      │ touch
      ▼
┌─────────────┐
│ touch path  │
└─────┬───────┘
      │ on_click
      ▼
┌──────────────┐
│ on_tone_click│
└──────┬───────┘
       ▼
┌─────────────┐
│  beep_play  │
└─────────────┘
```

---

## 7. How `void *user` works

One callback can serve **many** buttons if each button passes different `user` data.

In this lab, `user` holds the frequency as an integer disguised as a pointer:

```c
/* When registering the button: store 440 as a pointer-sized value */
sound_pad_add_button("Lo", RB_STYLE_INFO, on_tone_click,
                     (void *)(uintptr_t)440u);

/* Inside the callback: recover the integer */
static void on_tone_click(void *user)
{
    unsigned hz = (unsigned)(uintptr_t)user;
    (void)beep_play(BEEP_TONE, &(beep_params_t){
                                   .hz = (uint16_t)hz,
                                   .ms = 150u,
                               });
}
```

| Idea | Detail |
|------|--------|
| Why `void *`? | Generic “bag” for any data the library does not need to understand |
| Why cast? | We are packing a small integer into the pointer slot (common embedded pattern) |
| Safer alternative | Pass a pointer to a real `struct` / `static const unsigned` if data is larger |

**Do not** cast a stack address that goes out of scope. Frequency literals / `static` data are OK.

Same callback, three buttons:

| Button | `user` value | Tone |
|--------|--------------|------|
| Lo | `(void *)(uintptr_t)440` | low |
| Mid | `(void *)(uintptr_t)880` | mid |
| Hi | `(void *)(uintptr_t)1760` | high |

---

## 8. Full student sketch (`main/main.c`)

```c
#include "bsp.h"
#include "lab_beep.h"
#include "lab_sound_pad.h"

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void on_tone_click(void *user)
{
    unsigned hz = (unsigned)(uintptr_t)user;
    (void)beep_play(BEEP_TONE, &(beep_params_t){
                                   .hz = (uint16_t)hz,
                                   .ms = 150u,
                               });
}

void app_main(void)
{
    bsp_init();
    sound_pad_init();
    (void)sound_pad_add_gain_slider(NULL, NULL);
    (void)sound_pad_add_echo_slider(NULL, NULL);
    (void)sound_pad_add_release_slider(NULL, NULL);

    (void)sound_pad_add_button("Lo", RB_STYLE_INFO, on_tone_click,
                               (void *)(uintptr_t)440u);
    (void)sound_pad_add_button("Mid", RB_STYLE_SUCCESS, on_tone_click,
                               (void *)(uintptr_t)880u);
    (void)sound_pad_add_button("Hi", RB_STYLE_WARNING, on_tone_click,
                               (void *)(uintptr_t)1760u);

    sound_pad_start();

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
```

### How the code works

Read this once with the sketch open. Unlike a simple `app_main` loop that
plays sound itself, this program **registers** UI and waits. Touch events
arrive later and call *your* function.

**`bsp_init` then `sound_pad_init`.**  
Board bring-up first. `sound_pad_init` opens the LCD and prepares the pad
scaffold (title, button row, touch path). No buttons or knobs are visible yet.

**Knobs with `NULL` callbacks.**  
`sound_pad_add_gain_slider(NULL, NULL)` (and Echo / Release) show knobs
labeled **G** / **E** / **R**. Because `on_change` is `NULL`, the helper
applies `beep_set_gain` / `beep_set_echo` / `beep_set_release` for you. You
are still “using” the knobs: they change the next `beep_play`.

**One callback, three buttons.**  
`on_tone_click` is a normal C function. You pass its address into each
`sound_pad_add_button`. When the user lifts a finger on that button, the
framework calls `on_tone_click(user)`.

**What `user` is for.**  
All three buttons share the same function. The fourth argument packs a
frequency into a pointer-sized integer: `(void *)(uintptr_t)440u`. Inside the
callback you cast back to `unsigned` and call `beep_play(BEEP_TONE, …)`. That is
how one handler plays three pitches without three nearly identical functions.

**`sound_pad_start`.**  
Reflows the layout and starts the internal touch task. Call this **once**,
after every `add_*`. Taps before `start` do nothing useful.

**Idle loop in `app_main`.**  
After setup, `app_main` only sleeps. Sound happens in the click path (and
knob changes update `lab_beep` state). Playing a long tone inside the click
callback blocks that path until the tone finishes — fine for this lab; step 03
moves playback to another task.

| Piece | Job in one sentence |
|-------|---------------------|
| `sound_pad_init` | Open LCD + pad scaffold |
| `add_*_slider(NULL, …)` | Show G/E/R; helper updates `beep_set_*` |
| `on_tone_click` | Shared click handler; reads Hz from `user` |
| `add_button` × 3 | Lo / Mid / Hi with different `user` values |
| `sound_pad_start` | Show UI and start touch polling |
| `beep_play` | Play a short `rb_sfx` tone at that Hz |

Hold this model in mind: *you own the callback and the frequencies; the helper
owns drawing and touch.* Step 03 keeps the pad but moves `beep_play` out of the
click path.

Match the four skills to the code:

| Skill | Where in the sketch |
|-------|---------------------|
| Play sound | `beep_play(BEEP_TONE, …)` inside `on_tone_click` |
| Create button | three `sound_pad_add_button` lines |
| Callback | `on_tone_click` registered as `on_click` |
| `void *user` | `(void *)(uintptr_t)440u` … and cast back in the callback |

---

## FreeRTOS in depth — delay now, notify next

This step’s FreeRTOS surface is small on purpose. The big design lesson is
**where sound runs**.

### `vTaskDelay` in `app_main`

After `sound_pad_start()`, `app_main` only does:

```c
for (;;) {
    vTaskDelay(pdMS_TO_TICKS(1000));
}
```

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
| `idf.py` not found | Source `<IDF_PATH>\export.ps1` / `export.sh` |
| Wrong board / blank UI | Match `-DBOARD=` to the hardware (`ws28p` or `es3c28p`) |
| No touch response | Call `sound_pad_start()` after all `add_button`s; confirm touch panel |
| No sound (`es3c28p`) | Gain not zero; amp path is ES8311 — rebuild after latest `lab_beep` / I2S fixes |
| All buttons same pitch | Check each `user` cast / value |
| Continuous tone / reboot | Rebuild with current `lab_beep.h` (static PCM + silence flush) |

---

## 12. Next

[`../step-03-two-tasks`](../step-03-two-tasks/) — move beeps to a dedicated `audio_task` woken with **task notify** (cleaner than playing from the click path).
