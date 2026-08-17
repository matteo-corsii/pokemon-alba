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
$flags = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/flags.h') -Raw
$flagsFrlg = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/flags_frlg.h') -Raw
$opponents = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/opponents.h') -Raw
$opponentsFrlg = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/opponents_frlg.h') -Raw
$scripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/AlberaStorica_Anfiteatro/scripts.inc') -Raw
$eventScripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/event_scripts.s') -Raw
$emeraldTrainers = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/trainers.party') -Raw
$frlgTrainers = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/trainers_frlg.party') -Raw
$blockdata = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/ContestHall/map.bin'))

Assert-True ((@($groups.gMapGroup_TownsAndRoutes | Where-Object { $_ -eq 'AlberaStorica_Anfiteatro' }).Count) -eq 1) 'Amphitheatre map missing or duplicated.'
Assert-True ((@($layouts.layouts | Where-Object { $_.id -eq 'LAYOUT_ALBERA_STORICA_ANFITEATRO' }).Count) -eq 1) 'Amphitheatre layout missing or duplicated.'
Assert-True ($gym.id -eq 'MAP_ALBERA_STORICA_ANFITEATRO' -and $gym.layout -eq 'LAYOUT_ALBERA_STORICA_ANFITEATRO') 'Map and layout are not linked.'
Assert-True ($gym.music -eq 'MUS_GYM' -and $gym.battle_scene -eq 'MAP_BATTLE_SCENE_GYM') 'Unexpected Gym map presentation.'
Assert-True ($gym.region_map_section -eq 'MAPSEC_ALBERA_STORICA' -and $null -eq $gym.connections) 'Unexpected regional map section or connection.'
Assert-True (@($city.coord_events | Where-Object { $_.x -in @(17, 18) -and $_.y -eq 4 -and $_.elevation -eq 3 -and $_.var -eq 'VAR_ALBERA_GYM_INPUT' -and $_.var_value -eq '0' -and $_.script -eq 'AlberaStorica_EventScript_EnterAnfiteatro' }).Count -eq 2) 'City-to-Amphitheatre entrance triggers are invalid.'
Assert-True ($cityScripts -match '(?s)AlberaStorica_EventScript_EnterAnfiteatro::\s*warp MAP_ALBERA_STORICA_ANFITEATRO, 7, 10\s*waitstate\s*end') 'City-to-Amphitheatre entrance script is invalid.'
Assert-True (@($city.warp_events | Where-Object { $_.x -eq 17 -and $_.y -eq 5 -and $_.elevation -eq 0 -and $_.dest_map -eq 'MAP_ALBERA_STORICA_ANFITEATRO' -and $_.dest_warp_id -eq '0' }).Count -eq 1) 'Amphitheatre return anchor is invalid.'
Assert-True (@($gym.coord_events | Where-Object { $_.x -eq 7 -and $_.y -eq 10 -and $_.elevation -eq 3 -and $_.var -eq 'VAR_TEMP_1' -and $_.var_value -eq '0' -and $_.script -eq 'AlberaStorica_Anfiteatro_EventScript_Exit' }).Count -eq 1) 'Amphitheatre exit trigger is invalid.'
Assert-True ($scripts -match '(?s)AlberaStorica_Anfiteatro_EventScript_Exit::\s*setvar VAR_ALBERA_GYM_INPUT, 0\s*warp MAP_ALBERA_STORICA, 17, 5\s*waitstate\s*end') 'Amphitheatre return script is invalid.'
Assert-True ($eventScripts.Contains('.include "data/maps/AlberaStorica_Anfiteatro/scripts.inc"')) 'Amphitheatre scripts are not registered.'
Assert-True ($scripts -match '(?s)map_script MAP_SCRIPT_ON_FRAME_TABLE, AlberaStorica_Anfiteatro_OnFrame.*?map_script_2 VAR_TEMP_0, 0, AlberaStorica_Anfiteatro_EventScript_ShowTutorial.*?setflag FLAG_ALBERA_GYM_TUTORIAL_SEEN') 'One-time Gym tutorial setup is invalid.'

foreach ($pair in @(@('VAR_ALBERA_GYM_STATE', '0x40F9'), @('VAR_ALBERA_GYM_INPUT', '0x40FA'))) {
    Assert-True ($vars -match "#define\s+$($pair[0])\s+$($pair[1])\b") "Missing Emerald $($pair[0])."
    Assert-True ($varsFrlg -match "#define\s+$($pair[0])\s+$($pair[1])\b") "Missing FRLG $($pair[0])."
}

foreach ($pair in @(@('FLAG_ALBERA_GYM_TUTORIAL_SEEN', '0x8E5'), @('FLAG_ALBERA_GYM_STROFA_I_COMPLETE', '0x8E6'), @('FLAG_ALBERA_GYM_STROFA_II_COMPLETE', '0x8E7'), @('FLAG_ALBERA_GYM_STROFA_III_COMPLETE', '0x8E8'))) {
    Assert-True ($flags -match "#define\s+$($pair[0])\s+$($pair[1])\b") "Missing Emerald $($pair[0])."
    Assert-True ($flagsFrlg -match "#define\s+$($pair[0])\s+$($pair[1])\b") "Missing FRLG $($pair[0])."
}
foreach ($legacy in @('FLAG_UNUSED_0x8E5', 'FLAG_UNUSED_0x8E6', 'FLAG_UNUSED_0x8E7', 'FLAG_UNUSED_0x8E8')) {
    Assert-True (-not $flags.Contains($legacy)) "Legacy Emerald flag alias remains: $legacy"
}
foreach ($legacy in @('FLAG_0x8E5', 'FLAG_0x8E6', 'FLAG_0x8E7', 'FLAG_0x8E8')) {
    Assert-True ($flagsFrlg -notmatch "#define\s+$legacy\s+\(SYS_FLAGS") "Active FRLG flag alias remains: $legacy"
}

foreach ($pair in @(@('TRAINER_ALBERA_DARIO', '855', '624'), @('TRAINER_ALBERA_MARA', '856', '625'), @('TRAINER_ALBERA_ELIO', '857', '626'), @('TRAINER_LEADER_LIRIO', '858', '627'))) {
    Assert-True ($opponents -match "#define\s+$($pair[0])\s+$($pair[1])\b") "Invalid Emerald trainer ID for $($pair[0])."
    Assert-True ($opponentsFrlg -match "#define\s+$($pair[0])\s+$($pair[2])\b") "Invalid FRLG trainer ID for $($pair[0])."
}
Assert-True ($opponents -match '#define\s+TRAINERS_COUNT_EMERALD\s+862\b') 'Emerald trainer count must include the approved Via dei Cisternoni trainers.'
Assert-True ($opponentsFrlg -match '#define\s+TRAINERS_COUNT_FRLG\s+631\b') 'FRLG trainer count must include the approved Via dei Cisternoni trainers.'
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

foreach ($device in @(@(4, 'Tamburo'), @(7, 'Corda'), @(10, 'Voce'))) {
    Assert-True (@($gym.bg_events | Where-Object { $_.x -eq $device[0] -and $_.y -eq 5 -and $_.script -eq "AlberaStorica_Anfiteatro_EventScript_$($device[1])" }).Count -eq 1) "Missing $($device[1]) sign event."
    Assert-True (@($gym.object_events | Where-Object { $_.graphics_id -eq 'OBJ_EVENT_GFX_SIGN' -and $_.x -eq $device[0] -and $_.y -eq 5 -and $_.elevation -eq 3 -and $_.script -eq "AlberaStorica_Anfiteatro_EventScript_$($device[1])" }).Count -eq 1) "Missing visible $($device[1]) post."
    $word = [BitConverter]::ToUInt16($blockdata, ((5 * 15) + $device[0]) * 2)
    Assert-True ($word -eq 0x3281) "Missing physical device cube for $($device[1])."
}

foreach ($token in @('FLAG_ALBERA_GYM_TUTORIAL_SEEN', 'AlberaStorica_Anfiteatro_Text_Tutorial', 'TAMBuro!', 'CORDA!', 'VOCE!', 'Simbolo: CERCHIO.', 'Simbolo: TRE LINEE.', 'Simbolo: ONDA.', 'Riprova: TAMBuro -> CORDA.', 'Riprova: VOCE -> CORDA -> VOCE.', 'Riprova: TAMBuro -> CORDA -> VOCE.', 'TRAINER_ALBERA_DARIO', 'TRAINER_ALBERA_MARA', 'TRAINER_ALBERA_ELIO', 'TRAINER_LEADER_LIRIO', 'FLAG_ALBERA_GYM_STROFA_I_COMPLETE', 'FLAG_ALBERA_GYM_STROFA_II_COMPLETE', 'FLAG_ALBERA_GYM_STROFA_III_COMPLETE', 'setvar VAR_ALBERA_GYM_STATE, 1', 'setvar VAR_ALBERA_GYM_STATE, 2', 'setvar VAR_ALBERA_GYM_STATE, 3', 'setvar VAR_ALBERA_GYM_STATE, 4', 'setvar VAR_ALBERA_GYM_INPUT, 0', 'FLAG_BADGE01_GET', 'MEDAGLIA BALLATA', "L'eco è tornato prima della nota.")) {
    Assert-True ($scripts.Contains($token)) "Missing Gym script token: $token"
}
Assert-True ([regex]::Matches($scripts, '\btrainerbattle_single\b').Count -eq 4) 'Gym device completion must not start trainer battles automatically.'
foreach ($required in @('AlberaStorica_Anfiteatro_EventScript_DarioBattle:', 'AlberaStorica_Anfiteatro_EventScript_MaraBattle:', 'AlberaStorica_Anfiteatro_EventScript_ElioBattle:', 'AlberaStorica_Anfiteatro_Text_StrofaICompletata:', 'AlberaStorica_Anfiteatro_Text_StrofaIICompletata:', 'AlberaStorica_Anfiteatro_Text_StrofaIIICompletata:', 'AlberaStorica_Anfiteatro_Text_LirioSbloccato:')) {
    Assert-True ($scripts.Contains($required)) "Missing Gym progression fixture: $required"
}
Assert-True (-not $scripts.Contains('giveitem ITEM_TM')) 'No MT may be assigned in Demo 0.1.'
Assert-True ($scripts -match '(?s)AlberaStorica_Anfiteatro_EventScript_LirioDefeated:.*?setflag FLAG_BADGE01_GET.*?setvar VAR_ALBERA_GYM_STATE, 4.*?Text_MedagliaBallata.*?Text_LirioPostVictoryText.*?releaseall\s*end') 'Lirio post-battle flow must end safely after badge and dialogue.'
Assert-True ($scripts -notmatch '(?s)AlberaStorica_Anfiteatro_EventScript_LirioDefeated:.*?\breturn\b') 'Lirio post-battle flow must not return without a caller.'
Assert-True (@($gym.object_events | Where-Object { $_.local_id -eq 'LOCALID_ALBERA_GYM_LIRIO' -and $_.graphics_id -eq 'OBJ_EVENT_GFX_ARTIST' }).Count -eq 1) 'Lirio must use the male Artist overworld sprite.'
Assert-True ($emeraldTrainers -match '(?s)=== TRAINER_LEADER_LIRIO ===.*?Pic: Leader Brawly') 'Lirio Emerald trainer pic must be male.'
Assert-True ($frlgTrainers -match '(?s)=== TRAINER_LEADER_LIRIO ===.*?Pic: Leader Lt Surge Frlg') 'Lirio FRLG trainer pic must be male.'

foreach ($path in @('src/data/wild_encounters.json', 'src/data/pokemon', 'src/save.c', 'include/constants/species.h')) {
    & git -C $RepositoryRoot diff --quiet develop -- $path
    Assert-True ($LASTEXITCODE -eq 0) "Out-of-scope file changed: $path"
}
$changedArtifacts = @(& git -C $RepositoryRoot diff --name-only develop -- '*.gba' '*.elf' '*.map' '*.zip')
$untrackedArtifacts = @(& git -C $RepositoryRoot ls-files --others --exclude-standard -- '*.gba' '*.elf' '*.map' '*.zip')
Assert-True (($changedArtifacts.Count + $untrackedArtifacts.Count) -eq 0) 'Build artifacts or archives were found.'

Write-Output 'Albera Amphitheatre Gym validation passed.'
