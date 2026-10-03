param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Read-Json([string]$path) { Get-Content (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Get-Block([byte[]]$blockdata, [int]$width, [int]$x, [int]$y) { [BitConverter]::ToUInt16($blockdata, 2 * ($y * $width + $x)) }
function Get-Collision([int]$block) { (($block -shr 10) -band 1) }
function Get-Behavior([byte[]]$primary, [byte[]]$secondary, [int]$block) {
    $id = $block -band 0x3ff
    if ($id -lt 0x200) { return $primary[2 * $id] }
    return $secondary[2 * ($id - 0x200)]
}

$gardens = Read-Json 'data/maps/VillaPapaleGiardini/map.json'
$interior = Read-Json 'data/maps/VillaPapaleInterno/map.json'
$gardensRaw = Get-Content (Join-Path $RepositoryRoot 'data/maps/VillaPapaleGiardini/map.json') -Raw
$interiorRaw = Get-Content (Join-Path $RepositoryRoot 'data/maps/VillaPapaleInterno/map.json') -Raw
$gardenScripts = Get-Content (Join-Path $RepositoryRoot 'data/maps/VillaPapaleGiardini/scripts.inc') -Raw
$interiorScripts = Get-Content (Join-Path $RepositoryRoot 'data/maps/VillaPapaleInterno/scripts.inc') -Raw
$emeraldParties = Get-Content (Join-Path $RepositoryRoot 'src/data/trainers.party') -Raw
$frlgParties = Get-Content (Join-Path $RepositoryRoot 'src/data/trainers_frlg.party') -Raw
$opponents = Get-Content (Join-Path $RepositoryRoot 'include/constants/opponents.h') -Raw
$opponentsFrlg = Get-Content (Join-Path $RepositoryRoot 'include/constants/opponents_frlg.h') -Raw
$gardenBlocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/VillaPapaleGiardini/map.bin'))
$interiorBlocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/VillaPapaleInterno/map.bin'))
$generalAttributes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/primary/general/metatile_attributes.bin'))
$sootopolisAttributes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/secondary/sootopolis/metatile_attributes.bin'))

$gardenExpected = @(
    @{ id = 'LOCALID_VILLA_PAPALE_GIARDINI_CUSTODE'; gfx = 'OBJ_EVENT_GFX_GENTLEMAN'; x = 20; y = 15; trainer = $false; script = 'VillaPapaleGiardini_EventScript_Custode' },
    @{ id = 'LOCALID_VILLA_PAPALE_GIARDINI_GIARDINIERE'; gfx = 'OBJ_EVENT_GFX_MAN_3'; x = 11; y = 17; trainer = $false; script = 'VillaPapaleGiardini_EventScript_Giardiniere' },
    @{ id = 'LOCALID_VILLA_PAPALE_GIARDINI_MANUTENTORE'; gfx = 'OBJ_EVENT_GFX_MAN_3'; x = 41; y = 33; trainer = $false; script = 'VillaPapaleGiardini_EventScript_Manutentore' },
    @{ id = 'LOCALID_VILLA_PAPALE_GIARDINI_STUDIOSA'; gfx = 'OBJ_EVENT_GFX_WOMAN_1'; x = 36; y = 14; trainer = $false; script = 'VillaPapaleGiardini_EventScript_Studiosa' },
    @{ id = 'LOCALID_VILLA_PAPALE_GIARDINI_TRAINER_GIARDINIERE'; gfx = 'OBJ_EVENT_GFX_WOMAN_1'; x = 13; y = 33; trainer = $true; script = 'VillaPapaleGiardini_EventScript_TrainerGiardiniere' },
    @{ id = 'LOCALID_VILLA_PAPALE_GIARDINI_TRAINER_MANUTENTORE'; gfx = 'OBJ_EVENT_GFX_CAMPER'; x = 47; y = 34; trainer = $true; script = 'VillaPapaleGiardini_EventScript_TrainerManutentore' },
    @{ id = 'LOCALID_VILLA_PAPALE_GIARDINI_TRAINER_APPRENDISTA'; gfx = 'OBJ_EVENT_GFX_BUG_CATCHER'; x = 42; y = 50; trainer = $true; script = 'VillaPapaleGiardini_EventScript_TrainerApprendista' }
)

Assert-True (@($gardens.object_events).Count -eq 7) 'Villa gardens must contain exactly seven ambient or optional trainer events.'
Assert-True (@($gardens.object_events | Where-Object { $_.trainer_type -eq 'TRAINER_TYPE_NORMAL' }).Count -eq 3) 'Villa gardens must contain exactly three optional trainers.'
foreach ($expected in $gardenExpected) {
    $event = @($gardens.object_events | Where-Object { $_.local_id -eq $expected.id })
    Assert-True ($event.Count -eq 1 -and $event[0].graphics_id -eq $expected.gfx -and [int]$event[0].x -eq $expected.x -and [int]$event[0].y -eq $expected.y -and [int]$event[0].elevation -eq 3 -and $event[0].script -eq $expected.script) "Villa gardens event $($expected.id) is incorrect."
    Assert-True (($event[0].trainer_type -eq 'TRAINER_TYPE_NORMAL') -eq $expected.trainer) "Villa gardens trainer classification for $($expected.id) is incorrect."
    $block = Get-Block $gardenBlocks 60 $expected.x $expected.y
    $behavior = Get-Behavior $generalAttributes $sootopolisAttributes $block
    Assert-True ((Get-Collision $block) -eq 0 -and $behavior -ne 2 -and -not ($behavior -ge 0x10 -and $behavior -le 0x19)) "Villa gardens event $($expected.id) is on blocked terrain, grass, or water."
    Assert-True (-not ($expected.x -ge 28 -and $expected.x -le 30 -and $expected.y -ge 13) -and -not ($expected.x -eq 29 -and $expected.y -eq 9) -and -not ($expected.x -ge 28 -and $expected.x -le 30 -and $expected.y -eq 59)) "Villa gardens event $($expected.id) blocks a mandatory route or entrance."
}

$interiorExpected = @(
    @{ id = 'LOCALID_VILLA_PAPALE_INTERNO_FUNZIONARIO'; gfx = 'OBJ_EVENT_GFX_GENTLEMAN'; x = 15; y = 18; script = 'VillaPapaleInterno_EventScript_Funzionario' },
    @{ id = 'LOCALID_VILLA_PAPALE_INTERNO_STUDIOSO'; gfx = 'OBJ_EVENT_GFX_SCIENTIST'; x = 5; y = 16; script = 'VillaPapaleInterno_EventScript_Studioso' },
    @{ id = 'LOCALID_VILLA_PAPALE_INTERNO_ASSISTENTE'; gfx = 'OBJ_EVENT_GFX_WOMAN_1'; x = 20; y = 5; script = 'VillaPapaleInterno_EventScript_Assistente' }
)
$interiorAmbient = @($interior.object_events | Where-Object { $_.flag -eq '0' })
Assert-True ($interiorAmbient.Count -eq 4) 'Villa interior must contain the three ambient NPCs and the Archivista.'
foreach ($expected in $interiorExpected) {
    $event = @($interior.object_events | Where-Object { $_.local_id -eq $expected.id })
    Assert-True ($event.Count -eq 1 -and $event[0].graphics_id -eq $expected.gfx -and [int]$event[0].x -eq $expected.x -and [int]$event[0].y -eq $expected.y -and [int]$event[0].elevation -eq 3 -and $event[0].script -eq $expected.script -and $event[0].trainer_type -eq 'TRAINER_TYPE_NONE') "Villa interior event $($expected.id) is incorrect."
    Assert-True ((Get-Collision (Get-Block $interiorBlocks 32 $expected.x $expected.y)) -eq 0) "Villa interior event $($expected.id) is on blocked terrain."
}

$interiorPopulationScripts = $interiorScripts -replace '(?s)VillaPapaleInterno_EventScript_Archivista::.*?VillaPapaleInterno_EventScript_Funzionario::', 'VillaPapaleInterno_EventScript_Funzionario::'
$interiorPopulationScripts = $interiorPopulationScripts -replace '(?s)VillaPapaleInterno_Text_ArchivistaNotReady:.*\z', ''
foreach ($forbidden in @('NICO', 'LIA', 'AUREA', 'ECO', 'RIFLESSO')) {
    $pattern = "(?i)\b$forbidden\b"
    Assert-True ($gardenScripts -notmatch $pattern -and $interiorPopulationScripts -notmatch $pattern -and $gardensRaw -notmatch $pattern) "Villa population must not introduce $forbidden."
}
$wild = Read-Json 'src/data/wild_encounters.json'
$villaEncounters = @($wild.wild_encounter_groups | ForEach-Object { $_.encounters } | Where-Object { $_.map -eq 'MAP_VILLA_PAPALE_GIARDINI' })
$villaRates = @(20, 20, 10, 10, 10, 10, 5, 5, 4, 4, 1, 1)
$villaSpecies = @('SPECIES_MICIOLO', 'SPECIES_GAZZUOLA', 'SPECIES_MICIOLO', 'SPECIES_BRILLAZZA', 'SPECIES_CRISALVIA', 'SPECIES_CRISALVIA', 'SPECIES_GAZZUOLA', 'SPECIES_BRILLAZZA', 'SPECIES_FELIVATES', 'SPECIES_FELIVATES', 'SPECIES_FELIVATES', 'SPECIES_FELIVATES')
$villaLevels = @(@(16,18), @(16,18), @(16,18), @(18,20), @(17,19), @(17,19), @(16,18), @(18,20), @(19,21), @(19,21), @(19,21), @(19,21))
Assert-True ($villaEncounters.Count -eq 4) 'Villa gardens must have exactly four identical time-of-day land encounter tables.'
foreach ($label in @('gVillaPapaleGiardini_Morning', 'gVillaPapaleGiardini_Day', 'gVillaPapaleGiardini_Evening', 'gVillaPapaleGiardini_Night')) {
    $table = @($villaEncounters | Where-Object { $_.base_label -eq $label })
    Assert-True ($table.Count -eq 1 -and [int]$table[0].land_mons.encounter_rate -eq 20 -and @($table[0].land_mons.mons).Count -eq 12) "Villa encounter table $label is incomplete."
    for ($index = 0; $index -lt 12; $index++) {
        $mon = $table[0].land_mons.mons[$index]
        Assert-True ($mon.species -eq $villaSpecies[$index] -and [int]$mon.min_level -eq $villaLevels[$index][0] -and [int]$mon.max_level -eq $villaLevels[$index][1]) "Villa encounter slot $index in $label is incorrect."
    }
    Assert-True (-not (@($table[0].land_mons.mons | Where-Object { $_.species -eq 'SPECIES_INFIORALA' }).Count)) "Villa encounter table $label must not contain Infiorala."
}
foreach ($expected in @{ 'SPECIES_MICIOLO' = 30; 'SPECIES_GAZZUOLA' = 25; 'SPECIES_CRISALVIA' = 20; 'SPECIES_BRILLAZZA' = 15; 'SPECIES_FELIVATES' = 10 }.GetEnumerator()) {
    $total = 0
    for ($index = 0; $index -lt 12; $index++) { if ($villaSpecies[$index] -eq $expected.Key) { $total += $villaRates[$index] } }
    Assert-True ($total -eq $expected.Value) "Villa encounter rate for $($expected.Key) is incorrect."
}
Assert-True (@($wild.wild_encounter_groups | ForEach-Object { $_.encounters } | Where-Object { $_.map -eq 'MAP_VILLA_PAPALE_INTERNO' }).Count -eq 0) 'Villa interior must not add encounters.'
Assert-True ($opponents -match '#define TRAINER_VILLA_PAPALE_GIARDINIERE\s+173' -and $opponents -match '#define TRAINER_VILLA_PAPALE_MANUTENTORE\s+462' -and $opponents -match '#define TRAINER_VILLA_PAPALE_APPRENDISTA\s+702') 'Emerald Villa trainer slots are incorrect.'
Assert-True ($opponentsFrlg -match '#define TRAINER_VILLA_PAPALE_GIARDINIERE\s+638' -and $opponentsFrlg -match '#define TRAINER_VILLA_PAPALE_MANUTENTORE\s+639' -and $opponentsFrlg -match '#define TRAINER_VILLA_PAPALE_APPRENDISTA\s+640' -and $opponentsFrlg -match '#define TRAINERS_COUNT_FRLG\s+641') 'FRLG Villa trainer slots are incorrect.'
foreach ($trainer in @('TRAINER_VILLA_PAPALE_GIARDINIERE', 'TRAINER_VILLA_PAPALE_MANUTENTORE', 'TRAINER_VILLA_PAPALE_APPRENDISTA')) {
    Assert-True ($emeraldParties -match "=== $trainer ===" -and $frlgParties -match "=== $trainer ===") "Villa trainer $trainer must have parties for Emerald and FRLG."
}
$expectedParties = @{
    'TRAINER_VILLA_PAPALE_GIARDINIERE' = @('Crisalvia', 'Felivates')
    'TRAINER_VILLA_PAPALE_MANUTENTORE' = @('Pastufo', 'Molospsy')
    'TRAINER_VILLA_PAPALE_APPRENDISTA' = @('Miciolo', 'Brillazza', 'Luscinco')
}
foreach ($trainer in $expectedParties.Keys) {
    foreach ($partyText in @($emeraldParties, $frlgParties)) {
        $record = [regex]::Match($partyText, "(?ms)=== $trainer ===(.*?)(?=^=== |\z)").Groups[1].Value
        $actualSpecies = @([regex]::Matches($record, '(?m)^([A-Za-z]+)\r?$') | ForEach-Object { $_.Groups[1].Value } | Where-Object { $_ -notin @('Name', 'Class', 'Pic', 'Gender', 'Music', 'Double', 'AI', 'Level', 'IVs') })
        $actualLevels = @([regex]::Matches($record, '(?m)^Level: (\d+)\r?$') | ForEach-Object { [int]$_.Groups[1].Value })
        Assert-True (($actualSpecies -join ',') -eq ($expectedParties[$trainer] -join ',') -and $actualLevels.Count -eq $expectedParties[$trainer].Count -and @($actualLevels | Where-Object { $_ -ne 23 }).Count -eq 0) "Villa trainer $trainer party is not canonical in Emerald/FRLG."
    }
}
$changedBinaries = & git -C $RepositoryRoot diff --name-only -- data/layouts/VillaPapaleGiardini/map.bin data/layouts/VillaPapaleInterno/map.bin data/layouts/BorgoDiCastello/map.bin data/layouts/LagoDiAlbera/map.bin
Assert-True (@($changedBinaries).Count -eq 0) 'Villa population must not modify map.bin files.'
Write-Output 'Villa Papale population: PASS'
