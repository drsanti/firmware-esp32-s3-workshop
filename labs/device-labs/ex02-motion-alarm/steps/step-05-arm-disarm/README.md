# Lab 02 / Step 05 — Arm / disarm

**Device lab:** [Motion Alarm](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** [Step 04](../step-04-queue-audio/)

---

## 1. What this step must deliver

- **BOOT** rising edge toggles **IDLE ↔ ARMED** (both boards; ignore PWR).
- Shakes only trip when **ARMED**.
- While **ALARM**, ignore new shakes (cooldown / hold) and ignore BOOT.
- Audio still via event queue (unchanged from step 04).

**Success:** disarm → shake does nothing; arm → shake alarms; UI stays live.

---

## 2. Problems this step faces

- Product rules: ignore motion when idle; avoid spam while sounding.
- Same BOOT edge pattern as Lab 00 step 05 (`bsp_buttons` bit1).

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| Keep detect + audio + queue | Audio path already correct |
| State machine IDLE / ARMED / ALARM | Clear product behavior |
| BOOT edge toggle | Hardware input common to both SKUs |
| Hold while ALARM | Cooldown; no re-queue spam |

### Architecture

```
┌─────────┐  BOOT edge  ┌─────────────┐  Send     ┌─────────────┐  Receive  ┌─────────────┐
│  BOOT   │ ───────────►│   detect    │ ────────► │  alarm evt  │──────────►│    audio    │
│ button  │             │    task     │           │    queue    │           │    task     │
└─────────┘             └──────┬──────┘           └─────────────┘           └─────────────┘
                               │
                               │ alarm_*
                               ▼
                        ┌─────────────┐
                        │     LCD     │
                        │ IDLE/ARMED/ │
                        │    ALARM    │
                        └─────────────┘
```

### State machine

```
┌──────────────┐  BOOT arm   ┌──────────────┐  shake edge  ┌──────────────┐
│     IDLE     │ ──────────► │    ARMED     │ ───────────► │    ALARM     │
└──────────────┘             └──────────────┘              └──────────────┘
```

BOOT disarm returns ARMED to IDLE; after ALARM hold, return to ARMED (see sketch).

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
#define BOOT_BTN_MASK 2u  /* bit1 = BOOT on both boards */

static void audio_task(void *arg)
{
    alarm_evt_t evt;
    for (;;) {
        if (xQueueReceive(g_evt, &evt, portMAX_DELAY) == pdTRUE) {
            (void)beep_play(BEEP_ALARM, NULL);
        }
    }
}

/* inside detect_task loop: */
boot = bsp_buttons() & BOOT_BTN_MASK;
if (boot != 0u && boot_was == 0u) {          /* rising edge */
    if (st == ALARM_ST_IDLE) {
        st = ALARM_ST_ARMED;
    } else if (st == ALARM_ST_ARMED) {
        st = ALARM_ST_IDLE;
    }
    /* ignore BOOT while ALARM */
}
boot_was = boot;

horiz = sqrtf(acc[0]*acc[0] + acc[1]*acc[1]);
shake = (horiz >= 2.5f);
if (st == ALARM_ST_ARMED && shake && !was_shake) {
    st = ALARM_ST_ALARM;
    alarm_until = xTaskGetTickCount() + pdMS_TO_TICKS(1200);
    evt.kind = 0;
    (void)xQueueSend(g_evt, &evt, 0);        /* do not beep here */
}
was_shake = shake;

if (st == ALARM_ST_ALARM && xTaskGetTickCount() >= alarm_until) {
    st = ALARM_ST_ARMED;                     /* cooldown done */
}
```

### How the code works

**`audio_task`.**  
Unchanged from step 04 — Receive event, `beep_play`, wait again.

**BOOT edge.**  
Toggle IDLE ↔ ARMED only when not in ALARM. Same bit1 mask as Lab 00.

**Armed shake.**  
Only ARMED rising edges post to the queue and enter ALARM with a hold time.

**Cooldown.**  
While ALARM, new shakes and BOOT are ignored until `alarm_until`, then return
to ARMED. UI never calls `beep_play`.

---

## 6. FreeRTOS in depth — reuse, not new primitives

No new FreeRTOS API. The lesson is **state + the tools you already have**:

| Piece | Role |
|-------|------|
| Queue | Alarm events → audio (step 04) |
| `vTaskDelay` | Sample period |
| App state enum | IDLE / ARMED / ALARM rules |

BOOT handling is plain C edge detection — same idea as Lab 00 LEDs.

---

## 7. Checkpoint

- [ ] IDLE ignores shake  
- [ ] BOOT toggles arm (not PWR on ws28p)  
- [ ] ALARM hold blocks re-triggers; UI stays smooth  
- [ ] Can name the 3 FreeRTOS features used in Lab 02  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| No toggle | Press **BOOT**, not PWR |
| Alarms while IDLE | State check missing before QueueSend |
| Spam while ALARM | Hold / ignore shakes in ALARM |

---

## 9. Next

Lab 02 complete. Continue with [Lab 03 — Reaction Game](../../ex03-reaction-game/)
or return to [labs/README.md](../../README.md).
