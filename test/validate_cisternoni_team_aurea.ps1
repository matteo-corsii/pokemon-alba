$ErrorActionPreference = 'Stop'

$root = (Resolve-Path "$PSScriptRoot\\..").Path
$cisternoni = Get-Content -Raw (Join-Path $root 'data/maps/Cisternoni/map.json') | ConvertFrom-Json
$cisternoniScripts = Get-Content -Raw (Join-Path $root 'data/maps/Cisternoni/scripts.inc')
$routeScripts = Get-Content -Raw (Join-Path $root 'data/maps/Route103/scripts.inc')
$emeraldFlags = Get-Content -Raw (Join-Path $root 'include/constants/flags.h')
$frlgFlags = Get-Content -Raw (Join-Path $root 'include/constants/flags_frlg.h')
$opponents = Get-Content -Raw (Join-Path $root 'include/constants/opponents.h')
$opponentsFrlg = Get-Content -Raw (Join-Path $root 'include/constants/opponents_frlg.h')
$emeraldTrainers = Get-Content -Raw (Join-Path $root 'src/data/trainers.party')
$frlgTrainers = Get-Content -Raw (Join-Path $root 'src/data/trainers_frlg.party')

function Assert-True($condition, $message) {
    if (-not $condition) { throw $message }
}

Assert-True ($cisternoni.object_events.Count -eq 2) 'Cisternoni must contain only Lia and the Team Aurea recruit for this scene.'
$lia = $cisternoni.object_events | Where-Object { $_.local_id -eq 'LOCALID_CISTERNONI_LIA' }
$recruit = $cisternoni.object_events | Where-Object { $_.local_id -eq 'LOCALID_CISTERNONI_AUREA_RECRUIT' }
Assert-True ($null -ne $lia -and $lia.x -eq 15 -and $lia.y -eq 5 -and $lia.elevation -eq 3 -and $lia.graphics_id -eq 'OBJ_EVENT_GFX_MAY_NORMAL' -and $lia.flag -eq 'FLAG_HIDE_CISTERNONI_LIA') 'Lia Cisternoni event changed.'
Assert-True ($null -ne $recruit -and $recruit.x -eq 18 -and $recruit.y -eq 10 -and $recruit.elevation -eq 3 -and $recruit.graphics_id -eq 'OBJ_EVENT_GFX_WOMAN_2' -and $recruit.flag -eq 'FLAG_HIDE_CISTERNONI_AUREA_RECRUIT') 'Team Aurea recruit must remain a civilian-looking Woman 2 before the reveal.'
Assert-True ($cisternoni.coord_events.Count -eq 2) 'Cisternoni must retain exactly the two post-Gym scene triggers.'
foreach ($x in @(16, 17)) {
    $trigger = $cisternoni.coord_events | Where-Object { $_.x -eq $x -and $_.y -eq 6 }
    Assert-True ($null -ne $trigger -and $trigger.elevation -eq 3 -and $trigger.var -eq 'VAR_ALBERA_GYM_STATE' -and $trigger.var_value -eq '4' -and $trigger.script -eq 'Cisternoni_EventScript_StartAureaScene') "Cisternoni scene trigger ($x,6) changed."
}

foreach ($source in @($emeraldFlags, $frlgFlags)) {
    Assert-True ($source -match '#define FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE\s+0x8EE') 'Cisternoni completion flag must be 0x8EE.'
    Assert-True ($source -match '#define FLAG_HIDE_CISTERNONI_LIA\s+0x8EF') 'Lia visibility flag must be 0x8EF.'
    Assert-True ($source -match '#define FLAG_HIDE_CISTERNONI_AUREA_RECRUIT\s+0x8F0') 'Recruit visibility flag must be 0x8F0.'
}
Assert-True ($cisternoniScripts -match 'map_script MAP_SCRIPT_ON_LOAD, Cisternoni_OnLoad') 'Cisternoni visibility flags must be synchronized on map load.'
Assert-True ($cisternoniScripts -match 'goto_if_unset FLAG_BADGE01_GET, Cisternoni_OnLoad_HideScene') 'Cisternoni scene must be hidden before Badge 1.'
Assert-True ($cisternoniScripts -match 'goto_if_set FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE, Cisternoni_OnLoad_HideScene') 'Cisternoni scene must remain hidden after completion.'
Assert-True ($cisternoniScripts -match 'trainerbattle_single TRAINER_CISTERNONI_AUREA_RECRUIT') 'The Team Aurea confrontation must remain a mandatory single battle.'
Assert-True ($cisternoniScripts -match 'setflag FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE') 'The Team Aurea encounter must persist after victory.'
Assert-True ($cisternoniScripts -notmatch 'Salampolla') 'Salampolla must not be introduced by the Cisternoni encounter.'

foreach ($source in @($opponents, $opponentsFrlg)) {
    Assert-True ($source -match '#define TRAINER_CISTERNONI_MARCO') 'Marco trainer constant is missing.'
    Assert-True ($source -match '#define TRAINER_CISTERNONI_TEO') 'Teo trainer constant is missing.'
    Assert-True ($source -match '#define TRAINER_CISTERNONI_AUREA_RECRUIT') 'Team Aurea trainer constant is missing.'
}
foreach ($source in @($emeraldTrainers, $frlgTrainers)) {
    Assert-True ($source -match '(?s)=== TRAINER_CISTERNONI_AUREA_RECRUIT ===.*?Class: Team Aqua.*?Pic: Aqua Grunt F.*?Miciolo.*?Level: 12.*?Gazzuola.*?Level: 13') 'Team Aurea party must use Miciolo Lv. 12 and Gazzuola Lv. 13 ace with the approved temporary battle placeholder.'
    Assert-True ($source -notmatch '(?s)=== TRAINER_CISTERNONI_AUREA_RECRUIT ===.*?Salampolla') 'Team Aurea party must not include Salampolla.'
}
Assert-True ($routeScripts -match 'TRAINER_CISTERNONI_MARCO' -and $routeScripts -match 'TRAINER_CISTERNONI_TEO') 'Via dei Cisternoni must retain its two normal trainers.'

Write-Output 'Cisternoni Team Aurea validation passed.'
