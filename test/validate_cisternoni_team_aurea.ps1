$ErrorActionPreference = 'Stop'

$root = (Resolve-Path "$PSScriptRoot\\..").Path
$cisternoni = Get-Content -Raw (Join-Path $root 'data/maps/Cisternoni/map.json') | ConvertFrom-Json
$route = Get-Content -Raw (Join-Path $root 'data/maps/Route103/map.json') | ConvertFrom-Json
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
    $trigger = $cisternoni.coord_events | Where-Object { $_.x -eq $x -and $_.y -eq 9 }
    Assert-True ($null -ne $trigger -and $trigger.elevation -eq 3 -and $trigger.var -eq 'VAR_ALBERA_GYM_STATE' -and $trigger.var_value -eq '4' -and $trigger.script -eq 'Cisternoni_EventScript_StartAureaScene') "Cisternoni scene trigger ($x,9) changed."
}

foreach ($source in @($emeraldFlags, $frlgFlags)) {
    Assert-True ($source -match '#define FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE\s+0x8EE') 'Cisternoni completion flag must be 0x8EE.'
    Assert-True ($source -match '#define FLAG_HIDE_CISTERNONI_LIA\s+0x8EF') 'Lia visibility flag must be 0x8EF.'
    Assert-True ($source -match '#define FLAG_HIDE_CISTERNONI_AUREA_RECRUIT\s+0x8F0') 'Recruit visibility flag must be 0x8F0.'
    Assert-True ($source -match '#define FLAG_CISTERNONI_LIA_READY\s+0x8F1') 'Lia route-to-interior handoff flag must be 0x8F1.'
    Assert-True ($source -match '#define FLAG_HIDE_ROUTE103_LIA\s+0x8F2') 'Route103 Lia visibility flag must be 0x8F2.'
    Assert-True ($source -match '#define FLAG_HIDE_ROUTE103_NICO\s+0x8F3') 'Route103 Nico visibility flag must be 0x8F3.'
    Assert-True ($source -match '#define FLAG_CISTERNONI_NICO_POST_AUREA_TALKED\s+0x8F4') 'Nico reminder flag must be 0x8F4.'
}
Assert-True ($cisternoniScripts -match 'map_script MAP_SCRIPT_ON_LOAD, Cisternoni_OnLoad') 'Cisternoni visibility flags must be synchronized on map load.'
Assert-True ($cisternoniScripts -match 'goto_if_unset FLAG_BADGE01_GET, Cisternoni_OnLoad_HideScene') 'Cisternoni scene must be hidden before Badge 1.'
Assert-True ($cisternoniScripts -match 'goto_if_unset FLAG_CISTERNONI_LIA_READY, Cisternoni_OnLoad_HideScene') 'Cisternoni scene must wait until Lia directs the player inside.'
Assert-True ($cisternoniScripts -match 'goto_if_set FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE, Cisternoni_OnLoad_HideScene') 'Cisternoni scene must remain hidden after completion.'
Assert-True ($cisternoniScripts -notmatch 'trainerbattle_single TRAINER_CISTERNONI_AUREA_RECRUIT') 'The coord-triggered Team Aurea scene must not use the object-event trainer battle command.'
Assert-True ($cisternoniScripts -match 'msgbox Cisternoni_Text_AureaBattleIntro, MSGBOX_DEFAULT\s*\r?\n\s*trainerbattle_no_intro TRAINER_CISTERNONI_AUREA_RECRUIT, Cisternoni_Text_AureaDefeat\s*\r?\n\s*goto Cisternoni_EventScript_AureaDefeated') 'The Team Aurea cutscene must use the player-safe no-intro battle pattern and continue into the post-battle script.'
Assert-True ($cisternoniScripts -match 'setflag FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE') 'The Team Aurea encounter must persist after victory.'
Assert-True ($cisternoniScripts -notmatch 'Salampolla') 'Salampolla must not be introduced by the Cisternoni encounter.'

$secretBaseConstants = Get-Content -Raw (Join-Path $root 'include/constants/secret_bases.h')
$secretBaseCode = Get-Content -Raw (Join-Path $root 'src/secret_base.c')
Assert-True ($secretBaseConstants -match '#define\s+SECRET_BASE_CISTERNONI_TREE_1\s+252\b') 'Via dei Cisternoni Secret Base must use the freed append-only ID 252.'
Assert-True ($secretBaseConstants -match '#define\s+SECRET_BASE_CISTERNONI_TREE\s+SECRET_BASE_GROUP\(25\)') 'Via dei Cisternoni Secret Base must use its own group.'
Assert-True ($secretBaseConstants -match '#define\s+NUM_SECRET_BASE_GROUPS\s+26\b') 'Secret Base group count must include the Via dei Cisternoni tree.'
Assert-True ($secretBaseCode -match '\[SECRET_BASE_CISTERNONI_TREE\]\s*=\s*MAP_NUM\(MAP_SECRET_BASE_TREE1\),\s*0,\s*2,\s*3') 'Via dei Cisternoni tree must reuse the SecretBase_Tree1 interior entry.'
$cisternoniTree = @($route.bg_events | Where-Object { $_.type -eq 'secret_base' -and $_.x -eq 26 -and $_.y -eq 5 -and $_.elevation -eq 3 -and $_.secret_base_id -eq 'SECRET_BASE_CISTERNONI_TREE_1' })
Assert-True ($cisternoniTree.Count -eq 1) 'Via dei Cisternoni tree must have one dedicated Secret Base event at (26,5).'
$routeBlockdata = [System.IO.File]::ReadAllBytes((Join-Path $root 'data/layouts/Route103/map.bin'))
foreach ($cell in @(@{ X = 26; Y = 5; Raw = 0x3036 }, @{ X = 27; Y = 5; Raw = 0x3037 })) {
    $raw = [System.BitConverter]::ToUInt16($routeBlockdata, 2 * ($cell.Y * 80 + $cell.X))
    Assert-True ($raw -eq $cell.Raw) "Via dei Cisternoni Secret Base tree cell ($($cell.X),$($cell.Y)) must match the approved manual collision state."
}

$routeLia = $route.object_events | Where-Object { $_.local_id -eq 'LOCALID_ROUTE103_LIA' }
$routeNico = $route.object_events | Where-Object { $_.local_id -eq 'LOCALID_ROUTE103_NICO' }
Assert-True ($null -ne $routeLia -and $routeLia.x -eq 10 -and $routeLia.y -eq 14 -and $routeLia.graphics_id -eq 'OBJ_EVENT_GFX_MAY_NORMAL' -and $routeLia.flag -eq 'FLAG_HIDE_ROUTE103_LIA') 'Lia must preserve the approved Porymap position on Via dei Cisternoni with her route visibility flag.'
Assert-True ($null -ne $routeNico -and $routeNico.x -eq 50 -and $routeNico.y -eq 10 -and $routeNico.graphics_id -eq 'OBJ_EVENT_GFX_BRENDAN_NORMAL' -and $routeNico.flag -eq 'FLAG_HIDE_ROUTE103_NICO') 'Nico must be available near the Cisternoni exit only after the Team Aurea encounter.'
Assert-True ($route.coord_events.Count -eq 2) 'Lia must use exactly two walkable approach triggers because her approved visual position is not directly talkable.'
foreach ($coord in @(@(13, 14), @(13, 15))) {
    $trigger = $route.coord_events | Where-Object { $_.x -eq $coord[0] -and $_.y -eq $coord[1] }
    Assert-True ($null -ne $trigger -and $trigger.elevation -eq 3 -and $trigger.var -eq 'VAR_ALBERA_GYM_STATE' -and $trigger.var_value -eq '4' -and $trigger.script -eq 'Route103_EventScript_LiaBeforeCisternoni') "Lia approach trigger ($($coord[0]),$($coord[1])) changed."
}
Assert-True ($routeScripts -match 'map_script MAP_SCRIPT_ON_LOAD, Route103_OnLoad') 'Route103 must synchronize Lia/Nico visibility on map load.'
Assert-True ($routeScripts -notmatch 'Route103_EventScript_CisternoniAccessClosed|Route103_Text_CisternoniAccessClosed|Route103_EventScript_CloseCisternoniEntrance|Route103_EventScript_OpenCisternoniEntrance') 'Route103 must not retain obsolete Cisternoni physical access blocker scripts.'
Assert-True ($routeScripts -notmatch 'setmetatile 52, 7') 'Route103 must not dynamically block or unblock the Cisternoni entrance.'
Assert-True ($routeScripts -notmatch 'call_if_(?:un)?set FLAG_BADGE01_GET, Route103_EventScript_(?:Close|Open)CisternoniEntrance') 'Cisternoni physical access must not depend on Badge 1.'
Assert-True ($routeScripts -match 'goto_if_unset FLAG_BADGE01_GET, Route103_OnLoad_HideParty') 'Lia and Nico must both be hidden before Badge 1.'
Assert-True ($routeScripts -match 'goto_if_set FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE, Route103_OnLoad_ShowNico') 'Nico must appear after the Cisternoni Team Aurea encounter.'
Assert-True ($routeScripts -match 'goto_if_set FLAG_CISTERNONI_LIA_READY, Route103_OnLoad_HideParty') 'Lia must not remain duplicated outside once she has directed the player inside.'
Assert-True ($routeScripts -match 'clearflag FLAG_HIDE_ROUTE103_LIA[\s\S]*setflag FLAG_HIDE_ROUTE103_NICO') 'Before the Cisternoni scene, Lia must be visible and Nico hidden.'
Assert-True ($routeScripts -match 'setflag FLAG_HIDE_ROUTE103_LIA[\s\S]*clearflag FLAG_HIDE_ROUTE103_NICO') 'After the Cisternoni scene, Lia must be hidden and Nico visible.'
Assert-True ($routeScripts -match 'Route103_Text_LiaBeforeCisternoni:[\s\S]*documenti[\s\S]*indizio[\s\S]*CISTERNONI[\s\S]*Entriamo insieme') 'Lia route dialogue must reference the documents, the clue, and entering the Cisternoni.'
Assert-True ($routeScripts -match 'Route103_Text_NicoAfterCisternoni:[\s\S]*tipa losca[\s\S]*CISTERNONI[\s\S]*battuto LIRIO[\s\S]*VIA CONSOLARE') 'Nico dialogue must reference the suspicious woman, Lirio, and Via Consolare.'
Assert-True ($routeScripts -match 'Route103_Text_NicoReminder:[\s\S]*Direzione VIA CONSOLARE') 'Nico must have the approved short repeat reminder.'

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
