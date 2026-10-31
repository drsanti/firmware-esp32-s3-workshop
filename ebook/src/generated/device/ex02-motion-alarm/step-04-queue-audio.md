# Lab 02 / Step 04 — Queue + audio

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

```text
rbdev run 02
# step switch in ex02-motion-alarm.c:  ex02_step04_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex02-motion-alarm/steps/step-04-queue-audio/README.md</code></p>

</div>


**Device lab:** [Motion Alarm](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 03](../step-03-busy-alarm/) (felt the freeze)

---

## 1. What this step must deliver

- Shake still trips **ALARM** on the banner.
- Sound plays via `audio_task` + `beep_play(BEEP_ALARM)`.
- Banner / `horiz` keep updating **during** the sound.
- Detect path only `xQueueSend`s — never calls `beep_play`.

**Success:** trip alarm → hear sound → numbers keep moving.

---

## 2. Problems this step faces

- Step 03: audio in detect loop → freeze.
- Alarm is a **discrete event** (not a continuous tilt stream like Lab 01).
- Audio may backlog if shakes are fast — need a short queue policy.

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| `detect_task` | Owns IMU + banner; short work only |
| `audio_task` | Owns `beep_play`; may block safely |
| Queue length **4** + `xQueueSend(..., 0)` | Buffer a few events; drop if full (no freeze) |
| Tiny `alarm_evt_t` | Room for a pattern id later |

### Architecture

```
┌─────────────┐  Send      ┌─────────────┐  Receive  ┌─────────────┐
│   detect    │ ──────────►│  alarm evt  │──────────►│    audio    │
│    task     │            │ queue (x4)  │           │ beep_play   │
└──────┬──────┘            └─────────────┘           └─────────────┘
       │
       │ alarm_*
       ▼
┌─────────────┐
│     LCD     │
└─────────────┘
```

### Sequence

```
detect           alarm evt Q         audio
 |                    |                 |
 |-- shake edge       |                 |
 |-- Send ----------->|                 |
 |-- update banner    |                 |
 |                    |-- Receive ----->|
 |                    |                 |-- BEEP_ALARM
 |-- update horiz     |                 |
```

**Rejected here:**

| Option | Why not |
|--------|---------|
| Notify only | Works for “play once,” but this lab practices **event queues** |
| Len-1 overwrite (Lab 01 tilt) | Fine for newest sample; alarm cues are **events** you may want to keep a few of |
| `beep_play` in detect | Step 03 again |

---

## 4. Build / flash
```bash
cd labs/device-labs
# Lab: rbdev run 02  |  Step: ex02.c
rbdev run 02   # from labs/device-labs (step still in ex02.c)
```

---

## 5. Student sketch (`main/main.c`)

```c
static QueueHandle_t g_evt;

static void audio_task(void *arg)
{
    alarm_evt_t evt;
    for (;;) {
        if (xQueueReceive(g_evt, &evt, portMAX_DELAY) == pdTRUE) {
            (void)beep_play(BEEP_ALARM, NULL);
        }
    }
}

static void detect_task(void *arg)
{
    /* ... IMU / horiz / edge as step 02 ... */
    if (shake && !was_shake) {
        alarm_set_state(ALARM_ST_ALARM);
        evt.kind = 0;
        (void)xQueueSend(g_evt, &evt, 0);  /* do not block UI */
    }
    alarm_set_detail("horiz %.1f", (double)horiz);
    vTaskDelay(pdMS_TO_TICKS(40));
}

void app_main(void)
{
    bsp_init();
    g_evt = xQueueCreate(4, sizeof(alarm_evt_t));
    (void)xTaskCreate(audio_task, "audio", 4096, NULL, 5, NULL);
    (void)xTaskCreate(detect_task, "detect", 4096, NULL, 5, NULL);
}
```

### How the code works

**`app_main`.**  
Creates the event queue (length 4), then starts audio and detect. The queue
must exist before either task runs.

**`detect_task`.**  
Same IMU/edge logic as step 02; on edge posts to the queue and updates the
banner. `xQueueSend(..., 0)` means “don’t wait if full” — drop rather than
freeze.

**`audio_task`.**  
Blocks on `xQueueReceive` until an event arrives, plays `BEEP_ALARM`, waits
again. Banner work never waits on the speaker.

---

## 6. FreeRTOS in depth — event queue vs Lab 01 tilt queue

| | Lab 01 tilt | Lab 02 alarm |
|--|-------------|--------------|
| Data | Continuous `{ax,ay}` | Discrete “play” event |
| Length | **1** + Overwrite | **4** + Send |
| Full policy | Replace oldest (always newest) | Drop new if full (`ticks=0`) |
| Consumer | UI draw | Audio play |

### `xQueueCreate` / `Send` / `Receive`

| API | Role |
|-----|------|
| `xQueueCreate(4, sizeof(alarm_evt_t))` | Four slots; copies the struct |
| `xQueueSend(q, &evt, 0)` | Post; return immediately if full |
| `xQueueReceive(q, &evt, portMAX_DELAY)` | Wait forever for next event |

### Why not notify for this step?

Notify is enough for a bare “wake and chirp.” A queue makes the **event path**
explicit and ready for a payload (`kind` / pattern). Lab 00/01 already taught
notify — here you practice Send/Receive.

### Mental model

> **Stream of measurements** → overwrite queue (Lab 01).  
> **Discrete cues** → event queue (this step) or notify (Lab 00/01).

---

## 7. Checkpoint

- [ ] Can contrast tilt overwrite vs alarm event queue  
- [ ] Can explain `Send(..., 0)` vs blocking Send  
- [ ] Banner moves during `BEEP_ALARM`  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Smooth + silent | Queue not created / audio task missing |
| Still freezes | `beep_play` still in detect — move it |
| Missed chirps | Queue full while audio busy — expected with `ticks=0` |

---

## 9. Next

[Step 05 — Arm / disarm](../step-05-arm-disarm/).
