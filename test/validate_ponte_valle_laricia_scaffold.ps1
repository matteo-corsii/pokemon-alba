param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Read-Block([byte[]]$bytes, [int]$width, [int]$x, [int]$y) { [BitConverter]::ToUInt16($bytes, 2 * ($y * $width + $x)) }
function Collision([UInt16]$raw) { return (($raw -band 0x0C00) -shr 10) }
function Elevation([UInt16]$raw) { return (($raw -shr 12) -band 15) }
function Is-Walkable([UInt16]$raw) { return (Collision $raw) -eq 0 }
function Are-ElevationsCompatible([UInt16]$from, [UInt16]$to) {
    $fromElevation = Elevation $from
    $toElevation = Elevation $to
    return ($fromElevation -eq 0 -or $fromElevation -eq 15 -or $toElevation -eq 0 -or $toElevation -eq 15 -or $fromElevation -eq $toElevation)
}
function Set-Block([byte[]]$bytes, [int]$width, [int]$x, [int]$y, [UInt16]$metatile, [bool]$impassable) {
    $raw = Read-Block $bytes $width $x $y
    $entry = (($raw -band 0xF000) -bor $metatile)
    if ($impassable) { $entry = $entry -bor 0x0C00 }
    [Array]::Copy([BitConverter]::GetBytes([UInt16]$entry), 0, $bytes, 2 * ($y * $width + $x), 2)
}
function Test-Reachable([byte[]]$bytes, [int]$width, [int]$height, [string[]]$starts, [string[]]$targets) {
    $targetsByKey = @{}
    foreach ($target in $targets) { $targetsByKey[$target] = $true }
    $seen = @{}
    $queue = New-Object System.Collections.Generic.Queue[string]
    foreach ($start in $starts) {
        $startParts = $start.Split(',')
        $startX = [int]$startParts[0]; $startY = [int]$startParts[1]
        $raw = Read-Block $bytes $width $startX $startY
        if (Is-Walkable $raw) {
            $key = "$startX,$startY"
            if (-not $seen.ContainsKey($key)) { $seen[$key] = $true; $queue.Enqueue($key) }
        }
    }
    while ($queue.Count -gt 0) {
        $key = $queue.Dequeue()
        if ($targetsByKey.ContainsKey($key)) { return $true }
        $parts = $key.Split(',')
        $x = [int]$parts[0]; $y = [int]$parts[1]
        $from = Read-Block $bytes $width $x $y
        foreach ($delta in @(@(1, 0), @(-1, 0), @(0, 1), @(0, -1))) {
            $nx = $x + $delta[0]; $ny = $y + $delta[1]
            if ($nx -lt 0 -or $nx -ge $width -or $ny -lt 0 -or $ny -ge $height) { continue }
            $to = Read-Block $bytes $width $nx $ny
            # Mirrors the engine's transition/multi-level elevation exceptions.
            if ((Is-Walkable $to) -and (Are-ElevationsCompatible $from $to)) {
                $nextKey = "$nx,$ny"
                if (-not $seen.ContainsKey($nextKey)) { $seen[$nextKey] = $true; $queue.Enqueue($nextKey) }
            }
        }
    }
    return $false
}

$layouts = (Read-Json 'data/layouts/layouts.json').layouts
$groups = Read-Json 'data/maps/map_groups.json'
$map = Read-Json 'data/maps/PonteValleLaricia/map.json'
$route103 = Read-Json 'data/maps/Route103/map.json'
$layout = @($layouts | Where-Object { $_.id -eq 'LAYOUT_PONTE_VALLE_LARICIA' })

Assert-True ($map.id -eq 'MAP_PONTE_VALLE_LARICIA' -and $map.name -eq 'PonteValleLaricia' -and $map.layout -eq 'LAYOUT_PONTE_VALLE_LARICIA') 'Ponte/Valle map identity is incorrect.'
Assert-True ($map.region_map_section -eq 'MAPSEC_PONTE_VALLE_LARICIA') 'Ponte/Valle map section is incorrect.'
Assert-True ($layout.Count -eq 1 -and [int]$layout[0].width -eq 64 -and [int]$layout[0].height -eq 64) 'Ponte/Valle must have one 64x64 layout.'
Assert-True ($layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_PortaPretoria') 'Ponte/Valle tilesets are incorrect.'
Assert-True (@($groups.gMapGroup_TownsAndRoutes | Where-Object { $_ -eq 'PonteValleLaricia' }).Count -eq 1) 'Ponte/Valle must be registered once in TownsAndRoutes.'

$expectedBorder = [byte[]](0xD4,0x01,0xD5,0x01,0xDC,0x01,0xDD,0x01)
$border = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot $layout[0].border_filepath))
Assert-True ($layout[0].border_filepath -eq 'data/layouts/PonteValleLaricia/border.bin' -and $layout[0].blockdata_filepath -eq 'data/layouts/PonteValleLaricia/map.bin') 'Ponte/Valle layout paths are incorrect.'
Assert-True ($border.Length -eq 8 -and ([Linq.Enumerable]::SequenceEqual([byte[]]$border, $expectedBorder))) 'Ponte/Valle must use the approved forest border.'
$blocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot $layout[0].blockdata_filepath))
Assert-True ($blocks.Length -eq 64 * 64 * 2) 'Ponte/Valle map.bin size is incorrect.'

$left = @($map.connections | Where-Object { $_.direction -eq 'left' -and $_.map -eq 'MAP_ROUTE103' -and [int]$_.offset -eq 0 })
$right = @($route103.connections | Where-Object { $_.direction -eq 'right' -and $_.map -eq 'MAP_PONTE_VALLE_LARICIA' -and [int]$_.offset -eq 0 })
Assert-True ($left.Count -eq 1 -and $right.Count -eq 1 -and @($map.connections).Count -eq 1) 'Route103 connection must be reciprocal and be the only active Ponte/Valle connection.'

$routeBlocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/Route103/map.bin'))
foreach ($y in 14..17) {
    $routeRaw = Read-Block $routeBlocks 80 79 $y
    $ponteRaw = Read-Block $blocks 64 0 $y
    Assert-True ((Is-Walkable $routeRaw) -and (Is-Walkable $ponteRaw) -and (Elevation $routeRaw) -eq 3 -and (Elevation $ponteRaw) -eq 3) "Route103/Ponte connection lane $y is not functionally walkable."
}

$upperStarts = @(14..17 | ForEach-Object { "0,$_" })
$upperTargets = @(14..17 | ForEach-Object { "63,$_" })
$lowerStarts = @(38..41 | ForEach-Object { "63,$_" })
$lowerTargets = @()
foreach ($x in 0..63) { foreach ($y in 35..63) { if (Is-Walkable (Read-Block $blocks 64 $x $y)) { $lowerTargets += "$x,$y" } } }
Assert-True (Test-Reachable $blocks 64 64 $upperStarts $upperTargets) 'Upper bridge route is not continuous from the western entry to the high eastern access.'
Assert-True (-not (Test-Reachable $blocks 64 64 $upperStarts $lowerTargets)) 'Upper bridge route has a direct collision/elevation-compatible path to the lower valley.'
Assert-True (Test-Reachable $blocks 64 64 $lowerStarts @('20,39')) 'Lower valley cannot reach Casale 1 approach.'
Assert-True (Test-Reachable $blocks 64 64 $lowerStarts @('44,36')) 'Lower valley cannot reach Casale 2 approach.'
Assert-True (Test-Reachable $blocks 64 64 $lowerStarts @('43,59')) 'Lower valley cannot reach the agricultural house approach.'
Assert-True (Test-Reachable $blocks 64 64 $lowerStarts @('51,27')) 'Lower valley cannot reach the secret-passage approach.'
Assert-True (Test-Reachable $blocks 64 64 @('58,10') @('58,5')) 'The local upper secret-passage area cannot reach the future Emissario entrance.'

Assert-True (@($map.object_events).Count -eq 0 -and @($map.warp_events).Count -eq 0 -and @($map.coord_events).Count -eq 0 -and @($map.bg_events).Count -eq 0) 'Ponte/Valle scaffold must not contain gameplay events.'
$wild = Read-Json 'src/data/wild_encounters.json'
Assert-True (@($wild.wild_encounter_groups | ForEach-Object { $_.encounters } | Where-Object { $_.map -eq 'MAP_PONTE_VALLE_LARICIA' }).Count -eq 0) 'Ponte/Valle scaffold must not contain encounters.'
$scripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/event_scripts.s') -Raw
Assert-True ($scripts.Contains('.include "data/maps/PonteValleLaricia/scripts.inc"')) 'Ponte/Valle scripts are not included.'

$routeScripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/Route103/scripts.inc') -Raw
Assert-True ($routeScripts -match 'Route103_OnLoad:\s*\r?\n\s*call Route103_EventScript_UpdatePonteLariciaBarrier') 'Route103 must update the Ponte barrier before its existing OnLoad branches.'
Assert-True ($routeScripts -match 'goto_if_set FLAG_BADGE02_GET, Route103_EventScript_OpenPonteLariciaBarrier') 'Ponte barrier must open solely from FLAG_BADGE02_GET.'
foreach ($y in 14..17) {
    Assert-True ($routeScripts -match "setmetatile 78, $y, METATILE_Petalburg_PretoriaArch_PillarLeft, TRUE") "Closed Ponte barrier tile 78,$y is missing."
    Assert-True ($routeScripts -match "setmetatile 78, $y, 0x10C, FALSE") "Open Ponte lane restoration 78,$y is missing."
}

$closedRoute = [byte[]]::new($routeBlocks.Length); [Array]::Copy($routeBlocks, $closedRoute, $routeBlocks.Length)
$openRoute = [byte[]]::new($routeBlocks.Length); [Array]::Copy($routeBlocks, $openRoute, $routeBlocks.Length)
foreach ($y in 14..17) {
    Set-Block $closedRoute 80 78 $y 0x296 $true
    Set-Block $openRoute 80 78 $y 0x10C $false
    Assert-True ((Read-Block $closedRoute 80 78 $y) -eq 0x3E96) "Closed barrier state must preserve elevation 3 and set the full collision mask at 78,$y."
    Assert-True ((Read-Block $openRoute 80 78 $y) -eq 0x310C) "Open barrier state must restore the original Route103 block at 78,$y."
}
$routeStarts = @(14..17 | ForEach-Object { "60,$_" })
$routeTargets = @(14..17 | ForEach-Object { "79,$_" })
Assert-True (-not (Test-Reachable $closedRoute 80 22 $routeStarts $routeTargets)) 'Closed pre-bridge barrier is bypassable from Route103.'
Assert-True (Test-Reachable $openRoute 80 22 $routeStarts $routeTargets) 'Open pre-bridge barrier does not restore Route103 access to the connection.'

git -C $RepositoryRoot diff --quiet -- data/layouts/Route103/map.bin data/layouts/StradaBorgoCisternoni/map.bin
Assert-True ($LASTEXITCODE -eq 0) 'Existing Route103 or Strada map.bin changed.'
Write-Output 'Ponte/Valle Laricia scaffold: PASS'
