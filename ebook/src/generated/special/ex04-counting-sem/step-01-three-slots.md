# S4 / Step 01 — Three slots

<div class="ebook-run">

**Track:** `special-labs` · **Project:** `labs/special-labs`

```text
rbdev run 04
# step switch in ex04-counting-sem.c:  ex04_step01_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/special-labs/ex04-counting-sem/steps/step-01-three-slots/README.md</code></p>

</div>


**Special:** [Counting Sem](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- LCD claims a pool of **3** slots.
- Only **2** workers beep politely.

**Success:** `in use` stays ≤ 2; audio stays clean.

---

## 2. Design

Baseline under the limit — no semaphore yet.

```
┌─────────┐     ┌───────────┐
│ worker0 │ ──► │ beep_play │
└─────────┘     └───────────┘
┌─────────┐     ┌───────────┐
│ worker1 │ ──► │ beep_play │
└─────────┘     └───────────┘
```

---

## 3. Build

```bash
cd labs/special-labs
# Lab: rbdev run 04  |  Step: ex04.c
rbdev run 04   # from labs/special-labs (step still in ex04.c)
```

---

## 4. Next

[Step 02 — Oversubscribe](../step-02-oversubscribe/).
