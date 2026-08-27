$ErrorActionPreference = 'Stop'
Set-Location (Resolve-Path (Join-Path $PSScriptRoot '..'))
function Assert-True([bool]$c,[string]$m){if(-not $c){throw $m}}
function Section([string]$t,[string]$s){$a=$t.IndexOf($s);Assert-True ($a -ge 0) "Missing $s";$b=$t.IndexOf("`n    },",$a);Assert-True ($b -gt $a) "Unterminated $s";$t.Substring($a,$b-$a)}
$sp=(Get-Content include/constants/species.h -Raw)-replace "`r`n","`n"
$dex=(Get-Content include/constants/pokedex.h -Raw)-replace "`r`n","`n"
$info=(Get-Content src/data/pokemon/species_info.h -Raw)-replace "`r`n","`n"
$egg=(Get-Content src/data/pokemon/egg_moves.h -Raw)-replace "`r`n","`n"
$learn=Get-Content src/data/pokemon/all_learnables.json -Raw|ConvertFrom-Json
$wild=Get-Content src/data/wild_encounters.json -Raw
Assert-True $sp.Contains("SPECIES_LUSCERP,`n    SPECIES_LUMELLA,`n    SPECIES_OMPHALUX,`n    SPECIES_PALUDIX,`n    SPECIES_SANGUILEX,`n    SPECIES_TRITINO,`n    SPECIES_TRICREST,`n    SPECIES_SALAMPOLLA,`n    SPECIES_ALCHIMANDRA,`n    SPECIES_CISTERNIDE,`n    SPECIES_CALCISTERN,`n    SPECIES_CUSTOM_END,") 'Species order/count mismatch'
Assert-True $dex.Contains("NATIONAL_DEX_LUSCERP,`n    NATIONAL_DEX_LUMELLA,`n    NATIONAL_DEX_OMPHALUX,`n    NATIONAL_DEX_PALUDIX,`n    NATIONAL_DEX_SANGUILEX,`n    NATIONAL_DEX_TRITINO,`n    NATIONAL_DEX_TRICREST,`n    NATIONAL_DEX_SALAMPOLLA,`n    NATIONAL_DEX_ALCHIMANDRA,") 'National Dex order mismatch'
Assert-True $dex.Contains('#define NATIONAL_DEX_COUNT  NATIONAL_DEX_CALCISTERN') 'National Dex count mismatch'
$checks=@{
 LUMELLA=@('.types = MON_TYPES(TYPE_GRASS)','.baseHP = 50','.baseAttack = 35','.baseDefense = 55','.baseSpeed = 35','.baseSpAttack = 65','.baseSpDefense = 65','.catchRate = 190','.expYield = 62','.evYield_SpAttack = 1','.abilities = { ABILITY_ILLUMINATE, ABILITY_EFFECT_SPORE, ABILITY_RAIN_DISH }','.natDexNum = NATIONAL_DEX_LUMELLA','.categoryName = _("LUMEFUNGO")','.height = 3, .weight = 21','.frontPic = gMonFrontPic_Lumella','.backPic = gMonBackPic_Lumella','.palette = gMonPalette_Lumella','.shinyPalette = gMonShinyPalette_Lumella','.iconSprite = gMonIcon_Lumella','FOOTPRINT(Morelull)','sPicTable_Morelull','.frontAnimId = ANIM_V_SQUISH_AND_BOUNCE','.backAnimId = BACK_ANIM_SHRINK_GROW','CONDITIONS({IF_TIME, TIME_NIGHT})');
 OMPHALUX=@('.types = MON_TYPES(TYPE_GRASS, TYPE_ELECTRIC)','.baseHP = 80','.baseAttack = 45','.baseDefense = 85','.baseSpeed = 60','.baseSpAttack = 115','.baseSpDefense = 100','.catchRate = 75','.expYield = 170','.evYield_SpAttack = 2','.abilities = { ABILITY_ILLUMINATE, ABILITY_EFFECT_SPORE, ABILITY_LIGHTNING_ROD }','.natDexNum = NATIONAL_DEX_OMPHALUX','.categoryName = _("BIOLUMINE")','.height = 9, .weight = 185','.frontPic = gMonFrontPic_Omphalux','.backPic = gMonBackPic_Omphalux','.palette = gMonPalette_Omphalux','.shinyPalette = gMonShinyPalette_Omphalux','.iconSprite = gMonIcon_Omphalux','FOOTPRINT(Shiinotic)','sPicTable_Shiinotic','.frontAnimId = ANIM_GLOW_BLACK','.backAnimId = BACK_ANIM_SHRINK_GROW')
}
foreach($n in $checks.Keys){$r=Section $info "[SPECIES_$n] =";foreach($x in $checks[$n]){Assert-True $r.Contains($x) "$n missing $x"}}
Assert-True $egg.Contains('sLumellaEggMoveLearnset') 'Lumella Egg Moves missing';Assert-True $egg.Contains('#define sOmphaluxEggMoveLearnset sLumellaEggMoveLearnset') 'Omphalux Egg Moves missing'
foreach($n in 'LUMELLA','OMPHALUX'){Assert-True ($null -ne $learn.$n) "$n teachables missing";Assert-True (@($learn.$n).Count -eq 24) "$n teachable count mismatch"}
Assert-True (-not ($wild -match 'SPECIES_OMPHALUX')) 'Omphalux must not be in encounters'
foreach($n in 'lumella','omphalux'){$p="graphics/pokemon/$n";Assert-True ((Get-ChildItem $p -File).Count -eq 5) "$n must have exactly five assets";Assert-True (-not (Get-ChildItem $p -File|Where-Object {$_.Name -match 'zip|preview|concept'})) "$n contains forbidden asset"}
Write-Host 'Lumella/Omphalux validation passed.' -ForegroundColor Green
