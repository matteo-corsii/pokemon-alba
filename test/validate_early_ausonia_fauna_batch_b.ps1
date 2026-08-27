$ErrorActionPreference = 'Stop'

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
Set-Location $repoRoot

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) {
        throw $Message
    }
}

function Assert-Contains([string]$Text, [string]$Expected, [string]$Context) {
    Assert-True ($Text.Contains($Expected)) "$Context is missing: $Expected"
}

function Get-Section([string]$Text, [string]$Start, [string]$End) {
    $startIndex = $Text.IndexOf($Start)
    Assert-True ($startIndex -ge 0) "Missing section start: $Start"
    $endIndex = $Text.IndexOf($End, $startIndex + $Start.Length)
    Assert-True ($endIndex -gt $startIndex) "Missing section end: $End"
    return $Text.Substring($startIndex, $endIndex - $startIndex)
}

$speciesConstants = (Get-Content 'include/constants/species.h' -Raw) -replace "`r`n", "`n"
$dexConstants = (Get-Content 'include/constants/pokedex.h' -Raw) -replace "`r`n", "`n"
$speciesInfo = (Get-Content 'src/data/pokemon/species_info.h' -Raw) -replace "`r`n", "`n"
$eggMoves = (Get-Content 'src/data/pokemon/egg_moves.h' -Raw) -replace "`r`n", "`n"
$orders = (Get-Content 'src/data/pokemon/pokedex_orders.h' -Raw) -replace "`r`n", "`n"
$docs = (Get-Content 'docs/AUSONIA_REGIONAL_DEX_PLAN.md' -Raw) -replace "`r`n", "`n"
$learnables = Get-Content 'src/data/pokemon/all_learnables.json' -Raw | ConvertFrom-Json

Assert-Contains $speciesConstants "SPECIES_FELIVATES,`n    SPECIES_FOLIARVA,`n    SPECIES_CRISALVIA,`n    SPECIES_INFIORALA," 'Species append-only sequence'
Assert-Contains $speciesConstants 'SPECIES_EGG = SPECIES_CUSTOM_END' 'SPECIES_EGG definition'
Assert-Contains $speciesConstants 'NUM_SPECIES = SPECIES_EGG' 'NUM_SPECIES definition'
Assert-Contains $dexConstants "NATIONAL_DEX_FELIVATES,`n    NATIONAL_DEX_FOLIARVA,`n    NATIONAL_DEX_CRISALVIA,`n    NATIONAL_DEX_INFIORALA," 'Pokédex append-only sequence'
Assert-Contains $dexConstants "NATIONAL_DEX_MOLOSPSY,`n    NATIONAL_DEX_LENGHELIS,`n    NATIONAL_DEX_LUSCINCO,`n    NATIONAL_DEX_LUSCERP," 'Luscinco/Luscerp append-only National Dex sequence'
Assert-Contains $dexConstants '#define NATIONAL_DEX_COUNT  NATIONAL_DEX_CALCISTERN' 'National Dex count'

$speciesChecks = @{
    FOLIARVA = @(
        '.baseHP        = 45', '.baseAttack    = 35', '.baseDefense   = 40',
        '.baseSpeed     = 45', '.baseSpAttack  = 35', '.baseSpDefense = 40',
        '.types = MON_TYPES(TYPE_BUG)', '.catchRate = 255', '.expYield = 54',
        '.evYield_HP = 1', '.abilities = { ABILITY_SHIELD_DUST, ABILITY_SWARM, ABILITY_CHLOROPHYLL }',
        '.natDexNum = NATIONAL_DEX_FOLIARVA', '.categoryName = _("LARVAFOGLIA")', '.height = 3', '.weight = 24',
        'Si nutre delle foglie più tenere senza', 'una pianta sana.',
        '.frontPic = gMonFrontPic_Foliarva', '.backPic = gMonBackPic_Foliarva',
        '.iconSprite = gMonIcon_Foliarva', '.teachingType = EXPLICIT_TEACHABLES',
        '.evolutions = EVOLUTION({EVO_LEVEL, 10, SPECIES_CRISALVIA})'
    )
    CRISALVIA = @(
        '.baseHP        = 55', '.baseAttack    = 30', '.baseDefense   = 75',
        '.baseSpeed     = 25', '.baseSpAttack  = 40', '.baseSpDefense = 65',
        '.types = MON_TYPES(TYPE_BUG, TYPE_GRASS)', '.catchRate = 120', '.expYield = 108',
        '.evYield_Defense = 2', '.abilities = { ABILITY_SHED_SKIN, ABILITY_LEAF_GUARD, ABILITY_OVERCOAT }',
        '.natDexNum = NATIONAL_DEX_CRISALVIA', '.categoryName = _("CRISALIDE")', '.height = 5', '.weight = 68',
        'Avvolge il corpo in strati di fibra', 'accelera la metamorfosi.',
        '.frontPic = gMonFrontPic_Crisalvia', '.backPic = gMonBackPic_Crisalvia',
        '.iconSprite = gMonIcon_Crisalvia', '.teachingType = EXPLICIT_TEACHABLES',
        '.evolutions = EVOLUTION({EVO_LEVEL, 18, SPECIES_INFIORALA})'
    )
    INFIORALA = @(
        '.baseHP        = 70', '.baseAttack    = 45', '.baseDefense   = 65',
        '.baseSpeed     = 90', '.baseSpAttack  = 100', '.baseSpDefense = 80',
        '.types = MON_TYPES(TYPE_BUG, TYPE_GRASS)', '.catchRate = 75', '.expYield = 178',
        '.evYield_SpAttack = 2', '.abilities = { ABILITY_COMPOUND_EYES, ABILITY_CHLOROPHYLL, ABILITY_TINTED_LENS }',
        '.natDexNum = NATIONAL_DEX_INFIORALA', '.categoryName = _("FLOREALE")', '.height = 9', '.weight = 142',
        'Trasporta il polline tra i fiori delle', 'rapidamente.',
        '.frontPic = gMonFrontPic_Infiorala', '.backPic = gMonBackPic_Infiorala',
        '.iconSprite = gMonIcon_Infiorala', '.teachingType = EXPLICIT_TEACHABLES'
    )
}
$expectedBst = @{ FOLIARVA = 240; CRISALVIA = 290; INFIORALA = 450 }
foreach ($name in $speciesChecks.Keys) {
    $record = Get-Section $speciesInfo "[SPECIES_$name] =" "`n    },"
    foreach ($expected in $speciesChecks[$name]) {
        Assert-Contains $record $expected "SPECIES_$name"
    }
    foreach ($common in @('.genderRatio = PERCENT_FEMALE(50)', '.eggCycles = 15', '.friendship = 70', '.growthRate = GROWTH_MEDIUM_FAST', '.eggGroups = MON_EGG_GROUPS(EGG_GROUP_BUG)', '.bodyColor = BODY_COLOR_GREEN')) {
        Assert-Contains $record $common "SPECIES_$name"
    }
    $statValues = [regex]::Matches($record, '\.base(?:HP|Attack|Defense|Speed|SpAttack|SpDefense)\s*=\s*(\d+)') | ForEach-Object { [int]$_.Groups[1].Value }
    Assert-True ($statValues.Count -eq 6) "SPECIES_$name does not define six base stats"
    Assert-True ((($statValues | Measure-Object -Sum).Sum) -eq $expectedBst[$name]) "SPECIES_$name BST differs from canon"
}

$expectedLevelMoves = @{
    FOLIARVA = @('1:MOVE_TACKLE','1:MOVE_STRING_SHOT','4:MOVE_ABSORB','6:MOVE_BUG_BITE','8:MOVE_STUN_SPORE','10:MOVE_RAZOR_LEAF','13:MOVE_STRUGGLE_BUG','15:MOVE_MEGA_DRAIN')
    CRISALVIA = @('1:MOVE_HARDEN','1:MOVE_STRING_SHOT','1:MOVE_ABSORB','6:MOVE_BUG_BITE','8:MOVE_STUN_SPORE','10:MOVE_HARDEN','12:MOVE_PROTECT','15:MOVE_MEGA_DRAIN','18:MOVE_STRUGGLE_BUG')
    INFIORALA = @('1:MOVE_TACKLE','1:MOVE_STRING_SHOT','1:MOVE_ABSORB','6:MOVE_BUG_BITE','8:MOVE_STUN_SPORE','10:MOVE_RAZOR_LEAF','13:MOVE_STRUGGLE_BUG','15:MOVE_MEGA_DRAIN','18:MOVE_GUST','20:MOVE_SLEEP_POWDER','23:MOVE_AIR_CUTTER','26:MOVE_POLLEN_PUFF','30:MOVE_GIGA_DRAIN','34:MOVE_BUG_BUZZ','38:MOVE_QUIVER_DANCE','42:MOVE_ENERGY_BALL','46:MOVE_AROMATHERAPY')
}
foreach ($name in $expectedLevelMoves.Keys) {
    $learnsetBlock = Get-Section $speciesInfo "static const struct LevelUpMove s$($name.Substring(0,1))$($name.Substring(1).ToLower())LevelUpLearnset[]" '};'
    $actual = [regex]::Matches($learnsetBlock, 'LEVEL_UP_MOVE\(\s*(\d+),\s*(MOVE_[A-Z0-9_]+)\)') | ForEach-Object { "$($_.Groups[1].Value):$($_.Groups[2].Value)" }
    Assert-True (($actual -join ',') -eq ($expectedLevelMoves[$name] -join ',')) "$name level-up learnset differs from canon"
}

$expectedLearnables = @{
    FOLIARVA = @('MOVE_PROTECT','MOVE_REST','MOVE_SLEEP_TALK','MOVE_SUBSTITUTE','MOVE_SUNNY_DAY','MOVE_STRUGGLE_BUG','MOVE_GIGA_DRAIN','MOVE_ENERGY_BALL')
    CRISALVIA = @('MOVE_PROTECT','MOVE_REST','MOVE_SLEEP_TALK','MOVE_SUBSTITUTE','MOVE_SUNNY_DAY','MOVE_STRUGGLE_BUG','MOVE_GIGA_DRAIN','MOVE_ENERGY_BALL','MOVE_SOLAR_BEAM','MOVE_GRASS_KNOT')
    INFIORALA = @('MOVE_PROTECT','MOVE_REST','MOVE_SLEEP_TALK','MOVE_SUBSTITUTE','MOVE_SUNNY_DAY','MOVE_STRUGGLE_BUG','MOVE_GIGA_DRAIN','MOVE_ENERGY_BALL','MOVE_SOLAR_BEAM','MOVE_GRASS_KNOT','MOVE_U_TURN','MOVE_ACROBATICS','MOVE_AIR_SLASH','MOVE_POLLEN_PUFF')
}
foreach ($name in $expectedLearnables.Keys) {
    $actual = @($learnables.$name)
    Assert-True ($actual.Count -eq $expectedLearnables[$name].Count) "$name teachable count differs"
    Assert-True (($actual -join ',') -eq ($expectedLearnables[$name] -join ',')) "$name teachables differ from canon"
}

$eggBlock = Get-Section $eggMoves 'static const u16 sFoliarvaEggMoveLearnset[]' '};'
foreach ($move in @('MOVE_RAGE_POWDER','MOVE_WORRY_SEED','MOVE_BATON_PASS','MOVE_GRASSY_TERRAIN','MOVE_UNAVAILABLE')) {
    Assert-Contains $eggBlock $move 'Foliarva Egg Moves'
}

$alphabetical = Get-Section $orders 'const u16 gPokedexOrder_Alphabetical[]' 'const u16 gPokedexOrder_Weight[]'
$weight = Get-Section $orders 'const u16 gPokedexOrder_Weight[]' 'const u16 gPokedexOrder_Height[]'
$height = $orders.Substring($orders.IndexOf('const u16 gPokedexOrder_Height[]'))
foreach ($name in @('FOLIARVA','CRISALVIA','INFIORALA')) {
    foreach ($section in @($alphabetical, $weight, $height)) {
        $count = ([regex]::Matches($section, "NATIONAL_DEX_$name(?![A-Z0-9_])")).Count
        Assert-True ($count -eq 1) "NATIONAL_DEX_$name must occur once in each Pokédex order"
    }
}

foreach ($row in @(
    '| 014 | Foliarva | `SPECIES_FOLIARVA` = 1586 | `NATIONAL_DEX_FOLIARVA` = 1039 |',
    '| 015 | Crisalvia | `SPECIES_CRISALVIA` = 1587 | `NATIONAL_DEX_CRISALVIA` = 1040 |',
    '| 016 | Infiorala | `SPECIES_INFIORALA` = 1588 | `NATIONAL_DEX_INFIORALA` = 1041 |'
)) {
    Assert-Contains $docs $row 'Regional Dex documentation'
}
Assert-Contains $docs '| `AUS-FAM-EARLY-BUG` | Foliarva → Crisalvia → Infiorala | 3 | IMPLEMENTED; CANONICAL DESIGN |' 'Catalog status'

Write-Host 'Functional Fauna Batch B validation passed.' -ForegroundColor Green
