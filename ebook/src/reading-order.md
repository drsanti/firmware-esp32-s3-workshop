# Reading order

Suggested path through the four tracks. You can jump tracks; each is its own
ESP-IDF project (`cd labs/<track>` or `rbdev run … --project labs/<track>`).

## 1. device-labs (required class path)

`ex00` → `ex06` in order. Each lab has **five steps**; flash with
`rbdev run NN`, then select `exNN_stepMM_start()` in `exNN.c`.

Broken-on-purpose steps teach FreeRTOS by contrast — finish the lab’s last
step for the “working device.”

## 2. special-labs (optional, after device 00–06)

`ex01` → `ex06`: mutex → ISR → priority → counting semaphore → event group →
timer. Three steps each. Overlaps conceptually with several **rtos-labs**
exercises — use both as stepped vs deep-dive treatments.

## 3. rtos-labs (deep-dives + games)

1. Basics: `ex00`–`ex05` (and `ex02-os-tick`)
2. Sync / ISR / timer: `ex06`–`ex10`
3. Capstone games: `ex11`–`ex15`

## 4. demo-labs (catalog, not a forced sequence)

Widgets → forms → wire → charts / viz (`ex01`–`ex17`). Open when you need a
component reference, not as a linear course.

## Tooling

One-time enroll and PATH: [rbdev User Manual — Getting started](../../rbdev/user-manual/getting-started.html).
