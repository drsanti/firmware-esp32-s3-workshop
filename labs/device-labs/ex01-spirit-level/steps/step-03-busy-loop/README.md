# Lab 01 / Step 03 — Busy loop (broken on purpose)

**Device lab:** [Spirit Level](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 02](../step-02-imu-bubble/)

---

## 1. What this step must deliver

This step is a **failed design** you are meant to **feel**, not ship.

- Same spirit bubble + IMU/sim as step 02.
- When you **enter LEVEL**, play a chime.
- The chime is called **inside the same loop** that reads the IMU and draws
  the bubble — so while the speaker plays, the bubble **freezes**.

**Success for this step:** you can describe *why* the bubble stops, in one
sentence about blocking and one loop.

---

## 2. Problems this step faces

- A nice product feature (“beep when level”) seems small to add.
- `beep_play` **blocks** until the note (and echo) finish — hundreds of ms.
- Sense + draw + long audio share **one** thread → smooth motion dies.

---

## 3. Design approach (the wrong one — on purpose)

| Choice | Why we show it |
|--------|----------------|
| Keep one loop from step 02 | Same structure students just trusted |
| On LEVEL rising edge → `beep_play` **here** | Looks simple; breaks the short-loop contract |
| Comment marks it as intentional | So nobody mistakes this for the solution |

### Architecture

Sense, draw, and blocking audio share one loop — the bubble cannot refresh
during `beep_play`.

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│ IMU / sim   │ ──► │  spirit_*   │ ──► │   bubble    │
└─────────────┘     └──────┬──────┘     └─────────────┘
                           │ LEVEL edge
                           ▼
                    ┌─────────────┐
                    │  beep_play  │  *** blocks loop ***
                    └─────────────┘
```

### State machine

LEVEL detection uses a rising edge; the chime runs inside the only UI task.

```
     ┌────────┐
     │  TILT  │
     └───┬────┘
         │ spirit_is_level (rising)
         ▼
     ┌────────┐  beep_play blocks   ┌────────┐
     │ LEVEL  │ ──────────────────► │ frozen │
     └───┬────┘                     └───┬────┘
         │ leave LEVEL                  │ chime done
         └──────────────────────────────┘
                 back to TILT path
```

**Correct direction (step 04+):** separate who may block (audio) from who must
keep drawing (UI), and pass tilt data with a queue.

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
for (;;) {
    /* ... imu or sim ... */
    spirit_set_accel_mps2(acc[0], acc[1]);
    level = spirit_is_level();

    /* INTENTIONAL BUG — do not ship this */
    if (level && !was_level) {
        (void)beep_play(BEEP_CHIME, NULL);  /* blocks */
    }
    was_level = level;

    vTaskDelay(pdMS_TO_TICKS(33));
}
```

### How the code works

**Same as step 02** until the LEVEL edge. Then:

1. `spirit_is_level()` is true after the bubble is near center.  
2. `was_level` makes this a **rising edge** (one chime per enter, not every
   frame while level).  
3. `beep_play(BEEP_CHIME, NULL)` runs **in this task**. It does not return
   until the chime’s release/echo finish.  
4. During that time, the loop cannot read the IMU or call
   `spirit_set_accel_mps2` again → the bubble freezes on screen.  
5. After the chime, the loop resumes — motion jumps to the new tilt.

That is the whole lesson: **long work and smooth UI cannot share one loop.**

---

## 6. FreeRTOS in depth — when one loop is not enough

### What FreeRTOS is doing here

Still only **one** task loop (`app_main`) plus `vTaskDelay`. FreeRTOS cannot
“fix” a bad architecture: while `beep_play` blocks inside your task, that
task is not ready to draw — even though the kernel is fine.

| Myth | Reality |
|------|---------|
| “We have FreeRTOS, so UI stays smooth” | Only if **other** tasks keep running the UI |
| “Delay at the end of the loop helps” | Delay never runs during `beep_play` |

### Why `beep_play` blocks

It fills PCM and writes to the speaker until the envelope/echo tail ends
(see `lab_beep`). That is correct for **an audio task**. It is wrong for the
**only** task that also owns the spirit UI.

### Compare to Lab 00

Lab 00 step 03 already taught: click path must not call `beep_*`; use notify
and an `audio_task`. Here the “click” is “entered LEVEL,” but the same rule
applies — we break it so you feel the spirit level fail.

### What we are *not* fixing yet (step 04)

| Fix | Role |
|-----|------|
| `sensor_task` + `ui_task` | Sense and draw at their own rates |
| Len-1 overwrite **queue** | Carry latest tilt without backlog |
| Later: notify + `audio_task` | LEVEL cue without freezing (step 05) |

### Mental model

> A short single loop was a valid design in step 02. Adding a blocking chime
> in that loop violates the contract. FreeRTOS features (extra tasks, queue,
> notify) exist to **split** those jobs — next steps show how.

---

## 7. Checkpoint

- [ ] Can make the bubble freeze by entering LEVEL (or center the sim)  
- [ ] Can explain the freeze in terms of **blocking in the draw/sense loop**  
- [ ] Can say why `vTaskDelay` does not help during `beep_play`  
- [ ] Understand step 04 will split tasks — not “delete the beep”  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Never freezes | Tip board into LEVEL (or wait for sim to cross center); listen for chime |
| No sound | Volume / `lab_beep`; rebuild with matching `-DBOARD=` |
| Blank LCD | Same as step 02 |

---

## 9. Next

[Step 04 — Split + queue](../step-04-split-queue/).
