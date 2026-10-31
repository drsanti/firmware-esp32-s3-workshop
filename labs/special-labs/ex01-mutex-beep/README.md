# ex01 — Mutex Beep

**Shared speaker** → two tasks overlap `beep_play` → learn a **mutex**.

Optional track (after Labs 00–06). **3 steps** each. Same boards:
`ws28p` / `es3c28p`.

## What the finished device must do

- Two tasks (“hi” / “lo”) request tones on one speaker.
- LCD shows who is playing.
- Beeps never overlap / garble — mutex guards `beep_play`.

## FreeRTOS teaching set (1)

| Feature | Role |
|---------|------|
| Mutex (`xSemaphoreCreateMutex` / Take / Give) | Exclusive access to shared beep |

## Steps (3)

| Step | Folder | Focus |
|------|--------|-------|
| 01 | [`step-01-two-voices`](steps/step-01-two-voices/) | Two tasks, carefully staggered |
| 02 | [`step-02-overlap-crash`](steps/step-02-overlap-crash/) | Overlap on purpose (**broken**) |
| 03 | [`step-03-mutex-guard`](steps/step-03-mutex-guard/) | Mutex around `beep_play` |

## Design arc

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Step 01    │ ──► │  Step 02    │ ──► │  Step 03    │
│ two voices  │     │ overlap BAD │     │ mutex guard │
│ staggered   │     │ garbled beep│     │ clean audio │
└─────────────┘     └─────────────┘     └─────────────┘
```

## Prior

[Lab 06 — Task Monitor](../device-labs/ex06-task-monitor/) · [labs/README.md](../README.md)
