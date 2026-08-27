$ErrorActionPreference='Stop'
$root=Resolve-Path (Join-Path $PSScriptRoot '..')
function Assert([bool]$c,[string]$m){if(-not $c){throw $m}}
$sp=Get-Content (Join-Path $root 'include/constants/species.h') -Raw
$dx=Get-Content (Join-Path $root 'include/constants/pokedex.h') -Raw
$info=Get-Content (Join-Path $root 'src/data/pokemon/species_info.h') -Raw -Encoding UTF8
$learn=Get-Content (Join-Path $root 'src/data/pokemon/all_learnables.json') -Raw|ConvertFrom-Json
Assert ($sp -match 'SPECIES_ALCHIMANDRA,\s*SPECIES_CISTERNIDE,\s*SPECIES_CALCISTERN') 'Species not append-only.'
Assert ($dx -match 'NATIONAL_DEX_ALCHIMANDRA,\s*NATIONAL_DEX_CISTERNIDE,\s*NATIONAL_DEX_CALCISTERN') 'Dex not append-only.'
foreach($n in 'CISTERNIDE','CALCISTERN'){
 $sym=$n.Substring(0,1)+$n.Substring(1).ToLower(); $r=[regex]::Match($info,"(?s)\[SPECIES_$n\]\s*=\s*\{.*?\n    \},").Value
 Assert ($r.Length -gt 0) "$n info missing"; Assert ($r.Contains("gMonFrontPic_$sym") -and $r.Contains("gMonBackPic_$sym")) "$n graphics missing"; Assert ($null -ne $learn.$n) "$n learnables missing"; Assert ((Get-ChildItem (Join-Path $root "graphics/pokemon/$($n.ToLower())") -File).Count -eq 5) "$n asset count mismatch"
}
Assert ($info.Contains('SPECIES_CALCISTERN') -and $info.Contains('SPECIES_CISTERNIDE')) 'Species records missing.'
Write-Host 'Cisternide/Calcistern validation passed.' -ForegroundColor Green
