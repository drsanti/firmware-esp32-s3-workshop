# Demo 03 — Six terminal panes

Build a **screen → layout → rb_term** tree: **3 columns × 2 rows**
(six panes). Each FreeRTOS task pushes colored lines into its own term
via `rb_term_log_printf_ex`. No robotos node or serial wire.

## APIs this demo uses

| Layer | You will call |
|-------|----------------|
| Board | `bsp_init` |
| UI tree | `rb_display_open`, `rb_screen_*`, `rb_layout_*` |
| Term | `rb_term_init`, `rb_term_set_scale`, `rb_term_set_color`, `rb_term_log_printf_ex` |

## Build / flash

```bash
cd demos/03-term
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
# or: idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx
```

## Success

Six scrolling terminals fill a 3×2 grid; each pane logs `tN #k` at a
staggered rate in its own color. No host bridge required.
