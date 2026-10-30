<div align="center">

# Classroom setup

**Get every student laptop to a green `rbdev doctor` before the first lab.**

[![Audience](https://img.shields.io/badge/audience-students%20%7C%20instructors-blue.svg)](#)
[![Platform](https://img.shields.io/badge/platform-Windows-0078D6.svg?logo=windows&logoColor=white)](#)
[![ESP-IDF](https://img.shields.io/badge/ESP--IDF-v6.1-brightgreen.svg)](https://docs.espressif.com/projects/esp-idf/)
[![CLI](https://img.shields.io/badge/CLI-rbdev%200.1.1+-informational.svg)](rbdev/README.md)
[![Board](https://img.shields.io/badge/board-ws28p%20%7C%20es3c28p-orange.svg)](ports/boards/README.md)
[![Last commit](https://img.shields.io/github/last-commit/drsanti/firmware-esp32-s3-workshop)](https://github.com/drsanti/firmware-esp32-s3-workshop/commits/main)

[Ready check](#1-what-ready-looks-like) ·
[First-time setup](#2-student-steps-first-time-on-this-pc) ·
[Troubleshooting](#4-common-failures) ·
[Instructor](#5-instructor-playbook) ·
[Paste to class](#6-message-to-paste-to-students)

</div>

---

## Table of contents

1. [What “ready” looks like](#1-what-ready-looks-like)
2. [Student steps (first time on this PC)](#2-student-steps-first-time-on-this-pc)
3. [Every new PowerShell window](#3-every-new-powershell-window)
4. [Common failures](#4-common-failures)
5. [Instructor playbook](#5-instructor-playbook)
6. [Message to paste to students](#6-message-to-paste-to-students)
7. [Related docs](#7-related-docs)

---

## Overview

| | |
|---|---|
| **Audience** | Students and instructors |
| **OS** | Windows (course Path A) |
| **Toolchain** | ESP-IDF **v6.1** |
| **Success** | One green `rbdev doctor`, then `.\run 00` builds and flashes |
| **Full CLI guide** | [`rbdev/README.md`](rbdev/README.md) |

---

## 1. What “ready” looks like

From the **workshop root** (the folder that contains `rbdev/` and `labs/`):

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\rbdev\windows\rbdev.exe version
.\rbdev\windows\rbdev.exe doctor
```

| Check | Pass | Fail |
|-------|------|------|
| Tool | `rbdev 0.1.1` or newer | Missing / very old `rbdev.exe` |
| ESP-IDF | Path such as `C:\esp\v6.1\esp-idf` | IDF not found |
| **IDF Python** | `…\.espressif\python_env\idf6.1_py…\Scripts\python.exe` | `…\Python313\python.exe` or Microsoft Store Python |
| Packages | `IDF Python packages: rich_click OK` | Missing `rich_click` |
| Serial | At least one `COMx` | No ports listed |
| Sticky config | Port + board after `rb config` | Warn: flash port not configured |

> **Hard rule:** if Python is under `AppData\Local\Programs\Python\…`, stop.
> That is **system Python**. Labs need the **Espressif venv** created by `install.bat`.

---

## 2. Student steps (first time on this PC)

### A. Get the workshop

```powershell
# Prefer git (easy updates):
git clone https://github.com/drsanti/firmware-esp32-s3-workshop.git
cd firmware-esp32-s3-workshop

# Or: GitHub → Code → Download ZIP, then unzip
```

Always use the latest **`main`**. Old ZIPs cause Windows `_run.ps1` parse errors
and ship outdated `rbdev.exe`.

### B. Install / repair ESP-IDF Python (once per PC)

```powershell
cd C:\esp\v6.1\esp-idf          # your path — doctor prints it
.\install.bat                   # several minutes — creates the venv
.\export.ps1
idf.py --version
```

Confirm the venv folder exists:

```powershell
dir $env:USERPROFILE\.espressif\python_env
```

You should see a folder like `idf6.1_py3.11_env` (exact Python version may differ).

### C. Doctor, config, and first flash

```powershell
cd <workshop-root>
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass

.\rbdev\windows\rbdev.exe doctor
# expect: .espressif\python_env\...\python.exe  AND  rich_click OK

cd labs\device-labs
.\rb config --port COMx --board ws28p   # your port from doctor; or es3c28p
.\run 00
```

If `.\run 00` builds and flashes, this machine is ready for device, rtos,
special, and demo labs (same `.\run NN` pattern).

---

## 3. Every new PowerShell window

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

No admin rights required. Applies to **this window only**. After that,
`.\rb` and `.\run` work.

---

## 4. Common failures

| Symptom | Cause | Fix |
|---------|--------|-----|
| `python=…\Python313\python.exe` + missing `rich_click` | No Espressif venv | [§2B](#b-install--repair-esp-idf-python-once-per-pc) `install.bat` |
| `No module named 'rich_click'` | Incomplete venv | Run `install.bat` again |
| `not digitally signed` | Execution policy | [§3](#3-every-new-powershell-window) Bypass |
| `_run.ps1` Unexpected token / `â€"` | Old workshop ZIP | Re-clone or re-download `main` |
| No `COMx` | Cable / driver | Device Manager → Ports; try another cable |
| Wrong board UI / flash fail | Board mismatch | `.\rb config --board ws28p` or `es3c28p` |

**Do not** install into system Python:

```powershell
python -m pip install rich_click   # wrong — does not fix IDF
```

Only the interpreter under `.espressif\python_env\` counts.

Optional repair **inside the venv only** (if that folder already exists):

```powershell
& "$env:USERPROFILE\.espressif\python_env\idf6.1_py3.11_env\Scripts\python.exe" -m pip install rich_click
```

Adjust the `idf6.1_py…` folder name to match what you saw in [§2B](#b-install--repair-esp-idf-python-once-per-pc).

---

## 5. Instructor playbook

1. **Gate the room** — no lab until doctor shows Espressif venv + `rich_click OK`.
2. Walk the room once; glance at the `IDF Python:` line on each laptop.
3. Agree one board flag for the class (`ws28p` or `es3c28p`) unless hardware is mixed.
4. Prefer `git pull` mid-course over repeated ZIP downloads.
5. Keep a spare USB cable and one known-good board for port confusion.
6. If several people fail at once, paste [§6](#6-message-to-paste-to-students) into the class chat.

**TA green-light checklist**

```text
doctor → IDF Python under .espressif\python_env\...\Scripts\python.exe
       → rich_click OK
       → NOT Python313
then:  rb config --port COMx --board <sku>
then:  .\run 00
```

---

## 6. Message to paste to students

```text
1) Update workshop (git pull OR new ZIP from GitHub main)
2) Every PowerShell window:
   Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
3) First time on this PC:
   cd C:\esp\v6.1\esp-idf
   .\install.bat
   .\export.ps1
4) Workshop root:
   .\rbdev\windows\rbdev.exe doctor
   → must show .espressif\python_env\...\python.exe and rich_click OK
   → NOT AppData\Local\Programs\Python\Python313
5) labs\device-labs:
   .\rb config --port COMx --board ws28p
   .\run 00
```

---

## 7. Related docs

| Doc | Contents |
|-----|----------|
| [README.md](README.md) | Quick start and short Windows troubleshooting |
| [rbdev/README.md](rbdev/README.md) | Full student CLI and `rich_click` notes |
| [labs/README.md](labs/README.md) | Lab track index |
| [lib/README.md](lib/README.md) | `librobotos_fw.a` and public headers |
