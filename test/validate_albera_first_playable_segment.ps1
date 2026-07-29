param(
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

function Assert-Contains([string]$Text, [string]$Expected, [string]$Message) {
    Assert-True ($Text.Contains($Expected)) $Message
}

$flags = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/flags.h') -Raw
$flagsFrlg = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/flags_frlg.h') -Raw
$vars = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/vars.h') -Raw
$varsFrlg = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/vars_frlg.h') -Raw
$battleSetup = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/battle_setup.c') -Raw
$battleMessage = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/battle_message.c') -Raw
$newGame = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/scripts/new_game.inc') -Raw
$house = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/scripts/players_house.inc') -Raw
$houseText = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/LittlerootTown_BrendansHouse_1F/scripts.inc') -Raw
$town = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/LittlerootTown/scripts.inc') -Raw
$lab = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc') -Raw
$labMap = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/LittlerootTown_ProfessorBirchsLab/map.json') -Raw | ConvertFrom-Json
$route101 = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/Route101/scripts.inc') -Raw

$expectedFlags = [ordered]@{
    FLAG_ALBERA_HOME_ANOMALY_SEEN = '0x22'
    FLAG_ALBERA_NICO_BATTLE_COMPLETED = '0x23'
    FLAG_HIDE_ALBERA_LAB_NICO = '0x24'
    FLAG_HIDE_ALBERA_LAB_LIA = '0x25'
}
foreach ($entry in $expectedFlags.GetEnumerator()) {
    Assert-True ($flags -match "#define\s+$($entry.Key)\s+$($entry.Value)\b") "Flag Emerald inatteso: $($entry.Key)."
    $frlgValue = $entry.Value -replace '^0x', '0x0'
    Assert-True ($flagsFrlg -match "#define\s+$($entry.Key)\s+$frlgValue\b") "Flag FRLG inatteso: $($entry.Key)."
}
Assert-True ($vars -match '#define\s+VAR_ALBERA_OPENING_STATE\s+0x40F7\b') 'Variabile narrativa Emerald mancante.'
Assert-True ($varsFrlg -match '#define\s+VAR_ALBERA_OPENING_STATE\s+0x40F7\b') 'Variabile narrativa FRLG mancante.'
foreach ($flag in @('FLAG_HIDE_ALBERA_LAB_NICO', 'FLAG_HIDE_ALBERA_LAB_LIA')) {
    Assert-Contains $newGame "setflag $flag" "Il reset di nuova partita non nasconde $flag."
    Assert-Contains $house "clearflag $flag" "La convocazione domestica non mostra $flag."
}
Assert-Contains $house 'setflag FLAG_ALBERA_HOME_ANOMALY_SEEN' 'L’anomalia domestica non viene registrata.'
Assert-Contains $house 'setvar VAR_ALBERA_OPENING_STATE, 1' 'Lo stato narrativo non parte dalla casa.'
foreach ($italianText in @('pressione della rete', 'LABORATORIO DEL CRATERE', 'rumore profondo')) {
    Assert-Contains $houseText $italianText "Testo domestico italiano mancante: $italianText."
}

$nico = @($labMap.object_events | Where-Object local_id -eq 'LOCALID_ALBERA_LAB_NICO')
$lia = @($labMap.object_events | Where-Object local_id -eq 'LOCALID_ALBERA_LAB_LIA')
Assert-True ($nico.Count -eq 1 -and $nico[0].graphics_id -eq 'OBJ_EVENT_GFX_BRENDAN_NORMAL') 'Oggetto dedicato di Nico non valido.'
Assert-True ($lia.Count -eq 1 -and $lia[0].graphics_id -eq 'OBJ_EVENT_GFX_MAY_NORMAL') 'Oggetto dedicato di Lia non valido.'
Assert-True ($nico[0].flag -eq 'FLAG_HIDE_ALBERA_LAB_NICO') 'Flag oggetto Nico inatteso.'
Assert-True ($lia[0].flag -eq 'FLAG_HIDE_ALBERA_LAB_LIA') 'Flag oggetto Lia inatteso.'

$openingTriggers = @($labMap.coord_events | Where-Object {
    $_.type -eq 'trigger' -and
    $_.y -eq 9 -and
    $_.elevation -eq 3 -and
    $_.var -eq 'VAR_ALBERA_OPENING_STATE' -and
    $_.var_value -eq '1' -and
    $_.script -eq 'LittlerootTown_ProfessorBirchsLab_EventScript_AlberaOpeningTrigger'
})
Assert-True ($openingTriggers.Count -eq 6) 'La linea di trigger del laboratorio deve coprire sei celle.'
Assert-True ((@($openingTriggers.x | Sort-Object) -join ',') -eq '4,5,6,7,8,9') 'Coordinate dei trigger del laboratorio inattese.'
Assert-True (-not $lab.Contains('map_script_2 VAR_ALBERA_OPENING_STATE, 1')) 'L’apertura non deve partire dalla porta tramite map script.'
foreach ($token in @(
    'LittlerootTown_ProfessorBirchsLab_EventScript_AlberaOpeningTrigger',
    'LittlerootTown_ProfessorBirchsLab_EventScript_StageAlberaOpening',
    'LittlerootTown_ProfessorBirchsLab_Movement_NicoApproachPlayer',
    'applymovement LOCALID_PLAYER, Common_Movement_FaceLeft'
)) { Assert-Contains $lab $token "Regia correttiva del laboratorio mancante: $token." }

Assert-Contains $battleSetup 'if (VarGet(VAR_ALBERA_OPENING_STATE) == 2)' 'La modalità laboratorio dello selector starter manca.'
Assert-Contains $battleSetup 'CB2_ReturnToFieldContinueScriptPlayMapMusic' 'Lo selector non ritorna allo script del laboratorio.'
Assert-Contains $lab 'special ChooseStarter' 'La scelta starter non avviene nel laboratorio.'
Assert-True (-not $lab.Contains('setvar VAR_0x8004, TRUE')) 'Lo selector non deve dipendere da una variabile temporanea condivisa.'
Assert-Contains $lab 'setflag FLAG_SYS_POKEMON_GET' 'Lo starter non viene registrato come ottenuto.'

$mappings = @(
    @('LittlerootTown_ProfessorBirchsLab_EventScript_AssignAfterCingerm', 'SPECIES_SERBRACE', 'SPECIES_ARDEINO', 'TRAINER_BRENDAN_ROUTE_103_TREECKO'),
    @('LittlerootTown_ProfessorBirchsLab_EventScript_AssignAfterSerbrace', 'SPECIES_ARDEINO', 'SPECIES_CINGERM', 'TRAINER_BRENDAN_ROUTE_103_TORCHIC'),
    @('LittlerootTown_ProfessorBirchsLab_EventScript_AssignAfterArdeino', 'SPECIES_CINGERM', 'SPECIES_SERBRACE', 'TRAINER_BRENDAN_ROUTE_103_MUDKIP')
)
foreach ($mapping in $mappings) {
    foreach ($token in $mapping) { Assert-Contains $lab $token "Mapping starter mancante: $token." }
}
Assert-True (-not $lab.Contains('trainerbattle_earlyrival TRAINER_MAY_ROUTE_103')) 'Lia non deve combattere in questa milestone.'
Assert-Contains $lab 'trainerbattle_earlyrival TRAINER_BRENDAN_ROUTE_103_TREECKO, RIVAL_BATTLE_HEAL_AFTER' 'La battaglia di Nico non usa il flusso nativo senza whiteout.'
Assert-Contains $lab 'goto_if_eq VAR_RESULT, B_OUTCOME_LOST' 'Il flusso non distingue la breve battuta dopo la sconfitta.'
Assert-Contains $lab 'setflag FLAG_ALBERA_NICO_BATTLE_COMPLETED' 'La battaglia di Nico non è resa non ripetibile.'
Assert-Contains $lab 'special HealPlayerParty' 'La squadra non viene ripristinata dopo la lotta amichevole.'
Assert-Contains $battleSetup '(GetRivalBattleFlags() & RIVAL_BATTLE_TUTORIAL) == RIVAL_BATTLE_TUTORIAL' 'Il solo flag di cura continua ad attivare il tutorial della prima battaglia.'
Assert-Contains $battleSetup 'GetRivalBattleFlags() & RIVAL_BATTLE_HEAL_AFTER' 'La cura dopo la sconfitta non è più attiva.'
Assert-Contains $battleMessage '[STRINGID_PLAYERUSEDITEM]                       = COMPOUND_STRING("Hai usato {B_LAST_ITEM}!")' 'Messaggio di uso della Pozione non tradotto.'
Assert-Contains $battleMessage '[STRINGID_ITEMRESTOREDSPECIESHEALTH]            = COMPOUND_STRING("{B_BUFF1} recupera PS.")' 'Messaggio di recupero PS non tradotto.'
Assert-Contains $battleMessage '[STRINGID_TRAINER1MON1COMEBACK]                 = COMPOUND_STRING("{B_TRAINER1_NAME}: {B_OPPONENT_MON1_NAME},\nritorna!")' 'Messaggio di richiamo del Pokémon non tradotto.'

foreach ($required in @(
    'LiaDetectsAnomaly',
    'LauroFieldAssignment',
    'setflag FLAG_ALBERA_WATER_RESEARCH_STARTED',
    'setvar VAR_ROUTE101_STATE, 3',
    'setflag FLAG_HIDE_ROUTE_103_RIVAL',
    'setvar VAR_ALBERA_OPENING_STATE, 5'
)) { Assert-Contains $lab $required "Passaggio narrativo mancante: $required." }

Assert-Contains $town 'goto_if_unset FLAG_ALBERA_WATER_RESEARCH_STARTED, LittlerootTown_EventScript_BlockRouteUntilLabAssignment' 'L’uscita non dipende dall’incarico di Lauro.'
Assert-Contains $town 'LittlerootTown_Text_RouteClosedForMeasurements' 'Il blocco cittadino non ha una motivazione diegetica.'
Assert-Contains $town 'goto_if_ge VAR_ALBERA_OPENING_STATE, 1, LittlerootTown_EventScript_CampettoDeferred' 'Il vecchio evento obbligatorio del campetto resta raggiungibile nel flusso canonico.'
Assert-Contains $route101 'call_if_eq VAR_ALBERA_OPENING_STATE, 5, Route101_EventScript_SetAlberaCheckpoint' 'Il checkpoint di Route 101 non viene registrato.'
Assert-Contains $route101 'setvar VAR_ALBERA_OPENING_STATE, 6' 'Stato finale stabile del segmento mancante.'

foreach ($forbidden in @('DEBUG', 'CHEAT', 'WARP DI TEST', 'SPECIES_PLACEHOLDER')) {
    Assert-True (-not $lab.Contains($forbidden)) "Contenuto di debug o placeholder rilevato: $forbidden."
}

Write-Output 'Albèra first playable segment validation passed.'
