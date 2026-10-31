# ex03 — Priority Starve

<div class="ebook-run">

**Track:** `special-labs` · **Project:** `labs/special-labs`

```text
rbdev run 03
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/special-labs/ex03-priority-starve/README.md</code></p>

</div>


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
