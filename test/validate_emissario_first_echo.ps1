$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

$root = Split-Path -Parent $PSScriptRoot
$read = { param([string]$path) Get-Content -LiteralPath (Join-Path $root $path) -Raw }
$emissario = (& $read 'data/maps/Emissario/map.json') | ConvertFrom-Json
$via = (& $read 'data/maps/ViaConsolare/map.json') | ConvertFrom-Json
$lago = (& $read 'data/maps/LagoDiAlbera/map.json') | ConvertFrom-Json
$emissarioScripts = & $read 'data/maps/Emissario/scripts.inc'
$viaScripts = & $read 'data/maps/ViaConsolare/scripts.inc'
$lagoScripts = & $read 'data/maps/LagoDiAlbera/scripts.inc'
$emeraldOpponents = & $read 'include/constants/opponents.h'
$frlgOpponents = & $read 'include/constants/opponents_frlg.h'
$flags = & $read 'include/constants/flags.h'
$flagsFrlg = & $read 'include/constants/flags_frlg.h'
$emeraldParty = & $read 'src/data/trainers.party'
$frlgParty = & $read 'src/data/trainers_frlg.party'
$firstEcho = & $read 'src/first_echo.c'
$firstEchoHeader = & $read 'include/first_echo.h'
$battleSetup = & $read 'src/battle_setup.c'
$battleSwitchIn = & $read 'src/battle_switch_in.c'
$battleCommands = & $read 'src/battle_script_commands.c'
$battleScripts = & $read 'data/battle_scripts_1.s'
$battleStruct = & $read 'include/battle.h'
$battleAnimConstants = & $read 'include/constants/battle_anim.h'
$battleAnimTable = & $read 'src/battle_anim.c'
$battleAnimScripts = & $read 'data/battle_anim_scripts.s'
$battleStrings = & $read 'src/battle_message.c'
$specials = & $read 'data/specials.inc'
$runtimeTests = & $read 'test/first_echo.c'

Assert-True (($emissario.object_events | Measure-Object).Count -eq 3) 'Emissario must contain Lia, Nico and the second Aurea recruit only.'
foreach ($expectation in @(
    @{id='LOCALID_EMISSARIO_LIA'; x=14; y=24; flag='FLAG_HIDE_EMISSARIO_LIA'},
    @{id='LOCALID_EMISSARIO_NICO'; x=17; y=24; flag='FLAG_HIDE_EMISSARIO_NICO'},
    @{id='LOCALID_EMISSARIO_AUREA_RECRUIT'; x=15; y=22; flag='FLAG_HIDE_EMISSARIO_AUREA_RECRUIT'})) {
    $obj = $emissario.object_events | Where-Object local_id -eq $expectation.id
    Assert-True ($null -ne $obj -and $obj.x -eq $expectation.x -and $obj.y -eq $expectation.y -and $obj.elevation -eq 3 -and $obj.flag -eq $expectation.flag) "Invalid Emissario placement: $($expectation.id)."
}
Assert-True (($via.coord_events | Where-Object script -eq 'ViaConsolare_EventScript_StartEmissarioLead').Count -eq 6) 'Via Consolare must retain all six Emissario lead triggers.'
Assert-True (($lago.coord_events | Where-Object script -eq 'LagoDiAlbera_EventScript_StartEmissarioReunion').Count -eq 6) 'Lago must retain exactly six reunion triggers.'
Assert-True (($lago.coord_events | Where-Object { $_.script -eq 'LagoDiAlbera_EventScript_StartEmissarioReunion' -and $_.x -in 78,79 }).Count -eq 0) 'Lago reunion triggers must not use x=78 or x=79.'
Assert-True ($emissarioScripts -match 'trainerbattle_no_intro TRAINER_EMISSARIO_AUREA_RECRUIT') 'Emissario must use the narrative trainer battle flow.'
Assert-True ($emissarioScripts -match 'setflag FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE') 'Emissario completion flag is missing.'
Assert-True ($emissarioScripts -match 'BORGO DI CASTELLO') 'The post-battle direction must be Borgo di Castello.'
Assert-True ($viaScripts -match 'FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE') 'Via scene must remain gated after Cisternoni.'
Assert-True ($lagoScripts -match 'FLAG_RECEIVED_HM_SURF') 'Lago reunion must remain gated by Surf.'
foreach ($definition in @(
    'FLAG_ORIGINAL_STARTER_ID_REGISTERED\s+0x8FD',
    'FLAG_VIA_CONSOLARE_EMISSARIO_LEAD_COMPLETE\s+0x8FE',
    'FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE\s+0x8FF',
    'FLAG_FIRST_ECHO_SEEN\s+0x900')) {
    Assert-True ($flags -match $definition) "Missing Emerald flag: $definition"
    Assert-True ($flagsFrlg -match $definition) "Missing FRLG flag: $definition"
}
Assert-True ($emeraldOpponents -match 'TRAINER_EMISSARIO_AUREA_RECRUIT\s+853') 'Emerald must reuse inactive trainer slot 853.'
Assert-True ($frlgOpponents -match 'TRAINER_EMISSARIO_AUREA_RECRUIT\s+637' -and $frlgOpponents -match 'TRAINERS_COUNT_FRLG\s+638' -and $frlgOpponents -match 'MAX_TRAINERS_COUNT_FRLG\s+768') 'FRLG trainer allocation is invalid.'
Assert-True ($emeraldParty -match '(?s)=== TRAINER_EMISSARIO_AUREA_RECRUIT ===.*?Salampolla.*?Level: 23.*?Cisternide.*?Level: 25') 'Emerald Aurea party must be Salampolla 23 then Cisternide 25.'
Assert-True ($frlgParty -match '(?s)=== TRAINER_EMISSARIO_AUREA_RECRUIT ===.*?Salampolla.*?Level: 23.*?Cisternide.*?Level: 25') 'FRLG Aurea party must be Salampolla 23 then Cisternide 25.'

Assert-True ($firstEcho -match '(?s)sFirstEchoTriggers\[\].*?TRAINER_EMISSARIO_AUREA_RECRUIT\s*,\s*SPECIES_CISTERNIDE\s*,\s*FIRST_ECHO_EFFECT_ATK_SPATK') 'The first Echo must be configured by trainer, switch-in species and effect data.'
Assert-True ($firstEcho -notmatch '\bMAP_[A-Z0-9_]+\b') 'The first-Echo battle trigger must not depend on the loaded map.'
Assert-True ($firstEcho -match 'gBattleStruct->firstEchoEffect\s*=\s*trigger->effect' -and $firstEcho -match 'switch \(gBattleStruct->firstEchoEffect\)') 'The configured first-Echo effect must drive runtime application.'
Assert-True ($battleSwitchIn -match '(?s)DoSwitchInEvents\(void\).*?FirstEcho_TryActivateOnSwitchIn\(\)') 'The first Echo must be hooked into actual switch-in events.'
Assert-True ($firstEcho -match 'gBattleStruct->firstEchoTriggered' -and $firstEcho -match 'gBattleStruct->firstEchoTriggered\s*=\s*TRUE') 'The first Echo must have a once-per-battle runtime guard.'
Assert-True ($battleStruct -match 'firstEchoTriggered:1' -and $battleStruct -match 'firstEchoSwitchPromptPrepared:1') 'BattleStruct must hold only runtime first-Echo state.'

$faintedFlow = [regex]::Match($battleScripts, '(?s)BattleScript_FaintedMonTryChoose:.*?BattleScript_FaintedMonSendOutNew:').Value
Assert-True ($faintedFlow -match '(?s)switchhandleorder BS_FAINTED, 2\s+callnative BS_TryPrepareFirstEchoSwitch') 'The starter prelude must inspect the selected replacement.'
Assert-True ($faintedFlow -match '(?s)jumpifbyte CMP_EQUAL, sUNUSED_0x1A, TRUE, BattleScript_FirstEchoStarterPrelude\s+jumpifbyte CMP_EQUAL, sBATTLE_STYLE, OPTIONS_BATTLE_STYLE_SET') 'The one-time starter prompt must override SET only for the configured Echo.'
Assert-True ($battleScripts -match '(?s)BattleScript_FirstEchoStarterPrelude:.*?STRINGID_FIRSTECHOSTARTERSTIRS.*?STRINGID_FIRSTECHOSTARTERWANTSBATTLE.*?goto BattleScript_FirstEchoSwitchQuestion') 'The starter prelude must return to the vanilla switch question.'
Assert-True ($firstEcho -match '(?s)FirstEcho_ShouldOfferStarterSwitch.*?playerPartyIndex != gBattlerPartyIndexes\[playerBattler\].*?MON_DATA_HP.*?!= 0') 'The prompt must require a living original starter on the bench.'

Assert-True ($battleScripts -match '(?s)BattleScript_FirstEchoActivates::.*?STRINGID_FIRSTECHOWAVE.*?STRINGID_FIRSTECHOREACHES.*?B_ANIM_FIRST_ECHO.*?BS_ApplyFirstEchoBoost') 'The switch-in hook must present and apply the first Echo.'
$boost = [regex]::Match($firstEcho, '(?s)void FirstEcho_ApplyBoost\(void\).*?\n}').Value
Assert-True ($boost -match 'statStages\[STAT_ATK\]\+\+' -and $boost -match 'statStages\[STAT_SPATK\]\+\+' -and ($boost | Select-String -Pattern 'MAX_STAT_STAGE' -AllMatches).Matches.Count -eq 2) 'The first Echo must directly add one Attack and Sp. Attack stage with the normal cap.'
Assert-True ($boost -notmatch 'SetStatChange|TryStatChange|GetBattlerAbility|ABILITY_SIMPLE|ABILITY_CONTRARY') 'The first Echo boost must bypass the normal stat-change and Ability pipeline.'

Assert-True ($battleAnimConstants -match 'B_ANIM_FIRST_ECHO\s+64' -and $battleAnimConstants -match 'NUM_B_ANIMS_GENERAL\s+65') 'The dedicated general first-Echo animation ID is missing.'
Assert-True ($battleAnimTable -match '\[B_ANIM_FIRST_ECHO\]\s*=\s*gBattleAnimGeneral_FirstEcho') 'The first-Echo animation is not registered in the general table.'
Assert-True ($battleAnimScripts -match '(?s)gBattleAnimGeneral_FirstEcho::.*?simple_palette_blend.*?create_surf_wave.*?blendoff.*?end') 'The first-Echo animation must combine darkening, a water ripple and restoration.'
Assert-True ($battleCommands -match '(?s)animId == B_ANIM_FIRST_ECHO\).*?BtlController_EmitBattleAnimation') 'The first-Echo animation must remain enabled when battle animations are disabled.'

Assert-True ($battleSetup -match '(?s)ScriptGiveMon\(starterMon, 5, ITEM_NONE\);\s*FirstEcho_RegisterStarterIdentity\(starterMon\);') 'Starter PID registration is not wired to starter delivery.'
Assert-True ($emissarioScripts -match '(?s)Emissario_OnTransition_Show::\s*special TryMigrateOriginalStarterIdentity') 'Conservative starter migration must run before the Emissario scene.'
Assert-True ($firstEcho -match '(?s)TryMigrateOriginalStarterIdentity.*?gPlayerParty.*?TOTAL_BOXES_COUNT.*?gPokemonStoragePtr.*?daycare.*?candidates == 1') 'Starter migration must search party, Boxes and Day Care and accept one candidate only.'
Assert-True ($firstEcho -match '(?s)IsStarterCandidate.*?MON_DATA_OT_ID.*?GetPlayerIDAsU32\(\).*?MON_DATA_MET_LEVEL.*?!= 5') 'Starter migration must require the player OT and encounter level 5.'
Assert-True ($specials -match 'def_special Special_GetFirstEchoActiveMonResult' -and $emissarioScripts -match 'special Special_GetFirstEchoActiveMonResult') 'The post-battle scene must read the runtime first-Echo recipient result.'
Assert-True ($emissarioScripts -match '(?s)trainerbattle_no_intro TRAINER_EMISSARIO_AUREA_RECRUIT.*?Emissario_EventScript_AureaDefeated::.*?setflag FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE.*?setflag FLAG_FIRST_ECHO_SEEN') 'Narrative completion and first-Echo flags must be set only after battle victory.'
Assert-True ($battleStrings -match 'STRINGID_FIRSTECHOSTARTERSTIRS.*?freme nella sua POKé BALL' -and $battleStrings -match 'STRINGID_FIRSTECHOWAVE.*?Un''onda senza sorgente' -and $battleStrings -match 'STRINGID_FIRSTECHOREACHES.*?Il primo Eco raggiunge') 'Canonical first-Echo battle text is incomplete.'
Assert-True ($firstEchoHeader -match 'FirstEcho_RegisterStarterIdentity' -and $firstEchoHeader -match 'FirstEcho_TryActivateOnSwitchIn') 'The first-Echo API is incomplete.'
Assert-True ($runtimeTests -match 'First Echo directly raises Attack and Special Attack by one stage' -and $runtimeTests -match 'DEFAULT_STAT_STAGE \+ 1') 'The direct +1/+1 first-Echo runtime test is missing.'
Assert-True ($runtimeTests -match 'First Echo respects the normal maximum stat stage' -and ($runtimeTests | Select-String -Pattern 'MAX_STAT_STAGE' -AllMatches).Matches.Count -ge 4) 'The first-Echo +6 cap runtime test is missing.'

Write-Host 'PASS: Emissario first Echo narrative and battle runtime structure.'
