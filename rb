#!/usr/bin/env bash
# Forward to rbdev under rbdev/mac or rbdev/linux.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"

case "$(uname -s)" in
  Darwin) BIN="$ROOT/rbdev/mac/rbdev" ;;
  Linux)  BIN="$ROOT/rbdev/linux/rbdev" ;;
  MINGW*|MSYS*|CYGWIN*) BIN="$ROOT/rbdev/windows/rbdev.exe" ;;
  *)
    echo "unsupported OS: $(uname -s)" >&2
    exit 1
    ;;
esac

if [[ ! -x "$BIN" && ! -f "$BIN" ]]; then
  echo "rbdev binary not found: $BIN" >&2
  echo "Copy it into rbdev/<os>/ — see rbdev/README.md" >&2
  exit 1
fi

args=("$@")
if [[ ${#args[@]} -ge 1 && "${args[0]}" == "doctor" ]]; then
  has_ws=0
  for a in "${args[@]:1}"; do
    case "$a" in
      --workshop|--workshop=*) has_ws=1; break ;;
    esac
  done
  if [[ $has_ws -eq 0 ]]; then
    args+=( --workshop "$ROOT" )
  fi
fi

exec "$BIN" "${args[@]}"
