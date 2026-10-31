# ex14 — IMU Asteroids

<div class="ebook-run">

**Track:** `rtos-labs` · **Project:** `labs/rtos-labs`

```text
rbdev run 14
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/rtos-labs/ex14-asteroids/README.md</code></p>

</div>


**Goal:** tilt to **rotate**, hold **BOOT** to **thrust**, destroy rocks. Screen
**wraps**. Clear a wave → next wave.

| Input | Action |
|-------|--------|
| Tilt (`acc[0]`) | Rotate ship |
| **BOOT** hold | Thrust |
| Auto | Fire shots |
| Touch / **BOOT** (title/over) | Start / retry |

```
┌──────────────────────────────────────┐
│ SCORE 01200  L3 W2         LIVE|SIM  │
│     ◇          /\                    │
│         ·  ·                         │
│   ◇◇              ◇                  │
└──────────────────────────────────────┘
```

Enable in `main/main.c` with `ex14_start()`.

| File | Holds |
|------|--------|
| `ex14.c` | Tasks + queues |
| `ex14_game.*` | Physics (wrap, split rocks, waves) |
| `ex14_gfx.*` | Paint |

```powershell
idf.py -DBOARD=ws28p reconfigure
# or
idf.py -DBOARD=es3c28p reconfigure
```

---

## FreeRTOS

Same shape as ex11–13: IMU overwrite → game → gfx; SFX for start / hit /
wave / life (auto-fire silent).

| SFX | Event |
|-----|--------|
| Chime | Start / next wave |
| Click | Rock hit |
| Alarm | Life lost / game over |

---

## Rules

- 3 lives; brief invulnerability after spawn  
- Large → medium → small on hit; small dies  
- Score: 20 / 50 / 100 by size  
- Empty field → wave++ and more rocks  

---

## Troubleshooting

| Symptom | Try |
|---------|-----|
| Spins with no IMU | `SIM` drift — expected without sensor |
| Can't move | Hold **BOOT** for thrust |
| Linker miss | `reconfigure` after adding folder |

---

## Next

[ex15 — Frogger](../ex15-frogger/) — tilt strafe, tap hop, cars + logs.
