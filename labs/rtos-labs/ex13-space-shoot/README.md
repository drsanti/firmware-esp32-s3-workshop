# ex13 — IMU space shooter

**Goal:** tilt to move your ship, **auto-fire** upward, destroy enemies before
they reach the bottom or hit you.

| Input | Action |
|-------|--------|
| Tilt (`acc[0]`) | Move ship |
| Touch / **BOOT** | Start / retry |
| Auto | Fire shots on a cooldown |

```
┌──────────────────────────────────────┐
│ SCORE 00850   L3           LIVE|SIM  │
│    vv     vv                         │
│         |                            │
│         /\                           │
└──────────────────────────────────────┘
```

Enable in `main/main.c` with `ex13_start()`.

| File | Holds |
|------|--------|
| `ex13.c` | Tasks + queues |
| `ex13_game.*` | Physics |
| `ex13_gfx.*` | Paint |

```powershell
idf.py -DBOARD=ws28p reconfigure
# or
idf.py -DBOARD=es3c28p reconfigure
```

---

## FreeRTOS

Same as ex11/ex12: IMU overwrite → game → gfx; SFX queue for hit/life/start
(auto-fire is silent so `beep_play` does not stall).

| SFX | Event |
|-----|--------|
| Chime | Start |
| Click | Enemy hit |
| Alarm | Life lost / game over |

---

## Rules

- 3 lives  
- Enemy reaches bottom or touches ship → lose a life  
- Hit enemy → +50 score; spawn rate / speed ramp with score  

---

## Troubleshooting

| Symptom | Try |
|---------|-----|
| Ship drifts | `SIM` without IMU |
| Instant death | Enemies leaked — check lives / spawn |
| Linker miss | `reconfigure` |

---

## Next

[ex14 — Asteroids](../ex14-asteroids/) — tilt rotate, BOOT thrust, wrap rocks.
