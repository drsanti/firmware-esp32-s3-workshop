# Lab 00 — Instructor notes

## Doc spine

Same as Labs 01–06. Headline FreeRTOS: **`vTaskDelay` → tasks + notify**.

Point of emphasis by step: 01 = loop + delay; 02 = callback + `user`; 03 =
notify only in the click path; 04 = live vs sim is both success; 05 = physical
BOOT (bit1) on both boards, ignore PWR on ws28p; reinforce notify (no new
primitives).

## Architecture (finished lab — step 05)

```
┌──────────┐  creates  ┌─────────────┐  NotifyGive  ┌─────────────┐
│ app_main │ ────────► │  ui_task    │ ───────────► │ audio_task  │
└──────────┘           │ led_row     │              │ Take+chime  │
                       └──────┬──────┘              └─────────────┘
                              │ BOOT edge
                              ▼
                       ┌─────────────┐
                       │ bsp_buttons │
                       └─────────────┘
```

## Boards

| `BOARD` | Notes |
|---------|--------|
| `ws28p` | Waveshare 2.8" — PCM5101, capacitive touch, QMI8658; PWR+BOOT |
| `es3c28p` | Ternion — ES8311, FT6336; BOOT only; IMU may be sim in step 04 |

Students must pass matching `-DBOARD=`. Step 05 control is **BOOT**
(`bsp_buttons` bit1 / mask `2`) on both boards — not PWR.

## Step tips

| Step | Tip |
|------|-----|
| 01 | Confirm LCD backlight + counter on either board |
| 02 | Touch Lo/Mid/Hi + G/E/R; no sound → Gain / amp / wrong `BOARD` |
| 03 | Tap Beep → NotifyGive; insist beep is NOT in the callback |
| 04 | Banner live/sim is success either way; boot beep is only a ready cue |
| 05 | Press BOOT (not PWR); one LED lit; count + chime |

## Solutions

Each `step-*/main/main.c` is a **working reference**. For class, you can strip
bodies into `TODO` copies if you want students to type more.

## Common failures

- Missing `lib/librobotos_fw.a` → install from firmware-esp32-s3-dev  
- Wrong `BOARD` → rebuild with `-DBOARD=ws28p` or `-DBOARD=es3c28p`  
- Switching SKU mid-folder → `idf.py fullclean` then rebuild  
- Step 02 continuous tone → outdated beep/I2S; use current `lab_beep.h`  
- Step 05 no response → student pressing PWR on ws28p; point them at BOOT  
