# S3 / Step 02 — High-prio hog (broken)

<div class="ebook-run">

**Track:** `special-labs` · **Project:** `labs/special-labs`

<span class="ebook-badge warn">broken on purpose</span>

```text
rbdev run 03
# step switch in ex03-priority-starve.c:  ex03_step02_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/special-labs/ex03-priority-starve/steps/step-02-high-prio-hog/README.md</code></p>

</div>


**Special:** [Priority Starve](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- `hog` at priority **6**, tight loop, **no** delay.
- `ui` at priority **4**.

**Success:** LCD freezes (or barely updates); you name priority starvation.

---

## 2. Problems

- Ready higher-prio task that never blocks owns the CPU.

---

## 3. Design

```
┌──────────┐  prio6  busy forever
│ hog_task │ ─────────────────────► CPU
└──────────┘
┌──────────┐  prio4  ready but starved
│ ui_task  │ ─────────────────────► frozen LCD
└──────────┘
```

---

## 4. Build

```bash
cd labs/special-labs
# Lab: rbdev run 03  |  Step: ex03.c
rbdev run 03   # from labs/special-labs (step still in ex03.c)
```

---

## 5. How it works

Preemptive FreeRTOS always runs the highest-priority ready task. A hog at 6
that never calls a blocking API never lets UI at 4 run.

---

## 6. Next

[Step 03 — Yield fix](../step-03-yield-fix/).
