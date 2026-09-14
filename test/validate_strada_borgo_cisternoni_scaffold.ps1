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

$routeBase = Read-GitBlob 'HEAD:data/layouts/Route103/map.bin'
$allowed = @{}
foreach ($x in 16..19) { $allowed["$x,0"] = 0x310C }
foreach ($y in 14..17) { $allowed["79,$y"] = 0x310C }
$deltaCount = 0
for ($cell = 0; $cell -lt ($routeBlocks.Length / 2); $cell++) {
    $before = [BitConverter]::ToUInt16($routeBase, $cell * 2)
    $after = [BitConverter]::ToUInt16($routeBlocks, $cell * 2)
    if ($before -ne $after) {
        $key = "{0},{1}" -f ($cell % 80), [int][Math]::Floor($cell / 80)
        Assert-True ($allowed.ContainsKey($key) -and $after -eq $allowed[$key]) "Unexpected Route103 map.bin delta at $key."
        $deltaCount++
    }
}
Assert-True ($deltaCount -eq $allowed.Count) 'Route103 map.bin must contain exactly the two approved openings.'

Assert-True (@($strada.object_events).Count -eq 0 -and @($strada.warp_events).Count -eq 0 -and @($strada.coord_events).Count -eq 0 -and @($strada.bg_events).Count -eq 0) 'Strada must remain free of NPCs, warps and scripted events.'
Assert-True ((Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/StradaBorgoCisternoni/scripts.inc') -Raw).Trim() -eq "StradaBorgoCisternoni_MapScripts::`n`t.byte 0") 'Strada scripts must remain minimal.'
$wild = Read-Json 'src/data/wild_encounters.json'
Assert-True (@($wild.wild_encounter_groups | ForEach-Object { $_.encounters } | Where-Object { $_.map -eq 'MAP_STRADA_BORGO_CISTERNONI' }).Count -eq 0) 'Strada must not have encounters.'
git -C $RepositoryRoot diff --quiet -- data/layouts/BorgoDiCastello/map.bin
Assert-True ($LASTEXITCODE -eq 0) 'Borgo map.bin must remain unchanged.'

Write-Output 'Strada Borgo-Cisternoni scaffold: PASS'
