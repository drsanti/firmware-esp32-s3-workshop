# S5 / Step 01 — Checklist

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
# Lab: .\run 05  |  Step: ex05.c
.\run 05   # from labs/special-labs (step still in ex05.c)
```

---

## 4. Next

[Step 02 — One-bit arm](../step-02-one-bit-arm/).
