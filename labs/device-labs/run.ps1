# Select a device-lab (exNN) in main/main.c, then build + flash.
# Usage:
#   .\run 00
#   .\run 01
#   .\run 6
# Optional: .\run 00 monitor   (or --monitor) — build + flash + monitor

$ErrorActionPreference = "Stop"
$LabRoot = $PSScriptRoot

function Normalize-LabId([string] $Raw) {
    $s = $Raw.Trim()
    if ($s -match '^ex(\d+)$') { $s = $Matches[1] }
    if ($s -notmatch '^\d+$') {
        throw "lab id must be digits (e.g. 00, 01, 6) — got '$Raw'"
    }
    return ("{0:D2}" -f [int]$s)
}

function Get-AvailableLabs {
    Get-ChildItem -LiteralPath $LabRoot -Directory -Filter "ex*" |
        Where-Object { $_.Name -match '^ex(\d+)-' } |
        ForEach-Object {
            [pscustomobject]@{
                Id   = $Matches[1]
                Name = $_.Name
                Dir  = $_.FullName
            }
        } |
        Sort-Object { [int]$_.Id }
}

function Write-MainSwitcher([string] $SelectedId, $Labs) {
    $main = Join-Path $LabRoot "main\main.c"
    if (-not (Test-Path -LiteralPath $main)) {
        throw "missing $main"
    }

    $includes = foreach ($lab in $Labs) {
        $id = $lab.Id
        if ($id -eq $SelectedId) {
            "#include `"ex$id.h`""
        } else {
            "/* #include `"ex$id.h`" */"
        }
    }
    $starts = foreach ($lab in $Labs) {
        $id = $lab.Id
        if ($id -eq $SelectedId) {
            "    ex${id}_start();"
        } else {
            "    /* ex${id}_start(); */"
        }
    }

    $body = @"
/*
 * device-labs — exercise entry (lab switcher)
 *
 * Selected by ``run NN`` (or edit includes / start() calls below).
 * Inside the lab folder, exNN.c chooses which step runs.
 *
 * Index: ../README.md
 */
#include "bsp.h"

$($includes -join "`n")

void app_main(void)
{
    bsp_init();
$($starts -join "`n")
}
"@
    # Use UTF-8 without BOM for ESP-IDF / clangd friendliness
    $utf8 = New-Object System.Text.UTF8Encoding $false
    [System.IO.File]::WriteAllText($main, $body.Replace("`r`n", "`n") + "`n", $utf8)
}

if ($args.Count -lt 1) {
    $labs = Get-AvailableLabs
    Write-Host "Usage: .\run <NN> [monitor]"
    Write-Host "  NN = lab number (00..$($labs[-1].Id))"
    Write-Host "  monitor = also open serial monitor after flash"
    Write-Host ""
    Write-Host "Available labs:"
    foreach ($lab in $labs) {
        Write-Host ("  {0}  {1}" -f $lab.Id, $lab.Name)
    }
    exit 2
}

$wantMonitor = $false
$rawId = $null
foreach ($a in $args) {
    $s = [string]$a
    if ($s -eq "--monitor" -or $s -eq "-m" -or $s -eq "monitor") {
        $wantMonitor = $true
    } elseif (-not $rawId) {
        $rawId = $s
    } else {
        throw "unexpected argument: $a (did you mean: .\run $rawId monitor ?)"
    }
}

$id = Normalize-LabId $rawId
$labs = Get-AvailableLabs
$match = $labs | Where-Object { $_.Id -eq $id } | Select-Object -First 1
if (-not $match) {
    Write-Host "unknown lab '$id'. Available:"
    foreach ($lab in $labs) {
        Write-Host ("  {0}  {1}" -f $lab.Id, $lab.Name)
    }
    exit 2
}

Write-Host "run: selecting $($match.Name) (ex$id)"
Write-MainSwitcher -SelectedId $id -Labs $labs

$rb = Join-Path $LabRoot "rb.ps1"
if (-not (Test-Path -LiteralPath $rb)) {
    throw "missing $rb"
}

if ($wantMonitor) {
    Write-Host "run: build + flash + monitor…"
    & $rb run
} else {
    Write-Host "run: build + flash…"
    & $rb build
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    & $rb flash
}
exit $LASTEXITCODE
