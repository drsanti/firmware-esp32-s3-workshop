# ex05 — Instructor notes

## Headline

**Event group** = bitfield of conditions. `WaitBits` can require **all** or
**any** of a mask.

## Architecture (finished)

```
┌─────────────┐  SetBits BOOT   ┌─────────────┐
│ poll BOOT   │ ──────────────► │ event group │
└─────────────┘                 │             │
┌─────────────┐  SetBits STILL  │             │
│ IMU still   │ ──────────────► │             │
└─────────────┘                 └──────┬──────┘
                                       │ WaitBits both
                                       ▼
                                ┌─────────────┐
                                │  ui ARMED   │
                                └─────────────┘
```

## Tips

| Step | Tip |
|------|-----|
| 01 | Shake vs rest so STILL toggles |
| 02 | Press BOOT while shaking — false ARMED |
| 03 | Same press while still — real ARMED |
