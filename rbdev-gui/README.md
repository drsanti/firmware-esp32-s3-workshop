# rbdev GUI — optional desktop companion

Same engine as the CLI (`rbdev/<os>/rbdev`), with buttons for labs.
You do **not** need the private `rbdev` tool repo.

| Your OS | In this folder |
|---------|----------------|
| Windows x64 | `windows/rbdev-gui-setup.exe` (installer) |
| Windows x64 | `windows/rbdev-gui.exe` (portable — no install) |
| macOS arm64 | `mac/rbdev.app` (drag to Applications, or open in place) |
| macOS arm64 | `mac/rbdev_0.1.3_aarch64.dmg` (disk image) |
| Linux | Not shipped yet — use `rbdev/linux/rbdev` |

## Windows — install (recommended)

1. Run `windows/rbdev-gui-setup.exe`.
2. If Windows blocks it: **More info → Run anyway**.
3. Open **rbdev** from the Start menu.
4. Need WebView2? Install [Microsoft Edge WebView2 Runtime](https://developer.microsoft.com/microsoft-edge/webview2/) if the app won’t open.

## Windows — portable (USB / no install)

Double-click `windows/rbdev-gui.exe` (WebView2 still required).

## macOS

```bash
open rbdev-gui/mac/rbdev.app
# or mount:
open rbdev-gui/mac/rbdev_0.1.3_aarch64.dmg
```

First launch may need **right-click → Open** (Gatekeeper) until the build is notarized.

## Day-1 use

1. Top bar: **Port** · **Board** · **Project** → **Save defaults**.
2. Once: **Setup & tools → Doctor** (and **Init** on a new PC).
3. **Workshop → Labs / Run** → **Build** / **Flash** / **Run** / **Monitor** on a lab row.
4. **Stop job** (or Esc) ends monitor / long jobs.

CLI twin (always available in this workshop):

```powershell
.\rbdev\windows\rbdev.exe run 00 --monitor
```

```bash
./rbdev/mac/rbdev run 00 --monitor   # Linux: ./rbdev/linux/rbdev …
```

## Maintainers

Rebuild from the private tool repo, then sync:

```powershell
# Windows GUI + CLI
cd ..\rbdev
npm run tauri build --prefix apps\rbdev-gui
.\scripts\install-to-workshops.ps1 -SkipBuild
```

```bash
# macOS GUI + CLI
cd ../rbdev
npm run tauri build --prefix apps/rbdev-gui
./scripts/install-to-workshops.sh
# copy GUI into the public workshop:
mkdir -p ../firmware-esp32-s3-workshop/rbdev-gui/mac
cp -R target/release/bundle/macos/rbdev.app \
  ../firmware-esp32-s3-workshop/rbdev-gui/mac/
cp target/release/bundle/dmg/rbdev_*_aarch64.dmg \
  ../firmware-esp32-s3-workshop/rbdev-gui/mac/
```
