# Demo 15 — Live IMU charts

Two independent `rb_chart` panes fed by **`bsp_imu_read`** (QMI8658 on
**ws28p**):

| Pane | Mode | Channels | Units |
|------|------|----------|-------|
| Accel | `SCROLL` | ax ay az | m/s² |
| Gyro | `HISTORY` | gx gy gz | rad/s |

Tilt / rotate the board to see the traces move. HISTORY grows
left→right, then shifts.

**No IMU** (e.g. **es3c28p**): panes keep running on sine + noise
(`ports/imu_sim.h`); titles show `(sim)`.

## Build / flash

```bash
cd demos/demo-labs
.\run 15
# Ternion: .\rb config --board es3c28p then .\run 15
```

## Success

With a real IMU and the board flat, `az` sits near ~+9.8; tipping moves
`ax`/`ay`. Gyro spikes when you twist. On sim boards, traces animate
continuously without hardware motion.

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex15.h"`
2. `ex15_start();`

Or: `.\run 15` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
