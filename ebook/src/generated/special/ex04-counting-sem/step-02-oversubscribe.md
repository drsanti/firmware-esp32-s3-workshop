# S4 / Step 02 — Oversubscribe (broken)

<div class="ebook-run">

**Track:** `special-labs` · **Project:** `labs/special-labs`

<span class="ebook-badge warn">broken on purpose</span>

```text
rbdev run 04
# step switch in ex04-counting-sem.c:  ex04_step02_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/special-labs/ex04-counting-sem/steps/step-02-oversubscribe/README.md</code></p>

</div>


**Special:** [Counting Sem](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- **5** workers, same “3 slots” story, **no** limiter.
- `in use` can exceed 3; audio garbles.

**Success:** you hear/see oversubscribe.

---

## 2. Design

```
┌──────────┐
│ 5 workers│ ──► beep_play (no pool)
└──────────┘
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

[Step 03 — Sem pool](../step-03-sem-pool/).
