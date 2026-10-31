@echo off
setlocal EnableExtensions
cd /d "%~dp0"

rem Always put shipped rbdev on PATH for this window (no Cursor restart, no one-liner).
set "Path=%~dp0rbdev\windows;%Path%"

echo.
echo  firmware-esp32-s3-workshop
echo  ---------------------------
echo  Students: do NOT run export.ps1 / activate ESP-IDF.
echo  rbdev loads IDF for you.
echo.
echo  First time only (workshop root):
echo    rbdev init          ^(pick device-labs^)
echo.
echo  Every lab day:
echo    rbdev run 00        ^(sticky project; or cd labs\^<track^>^)
echo    rbdev run 00 --monitor
echo.
echo  Switch track ^(no full re-init^):
echo    cd labs\rtos-labs ^& rbdev run 00
echo.

if exist "labs\device-labs" (
  echo  Workshop root ready - try:  rbdev run 00
  echo  ^(Or: cd labs\device-labs^)
  echo.
  cmd /k
) else (
  echo  Missing labs\device-labs - stay here and run init first.
  cmd /k
)
