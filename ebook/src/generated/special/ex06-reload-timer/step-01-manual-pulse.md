# S6 / Step 01 — Manual pulse

<div class="ebook-run">

**Track:** `special-labs` · **Project:** `labs/special-labs`

```text
rbdev run 06
# step switch in ex06-reload-timer.c:  ex06_step01_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/special-labs/ex06-reload-timer/steps/step-01-manual-pulse/README.md</code></p>

</div>


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
# Lab: rbdev run 06  |  Step: ex06.c
rbdev run 06   # from labs/special-labs (step still in ex06.c)
```

---

## 4. Next

[Step 02 — Delay beat](../step-02-delay-beat/).
