param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
)

$ErrorActionPreference = 'Stop'

function Read-Json([string]$RelativePath) {
    return Get-Content -LiteralPath (Join-Path $RepositoryRoot $RelativePath) -Raw | ConvertFrom-Json
}
function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

$external = Read-Json 'data/maps/ViaConsolare/map.json'
$internal = Read-Json 'data/maps/ViaConsolare_Mansio/map.json'
$route = Read-Json 'data/maps/Route103/map.json'
$layouts = Read-Json 'data/layouts/layouts.json'
$groups = Read-Json 'data/maps/map_groups.json'
$externalLayout = @($layouts.layouts | Where-Object { $_.id -eq 'LAYOUT_VIA_CONSOLARE' })
$internalLayout = @($layouts.layouts | Where-Object { $_.id -eq 'LAYOUT_VIA_CONSOLARE_MANSIO' })
$scripts = Get-Content (Join-Path $RepositoryRoot 'data/maps/ViaConsolare_Mansio/scripts.inc') -Raw

Assert-True ($external.id -eq 'MAP_VIA_CONSOLARE' -and $external.layout -eq 'LAYOUT_VIA_CONSOLARE') 'Via Consolare identity changed.'
$routeConnection = @($external.connections | Where-Object { $_.direction -eq 'right' -and $_.map -eq 'MAP_ROUTE103' -and [int]$_.offset -eq 0 })
$lakeConnection = @($external.connections | Where-Object { $_.direction -eq 'up' -and $_.map -eq 'MAP_LAGO_DI_ALBERA' -and [int]$_.offset -eq -31 })
Assert-True ($external.connections.Count -eq 2 -and $routeConnection.Count -eq 1 -and $lakeConnection.Count -eq 1) 'Via Consolare connections changed.'
Assert-True (@($external.warp_events).Count -eq 2) 'Via Consolare must have exactly two Mansio entrance warps.'
Assert-True (@($external.warp_events | Where-Object { $_.x -eq 5 -and $_.y -eq 21 -and $_.dest_map -eq 'MAP_VIA_CONSOLARE_MANSIO' -and $_.dest_warp_id -eq '0' }).Count -eq 1) 'Left Mansio entrance warp is incorrect.'
Assert-True (@($external.warp_events | Where-Object { $_.x -eq 12 -and $_.y -eq 21 -and $_.dest_map -eq 'MAP_VIA_CONSOLARE_MANSIO' -and $_.dest_warp_id -eq '1' }).Count -eq 1) 'Right Mansio entrance warp is incorrect.'
Assert-True (@($external.coord_events).Count -eq 0) 'Via Consolare must not contain coordinate events.'

Assert-True ($internal.id -eq 'MAP_VIA_CONSOLARE_MANSIO' -and $internal.layout -eq 'LAYOUT_VIA_CONSOLARE_MANSIO') 'Mansio identity changed.'
Assert-True ($internal.map_type -eq 'MAP_TYPE_INDOOR' -and $internal.allow_cycling -eq $false -and $internal.allow_escaping -eq $false) 'Mansio indoor properties are incorrect.'
Assert-True ($null -eq $internal.connections) 'Mansio must not have connections.'
Assert-True (@($internal.object_events).Count -eq 4) 'Mansio must contain exactly four approved NPCs.'
Assert-True (@($internal.coord_events).Count -eq 0 -and @($internal.bg_events).Count -eq 0) 'Mansio must not contain coordinate or background events.'
$expectedObjects = @(
    @{ local_id = 'LOCALID_MANSIO_OSTE'; x = 5; y = 4; movement_type = 'MOVEMENT_TYPE_FACE_DOWN'; script = 'ViaConsolare_Mansio_EventScript_Oste' },
    @{ local_id = 'LOCALID_MANSIO_VIANDANTE'; x = 7; y = 10; movement_type = 'MOVEMENT_TYPE_FACE_LEFT'; script = 'ViaConsolare_Mansio_EventScript_Viandante' },
    @{ local_id = 'LOCALID_MANSIO_MANUTENTORE'; x = 14; y = 10; movement_type = 'MOVEMENT_TYPE_FACE_RIGHT'; script = 'ViaConsolare_Mansio_EventScript_Manutentore' },
    @{ local_id = 'LOCALID_MANSIO_STUDIOSA'; x = 19; y = 5; movement_type = 'MOVEMENT_TYPE_FACE_RIGHT'; script = 'ViaConsolare_Mansio_EventScript_Studiosa' }
)
foreach ($expected in $expectedObjects) {
    $found = @($internal.object_events | Where-Object {
        $_.local_id -eq $expected.local_id -and $_.x -eq $expected.x -and $_.y -eq $expected.y -and
        $_.movement_type -eq $expected.movement_type -and $_.script -eq $expected.script
    })
    Assert-True ($found.Count -eq 1) ("Mansio NPC is incorrect: $($expected.local_id)")
}
Assert-True (@($internal.object_events | Where-Object { $_.x -eq 5 -and $_.y -eq 15 -or $_.x -eq 17 -and $_.y -eq 15 }).Count -eq 0) 'Mansio NPC overlaps an exit warp.'
Assert-True (@($internal.warp_events).Count -eq 2) 'Mansio must have exactly two exit warps.'
Assert-True (@($internal.warp_events | Where-Object { $_.x -eq 5 -and $_.y -eq 15 -and $_.dest_map -eq 'MAP_VIA_CONSOLARE' -and $_.dest_warp_id -eq '0' }).Count -eq 1) 'Mansio left exit warp is incorrect.'
Assert-True (@($internal.warp_events | Where-Object { $_.x -eq 17 -and $_.y -eq 15 -and $_.dest_map -eq 'MAP_VIA_CONSOLARE' -and $_.dest_warp_id -eq '1' }).Count -eq 1) 'Mansio right exit warp is incorrect.'

Assert-True ($externalLayout.Count -eq 1 -and $externalLayout[0].width -eq 60 -and $externalLayout[0].height -eq 30) 'External Via Consolare dimensions changed.'
Assert-True ($internalLayout.Count -eq 1 -and $internalLayout[0].width -eq 24 -and $internalLayout[0].height -eq 16) 'Mansio dimensions must be 24x16.'
Assert-True ($internalLayout[0].primary_tileset -eq 'gTileset_Building' -and $internalLayout[0].secondary_tileset -eq 'gTileset_GenericBuilding') 'Mansio tilesets are incorrect.'
Assert-True (@($groups.gMapGroup_IndoorOldale | Where-Object { $_ -eq 'ViaConsolare_Mansio' }).Count -eq 1) 'Mansio is not registered in the indoor map group.'
Assert-True ((Get-Item (Join-Path $RepositoryRoot 'data/layouts/ViaConsolare_Mansio/map.bin')).Length -eq (24 * 16 * 2)) 'Mansio map.bin size is incorrect.'
# ViaConsolare/map.bin has an explicit exact-delta guard in the Lago tileset validator.
Assert-True ($scripts.Contains('msgbox ViaConsolare_Mansio_Text_OsteIntro, MSGBOX_YESNO')) 'Oste must offer a Yes/No rest choice.'
Assert-True ($scripts.Contains('case YES, ViaConsolare_Mansio_EventScript_OsteHeal') -and $scripts.Contains('special HealPlayerParty')) 'Oste Yes branch must heal the party.'
Assert-True ($scripts.Contains('case NO, ViaConsolare_Mansio_EventScript_OsteDecline')) 'Oste No branch is missing.'
Assert-True (-not $scripts.Contains('trainerbattle') -and -not $scripts.Contains('giveitem')) 'Mansio content must not add battles or items.'

Write-Output 'Mansio Consolare structural blockout: PASS'
