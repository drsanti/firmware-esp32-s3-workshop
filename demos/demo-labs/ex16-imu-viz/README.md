# Demo 16 — IMU visualization

Composes the spirit-level ball, numeric readouts, and one accel chart
from **`bsp_imu_read`** (QMI8658 on **ws28p**):

| Region | Widget | Role |
|--------|--------|------|
| Top-left | `rb_level` | Bubble from ax/ay (tilt) |
| Top-right | `rb_imu_readout` | Bipolar accel bars + gyro text |
| Bottom | `rb_chart` HISTORY | ax ay az ring buffer |

Tilt the board — the green ball moves; tip further and the bars /
trace follow.

**No IMU** (e.g. **es3c28p**): sine + noise via `ports/imu_sim.h` —
ball/bars/chart animate; status shows `IMU sim`, title `(sim)`.

## Build / flash

```bash
cd demos/demo-labs
.\run 16
# Ternion: .\rb config --board es3c28p then .\run 16
```

## Success

Board flat → ball near center, `az` ≈ +9.8. Tip → ball slides, `ax`/`ay`
bars grow, HISTORY fills left→right then shifts. On sim boards, the
bubble orbits from the synthetic ax/ay sines.

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex16.h"`
2. `ex16_start();`

Or: `.\run 16` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
