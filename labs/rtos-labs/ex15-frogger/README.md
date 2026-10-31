# ex15 — IMU Frogger

**Goal:** cross the road and river. Tilt to strafe, tap/**BOOT** to hop forward.
Fill all 5 goal pads to clear the wave.

| Input | Action |
|-------|--------|
| Tilt (`acc[0]`) | Step left / right |
| Touch / **BOOT** edge | Hop one lane up |
| Touch / **BOOT** (title/over) | Start / retry |

```
┌──────────────────────────────────────┐
│ SCORE  …  L3 W1            LIVE|SIM  │
│ [ ][ ][G][ ][ ]   ← goals            │
│ ~~~~ log ~~~~                        │
│   ROAD  ▓▓▓                          │
│              🐸                      │
└──────────────────────────────────────┘
```

Enable in `main/main.c` with `ex15_start()`.

| File | Holds |
|------|--------|
| `ex15.c` | Tasks + queues |
| `ex15_game.*` | Lanes, cars, logs, goals |
| `ex15_gfx.*` | Paint |

```powershell
idf.py -DBOARD=ws28p reconfigure
# or
idf.py -DBOARD=es3c28p reconfigure
```

---

## FreeRTOS

Same shape as ex11–14: IMU overwrite → game → gfx; SFX queue.

| SFX | Event |
|-----|--------|
| Chime | Start / wave clear |
| Click | Hop / goal |
| Alarm | Splat / game over |

---

## Rules

- Car hit → lose life  
- Water without a log → drown  
- Drift off screen on a log → drown  
- Occupied goal pad → lose life  
- 5 goals → wave++ (faster traffic)  

---

## Troubleshooting

| Symptom | Try |
|---------|-----|
| Drifts sideways | `SIM` without IMU |
| Can't hop | Use edge tap / BOOT (not hold) |
| Linker miss | `reconfigure` |
