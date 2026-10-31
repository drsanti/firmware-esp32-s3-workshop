#Requires -Version 5.1
<#
.SYNOPSIS
  Smoke-check Labs Ebook: rebuild, coverage, essential HTML markers.
#>
$ErrorActionPreference = 'Stop'
$EbookDir = Split-Path $PSScriptRoot -Parent
$WorkshopRoot = Split-Path $EbookDir -Parent
$LabsDir = Join-Path $WorkshopRoot 'labs'
$fail = 0

function Ok($m) { Write-Host "PASS  $m" -ForegroundColor Green }
function Bad($m) { Write-Host "FAIL  $m" -ForegroundColor Red; $script:fail++ }

Write-Host "=== build ==="
& (Join-Path $PSScriptRoot 'build.ps1')
if ($LASTEXITCODE -ne 0 -and -not $?) { Bad 'build.ps1 failed' }

$index = Join-Path $EbookDir 'book\index.html'
if (Test-Path $index) { Ok 'book/index.html exists' } else { Bad 'book/index.html missing' }

$gen = Join-Path $EbookDir 'src\generated'
$manifest = Join-Path $gen 'manifest.json'
if (Test-Path $manifest) {
  $m = Get-Content $manifest -Raw | ConvertFrom-Json
  Ok "manifest pages=$($m.pages)"
} else {
  Bad 'manifest.json missing'
}

# Every ex* folder has a generated index.md
foreach ($track in @('device-labs', 'special-labs', 'rtos-labs', 'demo-labs')) {
  $part = $track.Replace('-labs', '')
  if ($track -eq 'device-labs') { $part = 'device' }
  if ($track -eq 'special-labs') { $part = 'special' }
  if ($track -eq 'rtos-labs') { $part = 'rtos' }
  if ($track -eq 'demo-labs') { $part = 'demo' }
  foreach ($ex in @(Get-ChildItem (Join-Path $LabsDir $track) -Directory -Filter 'ex*')) {
    $md = Join-Path $gen "$part\$($ex.Name)\index.md"
    $html = Join-Path $EbookDir "book\generated\$part\$($ex.Name)\index.html"
    if (-not (Test-Path $md)) { Bad "missing staged $track/$($ex.Name)"; continue }
    if (-not (Test-Path $html)) { Bad "missing html $track/$($ex.Name)"; continue }
  }
}
Ok 'all exercises have staged MD + HTML'

$sample = Join-Path $EbookDir 'book\generated\device\ex00-hello-device\index.html'
if (Test-Path $sample) {
  $t = Get-Content $sample -Raw
  if ($t -match 'ebook-run' -and $t -match 'rbdev run') { Ok 'run callout present on device ex00' }
  else { Bad 'run callout missing on device ex00' }
} else {
  Bad 'sample device ex00 html missing'
}

$step = Join-Path $EbookDir 'book\generated\device\ex01-spirit-level\step-03-busy-loop.html'
if (Test-Path $step) {
  $t = Get-Content $step -Raw
  if ($t -match 'broken on purpose' -or $t -match 'ebook-badge warn') {
    Ok 'broken-on-purpose badge on busy step'
  } else {
    Bad 'expected warn badge on step-03-busy-loop'
  }
} else {
  Bad 'step-03-busy-loop html missing'
}

# INSTRUCTOR.md must not appear in SUMMARY
$sum = Get-Content (Join-Path $EbookDir 'src\SUMMARY.md') -Raw
if ($sum -match 'INSTRUCTOR') { Bad 'INSTRUCTOR.md leaked into SUMMARY' }
else { Ok 'no INSTRUCTOR.md in SUMMARY' }

if ($fail -eq 0) {
  Write-Host ''
  Write-Host 'ALL EBOOK SMOKE CHECKS PASSED' -ForegroundColor Green
  exit 0
}
Write-Host ''
Write-Host "$fail EBOOK SMOKE CHECK(S) FAILED" -ForegroundColor Red
exit 1
