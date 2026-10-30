# Classroom setup — make every student machine work

**Audience:** students + instructors  
**Goal:** one green `rbdev doctor` before any `.\run` lab.

Use this checklist on **Windows** with ESP-IDF **v6.1** (course Path A).
Full CLI guide: [`rbdev/README.md`](rbdev/README.md).

---

## 1. What “ready” looks like

From the **workshop root** (folder that contains `rbdev/` and `labs/`):

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\rbdev\windows\rbdev.exe version
.\rbdev\windows\rbdev.exe doctor
```

| Check | Must see | Fail if you see |
|-------|----------|-----------------|
| Tool | `rbdev 0.1.1` (or newer) | very old binary / missing `rbdev.exe` |
| ESP-IDF | system path e.g. `C:\esp\v6.1\esp-idf` | IDF not found |
| **IDF Python** | `…\.espressif\python_env\idf6.1_py…\Scripts\python.exe` | `…\Python313\python.exe` or Microsoft Store Python |
| Packages | `IDF Python packages: rich_click OK` | missing `rich_click` |
| Serial | at least one `COMx` | no ports |
| Sticky config | port + board after `rb config` | warn: flash port not configured |

**Rule:** if Python is under `AppData\Local\Programs\Python\…`, stop.
That is **system Python**. Labs need the **Espressif venv** from `install.bat`.

---

## 2. Student steps (first time on this PC)

### A. Get the workshop

```powershell
# Prefer git (easy updates):
git clone https://github.com/drsanti/firmware-esp32-s3-workshop.git
cd firmware-esp32-s3-workshop

# Or download ZIP from GitHub → Code → Download ZIP, then unzip
```

Always use **latest `main`**. Old ZIPs break on Windows (`_run.ps1` parse errors,
old `rbdev.exe`).

### B. Install / repair ESP-IDF Python (once per PC)

```powershell
cd C:\esp\v6.1\esp-idf          # YOUR path — doctor prints it
.\install.bat                   # several minutes — creates the venv
.\export.ps1
idf.py --version
```

Confirm the venv folder exists:

```powershell
dir $env:USERPROFILE\.espressif\python_env
```

You should see a folder like `idf6.1_py3.11_env` (Python version may differ).

### C. Doctor + config + first flash

```powershell
cd <workshop-root>
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass

.\rbdev\windows\rbdev.exe doctor
# expect: .espressif\python_env\...\python.exe  AND  rich_click OK

cd labs\device-labs
.\rb config --port COMx --board ws28p   # YOUR port from doctor; or es3c28p
.\run 00
```

If `.\run 00` builds and flashes, this machine is good for device / rtos /
special / demo labs (same `.\run NN` pattern).

---

## 3. Every new PowerShell window

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

No admin. Applies to **this window only**. Then `.\rb` / `.\run` work.

---

## 4. Common failures (fix)

| Symptom | Cause | Fix |
|---------|--------|-----|
| `python=…\Python313\python.exe` + missing `rich_click` | No Espressif venv | §2B `install.bat` |
| `No module named 'rich_click'` | Incomplete venv | `install.bat` again |
| `not digitally signed` | Execution policy | §3 Bypass |
| `_run.ps1` Unexpected token / `â€"` | Old workshop ZIP | Re-clone / re-download `main` |
| No `COMx` | Cable / driver | Device Manager → Ports; try another cable |
| Wrong board UI / flash fail | Board mismatch | `.\rb config --board ws28p` or `es3c28p` |

**Do not** run:

```powershell
python -m pip install rich_click
```

into system Python313. That does **not** fix IDF. Only the venv under
`.espressif\python_env\` counts.

Optional repair **inside the venv only** (if the folder already exists):

```powershell
& "$env:USERPROFILE\.espressif\python_env\idf6.1_py3.11_env\Scripts\python.exe" -m pip install rich_click
```

(Adjust the `idf6.1_py…` folder name to match `dir` in §2B.)

---

## 5. Instructor playbook

1. **Gate the room:** no lab until doctor shows Espressif venv + `rich_click OK`.
2. Walk once and glance at the `IDF Python:` line on each laptop.
3. Agree one board flag for the class (`ws28p` or `es3c28p`) unless hardware is mixed.
4. Prefer `git pull` mid-course over many ZIP re-downloads.
5. Keep a spare USB cable and one known-good board for port confusion.
6. Paste §6 to the class chat if several people fail at once.

Pasteable “green light” for TAs:

```text
doctor → IDF Python under .espressif\python_env\...\Scripts\python.exe
       → rich_click OK
       → NOT Python313
then: rb config --port COMx --board <sku>
then: .\run 00
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
| [README.md](README.md) | Quick start + short Windows troubleshooting |
| [rbdev/README.md](rbdev/README.md) | Full student CLI / `rich_click` section |
| [labs/README.md](labs/README.md) | Lab track index |
| [lib/README.md](lib/README.md) | `librobotos_fw.a` + headers |
