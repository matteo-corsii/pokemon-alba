param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'
function Read-Json([string]$p) { Get-Content (Join-Path $RepositoryRoot $p) -Raw | ConvertFrom-Json }
function Assert-True([bool]$c,[string]$m) { if (-not $c) { throw $m } }
$map = Read-Json 'data/maps/LagoDiAlbera/map.json'
$layout = Read-Json 'data/layouts/layouts.json'
$groups = Read-Json 'data/maps/map_groups.json'
$scripts = Get-Content (Join-Path $RepositoryRoot 'data/maps/LagoDiAlbera/scripts.inc') -Raw
$via = Read-Json 'data/maps/ViaConsolare/map.json'
$route = Read-Json 'data/maps/Route103/map.json'
Assert-True ($map.id -eq 'MAP_LAGO_DI_ALBERA' -and $map.layout -eq 'LAYOUT_LAGO_DI_ALBERA') 'Lago map identity is incorrect.'
Assert-True ($map.map_type -eq 'MAP_TYPE_ROUTE' -and $map.region_map_section -eq 'MAPSEC_ALBERA_STORICA') 'Lago map type or region is incorrect.'
Assert-True (@($map.object_events).Count -eq 0 -and @($map.warp_events).Count -eq 0 -and @($map.coord_events).Count -eq 0 -and @($map.bg_events).Count -eq 0) 'Lago must not contain events.'
Assert-True ($map.connections.Count -eq 1 -and $map.connections[0].direction -eq 'down' -and $map.connections[0].map -eq 'MAP_VIA_CONSOLARE' -and [int]$map.connections[0].offset -eq 31) 'Lago reciprocal connection is incorrect.'
$l = @($layout.layouts | Where-Object id -eq 'LAYOUT_LAGO_DI_ALBERA')
Assert-True ($l.Count -eq 1 -and [int]$l[0].width -eq 120 -and [int]$l[0].height -eq 120 -and $l[0].primary_tileset -eq 'gTileset_General' -and $l[0].secondary_tileset -eq 'gTileset_Pacifidlog') 'Lago layout is incorrect.'
Assert-True (@($groups.gMapGroup_TownsAndRoutes | Where-Object { $_ -eq 'LagoDiAlbera' }).Count -eq 1) 'Lago is not registered exactly once.'
Assert-True (($scripts -split "`n" | Where-Object { $_ -match '^LagoDiAlbera_MapScripts::' }).Count -eq 1) 'Lago scripts symbol is missing or duplicated.'
$bin = Join-Path $RepositoryRoot 'data/layouts/LagoDiAlbera/map.bin'
Assert-True ((Get-Item $bin).Length -eq 28800) 'Lago map.bin must be 120x120.'
Assert-True (@($via.connections | Where-Object { $_.direction -eq 'up' -and $_.map -eq 'MAP_LAGO_DI_ALBERA' -and [int]$_.offset -eq -31 }).Count -eq 1) 'Via Consolare north connection is missing.'
Assert-True (@($route.connections | Where-Object { $_.direction -eq 'left' -and $_.map -eq 'MAP_VIA_CONSOLARE' -and [int]$_.offset -eq 0 }).Count -eq 1) 'Route103 reciprocal connection changed.'
git -C $RepositoryRoot diff --quiet develop -- data/layouts/ViaConsolare/map.bin
Assert-True ($LASTEXITCODE -eq 0) 'Via Consolare map.bin changed.'
$wild = Read-Json 'src/data/wild_encounters.json'
Assert-True (@($wild.wild_encounter_groups.encounters | Where-Object map -eq 'MAP_LAGO_DI_ALBERA').Count -eq 0) 'Lago must not have encounters.'
Write-Output 'Lago di Albera structural blockout: PASS'
