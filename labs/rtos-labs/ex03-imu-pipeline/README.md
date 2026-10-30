# ex03 — IMU pipeline (cross-core queue)

**Goal:** exchange data **between cores** safely: core 0 **samples** the IMU,
core 1 **computes** `|a|`, and a UI task **displays** an instrument panel.
The contract is a FreeRTOS **queue** — not a bare global.

Builds on [ex00](../ex00-task-creation/) (tasks) and
[ex01](../ex01-task-affinity/) (pinning).

```
┌──────────────────────────────────────┐
│ ex03 IMU PIPELINE  c0->c1  LIVE      │
├────────────────────┬─────────────────┤
│                    │  ax ay az bars  │
│     |a|  GAUGE     │  + gyro text    │
│     (hero)         │                 │
└────────────────────┴─────────────────┘
```

Enable in `main/main.c` with `ex03_start()` (only one exercise at a time).

---

## 1. Learning objectives

| Idea | In this exercise |
|------|------------------|
| Cross-core data | Same RAM; **queue** is the safe hand-off |
| Latest wins | Queue depth **1** + `xQueueOverwrite` |
| Affinity | `imu` @ core 0, `compute` @ core 1 |
| Separation | Sample / compute / draw never share one loop |
| Evidence | Footer shows `c0` / `c1` from `xPortGetCoreID` |

**Success:** tilt or shake the board; bars move; `|a|` gauge sits near ~10 at
rest and spikes when shaken. Footer shows LIVE (or SIM) and stable core ids.

---

## 2. Mental model — pipeline, not shared float

```
┌──────────┐  overwrite   ┌───────┐  receive   ┌──────────┐  overwrite   ┌───────┐  receive   ┌─────────┐
│ imu_task │ ───────────► │ q_raw │ ─────────► │ compute  │ ───────────► │ q_out │ ─────────► │ ui_task │
│  core 0  │              │ len 1 │            │  core 1  │              │ len 1 │            │ widgets │
└──────────┘              └───────┘            └──────────┘              └───────┘            └─────────┘
```

**Rejected:** `volatile float g_ax` written on core 0 and read on core 1.
That is a data race. Two cores can truly overlap; the queue (or a mutex)
makes ownership clear.

**Analogy:** core 0 fills a sample tray; core 1 takes the tray, stamps `|a|`,
puts a result tray for the UI. Depth 1 means only the **newest** tray is kept.

---

## 3. Why length-1 + overwrite

| Choice | Why |
|--------|-----|
| Depth 1 | UI/compute want **now**, not a backlog of old shakes |
| `xQueueOverwrite` | Producer never blocks on “queue full” |
| `xQueueReceive` | Consumer sleeps until a sample exists |

Same pattern as the spirit-level lab queue step. A long queue here would make
`|a|` **lag** behind the real motion.

---

## 4. Tasks and pins

| Task | Pin | Responsibility |
|------|-----|----------------|
| `imu_task` | core **0** | `bsp_imu_read` or `imu_sim_fill` → `q_raw` |
| `compute_task` | core **1** | `mag = sqrt(ax²+ay²+az²)` → `q_out` |
| `ui_task` | none | `ex_imu_panel_set` only |

UI is unpinned on purpose: drawing is not the dual-core lesson; the
**sample → compute** hop is.

---

## 5. Payloads

```c
typedef struct {
    float acc[3];
    float gyro[3];
    uint8_t live;
    int8_t core_sample;
} raw_sample_t;

typedef struct {
    float acc[3];
    float gyro[3];
    float mag;
    uint8_t live;
    int8_t core_sample;
    int8_t core_compute;
} out_sample_t;
```

Carry `acc` through to the UI so readout and gauge stay **one coherent
frame**. Do not update bars from one queue and `|a|` from another without a
join.

---

## 6. Professional UI

| Region | Widget | Shows |
|--------|--------|--------|
| Top | `rb_label` | Thin title: pipeline tag + `c0->c1` + LIVE/SIM |
| Left | `rb_gauge` | Hero `|a|` (0..25 m/s², warn 15 / danger 20) |
| Right | `rb_imu_readout` | Supporting ax/ay/az bars + gyro |

Black glass; title height is patched after reflow (not an equal half-screen row).

Implementation lives in `common/ex_imu_panel.*` so `ex03.c` stays on FreeRTOS.

---

## 7. Tips and tricks

### Tip — pin for I/O, queue for truth

Pinning IMU sampling next to whatever owns the bus can help. Correctness
still comes from the queue. Affinity is not a mutex.

### Tip — keep sqrt off the sample core (here)

This lab puts `|a|` on core 1 so students **see** a real cross-core hop.
In products you might compute on the sample core and only queue the result —
fewer copies. Teach the hop first; optimize later with evidence.

### Tip — UI must not call the IMU

If `ui_task` calls `bsp_imu_read`, you have re-coupled the pipeline and lost
the lesson (and often the frame rate). Widgets only.

### Tip — check queue create

```c
if (s_q_raw == NULL || s_q_out == NULL) {
    return; /* out of heap — fail loud in real products */
}
```

### Tip — sim boards still teach the RTOS

On boards without IMU, `imu_sim_fill` keeps the pipeline honest. Status shows
`IMU sim` / `SIM` so nobody mistakes synthetic motion for hardware.

---

## 8. Focus / avoid / consider

### Focus on

- Two queues, who sends / who receives  
- Why overwrite beats a deep queue for live IMU  
- Footer `c0` / `c1` as proof of pinning  

### Avoid

- Shared writable globals across cores  
- Drawing inside `imu_task` or `compute_task`  
- Blocking `xQueueSend` on a full deep queue for “latest” data  

### Consider

- What if both tasks pin to core 0? (Still correct via queue; less parallel.)  
- What if `q_raw` depth is 16? (Lag / bursty `|a|`.)  
- Where would a future audio “shake alarm” task attach? (`q_out` fan-out or notify.)  

---

## 9. Checkpoint

- [ ] Can draw the pipeline from memory  
- [ ] Can explain overwrite vs send-on-full  
- [ ] Can say why `acc` is copied into `q_out`  
- [ ] Footer cores match the pins  
- [ ] Rest `|a|` near ~10 m/s² (gravity) when live / sim upright  

---

## 10. Experiments

1. Pin `compute` to core 0 as well — footer `cN` changes; motion still works.  
2. Change `q_raw` to depth 8 and use `xQueueSend` — shake fast; watch lag.  
3. Remove the queue and use a global `float` (then put the queue back) — feel
   why reviews reject that.  
4. Lower UI priority further — sampling/compute should still run.

---

## 11. Troubleshooting

| Symptom | Try |
|---------|-----|
| Blank LCD | `bsp_init`; panel begin; `-DBOARD=` |
| Always SIM | Expected on es3c28p; on ws28p check IMU power/I2C |
| `|a|` stuck | Queue create failed; compute not receiving |
| Cores both `c0` | Check `xTaskCreatePinnedToCore` last args |

---

## 12. Next

You can place work on a core and **hand off data**. Next skills: priorities
under load, notifications vs queues, and multiple consumers of one stream.
