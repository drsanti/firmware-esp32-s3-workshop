# device-labs

Lean **device workshop** app (same shape as [`rtos-labs`](../rtos-labs/)).
One ESP-IDF project; each `exNN-*` folder is one **device lab**.
Shared board HAL and `librobotos_fw.a` come from the **workshop root**.

## Two switches

1. **Lab** — `rbdev run NN` (rewrites [`main/main.c`](main/main.c)), or edit includes / `exNN_start()` by hand.
2. **Step** — `exNN.c`: uncomment one `exNN_stepMM_start()` (comment the others).

```text
main.c  →  ex00_start()  →  ex00_step01_start()   // or step02…step05
                │
                └─ ex00-hello-device/
                     ex00.c                 # step picker
                     steps/step-01-…/       # flashable step sources
                     steps/step-05-…/       # finished device (default)
```

## Layout

```text
device-labs/
  CMakeLists.txt
  main/main.c
  ex00-hello-device/
  ex01-spirit-level/
  …
  ex06-task-monitor/
```

## Build / flash

Once per PC (workshop root): `rbdev init` — see [CLASSROOM-SETUP.md](../../CLASSROOM-SETUP.md).

From this folder:

```powershell
rbdev doctor
rbdev run 00          # ex00-hello-device — select + build + flash
rbdev run 01          # ex01-spirit-level
rbdev run 06          # ex06-task-monitor
rbdev run 00 --monitor

# Or manual switch in main/main.c, then:
rbdev build
rbdev flash
rbdev monitor
```

`rbdev run NN` enables **lab** `exNN` (default step inside that lab stays whatever
`exNN.c` selects — usually the finished step). Change steps by editing
`exNN.c` as usual.

**Board SKU** — this workshop folder only (no private firmware tree):

```powershell
rbdev config --port COMx --board es3c28p   # or ws28p
rbdev reconfigure
rbdev run 00
```

See [ports/boards/README.md](../../ports/boards/README.md).

### ESP-IDF extension only (no terminal)

Open **this folder** (`labs/device-labs`) as the workspace. Chip target =
**`esp32s3`**.

1. Command Palette → **`ESP-IDF: Select Project Configuration`**
2. Choose **Waveshare ws28p** or **Ternion es3c28p** (`CMakePresets.json`)
3. **`ESP-IDF: Full Clean Project`** → **Build** → **Flash**

**Reconfigure** after adding a step folder or `.c` file, or after switching board:

```powershell
rbdev reconfigure
```

If you see `No module named 'rich_click'`, re-run `rbdev init` from the
workshop root — [../../rbdev/README.md](../../rbdev/README.md).

## Labs

| Folder | Device | Default step |
|--------|--------|--------------|
| [`ex00-hello-device`](ex00-hello-device/) | Hello Device | step-05 boot LEDs |
| [`ex01-spirit-level`](ex01-spirit-level/) | Spirit Level | step-05 LEVEL cue |
| [`ex02-motion-alarm`](ex02-motion-alarm/) | Motion Alarm | step-05 arm/disarm |
| [`ex03-reaction-game`](ex03-reaction-game/) | Reaction Game | step-05 score audio |
| [`ex04-metronome`](ex04-metronome/) | Metronome | step-05 BPM control |
| [`ex05-vibration-scope`](ex05-vibration-scope/) | Vibration Scope | step-05 drop stats |
| [`ex06-task-monitor`](ex06-task-monitor/) | Task Monitor | step-05 queue depth |

Helpers: [`../common`](../common/). FreeRTOS deep-dives: [`../rtos-labs`](../rtos-labs/). Specials: [`../special-labs`](../special-labs/).
