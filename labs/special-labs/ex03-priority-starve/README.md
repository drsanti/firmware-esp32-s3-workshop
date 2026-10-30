# ex03 — Priority Starve

**UI + worker** → **high-prio busy hog** freezes the UI → learn **priority /
yielding**.

Optional track. **3 steps.** Boards: `ws28p` / `es3c28p`.

## What the finished device must do

- LCD counter keeps ticking.
- A background “hog” may run hard work — but must not permanently starve UI.
- Fix by lowering hog priority and/or yielding (`vTaskDelay`).

## FreeRTOS teaching set (1)

| Feature | Role |
|---------|------|
| Task priority + preemption | Who runs when both want the CPU |

## Steps (3)

| Step | Folder | Focus |
|------|--------|-------|
| 01 | [`step-01-ui-worker`](steps/step-01-ui-worker/) | UI + polite worker |
| 02 | [`step-02-high-prio-hog`](steps/step-02-high-prio-hog/) | High-prio busy loop (**broken**) |
| 03 | [`step-03-yield-fix`](steps/step-03-yield-fix/) | Lower prio + yield |

## Design arc

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Step 01    │ ──► │  Step 02    │ ──► │  Step 03    │
│ UI + worker │     │ hog prio BAD│     │ yield / prio│
│ both live   │     │ UI frozen   │     │ UI alive    │
└─────────────┘     └─────────────┘     └─────────────┘
```

## Prior

[S2 — Boot ISR](../ex02-boot-isr/) · [labs/README.md](../README.md)
