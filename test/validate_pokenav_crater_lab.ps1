param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

$labPath = Join-Path $RepositoryRoot 'data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc'
$regionMapPath = Join-Path $RepositoryRoot 'src/region_map.c'
$matchCallPath = Join-Path $RepositoryRoot 'src/pokenav_match_call_data.c'
foreach ($path in @($labPath, $regionMapPath, $matchCallPath)) {
    Assert-True (Test-Path $path) "Required file is missing: $path"
}

$lab = Get-Content $labPath -Raw
$receive = [regex]::Match($lab, '(?s)LittlerootTown_ProfessorBirchsLab_EventScript_ReceivePokedex::.*?\n\s*return')
Assert-True $receive.Success 'Pokédex delivery helper is missing.'
$delivery = $receive.Value

foreach ($token in @(
    'setflag FLAG_SYS_POKEDEX_GET',
    'setflag FLAG_RECEIVED_POKEDEX_FROM_BIRCH',
    'setflag FLAG_SYS_POKENAV_GET',
    'setflag FLAG_RECEIVED_POKENAV',
    'setflag FLAG_HAS_MATCH_CALL',
    'setflag FLAG_ADDED_MATCH_CALL_TO_POKENAV',
    'setflag FLAG_ENABLE_PROF_BIRCH_MATCH_CALL',
    'setvar VAR_REGISTER_BIRCH_STATE, 2'
)) {
    Assert-True ($delivery.Contains($token)) "PokéNav delivery is missing: $token"
}

Assert-True ($delivery.Contains('Text_ReceivedPokenav')) 'PokéNav delivery message is missing.'
Assert-True ($lab -match 'map_script_2 VAR_BIRCH_LAB_STATE, 4, LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedexEvent') 'Delivery is not gated by the existing Pokédex scene.'
Assert-True ($lab -match 'LittlerootTown_ProfessorBirchsLab_EventScript_GivePokedexEvent::') 'Pokédex delivery event is missing.'

$regionMap = Get-Content $regionMapPath -Raw
Assert-True ($regionMap -match 'case REGION_AUSONIA:\s*return REGION_MAP_AUSONIA') 'Ausonia Region Map routing changed or is missing.'
Assert-True (-not ($delivery -match 'FLAG_.*FLY|FLY_')) 'PokéNav delivery must not modify Fly flags.'

$matchCallData = Get-Content $matchCallPath -Raw
Assert-True ($matchCallData -match 'MC_HEADER_PROF_BIRCH.*sProfBirchMatchCallHeader') 'Professor Birch Match Call entry is missing.'
Assert-True ($matchCallData -match 'FLAG_ENABLE_PROF_BIRCH_MATCH_CALL') 'Professor Birch contact flag is missing.'

Write-Output 'PokeNav Crater Lab delivery: PASS'
