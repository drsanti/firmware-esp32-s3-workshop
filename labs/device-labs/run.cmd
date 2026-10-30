@echo off
setlocal
REM Bypass Restricted execution policy (PowerShell prefers .ps1 over .cmd).
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0_run.ps1" %*
exit /b %ERRORLEVEL%
