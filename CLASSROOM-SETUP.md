<div align="center">

# Classroom setup

**One tool: `rbdev`. No `rb` / `run` wrappers. No ESP-IDF `export` activate.**

[![Audience](https://img.shields.io/badge/audience-students%20%7C%20instructors-blue.svg)](#)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-informational.svg)](#)
[![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v6.1-brightgreen.svg)](https://docs.espressif.com/projects/esp-idf/)
[![CLI](https://img.shields.io/badge/CLI-rbdev%200.1.3+-informational.svg)](rbdev/README.md)
[![GUI](https://img.shields.io/badge/GUI-rbdev--gui%20Windows-informational.svg)](rbdev-gui/README.md)
[![Board](https://img.shields.io/badge/board-ws28p%20%7C%20es3c28p-orange.svg)](ports/boards/README.md)

[Daily loop](#student-daily-loop) ·
[Windows](#windows) ·
[macOS](#macos) ·
[Linux](#linux) ·
[Pass criteria](#pass-criteria) ·
[Instructor](#instructor-playbook)

</div>

---

## Student daily loop

Open this folder in **Cursor / VS Code** (workshop root). New terminals already
have `rbdev` on PATH via committed [`.vscode/settings.json`](.vscode/settings.json)
(prepends `rbdev/<os>` + `%LOCALAPPDATA%\rbdev\bin` after `init`) — no PATH
one-liner, no IDE restart. Do not delete that file.

| Step | When | Command |
|------|------|---------|
| 1 | Once per PC / clone | From **workshop root**: `rbdev init` (Cursor: plain `rbdev init`; fallback `.\rbdev\windows\rbdev.exe init`). Pick **`device-labs`** unless the instructor says otherwise. |
| 2 | Every lab day | `rbdev run 00` (workshop root uses sticky project; or `cd labs/<track>` first) |
| — | Optional (Windows GUI) | Install [`rbdev-gui/windows/rbdev-gui-setup.exe`](rbdev-gui/README.md) → **Labs / Run** (Build · Flash · Run · Monitor). Same `.rbdev.toml` as the CLI. |

```text
# Workshop root (folder with rbdev/ + labs/)
rbdev init                              # once — choose device-labs
# fallback: .\rbdev\windows\rbdev.exe init
# macOS:  ./rbdev/mac/rbdev init
# Linux:  ./rbdev/linux/rbdev init

# NEW terminal tab in Cursor, then:
rbdev doctor                            # optional preflight
rbdev run 00                            # sticky project (usually device-labs)
rbdev run 00 --monitor
# Advanced IDF (no export.ps1): idf.py menuconfig | fullclean | erase-flash | size
```

**Do not:**

- run `export.ps1` / `export.sh` / “activate ESP-IDF”
- type bare `run 00` (on Windows that often hits **nvm**)
- use deleted `rb` / `run` wrappers

### `start-here.cmd` (Windows only — optional)

**Skip this** if you use Cursor/VS Code with the workshop root open — the
integrated terminal already has `rbdev` on PATH.

Double-click [`start-here.cmd`](start-here.cmd) when:

- you are **outside** Cursor (Explorer / plain cmd), or  
- the terminal says `rbdev` is not found and you do not want to type
  `.\rbdev\windows\rbdev.exe …`

It opens a **cmd** window, puts `rbdev\windows` on PATH for **that window
only**, and prints the daily commands. Then type `rbdev init` / `rbdev run 00`
yourself. It does **not** replace `init`.

### Lab tracks (why `init` asks for a project)

This repo has **four separate ESP-IDF apps**. `init` must pick one to:

1. run `idf.py set-target esp32s3` there, and  
2. save it as sticky `default_project` (so `rbdev run` works from the workshop root).

| Track | Folder | Pick at init when… |
|-------|--------|--------------------|
| **device-labs** (default) | `labs/device-labs` | Normal class path (ex00–06) |
| rtos-labs | `labs/rtos-labs` | FreeRTOS deep-dives |
| special-labs | `labs/special-labs` | Optional specials |
| demo-labs | `labs/demo-labs` | UI / widget demos |

**Switching tracks — do not re-run full enroll.** Port, board, IDF, and PATH from the first `init` stay.

| Goal | Command |
|------|---------|
| Work in another track today | `cd labs\rtos-labs` then `rbdev run 00` (cwd wins over sticky) |
| One-shot from workshop root | `rbdev run 00 --project labs/rtos-labs` |
| Make sticky default + `set-target` for a new track | `rbdev init --project rtos-labs -y` (optional; only if you want that track as the new default) |

First time you build a track that was never configured, if CMake/`set-target` complains, run the short `init --project … -y` line above (or `idf.py set-target esp32s3` inside that folder).

---

## Overview

| | |
|---|---|
| **Audience** | Students and instructors |
| **Toolchain** | ESP-IDF **v6.1** installed on the PC (Path A) |
| **Boards** | `ws28p` (default) or `es3c28p` — target **`esp32s3`** |
| **CLI** | In-repo `rbdev/<os>/` — Cursor PATH via `.vscode/settings.json`; `init` also adds user PATH |
| **Config** | Gitignored `.rbdev.toml` (port, board, IDF path, project, target) |
| **Full CLI** | [`rbdev/README.md`](rbdev/README.md) |

### Per-OS cheat sheet

| | Windows | macOS | Linux |
|---|---|---|---|
| First-run binary | `rbdev/windows/rbdev.exe` | `rbdev/mac/rbdev` | `rbdev/linux/rbdev` |
| After PATH | `rbdev …` | `rbdev …` | `rbdev …` |
| Serial | `COMx` | `/dev/cu.*` | `/dev/ttyACM*` / `/dev/ttyUSB*` |
| Extra once | — | `chmod +x rbdev/mac/rbdev` + Gatekeeper Open | `chmod +x rbdev/linux/rbdev` + `dialout` |

---

## Pass criteria

| Check | Pass | Fail |
|-------|------|------|
| Tool | `rbdev 0.1.2`+ | Missing / very old binary |
| ESP-IDF | Path under workshop discovery / sticky | IDF not found |
| **IDF Python** | `.espressif/python_env/idf6.1_py…/` | System Python313 / Homebrew / Store |
| Packages | `rich_click OK` | Missing `rich_click` |
| Serial | At least one port | No ports |
| Sticky | Port + board after `init` | Flash port unset |
| Chip | Build log `target esp32s3` | `target esp32` → GPIO errors |

> System Python is wrong. `init` runs `install.bat` / `install.sh` only when the Espressif venv is missing/broken.

---

## Windows

### 1. Get the workshop

```powershell
git clone https://github.com/drsanti/firmware-esp32-s3-workshop.git
cd firmware-esp32-s3-workshop
```

Always use latest **`main`**.

### 2. `rbdev init` (once)

In Cursor (workshop root open), plain `rbdev init` is enough. Use the
path below only if `rbdev` is not found. When prompted for a project,
choose **`device-labs`** unless the instructor names another track.
Switching tracks later: [Lab tracks](#lab-tracks-why-init-asks-for-a-project) — usually just `cd`, not a full re-init.

```powershell
rbdev init
# fallback: .\rbdev\windows\rbdev.exe init
# scripted: rbdev init --port COM6 --board ws28p --project device-labs -y
```

What it does:

1. Finds ESP-IDF  
2. Repairs Espressif Python venv if needed (`install.bat`)  
3. Picks serial port (menu / `auto`) and project  
4. Writes `.rbdev.toml`  
5. `idf.py set-target esp32s3`  
6. Adds `rbdev/windows` to **user PATH** (skip with `--no-add-path`)

### 3. Every lab day

Open a **new** terminal, then (sticky project from workshop root is enough):

```powershell
rbdev run 00
# other track: cd labs\rtos-labs ; rbdev run 00
rbdev doctor
```

### 4. Troubleshooting

| Symptom | Fix |
|---------|-----|
| `rbdev: command not found` | Open the **workshop root** in Cursor (not a subfolder); new terminal tab; or `.\rbdev\windows\rbdev.exe …` / [`start-here.cmd`](start-here.cmd) |
| System Python / missing `rich_click` | Re-run `rbdev init` (or `install.bat` in IDF tree) |
| Bare `run 00` → OneDrive / Corey / nvm | Use `rbdev run 00` only |
| No `COMx` | Cable / driver; close other monitors |
| Wrong board | `rbdev config --board ws28p` or `es3c28p` |
| `target esp32` / GPIO errors | `rbdev init` again (set-target) for that project |
| Want another track | `cd labs\<track>` — not a full re-init ([Lab tracks](#lab-tracks-why-init-asks-for-a-project)) |

### Paste to class

```text
1) Open firmware-esp32-s3-workshop in Cursor
2) git pull
3) Once: rbdev init  (pick device-labs; fallback: rbdev\windows\rbdev.exe init)
4) New terminal tab → rbdev run 00
5) Other track: cd labs\rtos-labs → rbdev run 00
6) Do NOT run export.ps1 / bare "run"
```

---

## macOS

### 1. Get the workshop

```bash
git clone https://github.com/drsanti/firmware-esp32-s3-workshop.git
cd firmware-esp32-s3-workshop
chmod +x rbdev/mac/rbdev
```

Gatekeeper: right-click → **Open** `rbdev/mac/rbdev` once if blocked.

### 2. `rbdev init` (once)

```bash
./rbdev/mac/rbdev init
```

### 3. Every lab day

New terminal (so PATH / shell rc loads), then:

```bash
rbdev run 00
# other track: cd labs/rtos-labs && rbdev run 00
```

### 4. Troubleshooting

| Symptom | Fix |
|---------|-----|
| `rbdev: command not found` | New terminal after `init`, or `./rbdev/mac/rbdev …` |
| Gatekeeper block | Right-click → Open |
| No `/dev/cu.*` | Unplug/replug; prefer `/dev/cu.*` over `/dev/tty.*` |
| Missing `rich_click` | Re-run `rbdev init` |
| Want another track | `cd labs/<track>` — see [Lab tracks](#lab-tracks-why-init-asks-for-a-project) |

### Paste to class

```text
1) git pull
2) chmod +x rbdev/mac/rbdev
3) ./rbdev/mac/rbdev init   # pick device-labs
4) NEW terminal → rbdev run 00
5) Other track: cd labs/rtos-labs && rbdev run 00
```

---

## Linux

### 1. Get the workshop

```bash
git clone https://github.com/drsanti/firmware-esp32-s3-workshop.git
cd firmware-esp32-s3-workshop
chmod +x rbdev/linux/rbdev
```

### 2. Serial access (once per user)

```bash
sudo usermod -aG dialout "$USER"
# then log out and back in
```

### 3. `rbdev init` (once)

```bash
./rbdev/linux/rbdev init
```

### 4. Every lab day

```bash
rbdev run 00
# other track: cd labs/rtos-labs && rbdev run 00
```

### Paste to class

```text
1) git pull
2) chmod +x rbdev/linux/rbdev
3) sudo usermod -aG dialout $USER   # re-login
4) ./rbdev/linux/rbdev init   # pick device-labs
5) NEW terminal → rbdev run 00
6) Other track: cd labs/rtos-labs && rbdev run 00
```

---

## One-screen cheat sheet

```text
ESP32-S3 WORKSHOP — memorize these

  rbdev init                 once (pick device-labs)
  NEW TERMINAL TAB           so PATH / idf.py shim load
  rbdev doctor               when anything looks wrong
  rbdev run 00               select lab + build + flash
  rbdev run 00 --monitor     same + serial monitor
  rbdev config --port COMx   after re-plug / new USB port

  Switch track (no full re-init):
    cd labs\rtos-labs && rbdev run 00
    # or: rbdev run 00 --project labs/rtos-labs
    # optional sticky update: rbdev init --project rtos-labs -y

  idf.py <anything>          full ESP-IDF workflow (after init)
  rbdev idf -- <anything>    same, without relying on PATH shim

Do NOT: export.ps1 / export.sh / bare `run 00`
Shell: Cursor terminal or cmd is fine — PowerShell is NOT required
```

## `rbdev` commands (students)

| Command | Purpose |
|---------|---------|
| `rbdev init` | Once: venv repair, `.rbdev.toml`, set-target, user PATH + `idf.py` shim |
| `rbdev doctor` | Preflight (IDF Python, ports, sticky config) |
| `rbdev run 00` | Select `ex00`, build + flash (sticky project from workshop root OK) |
| `rbdev run 00 --monitor` | Same + serial monitor |
| `rbdev run` | List labs (exit 0) |
| `rbdev run 00 --select-only` | Only rewrite `main/main.c` (no build) |
| `rbdev config --show` | Print sticky `.rbdev.toml` values |
| `rbdev config --port COMx --board ws28p` | Change sticky port/board |

### Full ESP-IDF workflow (`idf.py` after `init`)

No `export.ps1` / `export.sh`. Shim loads IDF env, injects `-DBOARD`, and sticky `ESPPORT`.

| Need | Command |
|------|---------|
| Build / flash / monitor | Prefer `rbdev run …`; or `idf.py build` / `flash` / `monitor` |
| Menuconfig | `idf.py menuconfig` |
| Clean rebuild | `idf.py fullclean` then `idf.py build` |
| Erase chip | `idf.py erase-flash` |
| Size report | `idf.py size` / `size-components` |
| App-only flash | `idf.py app-flash` |
| Any other idf.py verb | `idf.py …` or `rbdev idf -- …` |

Advanced (same IDF env): `rbdev build` / `flash` / `monitor` / `reconfigure` — optional; students can ignore these.

Init flags: `-y`, `--port auto`, `--board`, `--project`, `--skip-install`, `--skip-set-target`, `--no-add-path`.

---

## Instructor playbook

1. Point students at **only** their OS section + the daily loop.  
2. Gate on `rbdev doctor` → Espressif venv + `rich_click OK`.  
3. Agree one board (`ws28p` / `es3c28p`).  
4. Prefer `git pull` over ZIP re-downloads.  
5. Remind: **new terminal after `init`**, never bare `run`, never `export.ps1`.  
   Default project = **device-labs**; switching tracks is `cd` (not full re-init).  
6. Maintainer smoke (no flash): from `rbdev` repo  
   `powershell -File .\scripts\smoke-classroom.ps1`  
7. Rebuild **mac/linux** CLIs on a Mac from branch `feat/rbdev-init-run` —  
   see [`rbdev` docs/ship-binaries.md](https://github.com/drsanti/rbdev/blob/feat/rbdev-init-run/docs/ship-binaries.md)  
   (section **Handoff: Windows → macOS**)

**TA green-light**

```text
rbdev init   (once, pick device-labs)
new terminal
rbdev doctor → .espressif/.../python + rich_click OK
rbdev run 00          # sticky project; other track = cd labs/<track>
idf.py --version      # shim works (optional)
```

---

## Related docs

| Doc | Contents |
|-----|----------|
| [README.md](README.md) | Repo overview + quick start |
| [rbdev/README.md](rbdev/README.md) | Full CLI notes |
| [labs/README.md](labs/README.md) | Lab track index |
| [ports/boards/README.md](ports/boards/README.md) | Board SKUs |
| [lib/README.md](lib/README.md) | `librobotos_fw.a` |
| [`.rbdev.toml.example`](.rbdev.toml.example) | Sticky config shape |
| [`start-here.cmd`](start-here.cmd) | Windows optional: PATH for one cmd window (outside Cursor) |
| [`.vscode/settings.json`](.vscode/settings.json) | Cursor/VS Code: puts `rbdev` on integrated-terminal PATH |
| [`rbdev/user-manual/`](rbdev/user-manual/index.html) | HTML user manual (commands, idf.py, troubleshooting) |
| [`ebook/book/`](ebook/book/index.html) | HTML labs ebook (device / special / rtos / demo) |
