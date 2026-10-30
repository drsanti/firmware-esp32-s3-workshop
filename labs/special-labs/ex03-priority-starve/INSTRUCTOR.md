# ex03 — Instructor notes

## Headline

A **higher-priority** task that never blocks **starves** lower-priority work
(UI). Fix: lower its priority and/or `vTaskDelay` so others run.

## Architecture (finished)

```
┌─────────────┐  prio 5   always can preempt
│  ui_task    │ ─────────────────────────────► LCD ticks
└─────────────┘
┌─────────────┐  prio 3   yields with Delay
│  hog_task   │ ─────────────────────────────► burns CPU politely
└─────────────┘
```

## Tips

| Step | Tip |
|------|-----|
| 01 | Worker delays — UI stays smooth |
| 02 | Hog prio > UI, tight loop — counter freezes |
| 03 | Either drop hog prio below UI, or Delay(1) inside hog — both teach |

## Common failures

- Same priority + long hog without delay → depends on time slicing config  
- “Fix” that only raises UI without yielding hog — still fights on equal tier  
