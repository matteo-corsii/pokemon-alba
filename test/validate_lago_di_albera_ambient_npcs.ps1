param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'
function Assert-True([bool]$Condition,[string]$Message) { if (-not $Condition) { throw $Message } }
function Read-Json([string]$Path) { Get-Content (Join-Path $RepositoryRoot $Path) -Raw -Encoding utf8 | ConvertFrom-Json }

$map = Read-Json 'data/maps/LagoDiAlbera/map.json'
$scripts = Get-Content (Join-Path $RepositoryRoot 'data/maps/LagoDiAlbera/scripts.inc') -Raw -Encoding utf8

$expectedObjects = @(
    @('LOCALID_LAGO_DI_ALBERA_CUSTODE','OBJ_EVENT_GFX_GENTLEMAN',58,104,'MOVEMENT_TYPE_FACE_RIGHT','LagoDiAlbera_EventScript_Custode'),
    @('LOCALID_LAGO_DI_ALBERA_PESCATORE','OBJ_EVENT_GFX_FISHERMAN',30,86,'MOVEMENT_TYPE_FACE_RIGHT','LagoDiAlbera_EventScript_Pescatore'),
    @('LOCALID_LAGO_DI_ALBERA_GUARDABOSCHI','OBJ_EVENT_GFX_HIKER',17,61,'MOVEMENT_TYPE_FACE_DOWN','LagoDiAlbera_EventScript_Guardaboschi'),
    @('LOCALID_LAGO_DI_ALBERA_ARCHEOLOGA','OBJ_EVENT_GFX_SCIENTIST_1',7,34,'MOVEMENT_TYPE_FACE_LEFT','LagoDiAlbera_EventScript_Archeologa'),
    @('LOCALID_LAGO_DI_ALBERA_ANZIANO','OBJ_EVENT_GFX_EXPERT_M',35,12,'MOVEMENT_TYPE_FACE_RIGHT','LagoDiAlbera_EventScript_Anziano'),
    @('LOCALID_LAGO_DI_ALBERA_CARPENTIERE','OBJ_EVENT_GFX_MAN_4',81,73,'MOVEMENT_TYPE_FACE_LEFT','LagoDiAlbera_EventScript_Carpentiere'),
    @('LOCALID_LAGO_DI_ALBERA_ABITANTE_PALAFITTE','OBJ_EVENT_GFX_WOMAN_1',90,66,'MOVEMENT_TYPE_FACE_DOWN','LagoDiAlbera_EventScript_AbitantePalafitte'),
    @('LOCALID_LAGO_DI_ALBERA_BAGNANTE','OBJ_EVENT_GFX_SWIMMER_F_LAND',74,88,'MOVEMENT_TYPE_FACE_LEFT','LagoDiAlbera_EventScript_Bagnante'),
    @('LOCALID_LAGO_DI_ALBERA_CICLISTA','OBJ_EVENT_GFX_CYCLING_TRIATHLETE_M',113,73,'MOVEMENT_TYPE_FACE_LEFT','LagoDiAlbera_EventScript_Ciclista'),
    @('LOCALID_LAGO_DI_ALBERA_ASPIRANTE','OBJ_EVENT_GFX_YOUNGSTER',65,75,'MOVEMENT_TYPE_FACE_RIGHT','LagoDiAlbera_EventScript_Aspirante'),
    @('LOCALID_LAGO_DI_ALBERA_TECNICO','OBJ_EVENT_GFX_SCIENTIST_2',86,8,'MOVEMENT_TYPE_FACE_DOWN','LagoDiAlbera_EventScript_Tecnico'),
    @('LOCALID_LAGO_DI_ALBERA_VIANDANTE','OBJ_EVENT_GFX_MAN_5',117,44,'MOVEMENT_TYPE_FACE_LEFT','LagoDiAlbera_EventScript_Viandante')
)
Assert-True (@($map.object_events).Count -eq 12) 'Lago must contain exactly twelve ambient NPCs.'
foreach ($expected in $expectedObjects) {
    $event = @($map.object_events | Where-Object local_id -eq $expected[0])
    Assert-True ($event.Count -eq 1) "$($expected[0]) missing or duplicated."
    $event = $event[0]
    Assert-True ($event.graphics_id -eq $expected[1] -and [int]$event.x -eq $expected[2] -and [int]$event.y -eq $expected[3] -and $event.movement_type -eq $expected[4] -and $event.script -eq $expected[5]) "$($expected[0]) properties changed."
    Assert-True ([int]$event.elevation -eq 3 -and $event.flag -eq '0' -and $event.trainer_type -eq 'TRAINER_TYPE_NONE') "$($expected[0]) must remain an unconditional ambient NPC."
}

$expectedSigns = @(
    @(10,109,'LagoDiAlbera_EventScript_CasaMaestro'),
    @(115,71,'LagoDiAlbera_EventScript_NegozioBiciclette'),
    @(66,74,'LagoDiAlbera_EventScript_Palestra'),
    @(77,3,'LagoDiAlbera_EventScript_Emissario'),
    @(110,110,'LagoDiAlbera_EventScript_BottegaRifugi'),
    @(103,46,'LagoDiAlbera_EventScript_SalitaBorgo')
)
Assert-True (@($map.bg_events).Count -eq 6) 'Lago must contain exactly six approved signs.'
foreach ($expected in $expectedSigns) {
    $event = @($map.bg_events | Where-Object { [int]$_.x -eq $expected[0] -and [int]$_.y -eq $expected[1] -and $_.script -eq $expected[2] })
    Assert-True ($event.Count -eq 1 -and $event[0].type -eq 'sign') "Approved Lago sign missing at ($($expected[0]),$($expected[1]))."
}
Assert-True (@($map.warp_events).Count -eq 0 -and @($map.coord_events).Count -eq 0) 'NPC batch must not add warps or coordinate events.'

Assert-True ($scripts -match '(?s)LagoDiAlbera_EventScript_Pescatore::.*?goto_if_set FLAG_RECEIVED_GOOD_ROD.*?giveitem ITEM_GOOD_ROD.*?setflag FLAG_RECEIVED_GOOD_ROD') 'Fisherman must give the Good Rod exactly once.'
Assert-True ([regex]::Matches($scripts,'giveitem ITEM_GOOD_ROD').Count -eq 1) 'Good Rod reward must exist exactly once.'
Assert-True ($scripts -notmatch 'trainerbattle|ITEM_HM_SURF|FLAG_BADGE|MAP_LAGO_DI_ALBERA_REFUGE_SHOP') 'NPC batch introduced battle, Surf, Badge, or shop progression.'
foreach ($token in @('Casa del Maestro dei rifugi','Negozio di biciclette','PALESTRA DELLE MACINE','Entrata dell''EMISSARIO','BOTTEGA DEI RIFUGI','SALITA VERSO BORGO DI CASTELLO','rete dei CISTERNONI')) {
    Assert-True ($scripts.Contains($token)) "Missing canonical Lago text: $token"
}

$bytes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/LagoDiAlbera/map.bin'))
Assert-True ($bytes.Length -eq 120 * 120 * 2) 'Unexpected Lago layout size.'
foreach ($expected in $expectedObjects) {
    $x = [int]$expected[2]; $y = [int]$expected[3]
    $raw = [BitConverter]::ToUInt16($bytes, 2 * (($y * 120) + $x))
    Assert-True ((($raw -shr 10) -band 3) -eq 0) "$($expected[0]) is placed on a blocked tile."
    Assert-True ((($raw -shr 12) -band 0xF) -eq 3) "$($expected[0]) must be placed at elevation 3."
}
Write-Output 'Lago di Albera ambient NPC validation passed.'
