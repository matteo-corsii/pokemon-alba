$ErrorActionPreference = 'Stop'

$root = (Resolve-Path "$PSScriptRoot\\..").Path
$route = Get-Content -Raw (Join-Path $root 'data/maps/Route103/map.json') | ConvertFrom-Json
$cisternoni = Get-Content -Raw (Join-Path $root 'data/maps/Cisternoni/map.json') | ConvertFrom-Json
$layouts = Get-Content -Raw (Join-Path $root 'data/layouts/layouts.json') | ConvertFrom-Json

function Assert-True($condition, $message) {
    if (-not $condition) { throw $message }
}

$routeLayout = $layouts.layouts | Where-Object id -eq 'LAYOUT_ROUTE103'
$cisternoniLayout = $layouts.layouts | Where-Object id -eq 'LAYOUT_CISTERNONI'
Assert-True ($routeLayout.width -eq 80 -and $routeLayout.height -eq 22) 'Route103 must retain its 80x22 seamless-connection footprint.'
Assert-True ($routeLayout.primary_tileset -eq 'gTileset_General' -and $routeLayout.secondary_tileset -eq 'gTileset_PortaPretoria') 'Route103 tilesets must remain General + PortaPretoria.'
Assert-True ($route.connections.Count -eq 1 -and $route.connections[0].map -eq 'MAP_OLDALE_TOWN' -and $route.connections[0].direction -eq 'down') 'Route103 must retain only its south Porta Pretoria connection.'
Assert-True ($route.object_events.Count -eq 0) 'Route103 must not retain vanilla NPC or rival events.'
Assert-True ($route.bg_events.Count -eq 1 -and $route.bg_events[0].x -eq 52 -and $route.bg_events[0].y -eq 8 -and $route.bg_events[0].player_facing_dir -eq 'BG_EVENT_PLAYER_FACING_NORTH' -and $route.bg_events[0].script -eq 'Route103_EventScript_CisternoniAccessClosed') 'Cisternoni gate sign is missing, displaced, or uses an invalid facing direction.'
Assert-True ($route.warp_events.Count -eq 1 -and $route.warp_events[0].dest_map -eq 'MAP_CISTERNONI') 'Route103 must provide only the Cisternoni return-warp architecture.'
Assert-True ($cisternoniLayout.width -eq 34 -and $cisternoniLayout.height -eq 24) 'Cisternoni must remain a single 34x24 interior.'
Assert-True ($cisternoniLayout.primary_tileset -eq 'gTileset_General' -and $cisternoniLayout.secondary_tileset -eq 'gTileset_Cisternoni') 'Cisternoni tileset contract changed.'
Assert-True ($cisternoni.connections -eq $null -and $cisternoni.warp_events.Count -eq 1) 'Cisternoni must remain a one-level interior with one return warp.'
Assert-True ($cisternoni.warp_events[0].dest_map -eq 'MAP_ROUTE103' -and $cisternoni.warp_events[0].dest_warp_id -eq '0') 'Cisternoni return warp must target Route103 warp 0.'

$routeBytes = [IO.File]::ReadAllBytes((Join-Path $root 'data/layouts/Route103/map.bin'))
$cisternoniBytes = [IO.File]::ReadAllBytes((Join-Path $root 'data/layouts/Cisternoni/map.bin'))
Assert-True ($routeBytes.Length -eq 80 * 22 * 2) 'Route103 blockdata size is invalid.'
Assert-True ($cisternoniBytes.Length -eq 34 * 24 * 2) 'Cisternoni blockdata size is invalid.'
$waterCount = 0
for ($i = 0; $i -lt $cisternoniBytes.Length; $i += 2) {
    if (([BitConverter]::ToUInt16($cisternoniBytes, $i) -band 0x3FF) -eq 0x170) { $waterCount++ }
}
Assert-True ($waterCount -ge 100) 'Cisternoni needs meaningful lateral water basins.'
$readCell = {
    param($x, $y)
    [BitConverter]::ToUInt16($cisternoniBytes, 2 * ($y * 34 + $x)) -band 0x3FF
}
for ($y = 6; $y -le 21; $y++) {
    Assert-True ((& $readCell 1 $y) -ne 0x170 -and (& $readCell 10 $y) -ne 0x170) 'West basin must be physically separated from walkable space.'
    Assert-True ((& $readCell 22 $y) -ne 0x170 -and (& $readCell 23 $y) -ne 0x170 -and (& $readCell 32 $y) -ne 0x170) 'East basin must be physically separated from walkable space.'
}
Assert-True (Test-Path (Join-Path $root 'data/tilesets/secondary/cisternoni/metatiles.bin')) 'Dedicated Cisternoni secondary tileset is missing.'

Write-Output 'Porta Pretoria north / Cisternoni structural validation passed.'
