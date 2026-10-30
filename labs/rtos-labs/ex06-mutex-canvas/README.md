# ex06 — mutex (shared canvas)

**Goal:** see a FreeRTOS **mutex** protect a shared drawing surface.

Two small bouncing balls (**A** dark-blue, **B** dark-green). Every frame
clears the canvas, then draws **both** balls. That avoids ghost boxes.

| Mode | What happens | What you see |
|------|----------------|--------------|
| **OFF** | Flush after clear, after A, after B + pauses | **Strobe**: blank → only A → both |
| **ON** | One flush of a complete frame under mutex | **Clean** smooth pair of balls |

Important: the LCD updates only on `flush`. Editing the framebuffer
without flushing looks the same in ON and OFF — that was the earlier bug.

Boot **OFF**. Toggle the button to compare live.

```
┌──────────────────────────────────────┐
│ ex06 mutex OFF|ON                    │
│ A#..  B#..                           │
├──────────────────────────────────────┤
│                                      │
│   [A]         [B]   bouncing balls   │
│                                      │
├──────────────────────────────────────┤
│ [ OFF / ON ]                         │
└──────────────────────────────────────┘
```

Enable in `main/main.c` with `ex06_start()`.

| File | Holds |
|------|--------|
| `ex06.c` | Mutex, painter/UI tasks, `ex06_start` |
| `ex06_canvas.*` | LCD layout, balls, clean/strobe paint |

This exercise has **two** `.c` files. After the canvas helper is added (or if
you pull a tree that already has it), reconfigure once so the GLOB picks it up:

```powershell
idf.py -DBOARD=ws28p reconfigure
# or
idf.py -DBOARD=es3c28p reconfigure
```

Then build/flash as usual (see [rtos-labs README](../README.md#build--flash)).
Skip reconfigure → linker `undefined reference` to `ex06_canvas_*`.

---

## 1. Learning objectives

| Idea | In this exercise |
|------|------------------|
| Shared resource | The canvas framebuffer region |
| Critical section | clear → draw A → draw B → flush |
| Mutex | `xSemaphoreCreateMutex` / `Take` / `Give` |
| Race | Mid-frame yields when mutex OFF |
| No ghosts | Full canvas clear every frame |

**Success:** OFF looks broken; ON looks smooth with two solid balls.

---

## 2. What you should see

### Mutex OFF (status shows `STROBE`)

Repeating: **empty canvas → only ball A → A and B**. Easy to see.

### Mutex ON (status shows `CLEAN`)

Both balls always present, smooth bounce. No full-canvas clear (that caused
fast gray blinking) — only the previous ball rects are erased.

Both `A#` and `B#` keep increasing either way.

---

## 3. Mental model

```
  ON:   clear + draw A + draw B + flush     (once, under mutex)
  OFF:  clear+flush → wait → A+flush → wait → B+flush
```

---

## 4. How the code is wired

| Task | Prio | Pin | Role |
|------|------|-----|------|
| `paintA` | 5 | core 0 | Runs `paint_frame`, advances ball A |
| `paintB` | 5 | core 0 | Runs `paint_frame`, advances ball B |
| `ui` | 4 | core 1 | Touch + status |

```c
if (mutex_on) xSemaphoreTake(mtx, portMAX_DELAY);
paint_frame(/* unlocked = !mutex_on */);  /* yields only when OFF */
advance_ball(mine);
if (mutex_on) xSemaphoreGive(mtx);
```

### Mutex vs `bsp_display_lock`

| Lock | Role |
|------|------|
| FreeRTOS mutex | Whole frame (lesson) |
| `bsp_display_lock` | Each `rbgfx_*` / flush (driver) |

---

## 5. Tips

- Clear the **whole** canvas each frame — erasing only “my previous rect”
  leaves ghosts when two tasks race.
- Hold the mutex for the **entire** frame, not only `flush`.
- Same priority on painters — this lab is exclusion, not ranking.

---

## 6. Checkpoint

- [ ] Can name Take / Give APIs  
- [ ] Can point at the critical section (`paint_frame`)  
- [ ] Can explain why full clear prevents ghost boxes  
- [ ] Can contrast OFF flicker vs ON clean motion  

---

## 7. Experiments

1. Remove yields in OFF mode — race becomes rarer.  
2. Give mutex before `flush` — torn frames return.  
3. Pin painters on different cores — OFF gets worse.  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Ghost boxes | Confirm `clear_canvas()` every frame |
| No OFF/ON difference | Yields present when unlocked? Toggle updating `s_mutex_on`? |
| Freeze | Each `rbgfx_*` behind `bsp_display_lock`? |

---

## 9. Next

[ex07 — counting semaphore (resource pool)](../ex07-counting-sem/) — mutex is 1
owner; next, a pool of N tokens.
