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

function Read-Json([string]$RelativePath) {
    return [IO.File]::ReadAllText((Join-Path $RepositoryRoot $RelativePath), [Text.Encoding]::UTF8) | ConvertFrom-Json
}

$flags = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/flags.h') -Raw
$flagsFrlg = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/flags_frlg.h') -Raw
$vars = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/vars.h') -Raw
$varsFrlg = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/vars_frlg.h') -Raw
$newGame = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/scripts/new_game.inc') -Raw
$lab = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc') -Raw
$route = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/Route101/scripts.inc') -Raw
$routeMap = Read-Json 'data/maps/Route101/map.json'
$regionSections = Read-Json 'src/data/region_map/region_map_sections.json'

Assert-True ($flags -match '#define\s+FLAG_HIDE_ROUTE101_RESEARCH_PARTY\s+0x26\b') 'Missing Emerald research-party flag.'
Assert-True ($flagsFrlg -match '#define\s+FLAG_HIDE_ROUTE101_RESEARCH_PARTY\s+0x026\b') 'Missing FRLG research-party flag.'
Assert-True ($vars -match '#define\s+VAR_ALBERA_VIA_VERDI_STATE\s+0x40F8\b') 'Missing Emerald Via Verdi state variable.'
Assert-True ($varsFrlg -match '#define\s+VAR_ALBERA_VIA_VERDI_STATE\s+0x40F8\b') 'Missing FRLG Via Verdi state variable.'
Assert-Contains $newGame 'setflag FLAG_HIDE_ROUTE101_RESEARCH_PARTY' 'New games must hide the research party by default.'

Assert-Contains $lab 'checkitemspace ITEM_POKE_BALL, 10' 'The laboratory must check room for ten Poke Balls.'
Assert-Contains $lab 'giveitem ITEM_POKE_BALL, 10' 'The laboratory must give exactly ten Poke Balls.'
Assert-Contains $lab 'LittlerootTown_ProfessorBirchsLab_EventScript_NoRoomForViaVerdiPokeBalls' 'The full-bag retry flow is missing.'
Assert-Contains $lab 'setvar VAR_ALBERA_VIA_VERDI_STATE, 1' 'The investigation must start after the Poke Balls are given.'
Assert-True ($lab -match 'goto_if_eq VAR_ALBERA_OPENING_STATE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_CompleteAlberaOpening') 'Lauro cannot resume the full-bag handoff.'
Assert-Contains $lab 'setflag FLAG_ALBERA_WATER_RESEARCH_STARTED' 'The existing research flag must remain part of the canonical handoff.'

$expectedObjects = @{
    LOCALID_ROUTE101_LIA = 'OBJ_EVENT_GFX_MAY_NORMAL'
    LOCALID_ROUTE101_NICO = 'OBJ_EVENT_GFX_BRENDAN_NORMAL'
}
foreach ($localId in $expectedObjects.Keys) {
    $object = @($routeMap.object_events | Where-Object { $_.local_id -eq $localId })
    Assert-True ($object.Count -eq 1) "Missing Route101 object: $localId."
    Assert-True ($object[0].graphics_id -eq $expectedObjects[$localId]) "Unexpected graphics for $localId."
    Assert-True ($object[0].trainer_type -eq 'TRAINER_TYPE_NONE') "$localId must not initiate a trainer battle."
    Assert-True ($object[0].flag -eq 'FLAG_HIDE_ROUTE101_RESEARCH_PARTY') "$localId must use the investigation visibility flag."
}

$expectedTriggers = @(
    @(1, 10, 16, 'Route101_EventScript_InvestigateSource'),
    @(1, 11, 16, 'Route101_EventScript_InvestigateSource'),
    @(2, 21, 10, 'Route101_EventScript_InvestigatePokemonBehavior'),
    @(2, 22, 10, 'Route101_EventScript_InvestigatePokemonBehavior'),
    @(3, 10, 4, 'Route101_EventScript_InvestigateAncientCanal'),
    @(3, 11, 4, 'Route101_EventScript_InvestigateAncientCanal')
)
foreach ($trigger in $expectedTriggers) {
    $match = @($routeMap.coord_events | Where-Object {
        $_.type -eq 'trigger' -and $_.x -eq $trigger[1] -and $_.y -eq $trigger[2] -and
        $_.elevation -eq 3 -and $_.var -eq 'VAR_ALBERA_VIA_VERDI_STATE' -and
        $_.var_value -eq "$($trigger[0])" -and $_.script -eq $trigger[3]
    })
    Assert-True ($match.Count -eq 1) "Missing or invalid investigation trigger: $($trigger[1]),$($trigger[2])."
}

foreach ($token in @(
    'Route101_EventScript_InvestigateSource',
    'Route101_EventScript_InvestigatePokemonBehavior',
    'Route101_EventScript_InvestigateAncientCanal',
    'setvar VAR_ALBERA_VIA_VERDI_STATE, 2',
    'setvar VAR_ALBERA_VIA_VERDI_STATE, 3',
    'setvar VAR_ALBERA_VIA_VERDI_STATE, 4',
    'playse SE_M_UPROAR',
    'setvar VAR_0x8006, 4',
    'setvar VAR_0x8007, 2',
    'special ShakeScreen',
    'playse SE_M_WATERFALL',
    'setflag FLAG_HIDE_ROUTE101_RESEARCH_PARTY'
)) { Assert-Contains $route $token "Missing investigation script token: $token." }
Assert-True (-not $route.Contains('trainerbattle')) 'The Via Verdi investigation must not start a battle.'

foreach ($text in @('quasi asciutto', 'terreno', 'evitano il tratto', 'canalizzazione', 'PORTA PRETORIA', 'VIA VERDI')) {
    Assert-Contains $route $text "Missing required Via Verdi text: $text."
}
$forbiddenName = [string]::Concat('A', 'QUILA')
Assert-True (-not $route.Contains($forbiddenName)) 'The investigation must not name the future legendary Pokemon.'

$routeSection = @($regionSections.map_sections | Where-Object { $_.id -eq 'MAPSEC_ROUTE_101' })
Assert-True ($routeSection.Count -eq 1 -and $routeSection[0].name -eq 'VIA VERDI') 'Route101 must remain visible as Via Verdi.'
$north = @($routeMap.connections | Where-Object { $_.direction -eq 'up' -and $_.map -eq 'MAP_OLDALE_TOWN' })
$south = @($routeMap.connections | Where-Object { $_.direction -eq 'down' -and $_.map -eq 'MAP_LITTLEROOT_TOWN' })
Assert-True ($north.Count -eq 1 -and $south.Count -eq 1) 'Via Verdi connections changed unexpectedly.'
Assert-Contains $route 'call_if_eq VAR_ALBERA_OPENING_STATE, 5, Route101_EventScript_SetAlberaCheckpoint' 'The existing Route101 checkpoint transition changed.'

foreach ($path in @('src/data/wild_encounters.json', 'src/data/pokemon', 'include/constants/species.h', 'src/battle_setup.c', 'src/save.c')) {
    & git -C $RepositoryRoot diff --quiet develop -- $path
    Assert-True ($LASTEXITCODE -eq 0) "Out-of-scope file changed: $path"
}

$changedArtifacts = @(& git -C $RepositoryRoot diff --name-only develop -- '*.gba' '*.elf' '*.map' '*.zip')
$untrackedArtifacts = @(& git -C $RepositoryRoot ls-files --others --exclude-standard -- '*.gba' '*.elf' '*.map' '*.zip')
Assert-True (($changedArtifacts.Count + $untrackedArtifacts.Count) -eq 0) 'Build artifacts or archives were found.'

Write-Output 'Via Verdi first investigation validation passed.'
