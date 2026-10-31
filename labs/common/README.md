# labs/common

Shared helpers for FreeRTOS device / special labs (not used by `demo-labs`).

| Header | Role |
|--------|------|
| `lab_lcd.h` | Simple multi-line LCD text (`lcd_begin` / `lcd_printf` / `lcd_set_color` / `lcd_set_scale`) |
| `lab_beep.h` / `.c` | Tones via `rb_sfx` (`beep_tone` / `beep_chime` / gain / echo / release) |
| `lab_sound_pad.h` | Sound-pad scaffold (`init` / optional knobs / `add_button` / `start`) |
| `lab_metro.h` / `.c` | Metronome face (BPM / beat / jitter) |
| `lab_scope.h` / `.c` | Vibration scope (`rb_chart` HISTORY) |
| `lab_alarm.h` / `.c` | Motion alarm status face |
| `lab_react.h` / `.c` | Reaction game state face |
| `lab_spirit.h` / `.c` | Spirit-level bubble face |
| `lab_mon.h` / `.c` | Task Monitor face (title + 3 rows) |
| `lab_led_row.h` / `.c` | LCD “LED” row for BOOT chase |

Include path is added automatically by each app’s `main/CMakeLists.txt`
(`device-labs`, `special-labs`).

IDE: [`labs/common/.clangd`](.clangd) covers these headers when opened alone
(no `compile_commands.json` here). Restart clangd after changing it.

## `lcd_*` quick use

```c
bsp_init();
lcd_begin(2);
lcd_printf(0, "Hello Device");
lcd_printf(1, "step 01 - LCD");
```

ASCII only on the LCD (font has no unicode glyphs).
