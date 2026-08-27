$ErrorActionPreference='Stop'
function A([bool]$c,[string]$m){if(-not $c){throw $m}}
$sp=Get-Content include/constants/species.h -Raw; $dx=Get-Content include/constants/pokedex.h -Raw; $inf=Get-Content src/data/pokemon/species_info.h -Raw
A ($sp -match 'SPECIES_CISTERNIDE,\s*SPECIES_CALCISTERN,\s*SPECIES_CARPULUS,\s*SPECIES_LUCINUS,\s*SPECIES_NAUFRAGUS') 'Species order invalid'
A ($dx -match 'NATIONAL_DEX_CISTERNIDE,\s*NATIONAL_DEX_CALCISTERN,\s*NATIONAL_DEX_CARPULUS,\s*NATIONAL_DEX_LUCINUS,\s*NATIONAL_DEX_NAUFRAGUS') 'Dex order invalid'
$checks=@{CARPULUS=@('.types = MON_TYPES(TYPE_WATER)','.baseHP = 55','.baseAttack = 55','.baseDefense = 55','.baseSpeed = 55','.baseSpAttack = 50','.baseSpDefense = 60','ABILITY_SWIFT_SWIM','ABILITY_WATER_VEIL','ABILITY_HYDRATION');LUCINUS=@('.types = MON_TYPES(TYPE_WATER, TYPE_DARK)','.baseHP = 60','.baseAttack = 90','ABILITY_STRONG_JAW','ABILITY_SWIFT_SWIM','ABILITY_SNIPER');NAUFRAGUS=@('.types = MON_TYPES(TYPE_WATER, TYPE_STEEL)','.baseHP = 80','.baseAttack = 95','.baseDefense = 120','ABILITY_BATTLE_ARMOR','ABILITY_STURDY','ABILITY_HEAVY_METAL')}
foreach($n in $checks.Keys){$r=[regex]::Match($inf,"(?s)\[SPECIES_$n\].*?\n    \},").Value;A($r.Length -gt 0) "Missing $n";foreach($c in $checks[$n]){A $r.Contains($c) "$n missing $c"};foreach($f in 'anim_front.png','back.png','icon.png','normal.pal','shiny.pal'){A(Test-Path "graphics/pokemon/$($n.ToLower())/$f") "$n missing $f"}}
A ((Get-Content src/data/wild_encounters.json -Raw) -notmatch 'MAP_LAGO_DI_ALBERA') 'Lago encounter table must remain absent'
Write-Host 'Carpulus/Lucinus/Naufragus validation passed.'
