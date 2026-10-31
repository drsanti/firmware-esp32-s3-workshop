# Lab 04 — Instructor notes

## Doc spine

Same labsheet standard as Labs 01–03. Headline API: **`vTaskDelayUntil`**.

## Architecture (finished lab)

```
┌─────────────┐   beat++   ┌─────────────┐   poll   ┌─────────────┐
│  tick_task  │ ─────────► │  g_stats    │ ◄─────── │   ui_task   │
│ DelayUntil  │            │ bpm/jitter  │          │ BOOT / LCD  │
└─────────────┘            └─────────────┘          └─────────────┘
```

## Contrast

| Relative `vTaskDelay` | Absolute `vTaskDelayUntil` |
|-----------------------|----------------------------|
| Sleep N ms *from now* | Wake on a **schedule** of wake times |
| Period ≈ work + N | Period ≈ N (if work < N) |

```
  vTaskDelay (relative)          vTaskDelayUntil (absolute)
  .-- work --.-- N --.           .-- work --.-- N-work --.
  .<--- N + work --->.           .<-------- N ---------->.
```

Ask after step 03: “Where did the extra ms go?” → inside `beep_play` / draw
before the delay.

## Step tips

| Step | Tip |
|------|-----|
| 01 | Visual beat only |
| 02 | Click sounds OK until you watch a clock |
| 03 | Jitter climbs above a few ms — name the cause |
| 04 | Jitter drops; UI can lag-draw without shifting the beat |
| 05 | BOOT raises BPM; period updates on next DelayUntil cycle |

## Common failures

- Wrong `BOARD`  
- Step 05 PWR instead of BOOT  
- `DelayUntil` with period 0 / BPM 0 — clamp BPM  
