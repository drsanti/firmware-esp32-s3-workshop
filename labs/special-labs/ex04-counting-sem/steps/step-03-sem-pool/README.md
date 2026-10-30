# S4 / Step 03 — Sem pool

**Special:** [Counting Sem](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- `xSemaphoreCreateCounting(3, 3)`.
- 5 workers Take before beep, Give after.
- `in use` never exceeds 3; free count on LCD.

**Success:** pool enforced; audio cleaner than step 02.

---

## 2. Design

```
┌──────────┐  Take   ┌─────────────┐
│ workers  │ ──────► │ count sem   │
│   x5     │  Give   │  max 3      │
└──────────┘ ◄────── └─────────────┘
```

### Sequence

```
w0              sem             w4
 |               |               |
 |-- Take ------>|               |
 |-- beep        |               |
 |-- Give ------>|               |
 |               |<- Take -------|
```

---

## 3. Build

```bash
cd labs/special-labs
# Lab: .\run 04  |  Step: ex04.c
.\run 04   # from labs/special-labs (step still in ex04.c)
```

---

## 4. FreeRTOS in depth

| API | Role |
|-----|------|
| `xSemaphoreCreateCounting(max, init)` | Pool of tokens |
| `xSemaphoreTake` / `Give` | Acquire / release |
| `uxSemaphoreGetCount` | Tokens left (LCD) |

**vs mutex:** mutex max holders = 1 (owner). Counting sem max = N.

---

## 5. Next

[S5 — Event group](../../ex05-event-group/).
