# Lab 06 / Step 01 — Name face

**Device lab:** [Task Monitor](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`  
**Depends on:** none (first step)

---

## 1. What this step must deliver

- LCD title **Task Mon**.
- Three **static** task-name rows (`ui`, `load_a`, `load_b`).
- No kernel snapshot yet — only the face.

**Success:** you see a compact three-row list layout.

---

## 2. Problems this step faces

- Need a readable monitor face before learning introspection APIs.
- LCD has only four lines — title + three rows.

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| `mon_*` helper | Same pattern as metro/scope faces |
| Fake names | Prove layout; real names come in step 02 |
| `vTaskDelay` idle loop | Keep FreeRTOS scheduler happy |

### Architecture

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  app_main   │ ──► │   mon_*     │ ──► │  LCD rows   │
│ static names│     │ title+3     │     │  Task Mon   │
└─────────────┘     └─────────────┘     └─────────────┘
```

---

## 4. Build

```bash
cd labs/device-labs
# Lab: rbdev run 06  |  Step: ex06.c
rbdev run 06   # from labs/device-labs (step still in ex06.c)
```

---

## 5. Student sketch (`main/main.c`)

```c
bsp_init();
mon_init();
mon_start();
mon_set_title("Task Mon");
mon_set_row(0, "ui");
mon_set_row(1, "load_a");
mon_set_row(2, "load_b");
for (;;) {
    vTaskDelay(pdMS_TO_TICKS(500));
}
```

### How the code works

**`mon_start`.**  
Opens a 4-line `lab_lcd` face: cyan title + three green rows.

**Static strings.**  
Hard-coded names only. Step 02 replaces them with `uxTaskGetSystemState`.

---

## 6. FreeRTOS in depth — idle pacing

| Piece | Role |
|-------|------|
| `vTaskDelay` | Yield so IDLE / other system tasks can run |
| No snapshot API | Face-first pedagogy |

Mental model: **layout before kernel truth.**

---

## 7. Checkpoint

- [ ] Title + three names visible  
- [ ] Ready for live names  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Blank LCD | `bsp_init` / board flag |
| Wrong target | `set-target esp32s3` |

---

## 9. Next

[Step 02 — Live names](../step-02-live-names/).
