param(
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

function Read-Json([string]$RelativePath) {
    return [IO.File]::ReadAllText((Join-Path $RepositoryRoot $RelativePath), [Text.Encoding]::UTF8) | ConvertFrom-Json
}

$groups = Read-Json 'data/maps/map_groups.json'
$layouts = Read-Json 'data/layouts/layouts.json'
$city = Read-Json 'data/maps/AlberaStorica/map.json'
$gym = Read-Json 'data/maps/AlberaStorica_Anfiteatro/map.json'
$cityScripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/AlberaStorica/scripts.inc') -Raw
$vars = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/vars.h') -Raw
$varsFrlg = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/vars_frlg.h') -Raw
$opponents = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/opponents.h') -Raw
$opponentsFrlg = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/opponents_frlg.h') -Raw
$scripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/AlberaStorica_Anfiteatro/scripts.inc') -Raw
$eventScripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/event_scripts.s') -Raw
$emeraldTrainers = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/trainers.party') -Raw
$frlgTrainers = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/trainers_frlg.party') -Raw

Assert-True ((@($groups.gMapGroup_TownsAndRoutes | Where-Object { $_ -eq 'AlberaStorica_Anfiteatro' }).Count) -eq 1) 'Amphitheatre map missing or duplicated.'
Assert-True ((@($layouts.layouts | Where-Object { $_.id -eq 'LAYOUT_ALBERA_STORICA_ANFITEATRO' }).Count) -eq 1) 'Amphitheatre layout missing or duplicated.'
Assert-True ($gym.id -eq 'MAP_ALBERA_STORICA_ANFITEATRO' -and $gym.layout -eq 'LAYOUT_ALBERA_STORICA_ANFITEATRO') 'Map and layout are not linked.'
Assert-True ($gym.music -eq 'MUS_GYM' -and $gym.battle_scene -eq 'MAP_BATTLE_SCENE_GYM') 'Unexpected Gym map presentation.'
Assert-True ($gym.region_map_section -eq 'MAPSEC_ALBERA_STORICA' -and $null -eq $gym.connections) 'Unexpected regional map section or connection.'
Assert-True (@($city.coord_events | Where-Object { $_.x -in @(17, 18) -and $_.y -eq 4 -and $_.elevation -eq 3 -and $_.var -eq 'VAR_ALBERA_GYM_INPUT' -and $_.var_value -eq '0' -and $_.script -eq 'AlberaStorica_EventScript_EnterAnfiteatro' }).Count -eq 2) 'City-to-Amphitheatre entrance triggers are invalid.'
Assert-True ($cityScripts.Contains("warp MAP_ALBERA_STORICA_ANFITEATRO, 7, 10")) 'City-to-Amphitheatre entrance script is invalid.'
Assert-True (@($city.warp_events | Where-Object { $_.x -eq 17 -and $_.y -eq 5 -and $_.elevation -eq 0 -and $_.dest_map -eq 'MAP_ALBERA_STORICA_ANFITEATRO' -and $_.dest_warp_id -eq '0' }).Count -eq 1) 'Amphitheatre return anchor is invalid.'
Assert-True (@($gym.warp_events | Where-Object { $_.dest_map -eq 'MAP_ALBERA_STORICA' -and $_.dest_warp_id -eq '0' }).Count -eq 1) 'Amphitheatre return warp is invalid.'
Assert-True ($eventScripts.Contains('.include "data/maps/AlberaStorica_Anfiteatro/scripts.inc"')) 'Amphitheatre scripts are not registered.'

foreach ($pair in @(@('VAR_ALBERA_GYM_STATE', '0x40F9'), @('VAR_ALBERA_GYM_INPUT', '0x40FA'))) {
    Assert-True ($vars -match "#define\s+$($pair[0])\s+$($pair[1])\b") "Missing Emerald $($pair[0])."
    Assert-True ($varsFrlg -match "#define\s+$($pair[0])\s+$($pair[1])\b") "Missing FRLG $($pair[0])."
}

foreach ($pair in @(@('TRAINER_ALBERA_DARIO', '855', '624'), @('TRAINER_ALBERA_MARA', '856', '625'), @('TRAINER_ALBERA_ELIO', '857', '626'), @('TRAINER_LEADER_LIRIO', '858', '627'))) {
    Assert-True ($opponents -match "#define\s+$($pair[0])\s+$($pair[1])\b") "Invalid Emerald trainer ID for $($pair[0])."
    Assert-True ($opponentsFrlg -match "#define\s+$($pair[0])\s+$($pair[2])\b") "Invalid FRLG trainer ID for $($pair[0])."
}
Assert-True ($opponents -match '#define\s+TRAINERS_COUNT_EMERALD\s+859\b') 'Emerald trainer count must be 859.'
Assert-True ($opponentsFrlg -match '#define\s+TRAINERS_COUNT_FRLG\s+628\b') 'FRLG trainer count must be 628.'
Assert-True ($opponents -match '#define\s+MAX_TRAINERS_COUNT_EMERALD\s+864\b') 'Emerald trainer capacity changed.'
Assert-True ($opponentsFrlg -match '#define\s+MAX_TRAINERS_COUNT_FRLG\s+768\b') 'FRLG trainer capacity changed.'

foreach ($text in @($emeraldTrainers, $frlgTrainers)) {
    foreach ($required in @('=== TRAINER_ALBERA_DARIO ===', '=== TRAINER_ALBERA_MARA ===', '=== TRAINER_ALBERA_ELIO ===', '=== TRAINER_LEADER_LIRIO ===')) {
        Assert-True ($text.Contains($required)) "Missing trainer fixture: $required"
    }
    foreach ($required in @('Borgotto\s+Level:\s+8', 'Miciolo\s+Level:\s+9', 'Gazzuola\s+Level:\s+10', 'Borgotto\s+Level:\s+9', 'Gazzuola\s+Level:\s+11')) {
        Assert-True ($text -match $required) "Missing trainer level fixture: $required"
    }
}

foreach ($token in @('Tamburo', 'Corda', 'Voce', 'CERCHIO', 'TRE LINEE', 'ONDA', 'TRAINER_ALBERA_DARIO', 'TRAINER_ALBERA_MARA', 'TRAINER_ALBERA_ELIO', 'TRAINER_LEADER_LIRIO', 'setvar VAR_ALBERA_GYM_STATE, 1', 'setvar VAR_ALBERA_GYM_STATE, 2', 'setvar VAR_ALBERA_GYM_STATE, 3', 'setvar VAR_ALBERA_GYM_STATE, 4', 'setvar VAR_ALBERA_GYM_INPUT, 0', 'FLAG_BADGE01_GET', 'MEDAGLIA BALLATA', "L'eco è tornato prima della nota.")) {
    Assert-True ($scripts.Contains($token)) "Missing Gym script token: $token"
}
Assert-True (-not $scripts.Contains('giveitem ITEM_TM')) 'No MT may be assigned in Demo 0.1.'

foreach ($path in @('src/data/wild_encounters.json', 'src/data/pokemon', 'src/save.c', 'include/constants/species.h')) {
    & git -C $RepositoryRoot diff --quiet develop -- $path
    Assert-True ($LASTEXITCODE -eq 0) "Out-of-scope file changed: $path"
}
$changedArtifacts = @(& git -C $RepositoryRoot diff --name-only develop -- '*.gba' '*.elf' '*.map' '*.zip')
$untrackedArtifacts = @(& git -C $RepositoryRoot ls-files --others --exclude-standard -- '*.gba' '*.elf' '*.map' '*.zip')
Assert-True (($changedArtifacts.Count + $untrackedArtifacts.Count) -eq 0) 'Build artifacts or archives were found.'

Write-Output 'Albera Amphitheatre Gym validation passed.'
