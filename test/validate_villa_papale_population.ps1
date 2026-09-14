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
    @{ id = 'LOCALID_VILLA_PAPALE_GIARDINI_CUSTODE'; gfx = 'OBJ_EVENT_GFX_GENTLEMAN'; x = 24; y = 10; trainer = $false; script = 'VillaPapaleGiardini_EventScript_Custode' },
    @{ id = 'LOCALID_VILLA_PAPALE_GIARDINI_GIARDINIERE'; gfx = 'OBJ_EVENT_GFX_MAN_3'; x = 14; y = 20; trainer = $false; script = 'VillaPapaleGiardini_EventScript_Giardiniere' },
    @{ id = 'LOCALID_VILLA_PAPALE_GIARDINI_MANUTENTORE'; gfx = 'OBJ_EVENT_GFX_MAN_3'; x = 43; y = 33; trainer = $false; script = 'VillaPapaleGiardini_EventScript_Manutentore' },
    @{ id = 'LOCALID_VILLA_PAPALE_GIARDINI_STUDIOSA'; gfx = 'OBJ_EVENT_GFX_WOMAN_1'; x = 34; y = 15; trainer = $false; script = 'VillaPapaleGiardini_EventScript_Studiosa' },
    @{ id = 'LOCALID_VILLA_PAPALE_GIARDINI_TRAINER_GIARDINIERE'; gfx = 'OBJ_EVENT_GFX_WOMAN_1'; x = 13; y = 33; trainer = $true; script = 'VillaPapaleGiardini_EventScript_TrainerGiardiniere' },
    @{ id = 'LOCALID_VILLA_PAPALE_GIARDINI_TRAINER_MANUTENTORE'; gfx = 'OBJ_EVENT_GFX_CAMPER'; x = 48; y = 34; trainer = $true; script = 'VillaPapaleGiardini_EventScript_TrainerManutentore' },
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
    @{ id = 'LOCALID_VILLA_PAPALE_INTERNO_FUNZIONARIO'; gfx = 'OBJ_EVENT_GFX_GENTLEMAN'; x = 10; y = 22; script = 'VillaPapaleInterno_EventScript_Funzionario' },
    @{ id = 'LOCALID_VILLA_PAPALE_INTERNO_STUDIOSO'; gfx = 'OBJ_EVENT_GFX_SCIENTIST'; x = 5; y = 15; script = 'VillaPapaleInterno_EventScript_Studioso' },
    @{ id = 'LOCALID_VILLA_PAPALE_INTERNO_ASSISTENTE'; gfx = 'OBJ_EVENT_GFX_WOMAN_1'; x = 25; y = 4; script = 'VillaPapaleInterno_EventScript_Assistente' }
)
Assert-True (@($interior.object_events).Count -eq 3) 'Villa interior must contain exactly three ambient NPCs.'
foreach ($expected in $interiorExpected) {
    $event = @($interior.object_events | Where-Object { $_.local_id -eq $expected.id })
    Assert-True ($event.Count -eq 1 -and $event[0].graphics_id -eq $expected.gfx -and [int]$event[0].x -eq $expected.x -and [int]$event[0].y -eq $expected.y -and [int]$event[0].elevation -eq 3 -and $event[0].script -eq $expected.script -and $event[0].trainer_type -eq 'TRAINER_TYPE_NONE') "Villa interior event $($expected.id) is incorrect."
    Assert-True ((Get-Collision (Get-Block $interiorBlocks 32 $expected.x $expected.y)) -eq 0) "Villa interior event $($expected.id) is on blocked terrain."
}

foreach ($forbidden in @('NICO', 'LIA', 'AUREA', 'ECO', 'RIFLESSO')) {
    $pattern = "(?i)\b$forbidden\b"
    Assert-True ($gardenScripts -notmatch $pattern -and $interiorScripts -notmatch $pattern -and $gardensRaw -notmatch $pattern -and $interiorRaw -notmatch $pattern) "Villa population must not introduce $forbidden."
}
Assert-True ((Get-Content (Join-Path $RepositoryRoot 'src/data/wild_encounters.json') -Raw) -notmatch 'MAP_VILLA_PAPALE_(GIARDINI|INTERNO)') 'Villa population must not add encounters.'
Assert-True ($opponents -match '#define TRAINER_VILLA_PAPALE_GIARDINIERE\s+173' -and $opponents -match '#define TRAINER_VILLA_PAPALE_MANUTENTORE\s+462' -and $opponents -match '#define TRAINER_VILLA_PAPALE_APPRENDISTA\s+702') 'Emerald Villa trainer slots are incorrect.'
Assert-True ($opponentsFrlg -match '#define TRAINER_VILLA_PAPALE_GIARDINIERE\s+638' -and $opponentsFrlg -match '#define TRAINER_VILLA_PAPALE_MANUTENTORE\s+639' -and $opponentsFrlg -match '#define TRAINER_VILLA_PAPALE_APPRENDISTA\s+640' -and $opponentsFrlg -match '#define TRAINERS_COUNT_FRLG\s+641') 'FRLG Villa trainer slots are incorrect.'
foreach ($trainer in @('TRAINER_VILLA_PAPALE_GIARDINIERE', 'TRAINER_VILLA_PAPALE_MANUTENTORE', 'TRAINER_VILLA_PAPALE_APPRENDISTA')) {
    Assert-True ($emeraldParties -match "=== $trainer ===" -and $frlgParties -match "=== $trainer ===") "Villa trainer $trainer must have parties for Emerald and FRLG."
}
$changedBinaries = & git -C $RepositoryRoot diff --name-only -- data/layouts/VillaPapaleGiardini/map.bin data/layouts/VillaPapaleInterno/map.bin data/layouts/BorgoDiCastello/map.bin data/layouts/LagoDiAlbera/map.bin
Assert-True (@($changedBinaries).Count -eq 0) 'Villa population must not modify map.bin files.'
Write-Output 'Villa Papale population: PASS'
