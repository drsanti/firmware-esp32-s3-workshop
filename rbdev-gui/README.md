# rbdev GUI — optional desktop companion

Same engine as the CLI (`rbdev/windows/rbdev.exe`), with buttons for labs.
You do **not** need the private `rbdev` tool repo.

| Your OS | In this folder |
|---------|----------------|
| Windows x64 | `windows/rbdev-gui-setup.exe` (installer) |
| Windows x64 | `windows/rbdev-gui.exe` (portable — no install) |

macOS / Linux GUI builds are not shipped here yet — use the CLI under `rbdev/mac` or `rbdev/linux`.

## Install (recommended)

1. Run `windows/rbdev-gui-setup.exe`.
2. If Windows blocks it: **More info → Run anyway**.
3. Open **rbdev** from the Start menu.
4. Need WebView2? Install [Microsoft Edge WebView2 Runtime](https://developer.microsoft.com/microsoft-edge/webview2/) if the app won’t open.

## Portable (USB / no install)

Double-click `windows/rbdev-gui.exe` (WebView2 still required).

## Day-1 use

1. Top bar: **Port** · **Board** · **Project** → **Save defaults**.
2. Once: **Setup & tools → Doctor** (and **Init** on a new PC).
3. **Workshop → Labs / Run** → **Build** / **Flash** / **Run** / **Monitor** on a lab row.
4. **Stop job** (or Esc) ends monitor / long jobs.

CLI twin (always available in this workshop):

```powershell
.\rbdev\windows\rbdev.exe run 00 --monitor
```

## Maintainers

Rebuild from the private tool repo, then:

```powershell
cd ..\rbdev
npm run tauri build --prefix apps\rbdev-gui
.\scripts\install-to-workshops.ps1 -SkipBuild   # also syncs GUI when release artifacts exist
```

Or copy manually into `rbdev-gui/windows/` as `rbdev-gui-setup.exe` + `rbdev-gui.exe`.
