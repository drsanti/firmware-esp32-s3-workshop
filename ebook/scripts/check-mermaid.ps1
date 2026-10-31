#Requires -Version 5.1
$ErrorActionPreference = "Stop"
$EbookDir = Split-Path $PSScriptRoot -Parent
$BookDir = Join-Path $EbookDir "book"
$fail = 0

function Write-Fail([string]$Message) {
  Write-Host "FAIL  $Message" -ForegroundColor Red
  $script:fail++
}
function Write-Pass([string]$Message) {
  Write-Host "PASS  $Message" -ForegroundColor Green
}

if (-not (Test-Path $BookDir)) { throw "missing book/ - run build.ps1 first" }

$htmlFiles = @(Get-ChildItem $BookDir -Recurse -Filter "*.html")
$fenceCount = 0
$divCount = 0
$badDiv = New-Object System.Collections.Generic.List[string]
$dashPages = New-Object System.Collections.Generic.List[string]
$en = [char]0x2013
$em = [char]0x2014

foreach ($f in $htmlFiles) {
  $t = [System.IO.File]::ReadAllText($f.FullName)
  $rel = $f.FullName.Replace($BookDir, "").TrimStart([char[]]@("\", "/"))
  $fenceCount += ([regex]::Matches($t, 'class="language-mermaid"')).Count
  $divCount += ([regex]::Matches($t, 'div class="mermaid"')).Count
  if ($t -match '(?s)<div class="mermaid">.*?(?:<p>|<pre>|<code>)') {
    [void]$badDiv.Add($rel)
  }
  $blocks = [regex]::Matches($t, '(?s)class="language-mermaid"[^>]*>(.*?)</code>')
  foreach ($b in $blocks) {
    $inner = $b.Groups[1].Value
    if ($inner.IndexOf($en) -ge 0 -or $inner.IndexOf($em) -ge 0) {
      [void]$dashPages.Add($rel)
    }
  }
}

Write-Pass ("html=" + $htmlFiles.Count + " language-mermaid=" + $fenceCount + " div.mermaid=" + $divCount)
if ($badDiv.Count -gt 0) { Write-Fail ("mangled div.mermaid pages=" + $badDiv.Count) } else { Write-Pass "no mangled div.mermaid" }
if ($dashPages.Count -gt 0) { Write-Fail ("en-dash in language-mermaid pages=" + $dashPages.Count) } else { Write-Pass "no en-dash in language-mermaid" }
if ($divCount -gt 0 -and $fenceCount -eq 0) { Write-Fail "stager still emitting only raw div.mermaid" }

if ($fail -eq 0) {
  Write-Host ""
  Write-Host "MERMAID HTML CHECK PASSED" -ForegroundColor Green
  exit 0
}
Write-Host ""
Write-Host ("MERMAID CHECK FAILED (" + $fail + ")") -ForegroundColor Red
exit 1
