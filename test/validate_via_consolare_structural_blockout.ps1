param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
)

$ErrorActionPreference = 'Stop'

function Read-Json([string]$RelativePath) {
    $path = Join-Path $RepositoryRoot $RelativePath
    return Get-Content -LiteralPath $path -Raw | ConvertFrom-Json
}

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

$map = Read-Json 'data/maps/ViaConsolare/map.json'
$route = Read-Json 'data/maps/Route103/map.json'
$groups = Read-Json 'data/maps/map_groups.json'
$layouts = Read-Json 'data/layouts/layouts.json'
$wild = Read-Json 'src/data/wild_encounters.json'

Assert-True ($map.id -eq 'MAP_VIA_CONSOLARE') 'Via Consolare map id is incorrect.'
Assert-True ($map.layout -eq 'LAYOUT_VIA_CONSOLARE') 'Via Consolare layout id is incorrect.'
Assert-True ($map.map_type -eq 'MAP_TYPE_ROUTE') 'Via Consolare must be an outdoor route.'
Assert-True ($map.connections.Count -eq 1 -and $map.connections[0].direction -eq 'right' -and $map.connections[0].map -eq 'MAP_ROUTE103' -and [int]$map.connections[0].offset -eq 0) 'Via Consolare connection to Route103 is incorrect.'
Assert-True (@($map.warp_events).Count -eq 0) 'Via Consolare must not contain warps.'
Assert-True (@($map.object_events).Count -eq 0 -and @($map.coord_events).Count -eq 0 -and @($map.bg_events).Count -eq 0) 'Via Consolare must not contain events in the structural blockout.'

$reverse = @($route.connections | Where-Object { $_.direction -eq 'left' -and $_.map -eq 'MAP_VIA_CONSOLARE' })
Assert-True ($reverse.Count -eq 1 -and [int]$reverse[0].offset -eq 0) 'Route103 reciprocal connection is missing or misaligned.'
Assert-True (@($route.connections | Where-Object { $_.direction -eq 'down' -and $_.map -eq 'MAP_OLDALE_TOWN' }).Count -eq 1) 'Route103 south connection was not preserved.'
Assert-True (@($route.warp_events | Where-Object { $_.dest_map -eq 'MAP_CISTERNONI' }).Count -eq 2) 'Route103 Cisternoni warps were not preserved.'
Assert-True (@($route.object_events | Where-Object { $_.local_id -eq 'LOCALID_ROUTE103_LIA' }).Count -eq 1) 'Route103 Lia was not preserved.'
Assert-True (@($route.object_events | Where-Object { $_.local_id -eq 'LOCALID_ROUTE103_NICO' }).Count -eq 1) 'Route103 Nico was not preserved.'
Assert-True (@($route.bg_events | Where-Object { $_.secret_base_id -eq 'SECRET_BASE_CISTERNONI_TREE_1' }).Count -eq 1) 'Route103 Secret Base tree was not preserved.'

$layout = @($layouts.layouts | Where-Object { $_.id -eq 'LAYOUT_VIA_CONSOLARE' })
Assert-True ($layout.Count -eq 1 -and [int]$layout[0].width -eq 60 -and [int]$layout[0].height -eq 30) 'Via Consolare layout dimensions are incorrect.'
Assert-True ($layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_PortaPretoria') 'Via Consolare tilesets are incorrect.'

$group = @($groups.gMapGroup_TownsAndRoutes | Where-Object { $_ -eq 'ViaConsolare' })
Assert-True ($group.Count -eq 1) 'ViaConsolare is not registered exactly once in map groups.'

$mapPath = Join-Path $RepositoryRoot 'data/layouts/ViaConsolare/map.bin'
Assert-True ((Get-Item $mapPath).Length -eq (60 * 30 * 2)) 'Via Consolare map.bin size does not match 60x30.'
$raw = [IO.File]::ReadAllBytes($mapPath)
$allowed = @(0x3001, 0x300D, 0x310C)
for ($i = 0; $i -lt $raw.Length; $i += 2) {
    $value = [BitConverter]::ToUInt16($raw, $i)
    Assert-True ($allowed -contains $value) ('Unexpected block value in Via Consolare map.bin: 0x{0:X4}' -f $value)
}

$viaWild = @($wild.wild_encounter_groups.encounters | Where-Object { $_.map -eq 'MAP_VIA_CONSOLARE' })
Assert-True ($viaWild.Count -eq 0) 'Via Consolare must not have an encounter table in this blockout.'

Write-Output 'Via Consolare structural blockout: PASS'
