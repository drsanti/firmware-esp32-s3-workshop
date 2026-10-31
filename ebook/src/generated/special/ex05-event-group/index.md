# ex05 — Event Group

<div class="ebook-run">

**Track:** `special-labs` · **Project:** `labs/special-labs`

```text
rbdev run 05
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/special-labs/ex05-event-group/README.md</code></p>

</div>


**Arm checklist** → arm on one bit only (false arm) → learn
**`xEventGroupWaitBits`**.

Optional track. **3 steps.** Boards: `ws28p` / `es3c28p`.

## What the finished device must do

- Bits: **BOOT** pressed edge, **STILL** (low horizontal accel).
- Device arms only when **both** are true (within a wait).
- LCD shows checklist + ARMED / WAITING.

## FreeRTOS teaching set (1)

| Feature | Role |
|---------|------|
| Event group (`SetBits` / `WaitBits`) | Wait for several conditions |

## Steps (3)

| Step | Folder | Focus |
|------|--------|-------|
| 01 | [`step-01-checklist`](steps/step-01-checklist/) | Show BOOT / STILL flags |
| 02 | [`step-02-one-bit-arm`](steps/step-02-one-bit-arm/) | Arm on BOOT only (**broken**) |
| 03 | [`step-03-wait-bits`](steps/step-03-wait-bits/) | WaitBits both |

## Design arc

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  Step 01    │ ──► │  Step 02    │ ──► │  Step 03    │
│ checklist   │     │ BOOT only   │     │ both bits   │
│ display     │     │ false arm   │     │ WaitBits OK │
└─────────────┘     └─────────────┘     └─────────────┘
```

## Prior

[S4 — Counting Sem](../ex04-counting-sem/) · [labs/README.md](../README.md)
