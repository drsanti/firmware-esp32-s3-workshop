# ex01 — Instructor notes

## Headline

**Mutex** = lock around a shared resource (`beep_play` blocks and is not
re-entrant-safe across tasks).

## Architecture (finished)

```
┌─────────────┐  Take   ┌─────────────┐  Give  ┌─────────────┐
│  hi_task    │ ──────► │   mutex     │ ─────► │   speaker   │
└─────────────┘         │  beep_play  │        └─────────────┘
┌─────────────┐  Take   └─────────────┘
│  lo_task    │ ──────►
└─────────────┘
```

## Tips

| Step | Tip |
|------|-----|
| 01 | Stagger so it *sounds* fine — students may not see the race yet |
| 02 | Shrink delays so tones overlap — name the garbage |
| 03 | Take before beep, Give after; never forget Give on error paths |

## Common failures

- Take forever, forget Give → other voice dies  
- Take in one task only → still garbled  
