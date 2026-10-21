$ErrorActionPreference='Stop'
function A([bool]$c,[string]$m){if(-not $c){throw $m}}
$sp=Get-Content include/constants/species.h -Raw; $dx=Get-Content include/constants/pokedex.h -Raw; $inf=Get-Content src/data/pokemon/species_info.h -Raw
$learn=Get-Content src/data/pokemon/all_learnables.json -Raw | ConvertFrom-Json
A ($sp -match 'SPECIES_CISTERNIDE,\s*SPECIES_CALCISTERN,\s*SPECIES_CARPULUS,\s*SPECIES_LUCINUS,\s*SPECIES_NAUFRAGUS') 'Species order invalid'
A ($dx -match 'NATIONAL_DEX_CISTERNIDE,\s*NATIONAL_DEX_CALCISTERN,\s*NATIONAL_DEX_CARPULUS,\s*NATIONAL_DEX_LUCINUS,\s*NATIONAL_DEX_NAUFRAGUS') 'Dex order invalid'
$checks=@{CARPULUS=@('.types = MON_TYPES(TYPE_WATER)','.baseHP = 55','.baseAttack = 55','.baseDefense = 55','.baseSpeed = 55','.baseSpAttack = 50','.baseSpDefense = 60','ABILITY_SWIFT_SWIM','ABILITY_WATER_VEIL','ABILITY_HYDRATION');LUCINUS=@('.types = MON_TYPES(TYPE_WATER, TYPE_DARK)','.baseHP = 60','.baseAttack = 90','ABILITY_STRONG_JAW','ABILITY_SWIFT_SWIM','ABILITY_SNIPER');NAUFRAGUS=@('.types = MON_TYPES(TYPE_WATER, TYPE_STEEL)','.baseHP = 80','.baseAttack = 95','.baseDefense = 120','ABILITY_BATTLE_ARMOR','ABILITY_STURDY','ABILITY_HEAVY_METAL')}
foreach($n in $checks.Keys){$r=[regex]::Match($inf,"(?s)\[SPECIES_$n\].*?\n    \},").Value;A($r.Length -gt 0) "Missing $n";foreach($c in $checks[$n]){A $r.Contains($c) "$n missing $c"};foreach($f in 'anim_front.png','back.png','icon.png','normal.pal','shiny.pal'){A(Test-Path "graphics/pokemon/$($n.ToLower())/$f") "$n missing $f"}}
A ($learn.CARPULUS -ne $null -and $learn.LUCINUS -ne $null -and $learn.NAUFRAGUS -ne $null) 'all_learnables entries missing'
foreach($n in 'CARPULUS','LUCINUS','NAUFRAGUS'){ $r=[regex]::Match($inf,"(?s)\[SPECIES_$n\].*?\n    \},").Value; A($r.Contains('.teachingType = EXPLICIT_TEACHABLES')) "$n teaching type missing"; A($r -match '\.teachableLearnset = s[A-Za-z]+TeachableLearnset') "$n teachable symbol missing"; A($r -match '\.eggMoveLearnset = s[A-Za-z]+EggMoveLearnset') "$n egg moves missing" }
$levelUpMoves=@{
    Carpulus='1:MOVE_TACKLE,1:MOVE_TAIL_WHIP,4:MOVE_WATER_GUN,7:MOVE_FLAIL,10:MOVE_AQUA_JET,13:MOVE_HARDEN,16:MOVE_BITE,20:MOVE_AQUA_RING,24:MOVE_TAKE_DOWN,28:MOVE_WATER_PULSE,32:MOVE_AGILITY,36:MOVE_AQUA_TAIL,40:MOVE_DOUBLE_EDGE,44:MOVE_HYDRO_PUMP'
    Lucinus='1:MOVE_TACKLE,1:MOVE_LEER,4:MOVE_BITE,8:MOVE_AQUA_JET,12:MOVE_FOCUS_ENERGY,16:MOVE_ICE_FANG,20:MOVE_ASSURANCE,24:MOVE_WATER_PULSE,28:MOVE_CRUNCH,32:MOVE_AGILITY,36:MOVE_LIQUIDATION,40:MOVE_NIGHT_SLASH,44:MOVE_PSYCHIC_FANGS,48:MOVE_HYDRO_PUMP'
    Naufragus='1:MOVE_TACKLE,1:MOVE_HARDEN,5:MOVE_WATER_GUN,9:MOVE_METAL_CLAW,13:MOVE_PROTECT,17:MOVE_AQUA_JET,21:MOVE_IRON_DEFENSE,25:MOVE_BRINE,29:MOVE_ANCIENT_POWER,33:MOVE_AQUA_TAIL,37:MOVE_IRON_HEAD,41:MOVE_HEAVY_SLAM,45:MOVE_RAIN_DANCE,49:MOVE_HYDRO_PUMP,53:MOVE_GYRO_BALL'
}
foreach($n in $levelUpMoves.Keys){
    $table=[regex]::Match($inf,"(?s)static const struct LevelUpMove s${n}LevelUpLearnset\[\] = \{(.*?)\n\};").Groups[1].Value
    A($table.Length -gt 0) "$n level-up table missing"
    A(([regex]::Matches($table,'LEVEL_UP_END')).Count -eq 1) "$n level-up table must end once"
    $actual=[regex]::Matches($table,'LEVEL_UP_MOVE\(\s*(\d+),\s*(MOVE_[A-Z0-9_]+)\)') | ForEach-Object { "$($_.Groups[1].Value):$($_.Groups[2].Value)" }
    A(($actual -join ',') -eq $levelUpMoves[$n]) "$n level-up moves invalid"
}
A ($inf.Contains('Vive in branchi presso rive e pontili.\n') -and $inf.Contains('Le scaglie circolari riflettono la luce.\n') -and $inf.Contains('Si nasconde fra i canneti.\n') -and $inf.Contains('Scatta sulla preda senza increspare\n') -and $inf.Contains("l'acqua.") -and $inf.Contains('Le placche sembrano prue romane.\n') -and $inf.Contains('Nei laghi, le leggende lo scambiano\n') -and $inf.Contains('per una nave senza equipaggio.')) 'Canonical descriptions incomplete'
# Lago encounter ownership moved to validate_lago_di_albera_wild_fauna.ps1.
Write-Host 'Carpulus/Lucinus/Naufragus validation passed.'
