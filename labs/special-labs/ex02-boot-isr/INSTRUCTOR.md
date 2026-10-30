# ex02 — Instructor notes

## Headline

ISRs must be **short**. Defer LCD / `beep_play` to a task via
`vTaskNotifyGiveFromISR`.

## Architecture (finished)

```
┌─────────────┐  edge    ┌─────────────┐  NotifyGiveFromISR
│ BOOT GPIO0  │ ───────► │  boot_isr   │ ──────────────────► ui_task
└─────────────┘          │  (short!)   │                     LCD+beep
                         └─────────────┘
```

## Tips

| Step | Tip |
|------|-----|
| 01 | Same poll pattern as Lab 00 step 05 |
| 02 | Hold BOOT — UI counter freezes / feels dead during ISR spin |
| 03 | Debounce in the **task**, not the ISR |

## Common failures

- `beep_play` or `vTaskDelay` inside ISR → abort / deadlock risk  
- Forgetting `portYIELD_FROM_ISR` when `pxHigherPriorityTaskWoken`  
- Wrong edge / no `gpio_install_isr_service`  
