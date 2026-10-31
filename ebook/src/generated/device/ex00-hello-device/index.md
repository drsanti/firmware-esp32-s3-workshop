# ex00 — Hello Device

<div class="ebook-run">

**Track:** `device-labs` · **Project:** `labs/device-labs`

```text
rbdev run 00
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/device-labs/ex00-hello-device/README.md</code></p>

</div>


**Lab switch:** `rbdev run 00` (or `main/main.c` → `ex00_start()`)  
**Step switch:** `ex00.c` → one `ex00_stepMM_start()`

| Start | Folder |
|-------|--------|
| `ex00_step01_start()` | `steps/step-01-lcd-hello/` |

| `ex00_step02_start()` | `steps/step-02-sound-pad/` |

| `ex00_step03_start()` | `steps/step-03-two-tasks/` |

| `ex00_step04_start()` | `steps/step-04-imu-status/` |

| `ex00_step05_start()` | `steps/step-05-boot-leds/` |

Default in `ex00.c` is the **last** step (finished device).

```powershell
# from labs/device-labs (after rbdev config --port COMx):
rbdev run 00
# step still selected in ex00-hello-device/ex00.c
```

---

**Bring up the board** → touch sound → **two tasks + notify** → IMU status →
BOOT LED chase.

Work **one step at a time**. Enable it in this lab's `exNN.c`, flash from `labs/device-labs`.

Every step README follows the workshop labsheet standard: **what to deliver →
problems → design → sketch → How the code works → FreeRTOS in depth**. Prefer
plain language. Lab 01 is the depth model; this lab is where students first
meet that bar.

## What the finished device must do

- LCD shows workshop text and (later) status / LED chase.
- Touch pad can play tones (step 02); Beep uses notify → audio task (step 03).
- IMU line shows **live** or **sim** honestly (step 04).
- Physical **BOOT** advances four LCD LEDs, counts presses, and chimes (step 05).
- Works on `ws28p` and `es3c28p` (same student `main.c`).

## Problems along the way

- Need a proven display path before sensors or sound.
- Touch callbacks that call `beep_play` block the UI path.
- Need task decoupling (notify) without a queue yet.
- Boards differ on IMU — live vs sim must both count as success.
- Need a button common to both SKUs (BOOT, not PWR).

## FreeRTOS teaching set (3)

| Feature | Role in this lab |
|---------|------------------|
| `vTaskDelay` | Pace LCD / UI loops; yield |
| `xTaskCreate` | Split UI and audio |
| Task notify (`NotifyGive` / `NotifyTake`) | Click → wake audio (no beep in callback) |

## Steps (5)

| Step | Folder | Focus | FreeRTOS |
|------|--------|-------|----------|
| 01 | [`step-01-lcd-hello`](steps/step-01-lcd-hello/) | LCD + counter | `vTaskDelay` |
| 02 | [`step-02-sound-pad`](steps/step-02-sound-pad/) | Touch + beep in callback | delay only |
| 03 | [`step-03-two-tasks`](steps/step-03-two-tasks/) | Notify → audio task | **notify** |
| 04 | [`step-04-imu-status`](steps/step-04-imu-status/) | IMU live / sim | reuse notify |
| 05 | [`step-05-boot-leds`](steps/step-05-boot-leds/) | BOOT + LED chase | poll + notify |

## Design arc

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Step 01    │ ──► │  Step 02    │ ──► │  Step 03    │ ──► │  Step 04    │ ──► │  Step 05    │
│  LCD loop   │     │ touch+beep  │     │   notify    │     │  IMU status │     │ BOOT+LEDs   │
└─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘
```

### Finished architecture (step 05)

```
┌──────────┐  creates  ┌─────────────┐  NotifyGive  ┌─────────────┐
│ app_main │ ────────► │  ui_task    │ ───────────► │ audio_task  │
└──────────┘           │ led_row     │              │ Take+chime  │
                       └──────┬──────┘              └─────────────┘
                              │ polls
                              ▼
                       ┌─────────────┐
                       │ bsp_buttons │
                       └─────────────┘
```

Helpers: [`../../common`](../../common/) (`lab_lcd`, `lab_beep`, `lab_sound_pad`,
`lab_led_row`).

## Build / flash

```powershell
cd labs/device-labs
rbdev run 00
# Lab switch: rbdev run 00 (or main/main.c -> ex00_start())
# Step: ex00-hello-device/ex00.c -> ex00_stepMM_start()
```

## Next lab

[Lab 01 — Spirit Level](../ex01-spirit-level/)
