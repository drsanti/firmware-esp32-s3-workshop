# Lab 02 / Step 03 — Busy alarm (**broken**)

**Device lab:** [Motion Alarm](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 02](../step-02-shake-detect/)

---

## 1. What this step must deliver

- Same shake detect as step 02.
- On rising edge → `beep_play(BEEP_ALARM)` **in this loop**.
- You should **see the banner freeze** while the alarm sounds.

**Success:** you can feel/name the architecture bug (not a FreeRTOS failure).

---

## 2. Problems this step faces

- `beep_play` **blocks** until the two-tone finishes.
- Sense + draw + audio in one task → UI dies during sound.

---

## 3. Design approach (intentionally bad)

| Choice | Why (for the lesson) |
|--------|----------------------|
| `beep_play` on edge here | Looks simple; breaks the short-loop contract |

### Architecture

```
┌─────────┐  edge    ┌─────────┐  blocks   ┌─────────┐
│ IMU +   │ ───────► │  ALARM  │ ────────► │ beep_   │
│  horiz  │          │ banner  │           │  play   │
└─────────┘          └─────────┘           └─────────┘
      (same task: loop frozen until beep returns)
```

### State machine

```
┌──────────────┐  shake edge   ┌──────────────┐  beep done   ┌──────────────┐
│    ARMED     │ ─────────────►│    ALARM     │ ───────────► │    ARMED     │
└──────────────┘               └──────────────┘               └──────────────┘
```

---

## 4. Activate ESP-IDF, then build

```bash
cd labs/device-labs
# Lab: .\run 02  |  Step: ex02.c
.\run 02   # from labs/device-labs (step still in ex02.c)
```

---

## 5. Student sketch (`main/main.c`)

```c
/* same IMU / horiz / edge detect as step 02, then: */
if (shake && !was_shake) {
    alarm_set_state(ALARM_ST_ALARM);
    /*
     * INTENTIONAL BUG: play sound in the detect/draw loop.
     * beep_play blocks for the whole alarm — banner stops updating.
     */
    (void)beep_play(BEEP_ALARM, NULL);
    alarm_set_state(ALARM_ST_ARMED);
}
was_shake = shake;
alarm_set_detail("horiz %.1f", (double)horiz);
vTaskDelay(pdMS_TO_TICKS(40));  /* never reached during beep_play */
```

### How the code works

**Same detect loop as step 02** until the rising edge.

**The bug.**  
`beep_play(BEEP_ALARM, NULL)` runs **in this task**. It does not return until
the two-tone (and release/echo) finish — hundreds of ms. During that time the
loop cannot update `horiz` or the banner.

**Why the trailing delay does not help.**  
`vTaskDelay` at the bottom only runs *after* `beep_play` returns. It never
interrupts the freeze.

Step 04 moves `beep_play` into `audio_task` and posts an event on a queue.

---

## 6. FreeRTOS in depth — blocking in the wrong task

FreeRTOS is still scheduling correctly. **Your** task chose to block on speaker
I/O. Other tasks (if any) could run — but you have only this loop.

| Myth | Reality |
|------|---------|
| “Delay at the end helps” | Delay never runs during `beep_play` |
| “FreeRTOS failed” | Architecture put audio in the UI/sense path |

Step 04 moves audio to another task and uses a **queue** for the event.

---

## 7. Checkpoint

- [ ] Saw freeze during alarm sound  
- [ ] Can say why `vTaskDelay` does not help mid-beep  
- [ ] Ready for queue + audio task  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| No freeze | Trip again; listen for full `BEEP_ALARM` |
| No sound | Volume / `BOARD` / `lab_beep` |

---

## 9. Next

[Step 04 — Queue + audio](../step-04-queue-audio/).
