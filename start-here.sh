#!/usr/bin/env bash
# Optional PATH helper for macOS / Linux (outside Cursor).
# Usage:
#   ./start-here.sh          # new shell with rbdev on PATH
#   source ./start-here.sh   # same PATH in the current shell (bash or zsh)

set -eo pipefail

# Resolve this file's directory under bash or zsh (execute or source).
_SOURCED=0
if [ -n "${BASH_SOURCE-}" ]; then
  _ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
  if [ "${BASH_SOURCE[0]}" != "$0" ]; then
    _SOURCED=1
  fi
elif [ -n "${ZSH_VERSION-}" ]; then
  # eval so bash does not parse zsh ${(%):-%x} when this file is executed via shebang
  eval '_ROOT="$(cd "$(dirname "${(%):-%x}")" && pwd)"'
  case "${ZSH_EVAL_CONTEXT-}" in
    *:file*) _SOURCED=1 ;;
  esac
else
  _ROOT="$(cd "$(dirname "$0")" && pwd)"
fi

cd "$_ROOT"

case "$(uname -s)" in
  Darwin) OS_DIR=mac ;;
  Linux)  OS_DIR=linux ;;
  *)
    echo "Unsupported host $(uname -s) — use start-here.cmd on Windows." >&2
    unset _ROOT _SOURCED OS_DIR BIN
    return 1 2>/dev/null || exit 1
    ;;
esac

BIN="$_ROOT/rbdev/$OS_DIR"
if [ ! -x "$BIN/rbdev" ]; then
  echo "Missing executable: $BIN/rbdev" >&2
  echo "Clone/pull the workshop so rbdev/$OS_DIR/rbdev is present." >&2
  unset _ROOT _SOURCED OS_DIR BIN
  return 1 2>/dev/null || exit 1
fi

# Session PATH only (no Cursor restart, no one-liner).
export PATH="$BIN:$PATH"

echo
echo " firmware-esp32-s3-workshop"
echo " ---------------------------"
echo " Students: do NOT run export.sh / activate ESP-IDF."
echo " rbdev loads IDF for you."
echo
echo " First time only (workshop root):"
echo "   rbdev init          (pick device-labs)"
echo
echo " Every lab day:"
echo "   rbdev run 00        (sticky project; or cd labs/<track>)"
echo "   rbdev run 00 --monitor"
echo
echo " Switch track (no full re-init):"
echo "   cd labs/rtos-labs && rbdev run 00"
echo

if [ -d "labs/device-labs" ]; then
  echo " Workshop root ready — try:  rbdev run 00"
  echo " (Or: cd labs/device-labs)"
  echo
else
  echo " Missing labs/device-labs — stay here and run init first."
  echo
fi

if [ "$_SOURCED" -eq 1 ]; then
  unset _ROOT _SOURCED OS_DIR BIN
  return 0
fi

unset _ROOT _SOURCED OS_DIR BIN
exec "${SHELL:-bash}"
