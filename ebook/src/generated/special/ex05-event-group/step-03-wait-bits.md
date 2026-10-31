# S5 / Step 03 — WaitBits

<div class="ebook-run">

**Track:** `special-labs` · **Project:** `labs/special-labs`

<span class="ebook-badge ok">finished device</span>

```text
rbdev run 05
# step switch in ex05-event-group.c:  ex05_step03_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/special-labs/ex05-event-group/steps/step-03-wait-bits/README.md</code></p>

</div>


**Special:** [Event Group](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- Sense task sets/clears **BOOT** and **STILL** bits.
- UI `WaitBits` for **both** (`pdTRUE` wait-for-all), then ARMED + chime.

**Success:** BOOT while still → ARMED; BOOT while shaking → stays WAITING.

---

## 2. Design

```
┌─────────────┐  SetBits   ┌─────────────┐  WaitBits  ┌─────────┐
│ sense_task  │ ─────────► │ event group │ ─────────► │ ui_task │
└─────────────┘            └─────────────┘            └─────────┘
```

### Sequence

```
sense            bits             ui
 |                |                |
 |-- Set BOOT --->|                |
 |-- Set STILL -->|                |
 |                |<- Wait both ---|
 |                |                |-- ARMED
```

---

## 3. Build

```bash
cd labs/special-labs
# Lab: rbdev run 05  |  Step: ex05.c
rbdev run 05   # from labs/special-labs (step still in ex05.c)
```

Rest the board, then press BOOT.

---

## 4. FreeRTOS in depth

| API | Role |
|-----|------|
| `xEventGroupCreate` | Bitfield object |
| `xEventGroupSetBits` / `ClearBits` | Publish conditions |
| `xEventGroupWaitBits(..., waitAll=pdTRUE)` | Block until all bits in mask |

`clearOnExit=pdTRUE` clears the waited bits when the wait succeeds (good for
one-shot arm).

---

## 5. Next

[S6 — Reload timer](../../ex06-reload-timer/).
