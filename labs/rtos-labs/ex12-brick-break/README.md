# ex12 — IMU brick breaker (Breakout)

**Goal:** tilt the board to move the **paddle**, bounce the ball, clear the
**brick** grid. Capstone sibling of [ex11](../ex11-imu-racer/).

| Input | Action |
|-------|--------|
| Tilt (`acc[0]`) | Move paddle |
| Touch / **BOOT** | Start game, **launch** ball, or retry |
| No IMU | `imu_sim` (`SIM` in HUD) |

```
┌──────────────────────────────────────┐
│ SCORE 00420   L3           LIVE|SIM  │
│ ████ ████ ████ ████ ████ ████        │
│ ████ ████ ████ ████ ████ ████        │
│              (o)                     │
│           [========]                 │
└──────────────────────────────────────┘
```

Enable in `main/main.c` with `ex12_start()`.

| File | Holds |
|------|--------|
| `ex12.c` | Tasks + queues |
| `ex12_game.*` | Physics / phases |
| `ex12_gfx.*` | Full-screen paint |

```powershell
idf.py -DBOARD=ws28p reconfigure
# or
idf.py -DBOARD=es3c28p reconfigure
```

---

## FreeRTOS wiring

Same shape as ex11: IMU overwrite queue → game → gfx; SFX queue → `beep_play`.

| SFX | Event |
|-----|--------|
| Chime | Start / clear board |
| Click | Brick, wall, paddle, launch |
| Alarm | Life lost / game over |

---

## Play loop

1. Title → tap → play (ball stuck on paddle)  
2. Tap again → launch  
3. Clear all bricks → **CLEAR!**  
4. Miss bottom → life −1 (3 lives) → **GAME OVER**

---

## Troubleshooting

| Symptom | Try |
|---------|-----|
| Ball never moves | Second tap/BOOT to launch |
| Paddle drifts | `SIM` without real IMU |
| Linker miss | `reconfigure` after adding folder |

---

## Next

[ex13 — space shooter](../ex13-space-shoot/) — tilt ship, auto-fire, enemies.
