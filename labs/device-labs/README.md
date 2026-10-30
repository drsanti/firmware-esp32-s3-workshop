# device-labs

Lean **device workshop** app (same shape as [`rtos-labs`](../rtos-labs/)).
One ESP-IDF project; each `exNN-*` folder is one **device lab**.
Shared board HAL and `librobotos_fw.a` come from the **workshop root**.

## Two switches

1. **Lab** — `.\run NN` (rewrites [`main/main.c`](main/main.c)), or edit includes / `exNN_start()` by hand.
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

Preferred (`rb` — see [../../rbdev/README.md](../../rbdev/README.md)):

```powershell
# ESP-IDF PowerShell, from this folder
.\rb doctor
.\rb config --port COMx --board ws28p   # once — YOUR port from doctor; saves ../../.rbdev.toml

# Pick a lab, build, and flash (rewrites main/main.c for you):
.\run 00          # ex00-hello-device
.\run 01          # ex01-spirit-level
.\run 06          # ex06-task-monitor
.\run 00 monitor

# Or manual switch in main/main.c, then:
.\rb build
.\rb flash
.\rb monitor
```

`.\run NN` enables **lab** `exNN` (default step inside that lab stays whatever
`exNN.c` selects — usually the finished step). Change steps by editing
`exNN.c` as usual.

Ternion / one-shot board override:

```powershell
.\rb config --port COMx --board es3c28p
.\run 00
```

Plain `idf.py` (same boards):

```powershell
. <IDF_PATH>\export.ps1
cd labs/device-labs
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
# Ternion: -DBOARD=es3c28p
```

**Reconfigure** after adding a step folder or `.c` file, or after switching board:

```powershell
.\rb reconfigure --board ws28p
# or: idf.py -DBOARD=ws28p reconfigure
```

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
