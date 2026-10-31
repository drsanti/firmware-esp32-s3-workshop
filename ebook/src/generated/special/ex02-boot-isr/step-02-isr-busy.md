# S2 / Step 02 — ISR busy (broken)

<div class="ebook-run">

**Track:** `special-labs` · **Project:** `labs/special-labs`

<span class="ebook-badge warn">broken on purpose</span>

```text
rbdev run 02
# step switch in ex02-boot-isr.c:  ex02_step02_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/special-labs/ex02-boot-isr/steps/step-02-isr-busy/README.md</code></p>

</div>


**Special:** [Boot ISR](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- GPIO0 NEGEDGE ISR increments a hit counter **and busy-waits**.
- UI tries to tick every 100 ms.

**Success:** while ISR spins, `ui ticks` stalls — system feels frozen.

---

## 2. Problems

- Long work in ISR blocks scheduling (and other interrupts).

---

## 3. Design

| Choice | Why |
|--------|-----|
| Real `gpio_isr_handler_add` | Honest ISR path |
| Busy loop in ISR | Visible freeze (safer demo than calling `beep_play` in ISR) |

```
┌──────────┐  edge  ┌──────────────┐
│ BOOT GPIO│ ─────► │ boot_isr     │
└──────────┘        │ BUSY LOOP BAD│
                    └──────────────┘
                           │
                           ▼
                    ui_task starved
```

---

## 4. Build

```bash
cd labs/special-labs
# Lab: rbdev run 02  |  Step: ex02.c
rbdev run 02   # from labs/special-labs (step still in ex02.c)
```

Press/hold BOOT and watch `ui ticks` pause.

---

## 5. Sketch

```c
static void IRAM_ATTR boot_isr(void *arg)
{
    g_isr_hits++;
    for (i = 0; i < 800000; i++) { } /* BAD */
}
```

### How it works

While the ISR runs, the scheduler does not run your UI task. That is why
`ui ticks` freezes. Step 03 keeps the ISR tiny.

---

## 6. FreeRTOS — ISR rules (preview)

Allowed (typical): set flag, give sem/`NotifyGiveFromISR`, queue send FromISR.  
Not allowed here: `vTaskDelay`, `beep_play`, long loops, LCD draws.

---

## 7. Next

[Step 03 — ISR notify](../step-03-isr-notify/).
