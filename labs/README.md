# Labs — FreeRTOS device workshop

**`demos/demo-labs/`** = UI / widget catalog (separate one-project app).  
**`labs/`** = device workshop: build instruments, learn FreeRTOS from
problems you can see on the LCD and hear on the speaker.

## Single-project exercise apps

| App | Role |
|-----|------|
| [`device-labs/`](device-labs/) | Device curriculum **00–06** — one IDF project, switch `exNN_start()` in `main/main.c` |
| [`special-labs/`](special-labs/) | Optional FreeRTOS specials **ex01…ex06** (mutex, ISR, prio, sem, events, timer) |
| [`rtos-labs/`](rtos-labs/) | Lean FreeRTOS deep-dives + game capstones (`ex00`…`ex15`) |

```powershell
cd labs/device-labs
.\rb config --port COMx --board ws28p   # once — YOUR port from doctor (no universal default)
.\rb build
.\rb flash
# Ternion: --board es3c28p
# or: idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
```

After adding a new `exNN-*` folder or `.c` file: `.\rb reconfigure --board ws28p`
(or `idf.py -DBOARD=ws28p reconfigure`). Board flag details:
[../rbdev/README.md](../rbdev/README.md).

---

## Vision

**Title:** *Build a device → hit a real problem → learn FreeRTOS*

**6 devices** (+ Hello), same board every time.  
Flash from `device-labs`: pick the **lab** in `main/main.c`, pick the
**step** in that lab’s `exNN.c` (`exNN_stepMM_start`). Step READMEs live
under `exNN-*/steps/`.

**Board (v1):** `-DBOARD=ws28p` or `es3c28p` · ESP-IDF FreeRTOS · links
`librobotos_fw.a` for HAL/UI.

---

## Curriculum — device-labs (`ex00`…`ex06`)

| Ex | Device | FreeRTOS arc (enable each step in `exNN.c`) |
|----|--------|---------------------------------------------|
| [`ex00`](device-labs/ex00-hello-device/) | Hello Device | bring-up → two tasks + notify |
| [`ex01`](device-labs/ex01-spirit-level/) | Spirit Level | single loop fails → split sensor/UI (+ queue / notify) |
| [`ex02`](device-labs/ex02-motion-alarm/) | Motion Alarm | beep in loop → queue + audio task |
| [`ex03`](device-labs/ex03-reaction-game/) | Reaction Game | busy-wait → notify + timer + state |
| [`ex04`](device-labs/ex04-metronome/) | Metronome | delay in UI → tick task + jitter |
| [`ex05`](device-labs/ex05-vibration-scope/) | Vibration Scope | draw every sample → producer/consumer |
| [`ex06`](device-labs/ex06-task-monitor/) | Task Monitor | add snapshots screen by screen |

```
┌──────┐   ┌──────┐   ┌──────┐   ┌──────┐   ┌──────┐   ┌──────┐   ┌──────┐
│ ex00 │──►│ ex01 │──►│ ex02 │──►│ ex03 │──►│ ex04 │──►│ ex05 │──►│ ex06 │
│Hello │   │Spirit│   │Alarm │   │React │   │Metro │   │Scope │   │ Mon  │
└──────┘   └──────┘   └──────┘   └──────┘   └──────┘   └──────┘   └──────┘
```

---

## Specials — [`special-labs/`](special-labs/) (optional · 3 steps each)

One IDF project. Pick the lab in `main/main.c`, the step in that lab’s
`exNN.c`. Lean single-file counterparts also live in [`rtos-labs/`](rtos-labs/).

```powershell
cd labs/special-labs
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
```

| Ex | Folder | Headline | Also in rtos-labs |
|----|--------|----------|-------------------|
| **ex01** | [`ex01-mutex-beep`](special-labs/ex01-mutex-beep/) | Mutex | `ex06-mutex-canvas` |
| **ex02** | [`ex02-boot-isr`](special-labs/ex02-boot-isr/) | `…FromISR` notify | `ex10-boot-isr` |
| **ex03** | [`ex03-priority-starve`](special-labs/ex03-priority-starve/) | Priority / yield | `ex04-task-priority` |
| **ex04** | [`ex04-counting-sem`](special-labs/ex04-counting-sem/) | Counting semaphore | `ex07-counting-sem` |
| **ex05** | [`ex05-event-group`](special-labs/ex05-event-group/) | Event group | `ex08-event-group` |
| **ex06** | [`ex06-reload-timer`](special-labs/ex06-reload-timer/) | Auto-reload timer | `ex09-reload-timer` |

---

## Folder layout

```text
labs/
  README.md
  common/                 # helpers shared across device / special labs
  device-labs/            # ONE IDF project — devices ex00…ex06
    main/main.c
    ex00-hello-device/
      ex00.c / ex00.h     # step picker → ex00_stepMM_start()
      steps/step-NN-*/    # flashable step sources + README
    …
  special-labs/           # ONE IDF project — specials ex01…ex06 (3 steps each)
  rtos-labs/              # ONE IDF project — FreeRTOS ex00…ex15 (no ex02)
```

---

## Design principles

1. Prefer **one project / many `exNN`** for daily flash (device-labs, special-labs, rtos-labs, demo-labs).  
2. Make the problem obvious on the LCD / speaker.  
3. Put helpers in `common/` when they are truly shared.  
4. Prefer lab + step pickers over separate IDF apps per step.

## Labsheet depth standard

Step READMEs under `device-labs/exNN-*/steps/` keep the workshop labsheet
depth (deliver → problems → design → sketch → FreeRTOS in depth).  
Reference: [`device-labs/ex01-spirit-level/steps/step-01-draw-tube/README.md`](device-labs/ex01-spirit-level/steps/step-01-draw-tube/README.md).
