param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Read-Block([byte[]]$bytes, [int]$width, [int]$x, [int]$y) { [BitConverter]::ToUInt16($bytes, 2 * ($y * $width + $x)) }
function Collision([UInt16]$raw) { return (($raw -shr 10) -band 1) }
function Elevation([UInt16]$raw) { return (($raw -shr 12) -band 15) }

$layouts = (Read-Json 'data/layouts/layouts.json').layouts
$groups = Read-Json 'data/maps/map_groups.json'
$map = Read-Json 'data/maps/PonteValleLaricia/map.json'
$route103 = Read-Json 'data/maps/Route103/map.json'
$layout = @($layouts | Where-Object { $_.id -eq 'LAYOUT_PONTE_VALLE_LARICIA' })

Assert-True ($map.id -eq 'MAP_PONTE_VALLE_LARICIA' -and $map.name -eq 'PonteValleLaricia' -and $map.layout -eq 'LAYOUT_PONTE_VALLE_LARICIA') 'Ponte/Valle map identity is incorrect.'
Assert-True ($layout.Count -eq 1 -and [int]$layout[0].width -eq 64 -and [int]$layout[0].height -eq 64) 'Ponte/Valle must have one 64x64 layout.'
Assert-True ($layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_PortaPretoria') 'Ponte/Valle tilesets are incorrect.'
Assert-True ($layout[0].border_filepath -eq 'data/layouts/PonteValleLaricia/border.bin' -and $layout[0].blockdata_filepath -eq 'data/layouts/PonteValleLaricia/map.bin') 'Ponte/Valle layout paths are incorrect.'
Assert-True (@($groups.gMapGroup_TownsAndRoutes | Where-Object { $_ -eq 'PonteValleLaricia' }).Count -eq 1) 'Ponte/Valle must be registered once in TownsAndRoutes.'

$expectedBorder = [byte[]](0xD4,0x01,0xD5,0x01,0xDC,0x01,0xDD,0x01)
$border = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot $layout[0].border_filepath))
Assert-True ($border.Length -eq 8 -and ([Linq.Enumerable]::SequenceEqual([byte[]]$border, $expectedBorder))) 'Ponte/Valle must use the approved forest border.'
$blocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot $layout[0].blockdata_filepath))
Assert-True ($blocks.Length -eq 64 * 64 * 2) 'Ponte/Valle map.bin size is incorrect.'

$left = @($map.connections | Where-Object { $_.direction -eq 'left' -and $_.map -eq 'MAP_ROUTE103' -and [int]$_.offset -eq 0 })
$right = @($route103.connections | Where-Object { $_.direction -eq 'right' -and $_.map -eq 'MAP_PONTE_VALLE_LARICIA' -and [int]$_.offset -eq 0 })
Assert-True ($left.Count -eq 1 -and $right.Count -eq 1 -and @($map.connections).Count -eq 1) 'Route103 connection must be reciprocal and be the only active Ponte/Valle connection.'
foreach ($y in 14..17) {
    $routeBlocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/Route103/map.bin'))
    $routeRaw = Read-Block $routeBlocks 80 79 $y
    $ponteRaw = Read-Block $blocks 64 0 $y
    Assert-True ($routeRaw -eq 0x310C -and $ponteRaw -eq 0x310C -and (Collision $ponteRaw) -eq 0 -and (Elevation $ponteRaw) -eq 3) "Route103 bridge entry $y is not tileset-safe."
}

# The upper deck is reachable; an unbroken blocked terrace prevents direct access to the lower valley.
foreach ($y in 14..17) { foreach ($x in 0..63) { Assert-True ((Collision (Read-Block $blocks 64 $x $y)) -eq 0) "Upper bridge deck is blocked at $x,$y." } }
foreach ($y in 24..34) { foreach ($x in 0..63) { Assert-True ((Collision (Read-Block $blocks 64 $x $y)) -eq 1) "Altitude barrier is open at $x,$y." } }
foreach ($point in @(@(8,40), @(24,45), @(40,50), @(56,55))) { Assert-True ((Collision (Read-Block $blocks 64 $point[0] $point[1])) -eq 0) "Lower valley is not walkable at $($point[0]),$($point[1])." }
foreach ($y in 35..50) { Assert-True ((Collision (Read-Block $blocks 64 60 $y)) -eq 0) "Future Laricia descent apron is blocked at 60,$y." }

Assert-True (@($map.object_events).Count -eq 0 -and @($map.warp_events).Count -eq 0 -and @($map.coord_events).Count -eq 0 -and @($map.bg_events).Count -eq 0) 'Ponte/Valle scaffold must not contain gameplay events.'
$wild = Read-Json 'src/data/wild_encounters.json'
Assert-True (@($wild.wild_encounter_groups | ForEach-Object { $_.encounters } | Where-Object { $_.map -eq 'MAP_PONTE_VALLE_LARICIA' }).Count -eq 0) 'Ponte/Valle scaffold must not contain encounters.'
$scripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/event_scripts.s') -Raw
Assert-True ($scripts.Contains('.include "data/maps/PonteValleLaricia/scripts.inc"')) 'Ponte/Valle scripts are not included.'
git -C $RepositoryRoot diff --quiet -- data/layouts/Route103/map.bin data/layouts/StradaBorgoCisternoni/map.bin
Assert-True ($LASTEXITCODE -eq 0) 'Existing Route103 or Strada map.bin changed.'
Write-Output 'Ponte/Valle Laricia scaffold: PASS'
