# S2 / Step 01 — Poll BOOT

**Special:** [Boot ISR](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- Poll `bsp_buttons` bit1 (BOOT).
- Rising edge → count++ , notify audio click.

**Success:** presses increment; click on each edge.

---

## 2. Problems

- Need a working BOOT story before introducing GPIO ISR.

---

## 3. Design

| Choice | Why |
|--------|-----|
| Poll @20 ms | Simple; same as Lab 00 |
| Notify → audio | Keep beep out of UI loop body length |

```
┌─────────────┐  edge   ┌─────────┐  NotifyGive  ┌─────────────┐
│ bsp_buttons │ ──────► │ ui_task │ ───────────► │ audio_task  │
└─────────────┘         └─────────┘              └─────────────┘
```

---

## 4. Build

```bash
cd labs/special-labs
# Lab: rbdev run 02  |  Step: ex02.c
rbdev run 02   # from labs/special-labs (step still in ex02.c)
```

---

## 5. Sketch

```c
if (boot_edge(&was)) {
    presses++;
    xTaskNotifyGive(g_audio);
}
vTaskDelay(pdMS_TO_TICKS(20));
```

---

## 6. Next

[Step 02 — ISR busy](../step-02-isr-busy/).
