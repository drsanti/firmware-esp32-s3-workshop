# Lab 03 / Step 01 — Game face

**Device lab:** [Reaction Game](../README.md)  
**Boards:** `ws28p` **or** `es3c28p`

---

## 1. What this step must deliver

- LCD title **Reaction** with big states cycling READY → WAIT → GO → RESULT.
- Detail line shows the phase index.
- Smooth updates via `vTaskDelay` (no busy-spin).

**Success:** you can name each state before adding input.

---

## 2. Problems this step faces

- Need a clear **game face** before timing logic.
- Need paced redraw + yield.

---

## 3. Design approach

| Choice | Why |
|--------|-----|
| `react_*` helper | Owns layout; you own state |
| Single loop + delay | Demo cycle only |
| No BOOT / timer yet | Face first |

### Architecture

```
┌──────────┐  vTaskDelay   ┌──────────┐  redraw   ┌─────────┐
│ app_main │ ────────────► │ react_*  │ ────────► │   LCD   │
│   loop   │ ◄──────────── │  helper  │           │ banner  │
└──────────┘  next state   └──────────┘           └─────────┘
```

### State machine

Demo cycle only (no input yet):

```
┌────────┐   ┌────────┐   ┌────────┐   ┌────────┐
│ READY  │──►│  WAIT  │──►│   GO   │──►│ RESULT │
└────────┘   └────────┘   └────────┘   └───┬────┘
    ▲                                      │
    └──────────────────────────────────────┘
              vTaskDelay 800 ms each
```

---

## 4. Build

```bash
cd labs/device-labs
# Lab: .\run 03  |  Step: ex03.c
.\run 03   # from labs/device-labs (step still in ex03.c)
# or -DBOARD=es3c28p
```

---

## 5. Student sketch (`main/main.c`)

```c
void app_main(void)
{
    const react_st_t cycle[] = {
        REACT_ST_READY,
        REACT_ST_WAIT,
        REACT_ST_GO,
        REACT_ST_RESULT,
    };
    unsigned i = 0;

    bsp_init();
    react_init();
    react_start();
    react_set_hint("demo cycle");

    for (;;) {
        react_set_state(cycle[i]);
        react_set_detail("phase %u", i);
        i = (i + 1u) % 4u;
        vTaskDelay(pdMS_TO_TICKS(800));
    }
}
```

### How the code works

**`react_start`.**  
Lays out four rows: title, big state, detail, hint. Call once after `bsp_init`.

**State cycle.**  
Each loop picks the next `react_st_t`, updates the big banner and a phase
counter on the detail line, then advances `i`.

**`vTaskDelay(800)`.**  
Holds each state long enough to read, and yields so FreeRTOS idle can run.
Without the delay the LCD would thrash and the CPU would spin.

Hold this model: *face first, input later.* Step 02 adds BOOT and timing.

---

## 6. FreeRTOS in depth — `vTaskDelay`

Same pace + yield lesson as earlier labs. Step 02 will use delay for the
random WAIT (which blocks the task — fine for a first working game, then we
improve it).

| Call | Meaning |
|------|---------|
| `pdMS_TO_TICKS(800)` | Convert 800 ms → ticks |
| `vTaskDelay(...)` | Block this task ≥ that many ticks |

**Not yet:** timers, notify, BOOT. Those appear when later requirements force them.

---

## 7. Checkpoint

- [ ] Can name READY / WAIT / GO / RESULT  
- [ ] Can say why the loop needs `vTaskDelay`  
- [ ] Banner cycles on your board  

---

## 8. Troubleshooting

| Symptom | Try |
|---------|-----|
| Blank LCD | `bsp_init` + `react_start`; matching `-DBOARD=` |
| States flash too fast | Delay missing or too small |

---

## 9. Next

[Step 02 — GO and tap](../step-02-go-and-tap/).
