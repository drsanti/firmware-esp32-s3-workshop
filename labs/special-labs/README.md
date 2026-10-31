# special-labs

Optional **FreeRTOS specials** — one ESP-IDF project, six labs with
**3 steps** each (same two-switch pattern as [`device-labs`](../device-labs/)).

Lean single-file counterparts also live in [`rtos-labs`](../rtos-labs/).

## Two switches

1. **Lab** — `rbdev run NN` (rewrites [`main/main.c`](main/main.c)), or edit includes / `exNN_start()` by hand.
2. **Step** — `exNN.c`: uncomment one `exNN_stepMM_start()`.

## Build / flash

Once per PC (workshop root): `rbdev init` — see [CLASSROOM-SETUP.md](../../CLASSROOM-SETUP.md).

From this folder:

```powershell
rbdev doctor
rbdev run 01          # ex01-mutex-beep — select lab + build + flash
rbdev run 02          # ex02-boot-isr
rbdev run 01 --monitor
```

After adding a step folder or `.c` file: `rbdev reconfigure --board ws28p`.

If you see `No module named 'rich_click'`, re-run `rbdev init` from the
workshop root — [../../rbdev/README.md](../../rbdev/README.md).

## Labs

| Folder | Headline | Also in rtos-labs |
|--------|----------|-------------------|
| [`ex01-mutex-beep`](ex01-mutex-beep/) | Mutex around `beep_play` | `ex06-mutex-canvas` |
| [`ex02-boot-isr`](ex02-boot-isr/) | BOOT ISR + `…FromISR` notify | `ex10-boot-isr` |
| [`ex03-priority-starve`](ex03-priority-starve/) | Priority / yield | `ex04-task-priority` |
| [`ex04-counting-sem`](ex04-counting-sem/) | Counting semaphore pool | `ex07-counting-sem` |
| [`ex05-event-group`](ex05-event-group/) | Event group WaitBits | `ex08-event-group` |
| [`ex06-reload-timer`](ex06-reload-timer/) | Auto-reload software timer | `ex09-reload-timer` |

Helpers: [`../common`](../common/). Curriculum index: [`../README.md`](../README.md).
