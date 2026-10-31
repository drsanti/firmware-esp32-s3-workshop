# ex04 — Counting Semaphore

**Pool of N slots** → too many workers grab the speaker → learn a
**counting semaphore**.

Optional track. **3 steps.** Boards: `ws28p` / `es3c28p`.

## What the finished device must do

- At most **3** workers hold a “slot” and beep at once.
- LCD shows in-use / free slots.
- Extra workers **wait** until a slot is given back.

## FreeRTOS teaching set (1)

| Feature | Role |
|---------|------|
| Counting semaphore | Limit concurrent users of a pool (N tokens) |

## Steps (3)

| Step | Folder | Focus |
|------|--------|-------|
| 01 | [`step-01-three-slots`](steps/step-01-three-slots/) | Face + 2 polite workers |
| 02 | [`step-02-oversubscribe`](steps/step-02-oversubscribe/) | 5 workers, no limit (**broken**) |
| 03 | [`step-03-sem-pool`](steps/step-03-sem-pool/) | Counting sem (max 3) |

## Design arc

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Step 01    │ ──► │  Step 02    │ ──► │  Step 03    │
│ 2 workers   │     │ 5 no limit  │     │ count sem 3 │
│ looks fine  │     │ audio mess  │     │ pool OK     │
└─────────────┘     └─────────────┘     └─────────────┘
```

## Prior

[S3 — Priority](../ex03-priority-starve/) · [labs/README.md](../README.md)
