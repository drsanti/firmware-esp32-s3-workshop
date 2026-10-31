# ex02 — Boot ISR

<div class="ebook-run">

**Track:** `special-labs` · **Project:** `labs/special-labs`

```text
rbdev run 02
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/special-labs/ex02-boot-isr/README.md</code></p>

</div>


**Poll BOOT** → **busy work in ISR** freezes the system → learn
**`…FromISR` notify**.

Optional track. **3 steps.** Boards: `ws28p` / `es3c28p` (BOOT = GPIO0).

## What the finished device must do

- BOOT press counted on LCD + soft chime.
- GPIO ISR stays **short** — only notifies a task.
- Task does LCD + `beep_play`.

## FreeRTOS teaching set (1)

| Feature | Role |
|---------|------|
| `vTaskNotifyGiveFromISR` (+ yield) | Wake task from GPIO ISR |

## Steps (3)

| Step | Folder | Focus |
|------|--------|-------|
| 01 | [`step-01-poll-boot`](steps/step-01-poll-boot/) | Poll `bsp_buttons` (baseline) |
| 02 | [`step-02-isr-busy`](steps/step-02-isr-busy/) | Long work in ISR (**broken**) |
| 03 | [`step-03-isr-notify`](steps/step-03-isr-notify/) | ISR notify → task work |

## Design arc

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Step 01    │ ──► │  Step 02    │ ──► │  Step 03    │
│ poll BOOT   │     │ ISR busy BAD│     │ FromISR     │
│ + notify    │     │ UI freezes  │     │ notify OK   │
└─────────────┘     └─────────────┘     └─────────────┘
```

## Prior

[S1 — Mutex Beep](../ex01-mutex-beep/) · [labs/README.md](../README.md)
