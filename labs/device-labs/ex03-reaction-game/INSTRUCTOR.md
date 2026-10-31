# Lab 03 — Instructor notes

## Doc spine

Same as Lab 01/02. Headline new API: **software timer**. Notify is reuse for
the GO beep in step 05.

## Architecture (finished lab)

```
┌─────────────┐  NotifyGive   ┌─────────────┐  NotifyGive ┌─────────────┐
│  go timer   │ ─────────────►│   ui task   │────────────►│ audio task  │
│  (one-shot) │               │ game states │             │ BEEP_CLICK  │
└─────────────┘               └──────┬──────┘             └─────────────┘
                                     │
                                     │ react_*
                                     ▼
                              ┌─────────────┐
                              │     LCD     │
                              └─────────────┘
```

## Contrast with earlier labs

| Earlier | Lab 03 |
|---------|--------|
| Queue = tilt stream / alarm events | Timer = “do this once in the future” |
| `vTaskDelay` blocks *this* task | Timer callback runs in timer service task |
| Notify for audio (00/01) | Same pattern on GO |

Stress: timer callbacks must stay **short** — NotifyGive / set a flag; no
`beep_play`, no heavy LCD in the callback.

## Step tips

| Step | Tip |
|------|-----|
| 01 | Watch READY→WAIT→GO→RESULT cycle |
| 02 | BOOT after GO; ms should look plausible |
| 03 | Banner frozen / no pulse during spin — name the bug |
| 04 | Detail still updates during WAIT |
| 05 | False-start if BOOT in WAIT; best time; chime on GO |

## Common failures

- Wrong `BOARD` → rebuild with matching `-DBOARD=`  
- Step 05 no react → pressing PWR instead of BOOT  
- Timer never fires → forgot `xTimerStart` / one-shot reload period  
- Crash in callback → moved `beep_play` into timer callback (undo)  
