param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Read-Block([byte[]]$bytes, [int]$width, [int]$x, [int]$y) { [BitConverter]::ToUInt16($bytes, 2 * ($y * $width + $x)) }
function Get-Collision([int]$raw) { (($raw -shr 10) -band 1) }
function Read-GitBlob([string]$spec) {
    $temp = [IO.Path]::GetTempFileName()
    try {
        $process = New-Object Diagnostics.Process
        $process.StartInfo.FileName = 'git'
        $process.StartInfo.Arguments = "-C `"$RepositoryRoot`" cat-file blob $spec"
        $process.StartInfo.UseShellExecute = $false
        $process.StartInfo.RedirectStandardOutput = $true
        $null = $process.Start()
        $stream = [IO.File]::Create($temp)
        $process.StandardOutput.BaseStream.CopyTo($stream)
        $stream.Dispose()
        $process.WaitForExit()
        Assert-True ($process.ExitCode -eq 0) "Unable to read Git blob $spec."
        return [IO.File]::ReadAllBytes($temp)
    } finally { Remove-Item -LiteralPath $temp -Force -ErrorAction SilentlyContinue }
}

$layouts = (Read-Json 'data/layouts/layouts.json').layouts
$strada = Read-Json 'data/maps/StradaBorgoCisternoni/map.json'
$borgo = Read-Json 'data/maps/BorgoDiCastello/map.json'
$route103 = Read-Json 'data/maps/Route103/map.json'
$layout = @($layouts | Where-Object { $_.id -eq 'LAYOUT_STRADA_BORGO_CISTERNONI' })

Assert-True ($strada.id -eq 'MAP_STRADA_BORGO_CISTERNONI' -and $strada.layout -eq 'LAYOUT_STRADA_BORGO_CISTERNONI') 'Strada map identity is incorrect.'
Assert-True ($strada.region_map_section -eq 'MAPSEC_ALBERA_STORICA' -and $strada.map_type -eq 'MAP_TYPE_ROUTE') 'Strada map section or type is incorrect.'
Assert-True ($layout.Count -eq 1 -and [int]$layout[0].width -eq 36 -and [int]$layout[0].height -eq 44 -and $layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_PortaPretoria') 'Strada layout or tilesets are incorrect.'
$mapBinPath = Join-Path $RepositoryRoot $layout[0].blockdata_filepath
$borderPath = Join-Path $RepositoryRoot $layout[0].border_filepath
Assert-True ((Get-Item -LiteralPath $mapBinPath).Length -eq 36 * 44 * 2) 'Strada map.bin size is incorrect.'
Assert-True ((Get-Item -LiteralPath $borderPath).Length -eq 8) 'Strada border.bin size is incorrect.'
$referenceBorder = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/AlberaStorica/border.bin'))
Assert-True ([Linq.Enumerable]::SequenceEqual([byte[]][IO.File]::ReadAllBytes($borderPath), [byte[]]$referenceBorder)) 'Strada border must match Albera Storica forest border.'

Assert-True (@($borgo.connections | Where-Object { $_.direction -eq 'right' -and $_.map -eq 'MAP_STRADA_BORGO_CISTERNONI' -and [int]$_.offset -eq 50 }).Count -eq 1) 'Borgo east connection is incorrect.'
Assert-True (@($strada.connections | Where-Object { $_.direction -eq 'left' -and $_.map -eq 'MAP_BORGO_DI_CASTELLO' -and [int]$_.offset -eq -50 }).Count -eq 1) 'Strada west connection is incorrect.'
Assert-True (@($strada.connections | Where-Object { $_.direction -eq 'down' -and $_.map -eq 'MAP_ROUTE103' -and [int]$_.offset -eq 0 }).Count -eq 1) 'Strada south connection is incorrect.'
Assert-True (@($route103.connections | Where-Object { $_.direction -eq 'up' -and $_.map -eq 'MAP_STRADA_BORGO_CISTERNONI' -and [int]$_.offset -eq 0 }).Count -eq 1) 'Route103 north connection is incorrect.'
Assert-True (@($route103.connections | Where-Object { $_.direction -eq 'right' }).Count -eq 0) 'Route103 must not yet connect to Ponte di Laricia.'

$stradaBlocks = [IO.File]::ReadAllBytes($mapBinPath)
$borgoBlocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/BorgoDiCastello/map.bin'))
$routeBlocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/Route103/map.bin'))
foreach ($y in 0..3) {
    Assert-True ((Read-Block $stradaBlocks 36 0 $y) -eq 0x310C) "Strada west entry 0,$y must match Borgo's shared primary road tile."
    Assert-True ((Read-Block $borgoBlocks 60 59 (50 + $y)) -eq 0x310C) "Borgo east exit 59,$(50 + $y) must retain the shared primary road tile."
}
foreach ($x in 16..19) {
    Assert-True ((Read-Block $stradaBlocks 36 $x 43) -eq 0x310C) "Strada south exit $x,43 is incorrect."
    Assert-True ((Read-Block $routeBlocks 80 $x 0) -eq 0x310C) "Route103 north opening $x,0 is incorrect."
}
foreach ($y in 14..17) { Assert-True ((Read-Block $routeBlocks 80 79 $y) -eq 0x310C) "Route103 future east opening 79,$y is not preserved." }

git -C $RepositoryRoot diff --quiet -- data/layouts/Route103/map.bin
Assert-True ($LASTEXITCODE -eq 0) 'Route103 map.bin must remain unchanged.'

# The refined route remains continuously walkable from the four-cell Borgo
# connection to every cell of the four-cell Route 103 connection.
$walkable = @{}
for ($y = 0; $y -lt 44; $y++) {
    for ($x = 0; $x -lt 36; $x++) {
        $raw = Read-Block $stradaBlocks 36 $x $y
        if ((Get-Collision $raw) -eq 0) { $walkable["$x,$y"] = $true }
    }
}
$queue = New-Object 'System.Collections.Generic.Queue[string]'
$seen = @{}
foreach ($y in 0..3) {
    $key = "0,$y"
    Assert-True $walkable.ContainsKey($key) "Strada west entry $key is blocked."
    $seen[$key] = $true
    $queue.Enqueue($key)
}
while ($queue.Count -gt 0) {
    $key = $queue.Dequeue()
    $parts = $key.Split(',')
    $x = [int]$parts[0]
    $y = [int]$parts[1]
    foreach ($delta in @(@(1,0), @(-1,0), @(0,1), @(0,-1))) {
        $nextX = $x + $delta[0]
        $nextY = $y + $delta[1]
        $next = "$nextX,$nextY"
        if ($walkable.ContainsKey($next) -and -not $seen.ContainsKey($next)) {
            $seen[$next] = $true
            $queue.Enqueue($next)
        }
    }
}
foreach ($x in 16..19) {
    Assert-True $seen.ContainsKey("$x,43") "Strada south connection $x,43 is not reachable from Borgo."
}

# Reserve the approved future house footprint without adding events or warps.
Assert-True ((Get-Collision (Read-Block $stradaBlocks 36 8 29)) -eq 0) 'Future house doorway 8,29 must remain walkable.'

Assert-True (@($strada.object_events).Count -eq 6 -and @($strada.coord_events).Count -eq 9 -and @($strada.bg_events).Count -eq 2) 'Strada population event counts are incorrect.'
$houseWarp = @($strada.warp_events | Where-Object { [int]$_.x -eq 8 -and [int]$_.y -eq 29 -and [int]$_.elevation -eq 0 -and $_.dest_map -eq 'MAP_STRADA_BORGO_CISTERNONI_CASA' -and [int]$_.dest_warp_id -eq 0 })
Assert-True ($houseWarp.Count -eq 1 -and @($strada.warp_events).Count -eq 1) 'Strada may contain only the approved Itemfinder-house warp.'
$stradaScripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/StradaBorgoCisternoni/scripts.inc') -Raw
Assert-True ($stradaScripts.Contains('map_script MAP_SCRIPT_ON_TRANSITION, StradaBorgoCisternoni_OnTransition')) 'Strada must retain the approved Nico/Lia transition gate.'
git -C $RepositoryRoot diff --quiet -- data/layouts/BorgoDiCastello/map.bin
Assert-True ($LASTEXITCODE -eq 0) 'Borgo map.bin must remain unchanged.'

Write-Output 'Strada Borgo-Cisternoni scaffold: PASS'
