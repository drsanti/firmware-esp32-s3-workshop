# Demo 08 — Radios

Two exclusive groups (Mode / Speed), **2×4** collage. Tap selects;
re-tap does not clear — exclusivity is the lesson.

## Build / flash

```bash
cd demos/08-radio
# Must be esp32s3 (GPIO_NUM_45 undeclared ⇒ wrong target / plain esp32).
idf.py -DBOARD=ws28p set-target esp32s3 build flash -p COMx monitor
# or: idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx
```

## Success

Only one radio per column group is filled; switching picks a new
dot and clears the previous.
