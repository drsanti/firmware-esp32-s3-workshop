#Requires -Version 5.1
<#
.SYNOPSIS
  Stage labsheets and build the Labs Ebook with mdbook.
#>
$ErrorActionPreference = 'Stop'
$EbookDir = Split-Path $PSScriptRoot -Parent
$ThemeDir = Join-Path $EbookDir 'theme'
$MermaidJs = Join-Path $ThemeDir 'mermaid.min.js'

Write-Host "=== stage labs ==="
& (Join-Path $PSScriptRoot 'stage-labs.ps1')

if (-not (Test-Path $MermaidJs) -or ((Get-Item $MermaidJs).Length -lt 10000)) {
  Write-Host "=== download mermaid.min.js (offline file://) ==="
  $url = 'https://cdn.jsdelivr.net/npm/mermaid@11/dist/mermaid.min.js'
  New-Item -ItemType Directory -Path $ThemeDir -Force | Out-Null
  Invoke-WebRequest -Uri $url -OutFile $MermaidJs -UseBasicParsing
}

$mdbook = Get-Command mdbook -ErrorAction SilentlyContinue
if (-not $mdbook) {
  $cargoBin = Join-Path $env:USERPROFILE '.cargo\bin\mdbook.exe'
  if (Test-Path $cargoBin) {
    $env:Path = "$(Split-Path $cargoBin -Parent);$env:Path"
  }
}
if (-not (Get-Command mdbook -ErrorAction SilentlyContinue)) {
  throw 'mdbook not found. Install: cargo install mdbook'
}

Write-Host "=== mdbook build ==="
Push-Location $EbookDir
try {
  & mdbook build
  if ($LASTEXITCODE -ne 0) { throw "mdbook build failed: $LASTEXITCODE" }
}
finally {
  Pop-Location
}

$index = Join-Path $EbookDir 'book\index.html'
if (-not (Test-Path $index)) {
  throw "missing build output: $index"
}
Write-Host "OK  $index"
Write-Host "Open in browser (file://) or: mdbook serve (from ebook)"
