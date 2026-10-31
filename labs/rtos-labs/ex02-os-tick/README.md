# ex02 — OS tick

Lean FreeRTOS tick / timing lab in `labs/rtos-labs`.

## Goal

Observe how the FreeRTOS tick and delays interact with UI updates on the
workshop board (LCD + optional audio cues in the companion sources).

## Build / flash

```text
cd labs/rtos-labs
rbdev run 02
rbdev run 02 --monitor
```

Sources: `ex02.c`, `ex02.h`, `ex02_ui.c`, `ex02_ui.h`.

## Learning focus

- Tick rate vs wall-clock delays (`vTaskDelay` / tick counts)
- Keeping UI work off the critical path
- Comparing busy waits vs tick-based pacing (see also device/special
  “busy” steps)

## Checklist

- [ ] Flashes with `rbdev run 02`
- [ ] Behavior matches the comments in `ex02.c` / UI helper
- [ ] You can explain where the tick shows up in the design

## See also

- Track index: [../README.md](../README.md)
- Related stepped treatment: special-labs timer / delay themes
