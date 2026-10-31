# ex06 — Instructor notes

## Headline

**Auto-reload timer** (`pdTRUE`) = periodic. Callback stays short → notify UI.

## Architecture (finished)

```
┌─────────────┐  period   ┌─────────────┐  NotifyGive  ┌─────────┐
│  xTimer     │ ────────► │ timer cb    │ ───────────► │ ui_task │
│ auto-reload │           │ (short)     │              │ flash   │
└─────────────┘           └─────────────┘              └─────────┘
```

## Tips

| Step | Tip |
|------|-----|
| 01 | BOOT only — no FreeRTOS timer yet |
| 02 | BOOT count freezes during long Delay beat — name the cost |
| 03 | BOOT still responsive while heartbeat runs |
