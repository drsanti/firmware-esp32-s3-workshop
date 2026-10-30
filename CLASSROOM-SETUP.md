<div align="center">

# Classroom setup

**Get every student laptop to a green `rbdev doctor` before the first lab.**

[![Audience](https://img.shields.io/badge/audience-students%20%7C%20instructors-blue.svg)](#)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-informational.svg)](#)
[![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v6.1-brightgreen.svg)](https://docs.espressif.com/projects/esp-idf/)
[![CLI](https://img.shields.io/badge/CLI-rbdev%200.1.1+-informational.svg)](rbdev/README.md)
[![Board](https://img.shields.io/badge/board-ws28p%20%7C%20es3c28p-orange.svg)](ports/boards/README.md)
[![Last commit](https://img.shields.io/github/last-commit/drsanti/firmware-esp32-s3-workshop)](https://github.com/drsanti/firmware-esp32-s3-workshop/commits/main)

**Jump to your OS:**
[Windows](#windows) ·
[macOS](#macos) ·
[Linux](#linux)

[Ready checklist](#what-ready-looks-like) ·
[Instructor](#instructor-playbook) ·
[Related docs](#related-docs)

</div>

---

## Table of contents

1. [Overview](#overview)
2. [What “ready” looks like](#what-ready-looks-like)
3. [Windows](#windows)
4. [macOS](#macos)
5. [Linux](#linux)
6. [Instructor playbook](#instructor-playbook)
7. [Related docs](#related-docs)

---

## Overview

| | |
|---|---|
| **Audience** | Students and instructors |
| **OS** | Pick **one** section below and follow it end-to-end |
| **Toolchain** | ESP-IDF **v6.1** |
| **Boards** | `ws28p` (default) or `es3c28p` — both need target **`esp32s3`** |
| **Success** | Green `rbdev doctor`, then `run 00` builds and flashes |
| **Full CLI guide** | [`rbdev/README.md`](rbdev/README.md) |

### Per-OS cheat sheet

| | Windows | macOS | Linux |
|---|---|---|---|
| Binary | `rbdev\windows\rbdev.exe` | `rbdev/mac/rbdev` | `rbdev/linux/rbdev` |
| Lab wrapper | `.\rb` / `.\run` | `./rb` / `./run` | `./rb` / `./run` |
| Serial port | `COMx` | `/dev/cu.*` | `/dev/ttyACM*` / `/dev/ttyUSB*` |
| IDF install | `install.bat` | `./install.sh` | `./install.sh` |
| IDF export | `.\export.ps1` | `. ./export.sh` | `. ./export.sh` |
| IDF Python | `…\.espressif\python_env\…\Scripts\python.exe` | `~/.espressif/python_env/…/bin/python` | same as macOS |
| Extra once | Execution policy Bypass | `chmod +x` + Gatekeeper Open | `chmod +x` + `dialout` group |

---

## What “ready” looks like

From the **workshop root** (the folder that contains `rbdev/` and `labs/`):

| Check | Pass | Fail |
|-------|------|------|
| Tool | `rbdev 0.1.1` or newer | Missing / very old binary |
| ESP-IDF | Path such as `C:\esp\v6.1\esp-idf` or `~/esp/esp-idf` | IDF not found |
| **IDF Python** | Under `.espressif/python_env/idf6.1_py…/` | System Python (`Python313`, Homebrew, Microsoft Store, `/usr/bin/python3`) |
| Packages | `IDF Python packages: rich_click OK` | Missing `rich_click` |
| Serial | At least one `COMx` / `/dev/cu.*` / `/dev/ttyACM*` / `/dev/ttyUSB*` | No ports listed |
| Sticky config | Port + board after `rb config` | Warn: flash port not configured |
| Chip target | Build log: `target esp32s3` | Log says `target esp32` → GPIO errors |

> **Hard rule:** if Python is system Python, stop.
> Labs need the **Espressif venv** created by `install.bat` / `install.sh`.

---

# Windows

Self-contained guide for Windows (PowerShell). Do not mix these commands with macOS/Linux syntax.

## Windows — 1. Ready check

```powershell
cd <workshop-root>
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\rbdev\windows\rbdev.exe version
.\rbdev\windows\rbdev.exe doctor
```

Expect `IDF Python` under `.espressif\python_env\…` and `rich_click OK`.
If not, continue with the steps below.

## Windows — 2. Get the workshop

```powershell
git clone https://github.com/drsanti/firmware-esp32-s3-workshop.git
cd firmware-esp32-s3-workshop
```

Or: GitHub → **Code** → **Download ZIP**, then unzip. Always use latest **`main`**.
Old ZIPs cause `_run.ps1` parse errors and ship outdated `rbdev` binaries.

## Windows — 3. Install / repair ESP-IDF Python (once per machine)

Use **your** IDF path — `rbdev doctor` prints it when IDF is found.

```powershell
cd C:\esp\v6.1\esp-idf          # your path — doctor prints it
.\install.bat                   # several minutes — creates the venv
.\export.ps1
idf.py --version

dir $env:USERPROFILE\.espressif\python_env
```

You should see a folder like `idf6.1_py3.11_env` (exact Python version may differ).

Prefer **ESP-IDF PowerShell** from the Start Menu so `IDF_PATH` is set.

## Windows — 4. Doctor, config, set-target, first flash

```powershell
cd <workshop-root>
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass

.\rbdev\windows\rbdev.exe doctor
# expect: .espressif\python_env\...\python.exe  AND  rich_click OK

cd labs\rtos-labs   # or whichever lab: device-labs, special-labs, demos\demo-labs
.\rb config --port COMx --board ws28p   # your port from doctor; or es3c28p
.\rb shell
idf.py set-target esp32s3
exit
.\run 00
```

Confirm in the build log: `Building ESP-IDF components for target esp32s3`.

If `run 00` builds and flashes, this machine is ready for all lab tracks
(same `.\run NN` pattern).

## Windows — 5. Every new PowerShell window

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

No admin rights required. Applies to **this window only**. After that,
`.\rb` and `.\run` work.

## Windows — 6. Troubleshooting

| Symptom | Cause | Fix |
|---------|--------|-----|
| System Python + missing `rich_click` | No Espressif venv | Run [§3](#windows--3-install--repair-esp-idf-python-once-per-machine) `install.bat` |
| `No module named 'rich_click'` | Incomplete venv | Run `install.bat` again |
| `not digitally signed` | Execution policy | [§5](#windows--5-every-new-powershell-window) Bypass |
| `_run.ps1` Unexpected token / `â€"` | Old workshop ZIP | Re-clone or re-download `main` |
| No `COMx` | Cable / driver | Device Manager; close other serial monitors; try another cable |
| Wrong board UI / flash fail | Board mismatch | `.\rb config --board ws28p` or `es3c28p` |
| `GPIO_NUM_45` / `GPIO_NUM_48` undeclared; log says `target esp32` | Wrong chip target | Set target (below) |

**Wrong chip target (`esp32` vs `esp32s3`)**

```powershell
cd labs\rtos-labs   # or whichever lab
.\rb shell
idf.py set-target esp32s3
exit
.\run 00
```

**Do not** install into system Python:

```text
python -m pip install rich_click   # wrong — does not fix IDF
```

Optional repair **inside the Espressif venv only**:

```powershell
& "$env:USERPROFILE\.espressif\python_env\idf6.1_py3.11_env\Scripts\python.exe" -m pip install rich_click
```

Adjust the `idf6.1_py…` folder name to match what you saw in [§3](#windows--3-install--repair-esp-idf-python-once-per-machine).

## Windows — 7. Message to paste to students

**1) Update workshop** — `git pull` or new ZIP from GitHub `main`.

**2) Every PowerShell window**

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

**3) First time on this PC**

```powershell
cd C:\esp\v6.1\esp-idf
.\install.bat
.\export.ps1
```

**4) Workshop root — doctor**

```powershell
.\rbdev\windows\rbdev.exe doctor
```

Must show `.espressif\python_env\...\python.exe` and `rich_click OK`.  
Not `AppData\Local\Programs\Python\Python313`.

**5) Lab — config, set-target, flash**

```powershell
cd labs\rtos-labs
.\rb config --port COMx --board ws28p
.\rb shell
idf.py set-target esp32s3
exit
.\run 00
```

---

# macOS

Self-contained guide for macOS (zsh / bash Terminal or iTerm).

## macOS — 1. Ready check

```bash
cd <workshop-root>
chmod +x rb rbdev/mac/rbdev labs/*/rb labs/*/run demos/demo-labs/rb demos/demo-labs/run
./rbdev/mac/rbdev version
./rbdev/mac/rbdev doctor
```

Expect `IDF Python` under `~/.espressif/python_env/…` and `rich_click OK`.
If not, continue with the steps below.

## macOS — 2. Get the workshop

```bash
git clone https://github.com/drsanti/firmware-esp32-s3-workshop.git
cd firmware-esp32-s3-workshop
```

Or: GitHub → **Code** → **Download ZIP**, then unzip. Always use latest **`main`**.

Mark wrappers executable once after clone (or after a fresh ZIP):

```bash
chmod +x rb rbdev/mac/rbdev labs/*/rb labs/*/run demos/demo-labs/rb demos/demo-labs/run
```

First launch of `rbdev/mac/rbdev` may need **right-click → Open** (Gatekeeper).

## macOS — 3. Install / repair ESP-IDF Python (once per machine)

Use **your** IDF path — `rbdev doctor` prints it when IDF is found
(example: `/Users/you/esp/esp-idf`).

```bash
cd ~/esp/esp-idf                # your path — doctor prints it
./install.sh                    # several minutes — creates the venv
. ./export.sh
idf.py --version

ls ~/.espressif/python_env
```

You should see a folder like `idf6.1_py3.11_env` (exact Python version may differ).

## macOS — 4. Doctor, config, set-target, first flash

```bash
cd <workshop-root>
./rbdev/mac/rbdev doctor
# expect: ~/.espressif/python_env/.../bin/python  AND  rich_click OK

# List USB serial ports (prefer /dev/cu.* for flash):
ls /dev/cu.*

cd labs/rtos-labs   # or whichever lab: device-labs, special-labs, demos/demo-labs
./rb config --port /dev/cu.usbmodemXXXX --board ws28p   # port from ls or doctor
./rb shell
idf.py set-target esp32s3
exit
./run 00
```

Typical ports: `/dev/cu.usbmodemXXXX`, `/dev/cu.LX-10`, `/dev/cu.wchusbserial*`.
Prefer **`/dev/cu.*`** over `/dev/tty.*` for flash and monitor.

Confirm in the build log: `Building ESP-IDF components for target esp32s3`.

If `run 00` builds and flashes, this machine is ready for all lab tracks
(same `./run NN` pattern).

## macOS — 5. Every new terminal

New terminals do **not** need a special execution policy. Source IDF when you
open a plain shell without the Espressif env:

```bash
. ~/esp/esp-idf/export.sh        # your IDF path
```

Or set `IDF_PATH` / `RBDEV_IDF_PATH` if doctor cannot find IDF.

If `./run` fails with `permission denied` after a fresh ZIP:

```bash
chmod +x rb rbdev/mac/rbdev labs/*/rb labs/*/run demos/demo-labs/rb demos/demo-labs/run
```

## macOS — 6. Troubleshooting

| Symptom | Cause | Fix |
|---------|--------|-----|
| System Python + missing `rich_click` | No Espressif venv | Run [§3](#macos--3-install--repair-esp-idf-python-once-per-machine) `./install.sh` |
| `No module named 'rich_click'` | Incomplete venv | Run `./install.sh` again |
| “cannot be opened” / blocked | Gatekeeper | Right-click → **Open** `rbdev/mac/rbdev` once |
| `permission denied: ./run` | Missing executable bit | `chmod +x labs/*/run demos/demo-labs/run` |
| `zsh: command not found: .rb` | Windows syntax | Use `./rb` and `./run`, not `.\rb` |
| No `/dev/cu.*` board port | Cable / board | Unplug/replug; `ls /dev/cu.*`; close other monitors |
| Wrong board UI / flash fail | Board mismatch | `./rb config --board ws28p` or `es3c28p` |
| `GPIO_NUM_45` / `GPIO_NUM_48` undeclared; log says `target esp32` | Wrong chip target | Set target (below) |

**List / check serial ports**

```bash
ls /dev/cu.*
```

**Wrong chip target (`esp32` vs `esp32s3`)**

```bash
cd labs/rtos-labs   # or whichever lab
./rb shell
idf.py set-target esp32s3
exit
./run 00
```

**Do not** install into system Python:

```text
python3 -m pip install rich_click   # wrong — does not fix IDF
```

Optional repair **inside the Espressif venv only**:

```bash
~/.espressif/python_env/idf6.1_py3.11_env/bin/python -m pip install rich_click
```

Adjust the `idf6.1_py…` folder name to match what you saw in [§3](#macos--3-install--repair-esp-idf-python-once-per-machine).

## macOS — 7. Message to paste to students

**1) Update workshop** — `git pull` or new ZIP from GitHub `main`.

**2) Once after clone**

```bash
chmod +x rb rbdev/mac/rbdev labs/*/rb labs/*/run demos/demo-labs/rb demos/demo-labs/run
```

Gatekeeper: right-click → Open `rbdev/mac/rbdev` once if blocked.

**3) First time on this Mac**

```bash
cd ~/esp/esp-idf
./install.sh
. ./export.sh
```

**4) Workshop root — doctor**

```bash
./rbdev/mac/rbdev doctor
```

Must show `~/.espressif/python_env/.../bin/python` and `rich_click OK`.

**5) List ports, then lab — config, set-target, flash**

```bash
ls /dev/cu.*
cd labs/rtos-labs
./rb config --port /dev/cu.usbmodemXXXX --board ws28p
./rb shell
idf.py set-target esp32s3
exit
./run 00
```

---

# Linux

Self-contained guide for Linux (bash). Paths assume a typical desktop install.

## Linux — 1. Ready check

```bash
cd <workshop-root>
chmod +x rb rbdev/linux/rbdev labs/*/rb labs/*/run demos/demo-labs/rb demos/demo-labs/run
./rbdev/linux/rbdev version
./rbdev/linux/rbdev doctor
```

Expect `IDF Python` under `~/.espressif/python_env/…` and `rich_click OK`.
If not, continue with the steps below.

## Linux — 2. Get the workshop

```bash
git clone https://github.com/drsanti/firmware-esp32-s3-workshop.git
cd firmware-esp32-s3-workshop
```

Or: GitHub → **Code** → **Download ZIP**, then unzip. Always use latest **`main`**.

Mark wrappers executable once after clone (or after a fresh ZIP):

```bash
chmod +x rb rbdev/linux/rbdev labs/*/rb labs/*/run demos/demo-labs/rb demos/demo-labs/run
```

## Linux — 3. Serial port access (once per user)

If the port is missing or you get permission denied, add your user to
`dialout` (or `uucp` on some distros), then **log out and back in**:

```bash
sudo usermod -aG dialout "$USER"
```

List ports:

```bash
ls /dev/ttyACM* /dev/ttyUSB* 2>/dev/null
```

## Linux — 4. Install / repair ESP-IDF Python (once per machine)

Use **your** IDF path — `rbdev doctor` prints it when IDF is found.

```bash
cd ~/esp/esp-idf                # your path — doctor prints it
./install.sh                    # several minutes — creates the venv
. ./export.sh
idf.py --version

ls ~/.espressif/python_env
```

You should see a folder like `idf6.1_py3.11_env` (exact Python version may differ).

## Linux — 5. Doctor, config, set-target, first flash

```bash
cd <workshop-root>
./rbdev/linux/rbdev doctor
# expect: ~/.espressif/python_env/.../bin/python  AND  rich_click OK

cd labs/rtos-labs   # or whichever lab: device-labs, special-labs, demos/demo-labs
./rb config --port /dev/ttyACM0 --board ws28p   # or /dev/ttyUSB0 — from doctor
./rb shell
idf.py set-target esp32s3
exit
./run 00
```

Confirm in the build log: `Building ESP-IDF components for target esp32s3`.

If `run 00` builds and flashes, this machine is ready for all lab tracks
(same `./run NN` pattern).

## Linux — 6. Every new terminal

New terminals do **not** need a special execution policy. Source IDF when you
open a plain shell without the Espressif env:

```bash
. ~/esp/esp-idf/export.sh        # your IDF path
```

Or set `IDF_PATH` / `RBDEV_IDF_PATH` if doctor cannot find IDF.

If `./run` fails with `permission denied` after a fresh ZIP:

```bash
chmod +x rb rbdev/linux/rbdev labs/*/rb labs/*/run demos/demo-labs/rb demos/demo-labs/run
```

## Linux — 7. Troubleshooting

| Symptom | Cause | Fix |
|---------|--------|-----|
| System Python + missing `rich_click` | No Espressif venv | Run [§4](#linux--4-install--repair-esp-idf-python-once-per-machine) `./install.sh` |
| `No module named 'rich_click'` | Incomplete venv | Run `./install.sh` again |
| `permission denied: ./run` | Missing executable bit | `chmod +x labs/*/run demos/demo-labs/run` |
| Permission denied on `/dev/ttyACM*` | Not in `dialout` | [§3](#linux--3-serial-port-access-once-per-user), then re-login |
| No `/dev/ttyACM*` / `/dev/ttyUSB*` | Cable / udev | Unplug/replug; `ls /dev/ttyACM* /dev/ttyUSB*`; close other monitors |
| Wrong board UI / flash fail | Board mismatch | `./rb config --board ws28p` or `es3c28p` |
| `GPIO_NUM_45` / `GPIO_NUM_48` undeclared; log says `target esp32` | Wrong chip target | Set target (below) |

**Wrong chip target (`esp32` vs `esp32s3`)**

```bash
cd labs/rtos-labs   # or whichever lab
./rb shell
idf.py set-target esp32s3
exit
./run 00
```

**Do not** install into system Python:

```text
python3 -m pip install rich_click   # wrong — does not fix IDF
```

Optional repair **inside the Espressif venv only**:

```bash
~/.espressif/python_env/idf6.1_py3.11_env/bin/python -m pip install rich_click
```

Adjust the `idf6.1_py…` folder name to match what you saw in [§4](#linux--4-install--repair-esp-idf-python-once-per-machine).

## Linux — 8. Message to paste to students

**1) Update workshop** — `git pull` or new ZIP from GitHub `main`.

**2) Once after clone**

```bash
chmod +x rb rbdev/linux/rbdev labs/*/rb labs/*/run demos/demo-labs/rb demos/demo-labs/run
```

**3) Serial access (once per user, then re-login)**

```bash
sudo usermod -aG dialout $USER
```

**4) First time on this PC**

```bash
cd ~/esp/esp-idf
./install.sh
. ./export.sh
```

**5) Workshop root — doctor**

```bash
./rbdev/linux/rbdev doctor
```

Must show `~/.espressif/python_env/.../bin/python` and `rich_click OK`.

**6) Lab — config, set-target, flash**

```bash
cd labs/rtos-labs
./rb config --port /dev/ttyACM0 --board ws28p
./rb shell
idf.py set-target esp32s3
exit
./run 00
```

---

# Instructor playbook

1. **Gate the room** — no lab until doctor shows Espressif venv + `rich_click OK`.
2. Walk the room once; glance at the `IDF Python:` line on each laptop.
3. Agree one board flag for the class (`ws28p` or `es3c28p`) unless hardware is mixed.
4. Prefer `git pull` mid-course over repeated ZIP downloads.
5. Keep a spare USB cable and one known-good board for port confusion.
6. Point students at **only** their OS section: [Windows](#windows) · [macOS](#macos) · [Linux](#linux).
7. If several people fail at once, paste the matching **Message to paste** block into the class chat.
8. Remind everyone: first build in a lab folder needs `idf.py set-target esp32s3`.

**TA green-light checklist**

```text
doctor → IDF Python under .espressif/python_env/...
       → rich_click OK
       → NOT system Python (Python313 / Homebrew / /usr/bin/python3 alone)
then:  rb config --port <PORT> --board <sku>
then:  idf.py set-target esp32s3   (via rb shell)
then:  run 00
```

| OS | Port example | Binary | Paste block |
|----|--------------|--------|-------------|
| Windows | `COM6` | `rbdev\windows\rbdev.exe` | [§7](#windows--7-message-to-paste-to-students) |
| macOS | `/dev/cu.usbmodemXXXX` | `rbdev/mac/rbdev` | [§7](#macos--7-message-to-paste-to-students) |
| Linux | `/dev/ttyACM0` | `rbdev/linux/rbdev` | [§8](#linux--8-message-to-paste-to-students) |

---

## Related docs

| Doc | Contents |
|-----|----------|
| [README.md](README.md) | Quick start and troubleshooting (all OS) |
| [rbdev/README.md](rbdev/README.md) | Full student CLI and `rich_click` notes |
| [labs/README.md](labs/README.md) | Lab track index |
| [ports/boards/README.md](ports/boards/README.md) | Board SKUs and `set-target esp32s3` |
| [lib/README.md](lib/README.md) | `librobotos_fw.a` and public headers |
