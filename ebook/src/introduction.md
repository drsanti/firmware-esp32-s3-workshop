# Labs Ebook

This site is the **reading and planning** companion for the ESP32-S3 workshop
labs. The Markdown labsheets under `labs/` remain the source of truth; this
HTML is **generated** from those READMEs so it cannot silently drift into a
second curriculum.

## How to use it

| Mode | What to open |
|------|----------------|
| **Bench (flash / debug)** | Cursor + lab folder README next to the code |
| **Read / plan / review** | This ebook (`book/index.html`) |
| **Tooling (`rbdev`, PATH, doctor)** | [rbdev User Manual](../../rbdev/user-manual/index.html) |

## Four tracks (four ESP-IDF projects)

| Track | Role | Select lab |
|-------|------|------------|
| **device-labs** | Core curriculum (ex00–06 + steps) | `rbdev run NN` then pick step in `exNN.c` |
| **special-labs** | Optional FreeRTOS specials (ex01–06 + steps) | same pattern |
| **rtos-labs** | Deep-dives + game capstones (ex00–15) | `rbdev run NN` |
| **demo-labs** | UI / widget / wire catalog (ex01–17) | `rbdev run NN` |

After workshop-root `rbdev init` (usually pick **device-labs**):

```text
rbdev run 00
# other track:
cd labs/rtos-labs
rbdev run 00
```

See [Reading order](reading-order.md) and [rbdev daily workflow](../../rbdev/user-manual/daily-workflow.html).

## Rebuild this ebook

From the workshop root (requires `mdbook` on PATH):

```powershell
pwsh -File .\ebook\scripts\build.ps1
```

Do **not** edit files under `src/generated/` by hand — re-run the build after
changing a labsheet README.
