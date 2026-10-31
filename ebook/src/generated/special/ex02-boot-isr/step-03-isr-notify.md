# S2 / Step 03 — ISR notify

<div class="ebook-run">

**Track:** `special-labs` · **Project:** `labs/special-labs`

<span class="ebook-badge ok">finished device</span>

```text
rbdev run 02
# step switch in ex02-boot-isr.c:  ex02_step03_start
```

<p class="ebook-source"><strong>Source:</strong> <code>labs/special-labs/ex02-boot-isr/steps/step-03-isr-notify/README.md</code></p>

</div>


**Special:** [Boot ISR](../../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. Deliver

- ISR only `vTaskNotifyGiveFromISR` (+ yield if needed).
- UI task debounces, updates count, notifies audio.

**Success:** BOOT feels snappy; `ui` path stays responsive.

---

## 2. Problems

- Need deferred work without polling forever (optional) and without ISR bloat.

---

## 3. Design

| Choice | Why |
|--------|-----|
| `NotifyGiveFromISR` | Tiny ISR wake |
| Debounce in task | Needs time APIs / logic unfit for ISR |

```
BOOT ──► isr ──NotifyGiveFromISR──► ui_task ──NotifyGive──► audio
```

### Sequence

```
GPIO           isr             ui              audio
 |              |               |                |
 |-- edge ----->|               |                |
 |              |-- FromISR --->|                |
 |              |               |-- debounce     |
 |              |               |-- LCD          |
 |              |               |-- Give ------->|
 |              |               |                |-- beep
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
vTaskNotifyGiveFromISR(g_ui, &hp);
if (hp == pdTRUE) {
    portYIELD_FROM_ISR();
}
```

### How it works

**FromISR** variants are the only notify/queue/sem APIs safe in an ISR.
`pxHigherPriorityTaskWoken` + `portYIELD_FROM_ISR` switches sooner to the
woken task when it outranks the interrupted one.

---

## 6. FreeRTOS in depth — FromISR

| API | Role |
|-----|------|
| `vTaskNotifyGiveFromISR` | Increment notify count from ISR |
| `portYIELD_FROM_ISR` | Request switch if higher-prio task woken |
| `ulTaskNotifyTake` in task | Wait for the ISR signal |

Mental model: **ISR shouts; task does the work.**

---

## 7. Next

[S3 — Priority starve](../../ex03-priority-starve/).
