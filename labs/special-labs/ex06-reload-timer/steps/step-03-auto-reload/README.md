# S6 / Step 03 — Auto-reload

**Special:** [Reload Timer](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- `xTimerCreate(..., pdTRUE, …)` auto-reload every 500 ms.
- Callback only `xTaskNotifyGive`s UI.
- UI flashes beat **and** polls BOOT with a short timeout.

**Success:** steady heartbeat; BOOT count stays responsive.

---

## 2. Design

```
┌─────────────┐  pdTRUE   ┌─────────────┐  Notify  ┌─────────┐
│   xTimer    │ ────────► │  beat_cb    │ ───────► │ ui_task │
└─────────────┘           └─────────────┘          └─────────┘
```

### Sequence

```
timer            cb               ui
 |                |                |
 |-- fire ------->|                |
 |                |-- NotifyGive ->|
 |                |                |-- flash
 |-- fire ------->|                |
 |                |-- NotifyGive ->|
```

---

## 3. Build

```bash
cd labs/special-labs
# Lab: rbdev run 06  |  Step: ex06.c
rbdev run 06   # from labs/special-labs (step still in ex06.c)
```

---

## 4. FreeRTOS in depth

| API | Role |
|-----|------|
| `xTimerCreate(..., pdTRUE, …)` | Auto-reload (periodic) |
| `xTimerCreate(..., pdFALSE, …)` | One-shot (Lab 03) |
| `xTimerStart` | Arm |
| Callback | Timer daemon — notify only |

**vs Lab 04 `vTaskDelayUntil`:** task owns the period. **Timer:** system
daemon owns the period; any task can wait on notify.

---

## 5. Next

Specials S1–S6 complete — see [labs/README.md](../../README.md).
