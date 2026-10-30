# ex06 — Auto-Reload Timer

**Manual pulse** → heartbeat via `vTaskDelay` in UI → learn an **auto-reload
software timer**.

Optional track. **3 steps.** Boards: `ws28p` / `es3c28p`.

Contrast: Lab 03 = one-shot timer; Lab 04 = `vTaskDelayUntil` in a task.
Here the **timer service** fires periodically.

## What the finished device must do

- LCD heartbeat flash on a steady period.
- UI stays free for other work (BOOT count).
- Period comes from an auto-reload `xTimer`.

## FreeRTOS teaching set (1)

| Feature | Role |
|---------|------|
| Auto-reload software timer (`pdTRUE`) | Periodic callback without a tick task |

## Steps (3)

| Step | Folder | Focus |
|------|--------|-------|
| 01 | [`step-01-manual-pulse`](steps/step-01-manual-pulse/) | BOOT pulses beat |
| 02 | [`step-02-delay-beat`](steps/step-02-delay-beat/) | UI `vTaskDelay` beat (**awkward**) |
| 03 | [`step-03-auto-reload`](steps/step-03-auto-reload/) | Auto-reload timer |

## Design arc

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Step 01    │ ──► │  Step 02    │ ──► │  Step 03    │
│ manual BOOT │     │ Delay in UI │     │ auto-reload │
│ pulse       │     │ UI blocked  │     │ UI free     │
└─────────────┘     └─────────────┘     └─────────────┘
```

## Prior

[S5 — Event Group](../ex05-event-group/) · [labs/README.md](../README.md)
