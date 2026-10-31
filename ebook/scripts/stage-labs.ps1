#Requires -Version 5.1
<#
.SYNOPSIS
  Stage labs/*/ README labsheets into ebook/src/generated and write SUMMARY.md.
#>
$ErrorActionPreference = 'Stop'

# PSScriptRoot = workshop/ebook/scripts
$EbookDir = Split-Path $PSScriptRoot -Parent
$WorkshopRoot = Split-Path $EbookDir -Parent
$LabsDir = Join-Path $WorkshopRoot 'labs'
$SrcDir = Join-Path $EbookDir 'src'
$GenDir = Join-Path $SrcDir 'generated'
$script:MermaidFence = [string]::new([char]96, 3) # three backticks

function Repair-MermaidDiagram([string]$Diagram) {
  # Mermaid treats '#' as a line comment — CSS/DOM ids like span#count break parsers.
  $d = [regex]::Replace($Diagram, '(?<=\w)#(?=[A-Za-z_])', ' id=')
  $d = [regex]::Replace($d, '(?<=[\s\[\(\/])#(?=[A-Za-z_])', 'id=')
  return $d
}

function Get-LabNumber([string]$ExName) {
  if ($ExName -match '^ex(\d+)') { return $Matches[1] }
  return '00'
}

function New-RunBlock {
  param(
    [string]$Track,
    [string]$ExName,
    [string]$RelSource,
    [string]$StepFn = $null,
    [string]$Badge = $null
  )
  $nn = Get-LabNumber $ExName
  $lines = New-Object System.Collections.Generic.List[string]
  $lines.Add('')
  $lines.Add('<div class="ebook-run">')
  $lines.Add('')
  $lines.Add("**Track:** ``$Track`` · **Project:** ``labs/$Track``")
  if ($Badge) {
    $lines.Add('')
    $lines.Add($Badge)
  }
  $lines.Add('')
  $lines.Add('```text')
  $lines.Add("rbdev run $nn")
  if ($StepFn) {
    $lines.Add("# step switch in ${ExName}.c:  $StepFn")
  }
  $lines.Add('```')
  $lines.Add('')
  $lines.Add("<p class=`"ebook-source`"><strong>Source:</strong> <code>$RelSource</code></p>")
  $lines.Add('')
  $lines.Add('</div>')
  $lines.Add('')
  return ($lines -join "`n")
}

function Insert-AfterFirstHeading([string]$Body, [string]$Insert) {
  $m = [regex]::Match($Body, '(?m)^#\s+.+$')
  if (-not $m.Success) {
    return $Insert + $Body
  }
  $end = $m.Index + $m.Length
  return $Body.Substring(0, $end) + "`n" + $Insert + $Body.Substring($end)
}

function Write-StagedMarkdown {
  param(
    [string]$DestPath,
    [string]$RunBlock,
    [string]$Body
  )
  $dir = Split-Path $DestPath -Parent
  if (-not (Test-Path $dir)) {
    New-Item -ItemType Directory -Path $dir -Force | Out-Null
  }
  $body = $Body -replace "`r`n", "`n" -replace "`r", "`n"
  # Keep ```mermaid fences (do NOT wrap in <div class="mermaid">).
  # Raw HTML divs are still markdown-parsed by mdBook, which turns -->> into
  # an en-dash / nests <pre> and breaks sequenceDiagram. ebook.js upgrades fences.
  $body = [regex]::Replace(
    $body,
    ($script:MermaidFence + 'mermaid\s*\n([\s\S]*?)' + $script:MermaidFence),
    {
      param($m)
      $diagram = Repair-MermaidDiagram ($m.Groups[1].Value.Trim())
      return ($script:MermaidFence + 'mermaid' + "`n" + $diagram + "`n" + $script:MermaidFence)
    },
    [System.Text.RegularExpressions.RegexOptions]::IgnoreCase
  )
  $out = Insert-AfterFirstHeading -Body $body -Insert $RunBlock
  if (-not $out.EndsWith("`n")) { $out += "`n" }
  [System.IO.File]::WriteAllText($DestPath, $out, [System.Text.UTF8Encoding]::new($false))
}

if (Test-Path $GenDir) {
  Remove-Item $GenDir -Recurse -Force
}
New-Item -ItemType Directory -Path $GenDir -Force | Out-Null

$summary = New-Object System.Collections.Generic.List[string]
$summary.Add('# Summary')
$summary.Add('')
$summary.Add('[Introduction](introduction.md)')
$summary.Add('[Reading order](reading-order.md)')
$summary.Add('')

$tracks = @(
  @{ Id = 'device-labs'; Title = 'Device labs'; Part = 'device'; Stepped = $true },
  @{ Id = 'special-labs'; Title = 'Special labs'; Part = 'special'; Stepped = $true },
  @{ Id = 'rtos-labs'; Title = 'RTOS labs'; Part = 'rtos'; Stepped = $false },
  @{ Id = 'demo-labs'; Title = 'Demo labs'; Part = 'demo'; Stepped = $false }
)

$missing = New-Object System.Collections.Generic.List[string]
$pageCount = 0

foreach ($t in $tracks) {
  $trackPath = Join-Path $LabsDir $t.Id
  if (-not (Test-Path $trackPath)) {
    throw "missing track folder: $trackPath"
  }

  $summary.Add("# $($t.Title)")
  $summary.Add("- [$($t.Title)](tracks/$($t.Part).md)")

  $exercises = @(Get-ChildItem $trackPath -Directory -Filter 'ex*' | Sort-Object Name)
  foreach ($ex in $exercises) {
    $readme = Join-Path $ex.FullName 'README.md'
    $relSource = "labs/$($t.Id)/$($ex.Name)/README.md"
    $genExDir = Join-Path $GenDir "$($t.Part)/$($ex.Name)"
    $genExMd = Join-Path $genExDir 'index.md'
    $title = $ex.Name

    if (-not (Test-Path $readme)) {
      $missing.Add($relSource)
      $stub = @"
# $($ex.Name)

> Labsheet README is missing in the repo. Add ``labs/$($t.Id)/$($ex.Name)/README.md``.

"@
      $run = New-RunBlock -Track $t.Id -ExName $ex.Name -RelSource $relSource -Badge '<span class="ebook-badge warn">missing README</span>'
      Write-StagedMarkdown -DestPath $genExMd -RunBlock $run -Body $stub
      $summary.Add("  - [$title](generated/$($t.Part)/$($ex.Name)/index.md)")
      $pageCount++
      continue
    }

    $body = [System.IO.File]::ReadAllText($readme)
    $run = New-RunBlock -Track $t.Id -ExName $ex.Name -RelSource $relSource
    Write-StagedMarkdown -DestPath $genExMd -RunBlock $run -Body $body
    $summary.Add("  - [$title](generated/$($t.Part)/$($ex.Name)/index.md)")
    $pageCount++

    if (-not $t.Stepped) { continue }

    $stepsDir = Join-Path $ex.FullName 'steps'
    if (-not (Test-Path $stepsDir)) { continue }

    $steps = @(Get-ChildItem $stepsDir -Directory | Sort-Object Name)
    $stepIndex = 0
    $stepTotal = $steps.Count
    foreach ($st in $steps) {
      $stepIndex++
      $stepReadme = Join-Path $st.FullName 'README.md'
      if (-not (Test-Path $stepReadme)) {
        $missing.Add("labs/$($t.Id)/$($ex.Name)/steps/$($st.Name)/README.md")
        continue
      }
      $nn = Get-LabNumber $ex.Name
      $mm = if ($st.Name -match 'step-(\d+)') { $Matches[1] } else { '{0:00}' -f $stepIndex }
      $stepFn = "ex${nn}_step${mm}_start"
      $badge = $null
      if ($st.Name -match 'busy|crash|jitter|hog|overlap|oversub') {
        $badge = '<span class="ebook-badge warn">broken on purpose</span>'
      }
      elseif ($stepIndex -eq $stepTotal) {
        $badge = '<span class="ebook-badge ok">finished device</span>'
      }

      $relStep = "labs/$($t.Id)/$($ex.Name)/steps/$($st.Name)/README.md"
      $genStep = Join-Path $genExDir "$($st.Name).md"
      $stepBody = [System.IO.File]::ReadAllText($stepReadme)
      $stepRun = New-RunBlock -Track $t.Id -ExName $ex.Name -RelSource $relStep -StepFn $stepFn -Badge $badge
      Write-StagedMarkdown -DestPath $genStep -RunBlock $stepRun -Body $stepBody
      $summary.Add("    - [$($st.Name)](generated/$($t.Part)/$($ex.Name)/$($st.Name).md)")
      $pageCount++
    }
  }
  $summary.Add('')
}

$summaryPath = Join-Path $SrcDir 'SUMMARY.md'
[System.IO.File]::WriteAllText(
  $summaryPath,
  (($summary -join "`n") + "`n"),
  [System.Text.UTF8Encoding]::new($false)
)

$manifest = [ordered]@{
  generated_utc   = (Get-Date).ToUniversalTime().ToString('o')
  pages           = $pageCount
  missing_readmes = @($missing)
}
$manifestPath = Join-Path $GenDir 'manifest.json'
($manifest | ConvertTo-Json) | Set-Content -Path $manifestPath -Encoding utf8

Write-Host "Staged $pageCount pages -> $GenDir"
if ($missing.Count -gt 0) {
  Write-Host "WARN missing READMEs ($($missing.Count)):"
  $missing | ForEach-Object { Write-Host "  $_" }
}

$fail = 0
foreach ($t in $tracks) {
  $trackPath = Join-Path $LabsDir $t.Id
  foreach ($ex in @(Get-ChildItem $trackPath -Directory -Filter 'ex*')) {
    $expect = Join-Path $GenDir "$($t.Part)/$($ex.Name)/index.md"
    if (-not (Test-Path $expect)) {
      Write-Host "FAIL no generated page for $($t.Id)/$($ex.Name)"
      $fail++
    }
  }
}
if ($fail -gt 0) {
  throw "coverage failed: $fail exercise(s) missing pages"
}
Write-Host "Coverage OK"
