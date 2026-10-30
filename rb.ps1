# Forward to the OS-matched rbdev binary under rbdev/<os>/.
# Usage (from a lab folder):  ..\..\rb.ps1 build
#        (or use the local .\rb.ps1 / .\rb.cmd wrapper)

$ErrorActionPreference = "Stop"
$Root = $PSScriptRoot

if ($env:OS -eq "Windows_NT") {
    $Bin = Join-Path $Root "rbdev\windows\rbdev.exe"
} elseif (Test-Path "/System/Library/CoreServices/SystemVersion.plist") {
    $Bin = Join-Path $Root "rbdev/mac/rbdev"
} else {
    $Bin = Join-Path $Root "rbdev/linux/rbdev"
}

if (-not (Test-Path -LiteralPath $Bin)) {
    Write-Error "rbdev binary not found: $Bin`nCopy it into rbdev/<os>/ — see rbdev/README.md"
}

# doctor from a lab folder: default --workshop to repo root
$argv = [System.Collections.Generic.List[string]]::new()
foreach ($a in $args) { [void]$argv.Add([string]$a) }
if ($argv.Count -ge 1 -and $argv[0] -eq "doctor") {
    $hasWs = $false
    for ($i = 1; $i -lt $argv.Count; $i++) {
        if ($argv[$i] -eq "--workshop" -or $argv[$i].StartsWith("--workshop=")) {
            $hasWs = $true
            break
        }
    }
    if (-not $hasWs) {
        [void]$argv.Add("--workshop")
        [void]$argv.Add($Root)
    }
}

& $Bin @($argv.ToArray())
exit $LASTEXITCODE
