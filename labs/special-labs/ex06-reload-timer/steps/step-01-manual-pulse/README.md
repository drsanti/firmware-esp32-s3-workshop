# S6 / Step 01 — Manual pulse

**Special:** [Reload Timer](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- BOOT edge flashes `* BEAT *` once.

**Success:** you control the pulse by hand.

---

## 2. Design

```
┌─────────────┐  edge   ┌─────────────┐
│ BOOT poll   │ ──────► │ LCD flash   │
└─────────────┘         └─────────────┘
```

---

## 3. Build

```bash
cd labs/special-labs
# Lab: .\run 06  |  Step: ex06.c
.\run 06   # from labs/special-labs (step still in ex06.c)
```

---

## 4. Next

[Step 02 — Delay beat](../step-02-delay-beat/).
