# Board SKUs (students)

This workshop supports **two** ESP32-S3 boards. You only need **this repo** —
do **not** clone `firmware-esp32-s3-dev`. Chip target is always **`esp32s3`**.

| SKU | Hardware | Display | Touch | Notes |
|-----|----------|---------|-------|-------|
| **`ws28p`** (default) | Waveshare ESP32-S3-Touch-LCD-2.8 | ST7789 | CST328 / CST3530 (auto) | Class default |
| **`es3c28p`** | Ternion ESP32-S3 | ILI9341 | FT6336 | LED + mic labs |

Pick the SKU that matches the hardware on your desk. Wrong board → wrong pins
(blank LCD / no touch / missing sensors).

---

## Recommended — `rbdev` (class path)

Sticky board lives in gitignored `.rbdev.toml` (set once at `rbdev init`).

```powershell
# Show current sticky port + board
rbdev config --show

# Switch board for all later builds/flashes
rbdev config --board es3c28p          # or: ws28p
rbdev reconfigure                     # refresh CMake for the new BOARD
rbdev run 00                          # build + flash sticky project
```

```bash
# macOS / Linux
rbdev config --board es3c28p
rbdev reconfigure
rbdev run 00
```

One-shot (does not change sticky config):

```powershell
rbdev build --board es3c28p
rbdev flash --port COMx --board es3c28p
```

After **any** board switch: `rbdev reconfigure` or a clean rebuild
(`idf.py fullclean` / ESP-IDF: Full Clean).

---

## ESP-IDF VS Code / Cursor extension (no terminal)

Open the **lab project folder** as the workspace — not only the workshop root:

| Track | Folder to open |
|-------|----------------|
| Device curriculum | `labs/device-labs` |
| FreeRTOS deep-dives | `labs/rtos-labs` |
| FreeRTOS specials | `labs/special-labs` |
| UI / widget demos | `labs/demo-labs` |

**Chip target ≠ board SKU.** Status-bar / *Set Espressif device target*
must be **`esp32s3`**. That does **not** pick Waveshare vs Ternion.

### Option A — Command Palette (preferred)

Each track ships `CMakePresets.json` with **`ws28p`** and **`es3c28p`**.

1. **View → Command Palette**
2. Run **`ESP-IDF: Select Project Configuration`**
3. Pick **Waveshare ws28p** or **Ternion es3c28p**
4. **`ESP-IDF: Full Clean Project`** (first time / after switching)
5. **Build** → **Flash** (extension buttons / palette)

### Option B — Settings UI (fallback)

1. **File → Preferences → Settings** (workspace)
2. Search **`cmakeCompilerArgs`**
3. Edit **Idf: Cmake Compiler Args** so it includes **`-DBOARD=es3c28p`**
   (or `ws28p`). Keep the usual defaults if the list is not empty:

```json
"idf.cmakeCompilerArgs": [
  "-G", "Ninja",
  "-DPYTHON_DEPS_CHECKED=1",
  "-DESP_PLATFORM=1",
  "-DBOARD=es3c28p"
]
```

4. **`ESP-IDF: Full Clean Project`** → **Build** → **Flash**

Do **not** use only `["-DBOARD=…"]` — an non-empty list **replaces** the
extension defaults (you would lose `-G Ninja`).

Class path is still `rbdev config --board …` + `rbdev run` when students
use the terminal.

---

## Advanced — bare `idf.py`

Only if the instructor says so (ESP-IDF already on PATH / activated):

```bash
cd labs/device-labs
idf.py -DBOARD=ws28p  set-target esp32s3 build flash -p COMx
idf.py -DBOARD=es3c28p set-target esp32s3 build flash -p COMx
```

Switching SKU: `idf.py fullclean`, then rebuild with the new `-DBOARD=…`.
