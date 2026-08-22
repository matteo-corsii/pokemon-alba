$ErrorActionPreference = 'Stop'

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
Set-Location $repoRoot

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
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
$speciesInfo = (Get-Content 'src/data/pokemon/species_info.h' -Raw -Encoding UTF8) -replace "`r`n", "`n"
$eggMoves = (Get-Content 'src/data/pokemon/egg_moves.h' -Raw) -replace "`r`n", "`n"
$orders = (Get-Content 'src/data/pokemon/pokedex_orders.h' -Raw) -replace "`r`n", "`n"
$docs = (Get-Content 'docs/AUSONIA_REGIONAL_DEX_PLAN.md' -Raw -Encoding UTF8) -replace "`r`n", "`n"
$saveTests = (Get-Content 'test/save.c' -Raw) -replace "`r`n", "`n"
$learnables = Get-Content 'src/data/pokemon/all_learnables.json' -Raw | ConvertFrom-Json

Assert-Contains $speciesConstants "SPECIES_PEREGRINUS,`n    SPECIES_GAZZUOLA,`n    SPECIES_BRILLAZZA,`n    SPECIES_GAZZOMBRA,`n    SPECIES_MOLOSPSY,`n    SPECIES_LENGHELIS,`n    SPECIES_CUSTOM_END," 'Species append-only sequence'
Assert-Contains $dexConstants "NATIONAL_DEX_PEREGRINUS,`n    NATIONAL_DEX_GAZZUOLA,`n    NATIONAL_DEX_BRILLAZZA,`n    NATIONAL_DEX_GAZZOMBRA,`n    NATIONAL_DEX_MOLOSPSY,`n    NATIONAL_DEX_LENGHELIS," 'Pokedex append-only sequence'
Assert-Contains $dexConstants '#define NATIONAL_DEX_COUNT  NATIONAL_DEX_LENGHELIS' 'National Dex count'

$expected = @{
    GAZZUOLA = @{ Stats=@(45,40,40,60,35,40); Bst=260; Type='MON_TYPES(TYPE_NORMAL, TYPE_FLYING)'; Catch=255; Exp=56; Height=3; Weight=19; Abilities='ABILITY_PICKUP, ABILITY_KEEN_EYE, ABILITY_SUPER_LUCK'; Category='CURIOSA'; Evolution='EVO_LEVEL, 18, SPECIES_BRILLAZZA'; Placeholder='Rookidee'; Dex=1045; Regional=20 }
    BRILLAZZA = @{ Stats=@(60,65,55,80,45,55); Bst=360; Type='MON_TYPES(TYPE_DARK, TYPE_FLYING)'; Catch=120; Exp=116; Height=6; Weight=55; Abilities='ABILITY_PICKUP, ABILITY_FRISK, ABILITY_SUPER_LUCK'; Category='MONILE'; Evolution='EVO_LEVEL, 34, SPECIES_GAZZOMBRA'; Placeholder='Corvisquire'; Dex=1046; Regional=21 }
    GAZZOMBRA = @{ Stats=@(75,95,70,100,55,75); Bst=470; Type='MON_TYPES(TYPE_DARK, TYPE_FLYING)'; Catch=60; Exp=170; Height=9; Weight=98; Abilities='ABILITY_FRISK, ABILITY_PICKPOCKET, ABILITY_SUPER_LUCK'; Category='BOTTINO'; Evolution=$null; Placeholder='Corviknight'; Dex=1047; Regional=22 }
}
foreach ($name in $expected.Keys) {
    $value = $expected[$name]
    $record = Get-Section $speciesInfo "[SPECIES_$name] =" "`n    },"
    foreach ($field in @(
        ".types = $($value.Type)", ".catchRate = $($value.Catch)", ".expYield = $($value.Exp)",
        ".abilities = { $($value.Abilities) }", '.genderRatio = PERCENT_FEMALE(50)',
        '.eggCycles = 15', '.friendship = 70', '.growthRate = GROWTH_MEDIUM_FAST',
        '.eggGroups = MON_EGG_GROUPS(EGG_GROUP_FLYING)', '.bodyColor = BODY_COLOR_BLACK',
        ".natDexNum = NATIONAL_DEX_$name", ".categoryName = _(`"$($value.Category)`")",
        ".height = $($value.Height)", ".weight = $($value.Weight)", '.teachingType = EXPLICIT_TEACHABLES',
        ".cryId = CRY_$($value.Placeholder.ToUpper())", "FOOTPRINT($($value.Placeholder))", "sPicTable_$($value.Placeholder)"
    )) { Assert-Contains $record $field "SPECIES_$name" }
    if ($null -ne $value.Evolution) { Assert-Contains $record ".evolutions = EVOLUTION({$($value.Evolution)})" "SPECIES_$name evolution" }
    $stats = [regex]::Matches($record, '\.base(?:HP|Attack|Defense|Speed|SpAttack|SpDefense)\s*=\s*(\d+)') | ForEach-Object { [int]$_.Groups[1].Value }
    Assert-True (($stats -join ',') -eq ($value.Stats -join ',')) "SPECIES_$name base stats differ"
    Assert-True ((($stats | Measure-Object -Sum).Sum) -eq $value.Bst) "SPECIES_$name BST differs"
    Assert-True ($null -ne $learnables.$name) "$name is missing from all_learnables.json"
}

$expectedLearnables = @{
    GAZZUOLA = @('MOVE_PROTECT','MOVE_REST','MOVE_SLEEP_TALK','MOVE_SUBSTITUTE','MOVE_AERIAL_ACE','MOVE_ACROBATICS','MOVE_U_TURN','MOVE_THIEF','MOVE_FLY')
    BRILLAZZA = @('MOVE_PROTECT','MOVE_REST','MOVE_SLEEP_TALK','MOVE_SUBSTITUTE','MOVE_AERIAL_ACE','MOVE_ACROBATICS','MOVE_U_TURN','MOVE_THIEF','MOVE_FLY','MOVE_TAUNT','MOVE_SNARL','MOVE_DARK_PULSE','MOVE_FOUL_PLAY')
    GAZZOMBRA = @('MOVE_PROTECT','MOVE_REST','MOVE_SLEEP_TALK','MOVE_SUBSTITUTE','MOVE_AERIAL_ACE','MOVE_ACROBATICS','MOVE_U_TURN','MOVE_THIEF','MOVE_FLY','MOVE_TAUNT','MOVE_SNARL','MOVE_DARK_PULSE','MOVE_FOUL_PLAY','MOVE_KNOCK_OFF','MOVE_ROOST','MOVE_TAILWIND','MOVE_STEEL_WING')
}
foreach ($name in $expectedLearnables.Keys) {
    $actual = @($learnables.$name)
    Assert-True (($actual -join ',') -eq ($expectedLearnables[$name] -join ',')) "$name teachables differ from canon"
}

$eggBlock = Get-Section $eggMoves 'static const u16 sGazzuolaEggMoveLearnset[]' '};'
foreach ($move in @('MOVE_FEATHER_DANCE','MOVE_DEFOG','MOVE_ROOST','MOVE_SWITCHEROO','MOVE_UNAVAILABLE')) { Assert-Contains $eggBlock $move 'Gazzuola Egg Moves' }

$alphabetical = Get-Section $orders 'const u16 gPokedexOrder_Alphabetical[]' 'const u16 gPokedexOrder_Weight[]'
$weight = Get-Section $orders 'const u16 gPokedexOrder_Weight[]' 'const u16 gPokedexOrder_Height[]'
$height = $orders.Substring($orders.IndexOf('const u16 gPokedexOrder_Height[]'))
foreach ($name in @('GAZZUOLA','BRILLAZZA','GAZZOMBRA')) {
    foreach ($section in @($alphabetical, $weight, $height)) {
        Assert-True (([regex]::Matches($section, "NATIONAL_DEX_$name(?![A-Z0-9_])")).Count -eq 1) "NATIONAL_DEX_$name must occur once in each Pokedex order"
    }
}

foreach ($row in @(
    '| 020 | Gazzuola | `SPECIES_GAZZUOLA` = 1592 | `NATIONAL_DEX_GAZZUOLA` = 1045 |',
    '| 021 | Brillazza | `SPECIES_BRILLAZZA` = 1593 | `NATIONAL_DEX_BRILLAZZA` = 1046 |',
    '| 022 | Gazzombra | `SPECIES_GAZZOMBRA` = 1594 | `NATIONAL_DEX_GAZZOMBRA` = 1047 |'
)) { Assert-Contains $docs $row 'Regional Dex documentation' }
Assert-Contains $docs '| `AUS-FAM-MAGPIE` |' 'Magpie catalog row'
Assert-Contains (Get-Section $docs '| `AUS-FAM-MAGPIE` |' "`n") 'IMPLEMENTED; CANONICAL DESIGN' 'Magpie catalog status'

foreach ($dexNum in 1041..1048) { Assert-Contains $saveTests "        $dexNum," 'Save extension coverage' }
Assert-Contains $saveTests 'EXPECT_EQ(gSaveBlock1Ptr->extendedDexSeen[0], 0xFF);' 'Extended seen flags'
Assert-Contains $saveTests 'EXPECT_EQ(gSaveBlock1Ptr->extendedDexCaught[0], 0xFF);' 'Extended caught flags'

Write-Host 'Functional Fauna Batch D validation passed.' -ForegroundColor Green
