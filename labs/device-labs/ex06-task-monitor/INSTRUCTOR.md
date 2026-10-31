# Lab 06 — Instructor notes

## Doc spine

Same as Labs 01–05. Headline: **kernel snapshots** (not a schedule trace).

## Architecture (finished lab)

```
┌─────────────┐                 ┌─────────────┐
│ load_a/b    │                 │  ui_task    │
│ Blocked     │                 │ snapshot@2Hz│
└─────────────┘                 └──────┬──────┘
┌─────────────┐   Send                 │
│ fill_task   │ ──────────► ┌──────────┴──────────┐
└─────────────┘             │ demo Q + mon_* rows │
                            │ BOOT pages list     │
                            └─────────────────────┘
```

## Teaching tips

| Step | Tip |
|------|-----|
| 01 | Static strings — prove the face before any kernel API |
| 02 | Create 2–3 named workers so the list is not only `ui` / IDLE |
| 03 | State letter cheat-sheet: R Ready, B Blocked, r Running, S Suspended |
| 04 | HWM is **minimum free words ever** — lower means closer to overflow |
| 05 | Short queue + slow consumer → waiting count climbs; BOOT pages |

## FreeRTOS config

Steps 02+ need `CONFIG_FREERTOS_USE_TRACE_FACILITY=y` (in each step's
`sdkconfig.defaults`). Re-run `set-target` / `fullclean` if `uxTaskGetSystemState`
is missing after a config change.

## Common failures

- Wrong target (`esp32` vs `esp32s3`) → `idf.py set-target esp32s3`
- Wrong `BOARD`
- Array too small for `uxTaskGetSystemState` → truncated list (teach sizing)
- Snapshot every loop iteration with no delay → UI feels stuck (pace ~2 Hz)
