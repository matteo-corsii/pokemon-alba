$ErrorActionPreference = 'Stop'
$root = Resolve-Path (Join-Path $PSScriptRoot '..')
function Assert([bool]$c,[string]$m){if(-not $c){throw $m}}
$sp = Get-Content (Join-Path $root 'include/constants/species.h') -Raw
$info = Get-Content (Join-Path $root 'src/data/pokemon/species_info.h') -Raw -Encoding UTF8
$learn = Get-Content (Join-Path $root 'src/data/pokemon/all_learnables.json') -Raw | ConvertFrom-Json
Assert ($sp.Contains('SPECIES_TRICREST,') -and $sp.Contains('SPECIES_SALAMPOLLA,') -and $sp.Contains('SPECIES_ALCHIMANDRA,')) 'Salampolla line is missing.'
foreach($n in 'SALAMPOLLA','ALCHIMANDRA') { $sym=$n.Substring(0,1)+$n.Substring(1).ToLower(); $r=[regex]::Match($info,"(?s)\[SPECIES_$n\]\s*=\s*\{.*?\n    \},").Value; Assert ($r.Length -gt 0) "$n info missing"; Assert ($r.Contains("gMonFrontPic_$sym")) "$n front asset missing"; Assert ($null -ne $learn.$n) "$n teachables missing"; Assert ((Get-ChildItem (Join-Path $root "graphics/pokemon/$($n.ToLower())") -File).Count -eq 5) "$n asset count mismatch" }
Assert ($info.Contains('SPECIES_ALCHIMANDRA, CONDITIONS({IF_TIME, TIME_NIGHT})')) 'Night evolution condition missing.'
Write-Host 'Salampolla/Alchimandra validation passed.' -ForegroundColor Green
