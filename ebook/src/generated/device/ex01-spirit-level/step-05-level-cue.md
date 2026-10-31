# Lab 01 / Step 05 — LEVEL cue (notify + audio)

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

<span class="ebook-badge ok">finished device</span>

```text
rbdev run 01
# step switch in ex01-spirit-level.c:  ex01_step05_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex01-spirit-level/steps/step-05-level-cue/README.md</code></p>

</div>


**Device lab:** [Spirit Level](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 04](../step-04-split-queue/) (smooth tilt via queue)

---

## 1. What this step must deliver

- Bubble still tracks tilt via the length-1 overwrite queue (unchanged).
- Soft **chime when entering LEVEL** — same sound as step 03.
- Bubble stays **smooth during the chime** (unlike step 03).
- `beep_play` lives only in `audio_task`; UI only `xTaskNotifyGive`s.

**Success:** tip into LEVEL → hear the chime → bubble keeps moving. You can
name which FreeRTOS tool carries the **stream** and which carries the
**event**.

---

## 2. Problems this step faces

- Step 03 put the chime in the sense/draw loop → freeze.
- Step 04 fixed the data path but had **no audio**.
- LEVEL is a **discrete edge** (“just became level”), not a continuous sample.
- Audio still **blocks** for hundreds of ms — it must not share the UI task.

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| Keep `sensor` + queue + `ui` | Stream of tilt still needs Overwrite / Receive |
| Detect LEVEL **rising edge** in `ui_task` | `spirit_is_level()` is valid after `spirit_set_accel_mps2` |
| `xTaskNotifyGive` on edge | Event signal only — no payload, no blocking |
| `audio_task` + `ulTaskNotifyTake` | Owns `beep_play`; may block safely |
| Create audio **first** | `g_audio` valid before any Give |

### Architecture

Tilt still flows on the queue; LEVEL is an event on notify only.

```
┌─────────────┐  Overwrite   ┌─────────┐  Receive   ┌─────────┐  NotifyGive  ┌─────────────┐
│sensor_task  │ ───────────► │ tilt x1 │ ─────────► │ ui_task │ ───────────► │ audio_task  │
└─────────────┘              └─────────┘            └─────────┘   LEVEL edge └─────────────┘
```

### Sequence

Entering LEVEL: UI keeps receiving tilt while audio plays the chime.

```
ui_task          audio_task
    |                 |
    |-- spirit update |
    |-- LEVEL edge    |
    |-- NotifyGive -->|
    |-- Receive tilt  |-- Take wakes
    |-- spirit update |-- beep_play
    |                 |
```

**Rejected:**

| Option | Why not |
|--------|---------|
| `beep_play` in `ui_task` | Step 03 again — freezes the bubble |
| Queue of “play” structs for one chime | Overkill; notify matches Lab 00 event pattern |
| Notify for every sample while LEVEL | Spam; edge-only matches “entered level” |

---

## 4. Build / flash
Once per PC (workshop root): `rbdev init`, then a **new** terminal.
From the lab folder use `rbdev run NN` (no `export.ps1` / `export.sh`).


```bash
cd labs/device-labs
# Lab: rbdev run 01  |  Step: ex01.c

rbdev run 01   # from labs/device-labs (step still in ex01.c)
# or
idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx monitor
```

---

## 5. Student sketch (`main/main.c`)

```c
static QueueHandle_t g_tilt;
static TaskHandle_t g_audio;

static void audio_task(void *arg)
{
    for (;;) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        (void)beep_play(BEEP_CHIME, NULL);
    }
}

static void ui_task(void *arg)
{
    tilt_sample_t sample;
    int was_level = 0;
    int level;

    spirit_init();
    spirit_start();
    spirit_set_live(1);

    for (;;) {
        if (xQueueReceive(g_tilt, &sample, portMAX_DELAY) == pdTRUE) {
            spirit_set_accel_mps2(sample.ax, sample.ay);
            level = spirit_is_level();
            if (level && !was_level && g_audio != NULL) {
                (void)xTaskNotifyGive(g_audio);  /* wake audio — do not beep here */
            }
            was_level = level;
        }
    }
}

void app_main(void)
{
    bsp_init();
    g_tilt = xQueueCreate(1, sizeof(tilt_sample_t));
    (void)xTaskCreate(audio_task, "audio", 4096, NULL, 5, &g_audio);
    (void)xTaskCreate(sensor_task, "sensor", 4096, NULL, 5, NULL);
    (void)xTaskCreate(ui_task, "ui", 4096, NULL, 5, NULL);
}
```

(`sensor_task` is unchanged from step 04.)

### How the code works

**`app_main`.**  
Creates the tilt queue, then starts **audio first** (so `g_audio` is set),
then sensor and UI — same order idea as Lab 00.

**`sensor_task`.**  
Still owns IMU/sim → `xQueueOverwrite`. No LEVEL logic, no sound.

**`ui_task`.**  
Receives tilt, updates the bubble, reads `spirit_is_level()`. On a **rising
edge** (`level && !was_level`) it `xTaskNotifyGive`s — it never calls
`beep_play`. While audio is blocked on the chime, this task keeps Receiving.

**`audio_task`.**  
Blocks on `ulTaskNotifyTake`. One Give → one `beep_play(BEEP_CHIME)`. It does
not touch the LCD or the queue.

---

## 6. FreeRTOS in depth — stream vs event

### Two tools, two jobs

| Job | Tool | Why |
|-----|------|-----|
| Continuous tilt `{ax, ay}` | Length-1 overwrite **queue** | Carries a payload; newest sample wins |
| “Just entered LEVEL” | Task **notify** | Count / wake; no sample needed |

Hold this model:

> **Stream of measurements** → queue → UI.  
> **Discrete events** → notify → audio.

Mixing them (notify for tilt, or a long queue of “play chime” messages) either
loses data ownership or adds ceremony you do not need here.

### Rising edge, not level-high

```c
if (level && !was_level) { ... }
was_level = level;
```

| Condition | Meaning |
|-----------|---------|
| `level && !was_level` | Just crossed into LEVEL → cue once |
| `level` alone | Would re-fire every sample while centered → spam |

Leave LEVEL, tip back in → edge again → second chime. That matches a spirit
level “click” when you find true.

### `xTaskNotifyGive` / `ulTaskNotifyTake` (same as Lab 00)

| Call | Role |
|------|------|
| `xTaskNotifyGive(g_audio)` | Increment the audio task’s notify count; wake it |
| `ulTaskNotifyTake(pdTRUE, portMAX_DELAY)` | Wait forever; `pdTRUE` clears the count after Take |

If several edges happen during a long chime, Gives **stack** as a count.
`pdTRUE` on Take clears them — so after one long play you do not suddenly
replay every missed edge. For this lab that is fine: one chime per wake is
enough. (A product that must never drop cues would drain the count in a
loop — stretch idea.)

### Why this fixes step 03

| Step 03 | Step 05 |
|---------|---------|
| `beep_play` in the only loop | `beep_play` only in `audio_task` |
| UI blocked for whole chime | UI keeps Receiving / drawing |
| One task owns sense + draw + sound | Three owners: sensor, UI, audio |

FreeRTOS did not fail in step 03 — the **architecture** did. Same kernel APIs;
better task boundaries.

### Create order

```c
xTaskCreate(audio_task, ..., &g_audio);  /* first */
xTaskCreate(sensor_task, ...);
xTaskCreate(ui_task, ...);
```

UI must not NotifyGive before `g_audio` is set. Creating audio first (and
null-checking `g_audio`) matches Lab 00.

---

## 7. Checkpoint

- [ ] Can name stream tool (queue) vs event tool (notify)  
- [ ] Can explain why edge detection, not “while level”  
- [ ] Can say why `beep_play` must not live in `ui_task`  
- [ ] Tip into LEVEL → chime plays **and** bubble keeps moving  
- [ ] Works on your board (`ws28p` or `es3c28p`)  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Smooth tilt, no chime | Enter LEVEL (leave and re-enter); check `g_audio` / volume |
| Chime freezes bubble | You put `beep_play` in `ui_task` — move it back to `audio` |
| Spam of chimes | Edge condition missing; use `level && !was_level` |
| Blank LCD | Queue + `spirit_start` as in step 04 |

---

## 9. Next

Lab 01 complete. Continue with [Lab 02 — Motion Alarm](../../ex02-motion-alarm/)
or return to [labs/README.md](../../README.md).
