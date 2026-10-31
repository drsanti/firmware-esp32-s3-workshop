# S6 / Step 02 — Delay beat (awkward)

<div class="ebook-run">

**Track:** `special-labs` · **Project:** `labs/special-labs`

```text
rbdev run 06
# step switch in ex06-reload-timer.c:  ex06_step02_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/special-labs/ex06-reload-timer/steps/step-02-delay-beat/README.md</code></p>

</div>


**Special:** [Reload Timer](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- Automatic beat using `vTaskDelay(500)` in the **same** UI loop.
- BOOT count only checked between beats.

**Success:** BOOT feels laggy / missed while delaying.

---

## 2. Design

```
┌─────────────┐  Delay 500   ┌─────────────┐
│  ui loop    │ ───────────► │ blocks BOOT │
│ flash beat  │              │ polling     │
└─────────────┘              └─────────────┘
```

---

## 3. Build

```bash
cd labs/special-labs
# Lab: rbdev run 06  |  Step: ex06.c
rbdev run 06   # from labs/special-labs (step still in ex06.c)
```

---

## 4. Next

[Step 03 — Auto-reload](../step-03-auto-reload/).
