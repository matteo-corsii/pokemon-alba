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
$speciesInfo = (Get-Content 'src/data/pokemon/species_info.h' -Raw -Encoding UTF8) -replace "`r`n", "`n"
$graphics = (Get-Content 'src/data/graphics/pokemon.h' -Raw) -replace "`r`n", "`n"
$eggMoves = (Get-Content 'src/data/pokemon/egg_moves.h' -Raw) -replace "`r`n", "`n"
$orders = (Get-Content 'src/data/pokemon/pokedex_orders.h' -Raw) -replace "`r`n", "`n"
$docs = (Get-Content 'docs/AUSONIA_REGIONAL_DEX_PLAN.md' -Raw -Encoding UTF8) -replace "`r`n", "`n"
$saveTests = (Get-Content 'test/save.c' -Raw) -replace "`r`n", "`n"
$learnables = Get-Content 'src/data/pokemon/all_learnables.json' -Raw | ConvertFrom-Json

Assert-Contains $speciesConstants "SPECIES_GAZZOMBRA,`n    SPECIES_MOLOSPSY,`n    SPECIES_LENGHELIS,`n    SPECIES_LUSCINCO,`n    SPECIES_LUSCERP,`n    SPECIES_LUMELLA,`n    SPECIES_OMPHALUX,`n    SPECIES_PALUDIX,`n    SPECIES_SANGUILEX,`n    SPECIES_CUSTOM_END," 'Species append-only sequence'
Assert-Contains $speciesConstants 'SPECIES_EGG = SPECIES_CUSTOM_END' 'Species_EGG definition'
Assert-Contains $speciesConstants 'NUM_SPECIES = SPECIES_EGG' 'NUM_SPECIES definition'
Assert-Contains $dexConstants "NATIONAL_DEX_GAZZOMBRA,`n    NATIONAL_DEX_MOLOSPSY," 'Pokédex append-only sequence'
Assert-Contains $dexConstants "NATIONAL_DEX_MOLOSPSY,`n    NATIONAL_DEX_LENGHELIS,`n    NATIONAL_DEX_LUSCINCO,`n    NATIONAL_DEX_LUSCERP," 'Luscinco/Luscerp append-only National Dex sequence'
Assert-Contains $dexConstants '#define NATIONAL_DEX_COUNT  NATIONAL_DEX_SANGUILEX' 'National Dex count'

$record = Get-Section $speciesInfo '[SPECIES_MOLOSPSY] =' "`n    },"
Assert-True ($speciesInfo.IndexOf('[SPECIES_MOLOSPSY] =') -lt $speciesInfo.IndexOf('/* You may add any custom species below this point based on the following structure: */')) 'Molospsy must remain the last custom species record'
foreach ($asset in @('anim_front.png', 'back.png', 'icon.png', 'normal.pal', 'shiny.pal')) {
    Assert-True (Test-Path "graphics/pokemon/molospsy/$asset") "Missing Molospsy asset: $asset"
}
foreach ($symbol in @('gMonFrontPic_Molospsy', 'gMonBackPic_Molospsy', 'gMonIcon_Molospsy', 'gMonPalette_Molospsy', 'gMonShinyPalette_Molospsy')) {
    Assert-Contains $graphics $symbol 'Molospsy graphics symbols'
}
foreach ($expected in @(
    '.baseHP        = 65', '.baseAttack    = 85', '.baseDefense   = 80',
    '.baseSpeed     = 45', '.baseSpAttack  = 60', '.baseSpDefense = 70',
    '.types = MON_TYPES(TYPE_FIGHTING, TYPE_PSYCHIC)', '.catchRate = 90',
    '.expYield = 145', '.evYield_Attack = 1', '.genderRatio = PERCENT_FEMALE(50)',
    '.eggCycles = 20', '.friendship = 50', '.growthRate = GROWTH_MEDIUM_FAST',
    '.eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD)',
    '.abilities = { ABILITY_INNER_FOCUS, ABILITY_STEADFAST, ABILITY_GUARD_DOG }',
    '.bodyColor = BODY_COLOR_GRAY', '.natDexNum = NATIONAL_DEX_MOLOSPSY',
    '.categoryName = _("GUARDIANO")', '.height = 12', '.weight = 610',
    'Molospsy sorveglia rovine e', 'si avvicina prima di muoversi.',
    '.frontPic = gMonFrontPic_Molospsy', '.backPic = gMonBackPic_Molospsy',
    '.palette = gMonPalette_Molospsy', '.shinyPalette = gMonShinyPalette_Molospsy',
    '.iconSprite = gMonIcon_Molospsy', '.iconPalIndex = 0',
    '.pokemonJumpType = PKMN_JUMP_TYPE_NONE',
    '.frontAnimFrames = sAnims_SingleFramePlaceHolder',
    '.levelUpLearnset = sMolospsyLevelUpLearnset',
    '.teachingType = EXPLICIT_TEACHABLES',
    '.teachableLearnset = sMolospsyTeachableLearnset',
    '.eggMoveLearnset = sMolospsyEggMoveLearnset'
)) {
    Assert-Contains $record $expected 'SPECIES_MOLOSPSY'
}
Assert-Contains $record 'FOOTPRINT(Mabosstiff)' 'Molospsy temporary footprint placeholder'
Assert-Contains $record 'sPicTable_Mabosstiff' 'Molospsy temporary overworld placeholder'
Assert-Contains $record 'gOverworldPalette_Mabosstiff' 'Molospsy temporary overworld palette placeholder'
Assert-Contains $record 'gShinyOverworldPalette_Mabosstiff' 'Molospsy temporary shiny overworld palette placeholder'
Assert-Contains $record '.cryId = CRY_MABOSSTIFF' 'Molospsy temporary cry placeholder'
Assert-True ($record -notmatch 'SPECIES_MOLOSPSY.*EVOLUTION\(') 'Molospsy must remain single stage'

$learnsetBlock = Get-Section $speciesInfo 'static const struct LevelUpMove sMolospsyLevelUpLearnset[]' '};'
$expectedLevelMoves = @('1:MOVE_TACKLE','1:MOVE_LEER','4:MOVE_MEDITATE','7:MOVE_LOW_KICK','10:MOVE_PROTECT','13:MOVE_CONFUSION','16:MOVE_DETECT','20:MOVE_HELPING_HAND','24:MOVE_PSYBEAM','28:MOVE_BULK_UP','32:MOVE_SAFEGUARD','36:MOVE_FORCE_PALM','40:MOVE_ZEN_HEADBUTT','44:MOVE_IRON_DEFENSE','48:MOVE_CALM_MIND','52:MOVE_WIDE_GUARD')
$actualLevelMoves = [regex]::Matches($learnsetBlock, 'LEVEL_UP_MOVE\(\s*(\d+),\s*(MOVE_[A-Z0-9_]+)\)') | ForEach-Object { "$($_.Groups[1].Value):$($_.Groups[2].Value)" }
Assert-True (($actualLevelMoves -join ',') -eq ($expectedLevelMoves -join ',')) 'Molospsy level-up learnset differs from canon'

$teachableExpected = @('MOVE_PROTECT','MOVE_REST','MOVE_SLEEP_TALK','MOVE_SUBSTITUTE','MOVE_ENDURE','MOVE_MEDITATE','MOVE_SWIFT','MOVE_LOW_KICK','MOVE_ROCK_SMASH','MOVE_HELPING_HAND','MOVE_DETECT','MOVE_FORCE_PALM','MOVE_BULK_UP','MOVE_CALM_MIND','MOVE_SAFEGUARD','MOVE_IRON_DEFENSE','MOVE_ZEN_HEADBUTT','MOVE_POWER_UP_PUNCH','MOVE_QUICK_GUARD','MOVE_WIDE_GUARD','MOVE_PSYCH_UP','MOVE_MIRROR_COAT')
Assert-True ($learnables.MOLOSPSY.Count -eq $teachableExpected.Count) 'Molospsy teachable count differs'
for ($i = 0; $i -lt $teachableExpected.Count; $i++) {
    Assert-True ($learnables.MOLOSPSY[$i] -eq $teachableExpected[$i]) "Molospsy teachables differ at index $i"
}
$learnableNames = @($learnables.PSObject.Properties.Name)
$molospsyIndex = [Array]::IndexOf($learnableNames, 'MOLOSPSY')
Assert-True ($molospsyIndex -ge 0) 'Molospsy must remain in all_learnables.json'
Assert-True (($molospsyIndex + 1) -lt $learnableNames.Count -and $learnableNames[$molospsyIndex + 1] -ceq 'LENGHELIS') 'Lenghelis must follow Molospsy in all_learnables.json'
Assert-True ($learnableNames[-1] -ceq 'SANGUILEX') 'Sanguilex must be the last custom learnables entry'

$eggBlock = Get-Section $eggMoves 'static const u16 sMolospsyEggMoveLearnset[]' '};'
foreach ($move in @('MOVE_COUNTER','MOVE_DETECT','MOVE_ENDURE','MOVE_HELPING_HAND','MOVE_MIRROR_COAT','MOVE_POWER_UP_PUNCH','MOVE_QUICK_GUARD','MOVE_WIDE_GUARD','MOVE_UNAVAILABLE')) {
    Assert-Contains $eggBlock $move 'Molospsy Egg Moves'
}

$alphabetical = Get-Section $orders 'const u16 gPokedexOrder_Alphabetical[]' 'const u16 gPokedexOrder_Weight[]'
$weight = Get-Section $orders 'const u16 gPokedexOrder_Weight[]' 'const u16 gPokedexOrder_Height[]'
$height = $orders.Substring($orders.IndexOf('const u16 gPokedexOrder_Height[]'))
foreach ($section in @($alphabetical, $weight, $height)) {
    $count = ([regex]::Matches($section, 'NATIONAL_DEX_MOLOSPSY(?![A-Z0-9_])')).Count
    Assert-True ($count -eq 1) 'NATIONAL_DEX_MOLOSPSY must occur once in each Pokédex order'
}

Assert-Contains $docs '### Molospsy' 'Molospsy documentation section'
Assert-Contains $docs '| 023 | Molospsy | `SPECIES_MOLOSPSY` = 1595 | `NATIONAL_DEX_MOLOSPSY` = 1048 | Lotta/Psico | stadio singolo |' 'Molospsy regional dex row'
Assert-Contains $docs '| `AUS-CONCEPT-MOLOSPSY` | Molospsy | 1+ TBD | IMPLEMENTED; CANONICAL DESIGN; PLACEMENT CANDIDATE |' 'Molospsy catalog row'

foreach ($dexNum in 1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048) {
    Assert-Contains $saveTests "        $dexNum," 'Save extension coverage'
}

Write-Host 'Molospsy validation passed.' -ForegroundColor Green
