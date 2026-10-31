# Lab 02 — Instructor notes

## Doc spine

Same as Lab 01: **deliver → problems → design → sketch → How the code works →
FreeRTOS in depth**. Event **queue** is the new tool; delay + tasks are reuse.

## Architecture (finished lab)

```
┌─────────────┐  xQueueSend   ┌─────────────┐  Receive  ┌─────────────┐
│   detect    │ ─────────────►│  alarm evt  │──────────►│    audio    │
│    task     │               │ queue (x4)  │           │ beep_play   │
└──────┬──────┘               └─────────────┘           └─────────────┘
       │
       │ alarm_*
       ▼
┌─────────────┐
│     LCD     │
│ IDLE/ARMED/ │
│    ALARM    │
└─────────────┘
```

## Contrast with Lab 01

| Lab 01 | Lab 02 |
|--------|--------|
| Len-1 overwrite queue = **tilt stream** | Short queue = **alarm events** |
| Notify for LEVEL cue | Skip notify — reinforce queue for events with a payload slot |

Ask: “Would notify work for the chime?” → Yes, but this lab practices `Send` /
`Receive` so students can later put a pattern id in the item.

## Step tips

| Step | Tip |
|------|-----|
| 01 | Face only — ARMED + pulsing detail |
| 02 | Sim trips on its own; live board: shake firmly |
| 03 | Watch ALARM banner freeze during `BEEP_ALARM` |
| 04 | Banner keeps updating while audio plays |
| 05 | BOOT (bit1), not PWR on ws28p; IDLE ignores shake |

## Common failures

- Wrong `BOARD` → rebuild with matching `-DBOARD=`  
- Step 05 no arm toggle → pressing PWR instead of BOOT  
- Spam alarms → missing edge or cooldown  
