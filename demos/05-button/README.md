# Demo 05 — Normal + toggle buttons

**2×4** `rb_style` collage. Top = normal (DOWN thickens border, CLICK
counts presses). Bottom = toggle (CLICK mirrors peer ON state).

## APIs this demo uses

| Layer | You will call |
|-------|----------------|
| Board | `bsp_init`, `bsp_touch_read` |
| UI tree | `rb_display_open`, `rb_screen_*`, `rb_layout_*` |
| Style | `rb_button_set_style` (`RB_STYLE_*`) |
| Button | `set_on_event` (`DOWN`/`UP`/`CLICK`), `feed_touch` |

## Build / flash

```bash
cd demos/05-button
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
# or: idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx
```

## Success

Normal labels become press counts; toggles stay in sync pairwise.
