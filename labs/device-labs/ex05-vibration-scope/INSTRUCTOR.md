# Lab 05 — Instructor notes

## Doc spine

Same as Labs 01–04. Headline: **producer/consumer sample queue** (length > 1).

## Architecture (finished lab)

```
┌─────────────┐   Send    ┌─────────────┐  drain   ┌─────────────┐
│sensor_task  │ ────────► │ sample Q    │ ───────► │  ui_task    │
│ IMU @100Hz  │           │ len 32-64   │ @30Hz    │ rb_chart    │
└─────────────┘           └─────────────┘          └─────────────┘
```

Failed `Send` increments `g_stats.drops++` (non-blocking producer).

## Contrast

| Lab 01 tilt queue | Lab 05 scope queue |
|------------------|---------------------|
| Length **1** + Overwrite | Length **32–64** + Send |
| Newest sample only | Keep a short backlog for UI |
| UI as fast as sensor posts | UI draws ~30 Hz, drains many |

## Step tips

| Step | Tip |
|------|-----|
| 01 | Fake wave scrolls |
| 02 | Shake board / wait for sim |
| 03 | Chart stutters or UI feels dead — name “draw too often” |
| 04 | Smooth again; sensor still 100 Hz |
| 05 | Provoke drops by shortening queue in a demo (optional) |

## Common failures

- Wrong target (`esp32` vs `esp32s3`) → `set-target esp32s3`  
- Wrong `BOARD`  
- Step 04 empty chart → queue not created / sensor not Send  
