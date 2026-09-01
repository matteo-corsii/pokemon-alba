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
    @('LOCALID_LAGO_DI_ALBERA_BAGNANTE','OBJ_EVENT_GFX_SWIMMER_F',74,88,'MOVEMENT_TYPE_FACE_LEFT','LagoDiAlbera_EventScript_Bagnante'),
    @('LOCALID_LAGO_DI_ALBERA_CICLISTA','OBJ_EVENT_GFX_CYCLING_TRIATHLETE_M',113,73,'MOVEMENT_TYPE_FACE_LEFT','LagoDiAlbera_EventScript_Ciclista'),
    @('LOCALID_LAGO_DI_ALBERA_ASPIRANTE','OBJ_EVENT_GFX_YOUNGSTER',65,75,'MOVEMENT_TYPE_FACE_RIGHT','LagoDiAlbera_EventScript_Aspirante'),
    @('LOCALID_LAGO_DI_ALBERA_TECNICO','OBJ_EVENT_GFX_SCIENTIST_2',86,8,'MOVEMENT_TYPE_FACE_DOWN','LagoDiAlbera_EventScript_Tecnico'),
    @('LOCALID_LAGO_DI_ALBERA_VIANDANTE','OBJ_EVENT_GFX_MAN_5',117,44,'MOVEMENT_TYPE_FACE_LEFT','LagoDiAlbera_EventScript_Viandante')
)
Assert-True (@($map.object_events).Count -eq 13) 'Lago must contain exactly twelve ambient NPCs plus the temporary Lauro scene NPC.'
foreach ($expected in $expectedObjects) {
    $event = @($map.object_events | Where-Object local_id -eq $expected[0])
    Assert-True ($event.Count -eq 1) "$($expected[0]) missing or duplicated."
    $event = $event[0]
    Assert-True ($event.graphics_id -eq $expected[1] -and [int]$event.x -eq $expected[2] -and [int]$event.y -eq $expected[3] -and $event.movement_type -eq $expected[4] -and $event.script -eq $expected[5]) "$($expected[0]) properties changed."
    $expectedElevation = if ($expected[0] -eq 'LOCALID_LAGO_DI_ALBERA_BAGNANTE') { 1 } else { 3 }
    Assert-True ([int]$event.elevation -eq $expectedElevation -and $event.flag -eq '0' -and $event.trainer_type -eq 'TRAINER_TYPE_NONE') "$($expected[0]) must remain an unconditional ambient NPC."
}

$expectedSigns = @(
    @(10,109,'LagoDiAlbera_EventScript_CasaMaestro'),
    @(115,71,'LagoDiAlbera_EventScript_NegozioBiciclette'),
    @(66,74,'LagoDiAlbera_EventScript_Palestra'),
    @(77,3,'LagoDiAlbera_EventScript_Emissario'),
    @(110,110,'LagoDiAlbera_EventScript_BottegaRifugi'),
    @(103,46,'LagoDiAlbera_EventScript_SalitaBorgo')
)
Assert-True (@($map.bg_events).Count -eq 8) 'Lago must contain exactly eight approved background events.'
Assert-True (@($map.bg_events | Where-Object type -eq 'sign').Count -eq 6) 'Lago must retain exactly six approved signs.'
foreach ($expected in $expectedSigns) {
    $event = @($map.bg_events | Where-Object { [int]$_.x -eq $expected[0] -and [int]$_.y -eq $expected[1] -and $_.script -eq $expected[2] })
    Assert-True ($event.Count -eq 1 -and $event[0].type -eq 'sign') "Approved Lago sign missing at ($($expected[0]),$($expected[1]))."
}
Assert-True (@($map.bg_events | Where-Object { $_.type -eq 'secret_base' -and [int]$_.x -eq 5 -and [int]$_.y -eq 82 -and $_.secret_base_id -eq 'SECRET_BASE_LAGO_DI_ALBERA_ROCK_NORTH' }).Count -eq 1) 'North Secret Base event is missing.'
Assert-True (@($map.bg_events | Where-Object { $_.type -eq 'secret_base' -and [int]$_.x -eq 11 -and [int]$_.y -eq 116 -and $_.secret_base_id -eq 'SECRET_BASE_LAGO_DI_ALBERA_ROCK_SOUTH' }).Count -eq 1) 'South Secret Base event is missing.'
Assert-True (@($map.warp_events).Count -eq 5 -and @($map.coord_events).Count -eq 1) 'Lago warp or coordinate event counts are incorrect.'
Assert-True (@($map.warp_events | Where-Object { [int]$_.x -eq 81 -and [int]$_.y -eq 3 -and [int]$_.elevation -eq 3 -and $_.dest_map -eq 'MAP_EMISSARIO' -and [int]$_.dest_warp_id -eq 0 }).Count -eq 1) 'Lago Emissario entrance warp is incorrect.'
Assert-True (@($map.object_events | Where-Object { $_.local_id -eq 'LOCALID_LAGO_DI_ALBERA_LAURO_SURF' -and $_.graphics_id -eq 'OBJ_EVENT_GFX_PROF_BIRCH' -and [int]$_.x -eq 70 -and [int]$_.y -eq 75 -and $_.flag -eq 'FLAG_HIDE_LAGO_DI_ALBERA_LAURO_SURF' }).Count -eq 1) 'Lauro Surf scene NPC is incorrect.'
Assert-True (@($map.coord_events | Where-Object { [int]$_.x -eq 71 -and [int]$_.y -eq 75 -and $_.script -eq 'LagoDiAlbera_EventScript_LauroSurfScene' }).Count -eq 1) 'Lauro Surf scene trigger is incorrect.'

Assert-True ($scripts -match '(?s)LagoDiAlbera_EventScript_Pescatore::.*?goto_if_set FLAG_RECEIVED_GOOD_ROD.*?giveitem ITEM_GOOD_ROD.*?setflag FLAG_RECEIVED_GOOD_ROD') 'Fisherman must give the Good Rod exactly once.'
Assert-True ([regex]::Matches($scripts,'giveitem ITEM_GOOD_ROD').Count -eq 1) 'Good Rod reward must exist exactly once.'
Assert-True ($scripts -notmatch 'trainerbattle|MAP_LAGO_DI_ALBERA_REFUGE_SHOP') 'Ambient Lago scripts must not introduce battles or shop progression.'
foreach ($token in @('Casa del Maestro dei rifugi','Negozio di biciclette','PALESTRA DELLE MACINE','Entrata dell''EMISSARIO','BOTTEGA DEI RIFUGI','SALITA VERSO BORGO DI CASTELLO','rete dei CISTERNONI')) {
    Assert-True ($scripts.Contains($token)) "Missing canonical Lago text: $token"
}

$bytes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/LagoDiAlbera/map.bin'))
Assert-True ($bytes.Length -eq 120 * 120 * 2) 'Unexpected Lago layout size.'
foreach ($expected in $expectedObjects) {
    $x = [int]$expected[2]; $y = [int]$expected[3]
    $expectedElevation = if ($expected[0] -eq 'LOCALID_LAGO_DI_ALBERA_BAGNANTE') { 1 } else { 3 }
    $raw = [BitConverter]::ToUInt16($bytes, 2 * (($y * 120) + $x))
    Assert-True ((($raw -shr 10) -band 3) -eq 0) "$($expected[0]) is placed on a blocked tile."
    Assert-True ((($raw -shr 12) -band 0xF) -eq $expectedElevation) "$($expected[0]) has an unexpected tile elevation."
}
Write-Output 'Lago di Albera ambient NPC validation passed.'
