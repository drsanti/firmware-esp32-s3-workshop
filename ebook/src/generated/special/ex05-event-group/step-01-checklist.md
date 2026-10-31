# S5 / Step 01 — Checklist

<div class="ebook-run">

**Track:** `special-labs` · **Project:** `labs/special-labs`

```text
rbdev run 05
# step switch in ex05-event-group.c:  ex05_step01_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/special-labs/ex05-event-group/steps/step-01-checklist/README.md</code></p>

</div>


**Special:** [Event Group](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- LCD lines for **BOOT** (latched on edge) and **STILL** (low horiz accel).

**Success:** press BOOT → BOOT yes; rest board → STILL yes; shake → STILL -.

---

## 2. Design

```
┌─────────────┐     ┌─────────────┐
│ bsp_buttons │ ──► │ BOOT line   │
└─────────────┘     └─────────────┘
┌─────────────┐     ┌─────────────┐
│ IMU / sim   │ ──► │ STILL line  │
└─────────────┘     └─────────────┘
```

---

## 3. Build

```bash
cd labs/special-labs
# Lab: rbdev run 05  |  Step: ex05.c
rbdev run 05   # from labs/special-labs (step still in ex05.c)
```

---

## 4. Next

[Step 02 — One-bit arm](../step-02-one-bit-arm/).
