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
Assert-True (@($map.object_events).Count -eq 13 -and @($map.warp_events).Count -eq 4 -and @($map.coord_events).Count -eq 1 -and @($map.bg_events).Count -eq 8) 'Lago event counts are incorrect.'
Assert-True ($map.connections.Count -eq 1 -and $map.connections[0].direction -eq 'down' -and $map.connections[0].map -eq 'MAP_VIA_CONSOLARE' -and [int]$map.connections[0].offset -eq 31) 'Lago reciprocal connection is incorrect.'
$l = @($layout.layouts | Where-Object id -eq 'LAYOUT_LAGO_DI_ALBERA')
Assert-True ($l.Count -eq 1 -and [int]$l[0].width -eq 120 -and [int]$l[0].height -eq 120 -and $l[0].primary_tileset -eq 'gTileset_General' -and $l[0].secondary_tileset -eq 'gTileset_LagoDiAlbera') 'Lago layout is incorrect.'
Assert-True (@($groups.gMapGroup_TownsAndRoutes | Where-Object { $_ -eq 'LagoDiAlbera' }).Count -eq 1) 'Lago is not registered exactly once.'
Assert-True (($scripts -split "`n" | Where-Object { $_ -match '^LagoDiAlbera_MapScripts::' }).Count -eq 1) 'Lago scripts symbol is missing or duplicated.'
$bin = Join-Path $RepositoryRoot 'data/layouts/LagoDiAlbera/map.bin'
Assert-True ((Get-Item $bin).Length -eq 28800) 'Lago map.bin must be 120x120.'
$lagoBytes = [IO.File]::ReadAllBytes($bin)
$secondaryBytes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/secondary/lago_di_albera/metatiles.bin'))
$maxGlobalMetatile = 0x200 + ($secondaryBytes.Length / 16) - 1
for ($cell = 0; $cell -lt $lagoBytes.Length / 2; $cell++) {
    $raw = [BitConverter]::ToUInt16($lagoBytes, $cell * 2)
    Assert-True (($raw -band 0xC000) -eq 0) "Invalid Lago collision/elevation bits at cell $cell."
    Assert-True (($raw -band 0x3FF) -le $maxGlobalMetatile) "Lago metatile is outside the loaded tileset at cell $cell."
}
Assert-True (@($via.connections | Where-Object { $_.direction -eq 'up' -and $_.map -eq 'MAP_LAGO_DI_ALBERA' -and [int]$_.offset -eq -31 }).Count -eq 1) 'Via Consolare north connection is missing.'
Assert-True (@($route.connections | Where-Object { $_.direction -eq 'left' -and $_.map -eq 'MAP_VIA_CONSOLARE' -and [int]$_.offset -eq 0 }).Count -eq 1) 'Route103 reciprocal connection changed.'
# ViaConsolare/map.bin has an explicit exact-delta guard in the Lago tileset validator.
$wild = Read-Json 'src/data/wild_encounters.json'
Assert-True (@($wild.wild_encounter_groups.encounters | Where-Object map -eq 'MAP_LAGO_DI_ALBERA').Count -eq 4) 'Lago must retain four time-of-day encounter tables.'
Write-Output 'Lago di Albera structural blockout: PASS'
