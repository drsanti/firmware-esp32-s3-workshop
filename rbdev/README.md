# rbdev — student guide

`rbdev` is the host CLI for this workshop: **init**, doctor, run, build, flash,
monitor. Same commands on Windows, macOS, and Linux — only the binary path and
serial port differ.

Binaries ship **in this repo** under `rbdev/windows|mac|linux/`.  
`rbdev init` adds that folder to your **user PATH** so later shells can run
plain `rbdev`.

| Your OS | Binary in the clone |
|---------|---------------------|
| Windows x64 | `rbdev/windows/rbdev.exe` |
| macOS | `rbdev/mac/rbdev` |
| Linux x64 | `rbdev/linux/rbdev` |

**Classroom go-live:** [CLASSROOM-SETUP.md](../CLASSROOM-SETUP.md).

---

## 1. After clone

```powershell
git clone <this-repo-url>
cd firmware-esp32-s3-workshop
```

```bash
# macOS / Linux — mark executable if needed
chmod +x rbdev/mac/rbdev rbdev/linux/rbdev 2>/dev/null || true
```

On macOS, first launch may need **right-click → Open** (Gatekeeper).

**Maintainers / AI agents:** after any `rbdev` code change, rebuild and copy
into **this folder and**
`hardware-interface-workshop/firmware-prebuilt/rbdev/<os>/`. From the tool repo:

```powershell
.\scripts\install-to-workshops.ps1
.\scripts\smoke-classroom.ps1   # no-flash smoke matrix
```

Docs: [`ship-binaries.md`](https://github.com/drsanti/rbdev/blob/main/docs/ship-binaries.md) ·
[`AGENTS.md`](https://github.com/drsanti/rbdev/blob/main/AGENTS.md).

---

## 2. One-time enroll — `rbdev init`

From the **workshop root** (folder with `rbdev/` + `labs/`):

```powershell
# Windows
.\rbdev\windows\rbdev.exe init
# scripted: … init --port COM6 --board ws28p --project device-labs -y
```

```bash
# macOS / Linux
./rbdev/mac/rbdev init          # Linux: ./rbdev/linux/rbdev init
```

`init` will:

1. Find ESP-IDF (Path A on the PC)
2. Repair the Espressif Python venv if needed (`install.bat` / `install.sh`)
3. Pick serial port + **one lab track** (interactive, or flags) — usually **`device-labs`**
4. Write gitignored `.rbdev.toml` (paths use `/`)
5. `idf.py set-target esp32s3` for the selected project
6. Add `rbdev/<os>` to **user PATH** (skip with `--no-add-path`)

Then **open a new terminal** so PATH / shell rc reloads.

**Why project?** Four apps under `labs/` (`device-labs`, `rtos-labs`,
`special-labs`, `demo-labs`). Sticky `default_project` is which one
`rbdev run` uses from the workshop root.

**Switch track later — no full re-init:** `cd labs\rtos-labs` then
`rbdev run 00`, or `rbdev run 00 --project labs/rtos-labs`. Optional sticky
update: `rbdev init --project rtos-labs -y`.

Do **not** run `export.ps1` / `export.sh` for class — `rbdev` loads IDF for you.
Cursor terminal or **cmd** is fine; PowerShell is not required.

**Inside Cursor/VS Code:** committed [`.vscode/settings.json`](../.vscode/settings.json)
puts `rbdev/<os>` (and LocalAppData shims after `init`) on the integrated
terminal PATH — open the **workshop root**, then a new terminal.

**Windows outside Cursor:** double-click [`../start-here.cmd`](../start-here.cmd)
to open a cmd window with `rbdev` on PATH for that session only.
See [CLASSROOM-SETUP.md](../CLASSROOM-SETUP.md).

---

## 3. Everyday lab loop

After `init`, sticky `default_project` lets you run from the **workshop root**
(or `cd labs\<track>`). Full ESP-IDF verbs use the `idf.py` shim (no export).

```powershell
rbdev doctor                    # optional preflight
rbdev run 00                    # select ex00 + build + flash
rbdev run 00 --monitor          # + serial monitor
idf.py menuconfig               # or: fullclean / erase-flash / size / …
```

| Command | Purpose |
|---------|---------|
| `rbdev run` | List labs (exit 0) |
| `rbdev run 00` | Select `ex00`, rewrite `main/main.c`, build + flash |
| `rbdev run 00 --monitor` | Same + serial monitor |
| `rbdev run 00 --select-only` | Only rewrite `main/main.c` (no build) |
| `rbdev config --show` | Print sticky values |
| `rbdev config --port … --board …` | Update sticky port/board |
| `idf.py …` / `rbdev idf -- …` | Any Espressif `idf.py` action |

See [CLASSROOM-SETUP.md](../CLASSROOM-SETUP.md) for the one-screen cheat sheet.

**Windows:** never type bare `run 00` — that often hits **nvm**. Always `rbdev run 00`.

Lab ranges:

| Folder | Example |
|--------|---------|
| `labs/device-labs` | `rbdev run 00` … `06` |
| `labs/special-labs` | `rbdev run 01` … `06` |
| `labs/rtos-labs` | `rbdev run 00` … `15` |
| `labs/demo-labs` | `rbdev run 01` … `17` |

---

## 4. Prerequisites

| Need | Notes |
|------|--------|
| This repo | `firmware-esp32-s3-workshop` with `lib/` + `include/` present |
| ESP-IDF | Course install on the PC (**Path A**) — `init` discovers it |
| Board + USB | Port from `doctor` / `init` (`COMx` / `/dev/cu.*` / `/dev/ttyACM*`) |

`rbdev` search order: sticky `default_esp_idf_path` → `RBDEV_IDF_PATH` →
`IDF_PATH` → Espressif EIM registry → common install folders.

Optional override (rare):

```powershell
$env:IDF_PATH = "C:\Espressif\frameworks\esp-idf-v6.1"
```

```bash
export IDF_PATH=~/esp/esp-idf
# or: export RBDEV_IDF_PATH=~/esp/esp-idf
```

### Board (`ws28p` vs `es3c28p`)

| SKU | Product |
|-----|---------|
| `ws28p` | Waveshare ESP32-S3-Touch-LCD-2.8 (**default**) |
| `es3c28p` | Ternion ESP32-S3 |

```powershell
rbdev config --port COMx --board ws28p
rbdev flash --port COM6 --board es3c28p   # one-shot override
```

```bash
rbdev config --port /dev/cu.usbmodemXXXX --board ws28p
rbdev flash --port /dev/ttyACM0 --board es3c28p
```

After switching boards: `rbdev reconfigure` (or a clean build).

---

## 5. Path B — no ESP-IDF yet (full kit only)

Only if your instructor gave you a **full** kit with `sdk\*.zst`:

```powershell
rbdev setup --offline C:\path\to\kit\sdk\archive_….zst
rbdev doctor --workshop .
```

Then use §3. Slim kits have no `sdk\` folder — skip this section.

---

## 6. Doctor exit codes

| Exit | Meaning | Fix |
|------|---------|-----|
| 0 | OK | — |
| 10 | No SDK / IDF | Install ESP-IDF / set `IDF_PATH`; Path B: `setup --offline` |
| 11 | Missing `lib/` / headers | Update this workshop clone |
| 12 | No serial port | Cable, driver, close other monitors, correct `--port` |

### `No module named 'rich_click'`

`rbdev` must use the Espressif venv under `.espressif/python_env/`, **not**
system Python.

| OS | Expected interpreter |
|----|----------------------|
| Windows | `%USERPROFILE%\.espressif\python_env\idf*_py*_env\Scripts\python.exe` |
| macOS / Linux | `~/.espressif/python_env/idf*_py*_env/bin/python` |

Prefer: re-run `rbdev init` from the workshop root (runs `install.bat` /
`install.sh` when needed). Manual repair:

```powershell
cd C:\esp\v6.1\esp-idf        # YOUR IDF path
.\install.bat
# then from workshop root:
.\rbdev\windows\rbdev.exe doctor
```

```bash
cd ~/esp/esp-idf
./install.sh
./rbdev/mac/rbdev doctor      # Linux: ./rbdev/linux/rbdev doctor
```

---

## 7. CLI cheat sheet

```text
rbdev init [--port …] [--board …] [--project …] [-y]
           [--skip-install] [--skip-set-target] [--no-add-path]
rbdev config [--port <PORT>] [--board ws28p|es3c28p] [--show]
rbdev doctor [--workshop <path>] [--json]
rbdev run [NN] [--monitor] [--select-only] [--port …] [--board …]
rbdev build [--board …]
rbdev flash [--port …] [--board …]
rbdev monitor [--port …]
rbdev reconfigure [--board …]
rbdev setup --offline <archive>  # Path B only
rbdev fw sync --from <tar.gz>
rbdev shell
rbdev version
```

| OS | Example port |
|----|--------------|
| Windows | `COM6` |
| macOS | `/dev/cu.usbmodemXXXX` |
| Linux | `/dev/ttyACM0` or `/dev/ttyUSB0` |

Default board is **`ws28p`**. Ternion: `--board es3c28p`.

Maintainer packaging lives in the separate **`rbdev`** tool repo, not here.
