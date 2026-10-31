# ex04 — Instructor notes

## Headline

**Counting semaphore** = N tokens. Take consumes one; Give returns one.
Unlike a mutex, many holders can be “in” at once (up to N).

## Architecture (finished)

```
┌──────────┐  Take   ┌─────────────┐  Give   ┌───────────┐
│ workers  │ ──────► │ counting    │ ──────► │ beep_play │
│   x5     │         │ sem max=3   │         └───────────┘
└──────────┘         └─────────────┘
```

## Tips

| Step | Tip |
|------|-----|
| 01 | Only 2 workers — under the soft “3 slot” story |
| 02 | 5 workers collide — name oversubscribe |
| 03 | CreateCounting(3, 3) — max and initial both 3 |
