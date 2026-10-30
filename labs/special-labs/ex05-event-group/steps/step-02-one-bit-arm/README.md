# S5 / Step 02 — One-bit arm (broken)

**Special:** [Event Group](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- Arms and chimes on **BOOT only**.
- STILL is displayed but ignored.

**Success:** shake + BOOT → still shows ARMED (false arm).

---

## 2. Design

```
BOOT edge ──► ARMED   (STILL ignored)
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

[Step 03 — WaitBits](../step-03-wait-bits/).
