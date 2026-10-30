@echo off
setlocal EnableExtensions
set "ROOT=%~dp0"
set "EXE=%ROOT%rbdev\windows\rbdev.exe"
if not exist "%EXE%" (
  echo rbdev binary not found: %EXE%
  echo Copy rbdev.exe into rbdev\windows\ — see rbdev\README.md
  exit /b 1
)

if /I "%~1"=="doctor" (
  echo.%*| findstr /I /C:"--workshop" >nul
  if errorlevel 1 (
    "%EXE%" doctor --workshop "%ROOT%." %2 %3 %4 %5 %6 %7 %8 %9
    exit /b %ERRORLEVEL%
  )
)

"%EXE%" %*
exit /b %ERRORLEVEL%
