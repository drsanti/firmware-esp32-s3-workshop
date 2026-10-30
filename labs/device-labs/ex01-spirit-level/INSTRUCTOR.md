# Lab 01 — Instructor notes

## Doc spine

Every step README leads with **requirements → problems → design**, then sketch,
then a **FreeRTOS in depth** section (why this API, argument meanings, rejected
alternatives) — workshop standard in [`labs/README.md`](../README.md).

## Architecture (finished lab — step 05)

Capstone: one overwrite queue for latest tilt; notify only when entering LEVEL.

```
┌─────────────┐  Overwrite   ┌─────────┐  Receive   ┌─────────┐  NotifyGive  ┌─────────────┐
│sensor_task  │ ───────────► │ tilt x1 │ ─────────► │ ui_task │ ───────────► │ audio_task  │
└─────────────┘              └─────────┘            └─────────┘              └─────────────┘
```

## Step 01

- Point at the bubble walking and LEVEL/TILT flip.
- Walk the README FreeRTOS section: why delay (pace + yield), ticks,
  `pdMS_TO_TICKS`, why not `vTaskDelayUntil` yet.
- Ask: “What breaks if we delete the delay?” → CPU spin, uneven motion.

## Step 02

- Tilt the board on `ws28p` (live) or watch sim on `es3c28p` — both OK.
- Stress the **single-loop contract**: no `beep_play` in this loop.

## Step 03

- Have students enter LEVEL and **watch the bubble freeze** during the chime.
- Ask: “Did FreeRTOS fail?” → No — the **architecture** put blocking audio in
  the only UI/sense loop.

## Step 04

- Contrast with step 03: bubble stays smooth (no beep in this step).
- Walk overwrite queue: why length 1, why not notify for ax/ay.
- Preview step 05: LEVEL event → notify → audio, queue still carries tilt.

## Step 05 (capstone)

- Same chime as step 03 — bubble must **keep moving** during play.
- Ask: “What carries tilt? What carries the LEVEL cue?” → queue vs notify.
- Stress rising edge (`level && !was_level`), not “while level.”
- Optional demo: temporarily put `beep_play` back in `ui_task` → freeze returns
  (then undo).

## Boards

Same as Lab 00: `-DBOARD=ws28p` or `es3c28p`.

## Step tips

| Step | Tip |
|------|-----|
| 01 | Face of the device only |
| 02 | IMU/sim → bubble; name the contract |
| 03 | Feel the freeze; name the contract break |
| 04 | Queue + two tasks; why Overwrite |
| 05 | Notify + audio; stream vs event |

## Common failures

- Missing `librobotos_fw.a` → install robotos-fw release  
- Wrong `BOARD` → rebuild with matching `-DBOARD=`  
- Chime spam → missing edge latch (`was_level`)  
- Smooth + silent → not entering LEVEL; leave and re-enter  
