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

function Remove-AllowedGraphicsFields([string]$Record) {
    $normalized = $Record.Replace("`r`n", "`n")
    $normalized = [regex]::Replace(
        $normalized,
        '(?m)^\s*\.(frontPic|frontPicSize|frontPicYOffset|backPic|backPicSize|backPicYOffset|palette|shinyPalette|iconSprite|iconPalIndex)\s*=.*\n',
        ''
    )
    $normalized = [regex]::Replace(
        $normalized,
        '(?ms)^\s*\.frontAnimFrames\s*=\s*(?:ANIM_FRAMES\(.*?^\s*\),|[^,\n]+,)\n',
        ''
    )
    return $normalized
}

$speciesConstants = (Get-Content 'include/constants/species.h' -Raw) -replace "`r`n", "`n"
$dexConstants = (Get-Content 'include/constants/pokedex.h' -Raw) -replace "`r`n", "`n"
$speciesInfo = (Get-Content 'src/data/pokemon/species_info.h' -Raw -Encoding UTF8) -replace "`r`n", "`n"
$eggMoves = (Get-Content 'src/data/pokemon/egg_moves.h' -Raw) -replace "`r`n", "`n"
$orders = (Get-Content 'src/data/pokemon/pokedex_orders.h' -Raw) -replace "`r`n", "`n"
$docs = (Get-Content 'docs/AUSONIA_REGIONAL_DEX_PLAN.md' -Raw -Encoding UTF8) -replace "`r`n", "`n"
$saveTests = (Get-Content 'test/save.c' -Raw) -replace "`r`n", "`n"
$learnables = Get-Content 'src/data/pokemon/all_learnables.json' -Raw | ConvertFrom-Json

Assert-Contains $speciesConstants "SPECIES_INFIORALA,`n    SPECIES_GHEPIO,`n    SPECIES_TINUNCOL,`n    SPECIES_PEREGRINUS,`n    SPECIES_GAZZUOLA,`n    SPECIES_BRILLAZZA,`n    SPECIES_GAZZOMBRA,`n    SPECIES_MOLOSPSY,`n    SPECIES_LENGHELIS,`n    SPECIES_LUSCINCO,`n    SPECIES_LUSCERP,`n    SPECIES_CUSTOM_END," 'Species append-only sequence'
Assert-Contains $speciesConstants 'SPECIES_EGG = SPECIES_CUSTOM_END' 'SPECIES_EGG definition'
Assert-Contains $speciesConstants 'NUM_SPECIES = SPECIES_EGG' 'NUM_SPECIES definition'
Assert-Contains $dexConstants "NATIONAL_DEX_INFIORALA,`n    NATIONAL_DEX_GHEPIO,`n    NATIONAL_DEX_TINUNCOL,`n    NATIONAL_DEX_PEREGRINUS,`n    NATIONAL_DEX_GAZZUOLA,`n    NATIONAL_DEX_BRILLAZZA,`n    NATIONAL_DEX_GAZZOMBRA,`n    NATIONAL_DEX_MOLOSPSY,`n    NATIONAL_DEX_LENGHELIS,`n    NATIONAL_DEX_LUSCINCO,`n    NATIONAL_DEX_LUSCERP," 'Pokedex append-only sequence'
Assert-Contains $dexConstants '#define NATIONAL_DEX_COUNT  NATIONAL_DEX_LUSCERP' 'National Dex count'

$speciesChecks = @{
    GHEPIO = @(
        '.baseHP        = 40', '.baseAttack    = 45', '.baseDefense   = 35',
        '.baseSpeed     = 70', '.baseSpAttack  = 30', '.baseSpDefense = 35',
        '.types = MON_TYPES(TYPE_FLYING)', '.catchRate = 255', '.expYield = 56',
        '.evYield_Speed = 1', '.abilities = { ABILITY_KEEN_EYE, ABILITY_BIG_PECKS, ABILITY_RECKLESS }',
        '.bodyColor = BODY_COLOR_BROWN', '.natDexNum = NATIONAL_DEX_GHEPIO',
        '.categoryName = _("FALCHETTO")', '.height = 3', '.weight = 21',
        'Rimane sospeso controvento scrutando', 'con sorprendente precisione.',
        '.frontPic = gMonFrontPic_Ghepio', '.backPic = gMonBackPic_Ghepio',
        '.palette = gMonPalette_Ghepio', '.shinyPalette = gMonShinyPalette_Ghepio',
        '.iconSprite = gMonIcon_Ghepio', '.iconPalIndex = 3',
        '.evolutions = EVOLUTION({EVO_LEVEL, 16, SPECIES_TINUNCOL})'
    )
    TINUNCOL = @(
        '.baseHP        = 55', '.baseAttack    = 65', '.baseDefense   = 50',
        '.baseSpeed     = 90', '.baseSpAttack  = 40', '.baseSpDefense = 50',
        '.types = MON_TYPES(TYPE_FLYING)', '.catchRate = 120', '.expYield = 113',
        '.evYield_Speed = 2', '.abilities = { ABILITY_KEEN_EYE, ABILITY_BIG_PECKS, ABILITY_RECKLESS }',
        '.bodyColor = BODY_COLOR_BROWN', '.natDexNum = NATIONAL_DEX_TINUNCOL',
        '.categoryName = _("GHEPPIO")', '.height = 6', '.weight = 72',
        'Studia le correnti ascensionali per ore', 'impercettibili delle ali.',
        '.frontPic = gMonFrontPic_Tinuncol', '.backPic = gMonBackPic_Tinuncol',
        '.palette = gMonPalette_Tinuncol', '.shinyPalette = gMonShinyPalette_Tinuncol',
        '.iconSprite = gMonIcon_Tinuncol', '.iconPalIndex = 3',
        '.evolutions = EVOLUTION({EVO_LEVEL, 34, SPECIES_PEREGRINUS})'
    )
    PEREGRINUS = @(
        '.baseHP        = 75', '.baseAttack    = 110', '.baseDefense   = 70',
        '.baseSpeed     = 120', '.baseSpAttack  = 55', '.baseSpDefense = 70',
        '.types = MON_TYPES(TYPE_FLYING, TYPE_FIGHTING)', '.catchRate = 45', '.expYield = 177',
        '.evYield_Speed = 2', '.abilities = { ABILITY_KEEN_EYE, ABILITY_DEFIANT, ABILITY_RECKLESS }',
        '.bodyColor = BODY_COLOR_GRAY', '.natDexNum = NATIONAL_DEX_PEREGRINUS',
        '.categoryName = _("PICCHIATA")', '.height = 11', '.weight = 234',
        'In picchiata concentra tutto il peso del', 'senza perdere velocit',
        '.frontPic = gMonFrontPic_Peregrinus', '.backPic = gMonBackPic_Peregrinus',
        '.palette = gMonPalette_Peregrinus', '.shinyPalette = gMonShinyPalette_Peregrinus',
        '.iconSprite = gMonIcon_Peregrinus', '.iconPalIndex = 3'
    )
}
$expectedBst = @{ GHEPIO = 255; TINUNCOL = 350; PEREGRINUS = 500 }
foreach ($name in $speciesChecks.Keys) {
    $record = Get-Section $speciesInfo "[SPECIES_$name] =" "`n    },"
    foreach ($expected in $speciesChecks[$name]) {
        Assert-Contains $record $expected "SPECIES_$name"
    }
    foreach ($common in @(
        '.genderRatio = PERCENT_FEMALE(50)', '.eggCycles = 15', '.friendship = 70',
        '.growthRate = GROWTH_MEDIUM_FAST', '.eggGroups = MON_EGG_GROUPS(EGG_GROUP_FLYING)',
        '.teachingType = EXPLICIT_TEACHABLES'
    )) {
        Assert-Contains $record $common "SPECIES_$name"
    }
    $statValues = [regex]::Matches($record, '\.base(?:HP|Attack|Defense|Speed|SpAttack|SpDefense)\s*=\s*(\d+)') | ForEach-Object { [int]$_.Groups[1].Value }
    Assert-True ($statValues.Count -eq 6) "SPECIES_$name does not define six base stats"
    Assert-True ((($statValues | Measure-Object -Sum).Sum) -eq $expectedBst[$name]) "SPECIES_$name BST differs from canon"
}

$expectedLevelMoves = @{
    GHEPIO = @('1:MOVE_PECK','1:MOVE_GROWL','4:MOVE_QUICK_ATTACK','7:MOVE_LEER','10:MOVE_WING_ATTACK','13:MOVE_FOCUS_ENERGY','16:MOVE_AERIAL_ACE','20:MOVE_AGILITY','24:MOVE_TAILWIND')
    TINUNCOL = @('1:MOVE_PECK','1:MOVE_GROWL','1:MOVE_QUICK_ATTACK','1:MOVE_LEER','10:MOVE_WING_ATTACK','13:MOVE_FOCUS_ENERGY','16:MOVE_AERIAL_ACE','20:MOVE_AGILITY','24:MOVE_TAILWIND','28:MOVE_DETECT','32:MOVE_ACROBATICS')
    PEREGRINUS = @('1:MOVE_PECK','1:MOVE_GROWL','1:MOVE_QUICK_ATTACK','1:MOVE_LEER','10:MOVE_WING_ATTACK','13:MOVE_FOCUS_ENERGY','16:MOVE_AERIAL_ACE','20:MOVE_AGILITY','24:MOVE_TAILWIND','28:MOVE_DETECT','32:MOVE_ACROBATICS','34:MOVE_CLOSE_COMBAT','38:MOVE_ROOST','42:MOVE_DUAL_WINGBEAT','46:MOVE_BRAVE_BIRD','50:MOVE_QUICK_GUARD')
}
foreach ($name in $expectedLevelMoves.Keys) {
    $displayName = $name.Substring(0,1) + $name.Substring(1).ToLower()
    $learnsetBlock = Get-Section $speciesInfo "static const struct LevelUpMove s${displayName}LevelUpLearnset[]" '};'
    $actual = [regex]::Matches($learnsetBlock, 'LEVEL_UP_MOVE\(\s*(\d+),\s*(MOVE_[A-Z0-9_]+)\)') | ForEach-Object { "$($_.Groups[1].Value):$($_.Groups[2].Value)" }
    Assert-True (($actual -join ',') -eq ($expectedLevelMoves[$name] -join ',')) "$name level-up learnset differs from canon"
}

$expectedLearnables = @{
    GHEPIO = @('MOVE_PROTECT','MOVE_REST','MOVE_SLEEP_TALK','MOVE_SUBSTITUTE','MOVE_SUNNY_DAY','MOVE_AERIAL_ACE','MOVE_ACROBATICS','MOVE_U_TURN','MOVE_FLY')
    TINUNCOL = @('MOVE_PROTECT','MOVE_REST','MOVE_SLEEP_TALK','MOVE_SUBSTITUTE','MOVE_SUNNY_DAY','MOVE_AERIAL_ACE','MOVE_ACROBATICS','MOVE_U_TURN','MOVE_FLY','MOVE_ROOST','MOVE_TAILWIND','MOVE_STEEL_WING')
    PEREGRINUS = @('MOVE_PROTECT','MOVE_REST','MOVE_SLEEP_TALK','MOVE_SUBSTITUTE','MOVE_SUNNY_DAY','MOVE_AERIAL_ACE','MOVE_ACROBATICS','MOVE_U_TURN','MOVE_FLY','MOVE_ROOST','MOVE_TAILWIND','MOVE_STEEL_WING','MOVE_CLOSE_COMBAT','MOVE_BRICK_BREAK','MOVE_LOW_SWEEP','MOVE_BULK_UP')
}
foreach ($name in $expectedLearnables.Keys) {
    $actual = @($learnables.$name)
    Assert-True ($actual.Count -eq $expectedLearnables[$name].Count) "$name teachable count differs"
    Assert-True (($actual -join ',') -eq ($expectedLearnables[$name] -join ',')) "$name teachables differ from canon"
}
$eggBlock = Get-Section $eggMoves 'static const u16 sGhepioEggMoveLearnset[]' '};'
foreach ($move in @('MOVE_FEINT','MOVE_QUICK_GUARD','MOVE_DEFOG','MOVE_SKY_ATTACK','MOVE_UNAVAILABLE')) {
    Assert-Contains $eggBlock $move 'Ghepio Egg Moves'
}

$placeholders = @{
    GHEPIO = 'Fletchling'
    TINUNCOL = 'Fletchinder'
    PEREGRINUS = 'Talonflame'
}
foreach ($name in $placeholders.Keys) {
    $placeholder = $placeholders[$name]
    $symbol = $name.Substring(0,1) + $name.Substring(1).ToLower()
    $record = Get-Section $speciesInfo "[SPECIES_$name] =" "`n    },"
    foreach ($reference in @(
        ".frontPic = gMonFrontPic_$symbol", ".backPic = gMonBackPic_$symbol",
        ".palette = gMonPalette_$symbol", ".shinyPalette = gMonShinyPalette_$symbol",
        ".iconSprite = gMonIcon_$symbol", '.frontAnimFrames = sAnims_SingleFramePlaceHolder',
        ".cryId = CRY_$($placeholder.ToUpper())",
        "FOOTPRINT($placeholder)", "sPicTable_$placeholder"
    )) {
        Assert-Contains $record $reference "SPECIES_$name placeholder"
    }
}

$alphabetical = Get-Section $orders 'const u16 gPokedexOrder_Alphabetical[]' 'const u16 gPokedexOrder_Weight[]'
$weight = Get-Section $orders 'const u16 gPokedexOrder_Weight[]' 'const u16 gPokedexOrder_Height[]'
$height = $orders.Substring($orders.IndexOf('const u16 gPokedexOrder_Height[]'))
foreach ($name in @('GHEPIO','TINUNCOL','PEREGRINUS')) {
    foreach ($section in @($alphabetical, $weight, $height)) {
        $count = ([regex]::Matches($section, "NATIONAL_DEX_$name(?![A-Z0-9_])")).Count
        Assert-True ($count -eq 1) "NATIONAL_DEX_$name must occur once in each Pokedex order"
    }
}

foreach ($row in @(
    '| 017 | Ghepio | `SPECIES_GHEPIO` = 1589 | `NATIONAL_DEX_GHEPIO` = 1042 |',
    '| 018 | Tinuncol | `SPECIES_TINUNCOL` = 1590 | `NATIONAL_DEX_TINUNCOL` = 1043 |',
    '| 019 | Peregrinus | `SPECIES_PEREGRINUS` = 1591 | `NATIONAL_DEX_PEREGRINUS` = 1044 |'
)) {
    Assert-Contains $docs $row 'Regional Dex documentation'
}
Assert-Contains $docs '| `AUS-FAM-FALCON` |' 'Falcon catalog row'
Assert-Contains (Get-Section $docs '| `AUS-FAM-FALCON` |' "`n") 'IMPLEMENTED; CANONICAL DESIGN' 'Catalog status'

foreach ($dexNum in 1041, 1042, 1043, 1044) {
    Assert-Contains $saveTests "        $dexNum," 'Save extension coverage'
}
Write-Host 'Functional Fauna Batch C validation passed.' -ForegroundColor Green
