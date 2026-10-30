# rbdev — student guide

`rbdev` is the host CLI for this workshop: doctor, build, flash, monitor
(and optional managed ESP-IDF setup). Same commands on Windows, macOS, and
Linux — only the binary path and serial port differ.

Binaries ship **in this repo** under `rbdev/windows|mac|linux/` so a normal
`git clone` is enough. Do **not** permanently add this folder to PATH.

| Your OS | Binary in the clone |
|---------|---------------------|
| Windows x64 | `rbdev/windows/rbdev.exe` |
| macOS | `rbdev/mac/rbdev` |
| Linux x64 | `rbdev/linux/rbdev` |

---

## 1. After clone

```powershell
git clone <this-repo-url>
cd firmware-esp32-s3-workshop
# Windows: binary already at rbdev\windows\rbdev.exe
```

```bash
# macOS / Linux — mark executable if needed
chmod +x rbdev/mac/rbdev rbdev/linux/rbdev rb ./labs/*/rb ./labs/*/run ./demos/demo-labs/rb ./demos/demo-labs/run 2>/dev/null || true
```

Windows, macOS (Apple Silicon), and Linux x64 binaries ship in this repo.
On macOS, first launch may need **right-click → Open** (Gatekeeper) until
the binary is notarized.

If your OS binary is somehow missing after clone, ask your instructor for
that file and place it in the matching folder above.

**Maintainers / AI agents:** after any `rbdev` code change, rebuild and
copy into **this folder and**
`hardware-interface-workshop/firmware-prebuilt/rbdev/<os>/` (keep CLIs in
sync). From the tool repo:

```powershell
.\scripts\install-to-workshops.ps1
```

```bash
./scripts/install-to-workshops.sh   # host OS; Linux from Mac → cross (see ship-binaries)
```

Docs: [`ship-binaries.md`](https://github.com/drsanti/rbdev/blob/main/docs/ship-binaries.md) ·
[`AGENTS.md`](https://github.com/drsanti/rbdev/blob/main/AGENTS.md).
---

## 2. macOS / Linux Quick Start

After cloning, run this once to make the scripts executable:

```bash
cd firmware-esp32-s3-workshop
chmod +x rb rbdev/mac/rbdev rbdev/linux/rbdev labs/*/rb labs/*/run demos/demo-labs/rb demos/demo-labs/run
```

Then from any lab folder:

```bash
cd labs/device-labs  # or labs/rtos-labs, demos/demo-labs, etc.
./rb doctor
# macOS:
./rb config --port /dev/cu.usbmodemXXXX --board ws28p
# Linux:
# ./rb config --port /dev/ttyACM0 --board ws28p   # or /dev/ttyUSB0
./rb build
./rb flash
./run 00
```

Your serial port is listed in the `doctor` output
(e.g. `/dev/cu.LX-10`, `/dev/cu.usbmodemXXXX`, `/dev/ttyACM0`).

- **macOS — list ports:** `ls /dev/cu.*` (use a `/dev/cu.*` path for flash).
- **macOS Gatekeeper:** right-click → Open `rbdev/mac/rbdev` once if blocked.
- **Linux dialout:** if permission denied on the port:
  `sudo usermod -aG dialout "$USER"` then log out and back in.

---

## 3. Prerequisites

| Need | Notes |
|------|--------|
| This repo | `firmware-esp32-s3-workshop` with `lib/` + `include/` present |
| ESP-IDF | Already installed for this course (**Path A**) |
| Board + USB | Correct `COMx` / `/dev/cu.*` / `/dev/ttyACM*` / `/dev/ttyUSB*` port |

**Easiest on Windows:** open **ESP-IDF PowerShell** from the Start Menu so
`IDF_PATH` is set, then run the commands below from the **workshop root**.

**macOS / Linux:** source the IDF env in each shell, or set `IDF_PATH`:

```bash
. ~/esp/esp-idf/export.sh         # YOUR path (e.g. /Users/you/esp/esp-idf)
# or: export IDF_PATH=~/esp/esp-idf
# or: export RBDEV_IDF_PATH=~/esp/esp-idf
```

If doctor cannot find IDF on Windows:

```powershell
# Examples — use YOUR install path
$env:IDF_PATH = "C:\Espressif\frameworks\esp-idf-v6.1"
# or: $env:RBDEV_IDF_PATH = "D:\dev\esp\v6.1\esp-idf"
```

`rbdev` search order: `RBDEV_IDF_PATH` → `IDF_PATH` → Espressif EIM registry →
common `C:\Espressif\frameworks\…` folders.

---

## 4. Everyday commands (Path A)

Each lab folder has a short **`rb`** wrapper (`rb.cmd` / `rb`) that
calls your OS binary under `rbdev/<os>/`.

**Windows PowerShell** — use `.\rb` (PowerShell does not run cwd commands
without `.\`). Wrappers resolve to **`rb.cmd`** / **`run.cmd`**. Still run
this **once per PowerShell window** so any helper scripts are allowed:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass

cd labs\device-labs
.\rb doctor                 # lists COM ports; warns if flash port not set yet

# REQUIRED once per PC — there is no default COM port (each machine differs).
# Pick your board's port from the doctor list / Device Manager:
.\rb config --port COMx --board ws28p

# Then short commands use the saved port + board:
.\rb build
.\rb flash
.\rb monitor

# Select exercise NN, rewrite main/main.c, build + flash:
.\run 00
.\run 01
.\run 06 monitor
```

`.\run NN` exists in every one-project app:

| Folder | Example |
|--------|---------|
| `labs/device-labs` | `.\run 00` … `.\run 06` (step still in `exNN.c`) |
| `labs/special-labs` | `.\run 01` … `.\run 06` |
| `labs/rtos-labs` | `.\run 00` … `.\run 15` |
| `demos/demo-labs` | `.\run 01` … `.\run 17` |

`.\rb config --show` prints the saved values. Board defaults to **`ws28p`** if
you omit `--board`. Port must be set (config, `--port`, or `ESPPORT`) — `rb`
will not guess.

### Board (`ws28p` vs `es3c28p`)

| SKU | Product |
|-----|---------|
| `ws28p` | Waveshare ESP32-S3-Touch-LCD-2.8 (**default**) |
| `es3c28p` | Ternion ESP32-S3 |

```powershell
# Windows — replace COMx with YOUR port
.\rb config --port COMx --board ws28p
.\rb build
.\rb flash

# Ternion:
.\rb config --port COMx --board es3c28p

# One-shot override (does not change saved config):
.\rb flash --port COM6 --board es3c28p
```

```bash
# macOS / Linux — same flags; different port path
./rb config --port /dev/cu.usbmodemXXXX --board ws28p   # Linux: /dev/ttyACM0
./rb build
./rb flash
./rb flash --port /dev/ttyACM0 --board es3c28p          # one-shot
```

After switching boards, run `rb reconfigure` (or a clean build) so CMake
picks up the new `-DBOARD=`.

macOS and Linux users: see **Section 2** — `chmod` once, then `./rb` from any lab folder.

Goal for day 0: flash **Hello Device (ex00)** (or the lab your instructor names).
You can still call the OS binary under `rbdev/<os>/` or plain `idf.py` if you prefer.

---

## 5. Path B — no ESP-IDF yet (full kit only)

Only if your instructor gave you a **full** kit with `sdk\*.zst`:

```powershell
& $esp setup --offline C:\path\to\kit\sdk\archive_….zst
& $esp doctor --workshop .
```

Then build/flash as in §3. Slim kits have no `sdk\` folder — skip this section.

---

## 6. Doctor exit codes

| Exit | Meaning | Fix |
|------|---------|-----|
| 0 | OK | — |
| 10 | No SDK / IDF | ESP-IDF PowerShell, or set `IDF_PATH` / `RBDEV_IDF_PATH`; Path B: `setup --offline` |
| 11 | Missing `lib/` / headers | Update this workshop clone |
| 12 | No serial port | Cable, driver, close other monitors, correct `--port` |

### “not digitally signed” / cannot load `*.ps1`

Windows PowerShell with **Restricted** execution policy blocks `.ps1` files.
**Always** start a lab session with:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

That applies to **this window only** (no admin, no permanent change). Then:

```powershell
.\rb doctor
.\run 00
```

If you still see the error on an old zip: `.\rb.cmd doctor` / `.\run.cmd 00`.

### `No module named 'rich_click'` (or `click` / `rich`)

This means the **ESP-IDF Python venv is missing or incomplete** — not a lab
bug. `rbdev` must use the Espressif venv under `.espressif/python_env/`,
**not** system Python (Windows Store / `Python313`, Homebrew, `/usr/bin/python3`).

| OS | Expected interpreter |
|----|----------------------|
| Windows | `%USERPROFILE%\.espressif\python_env\idf*_py*_env\Scripts\python.exe` |
| macOS / Linux | `~/.espressif/python_env/idf*_py*_env/bin/python` |

Create / repair the venv:

```powershell
# Windows — prefer ESP-IDF PowerShell from the Start Menu
cd C:\esp\v6.1\esp-idf        # YOUR IDF path (rbdev doctor shows it)
.\install.bat                 # several minutes — creates venv + rich_click
.\export.ps1
idf.py --version
.\rbdev\windows\rbdev.exe doctor   # from workshop root — rich_click OK
```

```bash
# macOS / Linux
cd ~/esp/esp-idf              # YOUR IDF path (e.g. /Users/you/esp/esp-idf)
./install.sh
. ./export.sh
idf.py --version
./rbdev/mac/rbdev doctor      # Linux: ./rbdev/linux/rbdev doctor
```

Only if the venv folder already exists but `rich_click` is still missing:

```powershell
# Windows
& "$env:USERPROFILE\.espressif\python_env\idf6.1_py3.11_env\Scripts\python.exe" -m pip install rich_click
```

```bash
# macOS / Linux
~/.espressif/python_env/idf6.1_py3.11_env/bin/python -m pip install rich_click
```

Then retry: `run 01 monitor`.

Classroom go-live (all OS — doctor green light + pasteable student messages):
[CLASSROOM-SETUP.md](../CLASSROOM-SETUP.md).

---

## 7. CLI cheat sheet

```text
rbdev config [--port <PORT>] [--board ws28p|es3c28p] [--show]
rbdev doctor [--workshop <path>] [--json]
rbdev build [--board ws28p|es3c28p]
rbdev flash [--port <PORT>] [--board ws28p|es3c28p]
rbdev monitor [--port <PORT>]
rbdev run [--port <PORT>] [--board ws28p|es3c28p]
rbdev reconfigure [--board ws28p|es3c28p]
rbdev setup --offline <archive>  # Path B only
rbdev fw sync --from <tar.gz>    # repair lib/include (rarely needed)
rbdev shell
rbdev version
```

Typical: `rb config --port <PORT> --board ws28p` once (port is required — no
universal default), then `rb build` / `rb flash`.

| OS | Example port |
|----|--------------|
| Windows | `COM6` |
| macOS | `/dev/cu.usbmodemXXXX` |
| Linux | `/dev/ttyACM0` or `/dev/ttyUSB0` |

Default board is **`ws28p`**. Ternion: `--board es3c28p`.

Maintainer / kit packaging lives in the separate **`rbdev`** tool repo, not here.
