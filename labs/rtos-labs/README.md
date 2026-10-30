# rtos-labs

Lean **FreeRTOS exercises** app inside the workshop. One ESP-IDF project;
each `exNN-*` folder is one exercise (`.c` / `.h`). Shared board HAL and
`librobotos_fw.a` come from the **workshop root** (not copied here).

## Layout

```text
rtos-labs/
  CMakeLists.txt
  main/main.c           # app_main — picks which exercise to run
  common/               # shared helpers (ex_terms, …)
  ex00-task-creation/
  ex01-task-affinity/
  ex02-os-tick/
  ex03-imu-pipeline/
  ex04-task-priority/
  ex05-task-suspend/
  ex06-mutex-canvas/
  ex07-counting-sem/
  ex08-event-group/
  ex09-reload-timer/
  ex10-boot-isr/
  ex11-imu-racer/
  ex12-brick-break/
  ex13-space-shoot/
  ex14-asteroids/
  ex15-frogger/
  …
```

Switch exercise with **`run NN`** (rewrites `main/main.c`), or edit the
includes / `exNN_start()` calls by hand.

## Build / flash

Preferred (`rb` — see [../../rbdev/README.md](../../rbdev/README.md)):

```powershell
# ESP-IDF PowerShell, from this folder
# Once per window (avoids "not digitally signed"):
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass

.\rb doctor
.\rb config --port COMx --board ws28p   # once — YOUR port from doctor

.\run 00          # ex00-task-creation — select + build + flash
.\run 02          # ex02-os-tick
.\run 15          # ex15-frogger
.\run 00 monitor
```

If you see `No module named 'rich_click'`, repair the ESP-IDF Python venv
(`install.bat` / `install.sh` in your IDF tree) — see
[../../rbdev/README.md](../../rbdev/README.md) and the root
[README troubleshooting](../../README.md#troubleshooting).

Plain `idf.py`:

```powershell
. <IDF_PATH>\export.ps1
cd labs/rtos-labs
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
# or
idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx monitor
```

**Reconfigure when sources change.** CMake GLOB does **not** auto-refresh.
Run this after adding a new `exNN-*` folder **or** a new `.c` file inside one
(e.g. `ex06_canvas.c`), **before** `build`:

```powershell
idf.py -DBOARD=ws28p reconfigure
# or (same board you use for build/flash)
idf.py -DBOARD=es3c28p reconfigure
```

If you skip it, the linker usually fails with `undefined reference` to symbols
in the new `.c` file.

## Exercises

| Folder | Topic |
|--------|--------|
| [`ex00-task-creation`](ex00-task-creation/) | Create tasks, scheduler, `vTaskDelay`, observe core via `xPortGetCoreID` |
| [`ex01-task-affinity`](ex01-task-affinity/) | `xTaskCreatePinnedToCore` — pin to core 0 / core 1 |
| [`ex02-os-tick`](ex02-os-tick/) | OS tick + tick hook — LITE vs BUSY (ISR must stay short) |
| [`ex03-imu-pipeline`](ex03-imu-pipeline/) | Cross-core queue: IMU @0 → `|a|` @1 → instrument UI |
| [`ex04-task-priority`](ex04-task-priority/) | Prio 6/5/4 — YIELD vs HOG starvation on one core |
| [`ex05-task-suspend`](ex05-task-suspend/) | `vTaskSuspend` / `vTaskResume` via two toggle buttons |
| [`ex06-mutex-canvas`](ex06-mutex-canvas/) | Shared canvas — mutex OFF (race) vs ON (clean strokes) |
| [`ex07-counting-sem`](ex07-counting-sem/) | Counting sem pool — 4 workers / 2 slots, OFF oversubscribe |
| [`ex08-event-group`](ex08-event-group/) | Event group — WaitBits AND vs OR on bits A/B |
| [`ex09-reload-timer`](ex09-reload-timer/) | Auto-reload timer vs `vTaskDelay` beat (500 ms) |
| [`ex10-boot-isr`](ex10-boot-isr/) | BOOT GPIO — poll vs `vTaskNotifyGiveFromISR` |
| [`ex11-imu-racer`](ex11-imu-racer/) | Capstone — IMU tilt racer, gfx + queued SFX |
| [`ex12-brick-break`](ex12-brick-break/) | Capstone — IMU Breakout, bricks + paddle |
| [`ex13-space-shoot`](ex13-space-shoot/) | Capstone — IMU space shooter, auto-fire |
| [`ex14-asteroids`](ex14-asteroids/) | Capstone — IMU Asteroids, wrap + waves |
| [`ex15-frogger`](ex15-frogger/) | Capstone — IMU Frogger, cars + logs |
