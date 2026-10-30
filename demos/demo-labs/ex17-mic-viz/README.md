# Demo 17 — realtime mic dashboard

Wireframe layout: stacked **VU** + **PLOT** on the left (~80%),
full-height vertical **GAIN** on the right (~20%).

```
┌────────────────────┬──────┐
│ A  rb_vu (dBFS)    │      │
│    -xx dBFS  PCM n │  B   │
├────────────────────┤ GAIN │
│ C  SCROLL plot     │  (V) │
│                    │      │
└────────────────────┴──────┘
```

| Cell | Widget | Role |
|------|--------|------|
| A | `rb_vu` DBFS −60…0 | Live level (bar + caption dB) |
|   | labels | Peak **dBFS** and **PCM** (int16 peak after dig gain) |
| B | `rb_slider` vertical | PGA → `bsp_mic_gain` |
| C | `rb_chart` SCROLL | Waveform (16 cols / 64 PCM), display-AGC so speech fills ±1 |

Feed paced by I2S (~4 ms / 64 frames @ 16 kHz). `mic_sim` when
`BOARD_CAP_MIC` is absent.

## Gain tuning

Three stages (do not max all at once):

| Stage | Where | Default |
|-------|--------|---------|
| ADC digital | `0x17` via unmute | **0 dB** (`0xBF`) |
| PGA scale | GAIN slider → `0x16` (0…+42 dB) | **45%** (~+18 dB) |
| Soft dig | `DIG_GAIN` in `main.c` | **×4** |

Target: silence ≈ 0–5% VU, speech ≈ 30–60%, yell clips near 100%.
Drag GAIN first; only raise `DIG_GAIN` if the slider top is still quiet.

## Build / flash

```bash
cd demos/demo-labs
.\rb flash --board es3c28p   # after .\run 17 / config
# SIM path uses ws28p mic_sim — default .\run 17
```

## Success

- Speak → VU + strip move in near-realtime  
- Drag **GAIN** → louder/softer  
- No host bridge required  

---

## device-labs-style enable

In `demos/demo-labs/main/main.c`:

1. `#include "ex17.h"`
2. `ex17_start();`

Or: `.\run 17` from `demos/demo-labs` (rewrites `main/main.c`). Run `.\rb reconfigure` if you just added this folder.
