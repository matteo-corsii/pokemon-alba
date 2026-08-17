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
Assert-True ($route.object_events.Count -eq 5) 'Route103 must contain exactly the approved Via dei Cisternoni trainers and ambient NPCs.'
$routeExpectedObjects = @(
    @{ x = 22; y = 8; graphics_id = 'OBJ_EVENT_GFX_HIKER'; script = 'Route103_EventScript_Marco'; trainer_type = 'TRAINER_TYPE_NORMAL' },
    @{ x = 34; y = 11; graphics_id = 'OBJ_EVENT_GFX_CAMPER'; script = 'Route103_EventScript_Teo'; trainer_type = 'TRAINER_TYPE_NORMAL' },
    @{ x = 27; y = 4; graphics_id = 'OBJ_EVENT_GFX_FISHERMAN'; script = 'Route103_EventScript_Fisherman'; trainer_type = 'TRAINER_TYPE_NONE' },
    @{ x = 38; y = 10; graphics_id = 'OBJ_EVENT_GFX_WOMAN_5'; script = 'Route103_EventScript_Visitor'; trainer_type = 'TRAINER_TYPE_NONE' },
    @{ x = 45; y = 8; graphics_id = 'OBJ_EVENT_GFX_MAN_2'; script = 'Route103_EventScript_Walker'; trainer_type = 'TRAINER_TYPE_NONE' }
)
foreach ($expected in $routeExpectedObjects) {
    $actual = $route.object_events | Where-Object { $_.x -eq $expected.x -and $_.y -eq $expected.y }
    Assert-True ($null -ne $actual -and $actual.graphics_id -eq $expected.graphics_id -and $actual.script -eq $expected.script -and $actual.trainer_type -eq $expected.trainer_type -and $actual.elevation -eq 3) "Route103 event at ($($expected.x),$($expected.y)) changed."
}
Assert-True ($route.bg_events.Count -eq 1 -and $route.bg_events[0].x -eq 52 -and $route.bg_events[0].y -eq 8 -and $route.bg_events[0].player_facing_dir -eq 'BG_EVENT_PLAYER_FACING_NORTH' -and $route.bg_events[0].script -eq 'Route103_EventScript_CisternoniAccessClosed') 'Cisternoni gate sign is missing, displaced, or uses an invalid facing direction.'
Assert-True ($route.warp_events.Count -eq 2) 'Route103 must provide the blocked entrance and the safe Cisternoni return destination.'
Assert-True ($route.warp_events[0].x -eq 52 -and $route.warp_events[0].y -eq 7 -and $route.warp_events[0].elevation -eq 0 -and $route.warp_events[0].dest_map -eq 'MAP_CISTERNONI' -and $route.warp_events[0].dest_warp_id -eq '0') 'Cisternoni blocked entrance warp changed.'
Assert-True ($route.warp_events[1].x -eq 52 -and $route.warp_events[1].y -eq 9 -and $route.warp_events[1].elevation -eq 3 -and $route.warp_events[1].dest_map -eq 'MAP_CISTERNONI' -and $route.warp_events[1].dest_warp_id -eq '0') 'Cisternoni safe return destination changed.'
Assert-True ($cisternoniLayout.width -eq 34 -and $cisternoniLayout.height -eq 24) 'Cisternoni must remain a single 34x24 interior.'
Assert-True ($cisternoniLayout.primary_tileset -eq 'gTileset_General' -and $cisternoniLayout.secondary_tileset -eq 'gTileset_Cisternoni') 'Cisternoni tileset contract changed.'
Assert-True ($cisternoni.connections -eq $null -and $cisternoni.warp_events.Count -eq 2) 'Cisternoni must remain a one-level interior with two adjacent return cells.'
foreach ($warp in $cisternoni.warp_events) {
    Assert-True ($warp.y -eq 2 -and $warp.x -in @(16, 17) -and $warp.elevation -eq 3 -and $warp.dest_map -eq 'MAP_ROUTE103' -and $warp.dest_warp_id -eq '1') 'Cisternoni return warps must use the adjacent entrance cells and safe Route103 warp 1.'
}

$routeBytes = [IO.File]::ReadAllBytes((Join-Path $root 'data/layouts/Route103/map.bin'))
$cisternoniBytes = [IO.File]::ReadAllBytes((Join-Path $root 'data/layouts/Cisternoni/map.bin'))
Assert-True ($routeBytes.Length -eq 80 * 22 * 2) 'Route103 blockdata size is invalid.'
Assert-True ($cisternoniBytes.Length -eq 34 * 24 * 2) 'Cisternoni blockdata size is invalid.'
$readRouteRawCell = {
    param($x, $y)
    [BitConverter]::ToUInt16($routeBytes, 2 * ($y * 80 + $x))
}
Assert-True ((0..21 | Where-Object { ((& $readRouteRawCell 0 $_) -band 0x400) -eq 0 }).Count -gt 0) 'The west Via Consolare passage must remain physically open.'
Assert-True ((0..79 | Where-Object { ((& $readRouteRawCell $_ 0) -band 0x400) -eq 0 }).Count -eq 0) 'The north Castel Gandolfo boundary must remain blocked until its future connection exists.'
$waterCount = 0
for ($i = 0; $i -lt $cisternoniBytes.Length; $i += 2) {
    if (([BitConverter]::ToUInt16($cisternoniBytes, $i) -band 0x3FF) -eq 0x170) { $waterCount++ }
}
Assert-True ($waterCount -ge 100) 'Cisternoni needs meaningful lateral water basins.'
$readCell = {
    param($x, $y)
    [BitConverter]::ToUInt16($cisternoniBytes, 2 * ($y * 34 + $x)) -band 0x3FF
}
$readRawCell = {
    param($x, $y)
    [BitConverter]::ToUInt16($cisternoniBytes, 2 * ($y * 34 + $x))
}
for ($y = 0; $y -lt 24; $y++) {
    for ($x = 0; $x -lt 34; $x++) {
        if ((& $readCell $x $y) -ne 0x170) { continue }
        foreach ($offset in @(@(-1, 0), @(1, 0), @(0, -1), @(0, 1))) {
            $nx = $x + $offset[0]
            $ny = $y + $offset[1]
            if ($nx -lt 0 -or $nx -ge 34 -or $ny -lt 0 -or $ny -ge 24) { continue }
            if ((& $readCell $nx $ny) -ne 0x170) {
                Assert-True (((& $readRawCell $nx $ny) -band 0x400) -ne 0) "Cisternoni basin leaks through passable cell ($nx,$ny)."
            }
        }
    }
}
Assert-True (Test-Path (Join-Path $root 'data/tilesets/secondary/cisternoni/metatiles.bin')) 'Dedicated Cisternoni secondary tileset is missing.'
$cisternoniMetatiles = [IO.File]::ReadAllBytes((Join-Path $root 'data/tilesets/secondary/cisternoni/metatiles.bin'))
$cisternoniAttributes = [IO.File]::ReadAllBytes((Join-Path $root 'data/tilesets/secondary/cisternoni/metatile_attributes.bin'))
Assert-True (($cisternoniMetatiles.Length / 16) -ge 318 -and ($cisternoniAttributes.Length / 2) -ge 318) 'Cisternoni warp-floor metatile is missing.'
$warpFloorId = 0x33D
$warpFloorAttr = [BitConverter]::ToUInt16($cisternoniAttributes, 2 * ($warpFloorId - 0x200))
Assert-True (($warpFloorAttr -band 0xFF) -eq 0x60) 'Cisternoni entrance cells must use a non-animated door warp behavior.'
foreach ($x in @(16, 17)) {
    Assert-True ((& $readCell $x 2) -eq $warpFloorId) 'Cisternoni entrance floor must use the dedicated warp-compatible metatile.'
}

Write-Output 'Porta Pretoria north / Cisternoni structural validation passed.'
