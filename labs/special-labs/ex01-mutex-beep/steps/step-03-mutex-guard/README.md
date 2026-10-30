# S1 / Step 03 — Mutex guard

**Special:** [Mutex Beep](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- `xSemaphoreCreateMutex` guards every `beep_play`.
- Aggressive timing from step 02, but audio stays clean.
- LCD still shows hi/lo.

**Success:** overlapping *requests*, serialized *playback*.

---

## 2. Problems

- Need exclusive access without redesigning as an audio queue (Lab 02).

---

## 3. Design

| Choice | Why |
|--------|-----|
| Mutex Take / Give | Critical section around beep |
| `portMAX_DELAY` Take | Wait for the other voice to finish |

```
┌─────────┐  Take   ┌───────┐  Give
│ hi/lo   │ ──────► │ mutex │ ─────► speaker
└─────────┘         │ beep  │
                    └───────┘
```

### Sequence

```
hi               mutex            lo
 |                 |               |
 |-- Take -------->|               |
 |-- beep_play     |               |
 |-- Give -------->|               |
 |                 |<- Take -------|
 |                 |  beep_play    |
 |                 |<- Give -------|
```

---

## 4. Build

```bash
cd labs/special-labs
# Lab: .\run 01  |  Step: ex01.c
.\run 01   # from labs/special-labs (step still in ex01.c)
```

---

## 5. Sketch

```c
g_beep_mu = xSemaphoreCreateMutex();
xSemaphoreTake(g_beep_mu, portMAX_DELAY);
beep_play(...);
xSemaphoreGive(g_beep_mu);
```

### How it works

**Take** blocks until the mutex is free. Only one task runs `beep_play`.
**Give** unlocks. Always Give after Take (even if you add error paths later).

---

## 6. FreeRTOS in depth — mutex

| API | Role |
|-----|------|
| `xSemaphoreCreateMutex` | Create lock (initial “given”) |
| `xSemaphoreTake` | Enter critical section |
| `xSemaphoreGive` | Leave |

**vs binary semaphore:** mutex knows its owner (priority inheritance on many
ports). Use mutex for *shared data/hardware*; binary sem for *signal*.

**vs Lab 02 queue:** queue serializes *messages* to one audio task. Mutex
serializes *callers* of a shared function. Both valid; this lab teaches the
lock.

---

## 7. Next

[S2 — Boot ISR](../../ex02-boot-isr/).
