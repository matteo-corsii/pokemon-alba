$ErrorActionPreference = 'Stop'
$root = Resolve-Path (Join-Path $PSScriptRoot '..')
function Assert([bool]$c,[string]$m){if(-not $c){throw $m}}
$sp = Get-Content (Join-Path $root 'include/constants/species.h') -Raw
$dx = Get-Content (Join-Path $root 'include/constants/pokedex.h') -Raw
$info = Get-Content (Join-Path $root 'src/data/pokemon/species_info.h') -Raw -Encoding UTF8
$learn = Get-Content (Join-Path $root 'src/data/pokemon/all_learnables.json') -Raw | ConvertFrom-Json
Assert ($sp.Contains('SPECIES_SANGUILEX,`r`n    SPECIES_TRITINO,') -or $sp.Contains("SPECIES_SANGUILEX,`n    SPECIES_TRITINO,")) 'Tritino species is not append-only.'
Assert ($dx.Contains('NATIONAL_DEX_SANGUILEX,`r`n    NATIONAL_DEX_TRITINO,') -or $dx.Contains("NATIONAL_DEX_SANGUILEX,`n    NATIONAL_DEX_TRITINO,")) 'Tritino dex is not append-only.'
foreach($n in 'TRITINO','TRICREST') { $sym=$n.Substring(0,1)+$n.Substring(1).ToLower(); $r=[regex]::Match($info,"(?s)\[SPECIES_$n\]\s*=\s*\{.*?\n    \},").Value; Assert ($r.Length -gt 0) "$n info missing"; Assert ($r.Contains("gMonFrontPic_$sym")) "$n front asset missing"; Assert ($null -ne $learn.$n) "$n teachables missing"; Assert ((Get-ChildItem (Join-Path $root "graphics/pokemon/$($n.ToLower())") -File).Count -eq 5) "$n asset count mismatch" }
Assert ($info.Contains('[SPECIES_TRITINO]') -and $info.Contains('SPECIES_TRICREST')) 'Tritino line missing.'
Write-Host 'Tritino/Tricrest validation passed.' -ForegroundColor Green
