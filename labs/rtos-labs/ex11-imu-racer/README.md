# ex11 — IMU road racer (capstone)

**Goal:** play a tiny **endless road racer** that ties FreeRTOS pieces together —
IMU queue, game task, deferred SFX, and `rbgfx` frames.

**Tilt** the board left/right to steer. Dodge red blocks. Crash → retry.

| Input | Action |
|-------|--------|
| Tilt (IMU `acc[0]`) | Steer car |
| Touch or **BOOT** | Start / retry |
| No IMU | `imu_sim` auto-wiggle (`SIM` in HUD) |

```
┌──────────────────────────────────────┐
│ SCORE 01240              LIVE|SIM    │
├────┬────────────────────────────┬────┤
│    │  |   ##   |        |       │    │
│    │  |        |   ##   |       │    │
│    │  |   ██   |        |       │    │  car
└────┴────────────────────────────┴────┘
```

Enable in `main/main.c` with `ex11_start()`.

| File | Holds |
|------|--------|
| `ex11.c` | Tasks + queues |
| `ex11_game.*` | Physics / phases |
| `ex11_gfx.*` | Full-screen paint |

After adding this folder, reconfigure once:

```powershell
idf.py -DBOARD=ws28p reconfigure
# or
idf.py -DBOARD=es3c28p reconfigure
```

---

## FreeRTOS wiring

```
imu_task @0 ──overwrite──► q_tilt ──► game_task @1 ──► rbgfx
                                │
                                └── q_sfx ──► sfx_task ──► beep_play
```

`beep_play` **blocks** — never call it from the game/render path.

| Task | Role |
|------|------|
| `imu` | Sample / sim → latest tilt |
| `game` | Steer, spawn, collide, paint |
| `sfx` | Play chime / click / alarm |

---

## Phases

1. **TITLE** — tap/BOOT → chime + play  
2. **PLAY** — score climbs; speed ramps  
3. **CRASH** — alarm; tap/BOOT retries  

---

## Checkpoint

- [ ] Can name why SFX is a separate task  
- [ ] Can explain length-1 overwrite for tilt  
- [ ] Can steer with a real board (`LIVE`)  
- [ ] Knows BOOT or touch restarts  

---

## Troubleshooting

| Symptom | Try |
|---------|-----|
| Car drifts alone | `SIM` mode — normal without IMU |
| No sound | Audio path / gain; sfx queue not full |
| No start | Tap screen or press **BOOT** |
| Linker miss | `idf.py -DBOARD=… reconfigure` |

---

## Next

[ex12 — brick breaker](../ex12-brick-break/) — IMU paddle Breakout.
