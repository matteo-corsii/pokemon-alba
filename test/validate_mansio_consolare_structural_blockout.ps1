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

Assert-True ($external.id -eq 'MAP_VIA_CONSOLARE' -and $external.layout -eq 'LAYOUT_VIA_CONSOLARE') 'Via Consolare identity changed.'
Assert-True ($external.connections.Count -eq 1 -and $external.connections[0].direction -eq 'right' -and $external.connections[0].map -eq 'MAP_ROUTE103' -and $external.connections[0].offset -eq 0) 'Via Consolare Route103 connection changed.'
Assert-True (@($external.warp_events).Count -eq 2) 'Via Consolare must have exactly two Mansio entrance warps.'
Assert-True (@($external.warp_events | Where-Object { $_.x -eq 5 -and $_.y -eq 21 -and $_.dest_map -eq 'MAP_VIA_CONSOLARE_MANSIO' -and $_.dest_warp_id -eq '0' }).Count -eq 1) 'Left Mansio entrance warp is incorrect.'
Assert-True (@($external.warp_events | Where-Object { $_.x -eq 12 -and $_.y -eq 21 -and $_.dest_map -eq 'MAP_VIA_CONSOLARE_MANSIO' -and $_.dest_warp_id -eq '1' }).Count -eq 1) 'Right Mansio entrance warp is incorrect.'
Assert-True (@($external.object_events).Count -eq 0 -and @($external.coord_events).Count -eq 0 -and @($external.bg_events).Count -eq 0) 'Via Consolare received unexpected events.'

Assert-True ($internal.id -eq 'MAP_VIA_CONSOLARE_MANSIO' -and $internal.layout -eq 'LAYOUT_VIA_CONSOLARE_MANSIO') 'Mansio identity changed.'
Assert-True ($internal.map_type -eq 'MAP_TYPE_INDOOR' -and $internal.allow_cycling -eq $false -and $internal.allow_escaping -eq $false) 'Mansio indoor properties are incorrect.'
Assert-True ($null -eq $internal.connections) 'Mansio must not have connections.'
Assert-True (@($internal.object_events).Count -eq 0 -and @($internal.coord_events).Count -eq 0 -and @($internal.bg_events).Count -eq 0) 'Mansio must be an empty structural blockout.'
Assert-True (@($internal.warp_events).Count -eq 2) 'Mansio must have exactly two exit warps.'
Assert-True (@($internal.warp_events | Where-Object { $_.x -eq 5 -and $_.y -eq 15 -and $_.dest_map -eq 'MAP_VIA_CONSOLARE' -and $_.dest_warp_id -eq '0' }).Count -eq 1) 'Mansio left exit warp is incorrect.'
Assert-True (@($internal.warp_events | Where-Object { $_.x -eq 17 -and $_.y -eq 15 -and $_.dest_map -eq 'MAP_VIA_CONSOLARE' -and $_.dest_warp_id -eq '1' }).Count -eq 1) 'Mansio right exit warp is incorrect.'

Assert-True ($externalLayout.Count -eq 1 -and $externalLayout[0].width -eq 60 -and $externalLayout[0].height -eq 30) 'External Via Consolare dimensions changed.'
Assert-True ($internalLayout.Count -eq 1 -and $internalLayout[0].width -eq 24 -and $internalLayout[0].height -eq 16) 'Mansio dimensions must be 24x16.'
Assert-True ($internalLayout[0].primary_tileset -eq 'gTileset_Building' -and $internalLayout[0].secondary_tileset -eq 'gTileset_GenericBuilding') 'Mansio tilesets are incorrect.'
Assert-True (@($groups.gMapGroup_IndoorOldale | Where-Object { $_ -eq 'ViaConsolare_Mansio' }).Count -eq 1) 'Mansio is not registered in the indoor map group.'
Assert-True ((Get-Item (Join-Path $RepositoryRoot 'data/layouts/ViaConsolare_Mansio/map.bin')).Length -eq (24 * 16 * 2)) 'Mansio map.bin size is incorrect.'
git -C $RepositoryRoot diff --quiet develop -- data/layouts/ViaConsolare/map.bin
Assert-True ($LASTEXITCODE -eq 0) 'External ViaConsolare map.bin changed.'

Write-Output 'Mansio Consolare structural blockout: PASS'
