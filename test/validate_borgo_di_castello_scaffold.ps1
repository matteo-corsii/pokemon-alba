param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Read-Json([string]$path) { Get-Content (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Get-Block([byte[]]$blockdata, [int]$x, [int]$y) { [int]$offset = 2 * ($y * 60 + $x); return ([int]$blockdata[$offset]) -bor (([int]$blockdata[($offset + 1)]) -shl 8) }
function Get-Collision([int]$block) { return (($block -shr 10) -band 1) }

$borgo = Read-Json 'data/maps/BorgoDiCastello/map.json'
$lago = Read-Json 'data/maps/LagoDiAlbera/map.json'
$layouts = Read-Json 'data/layouts/layouts.json'
$groups = Read-Json 'data/maps/map_groups.json'
$bosco = Read-Json 'data/maps/BoscoDelRomitorio/map.json'
$scripts = Get-Content (Join-Path $RepositoryRoot 'data/maps/BorgoDiCastello/scripts.inc') -Raw
$eventScripts = Get-Content (Join-Path $RepositoryRoot 'data/event_scripts.s') -Raw

$layout = @($layouts.layouts | Where-Object { $_.id -eq 'LAYOUT_BORGO_DI_CASTELLO' })
Assert-True ($borgo.id -eq 'MAP_BORGO_DI_CASTELLO' -and $borgo.layout -eq 'LAYOUT_BORGO_DI_CASTELLO') 'Borgo map identity is incorrect.'
Assert-True ($borgo.map_type -eq 'MAP_TYPE_TOWN' -and $borgo.region_map_section -eq 'MAPSEC_ALBERA_STORICA') 'Borgo map metadata is incorrect.'
Assert-True ($layout.Count -eq 1 -and [int]$layout[0].width -eq 60 -and [int]$layout[0].height -eq 60 -and $layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_Sootopolis') 'Borgo layout is incorrect.'
Assert-True ((Get-Item (Join-Path $RepositoryRoot $layout[0].blockdata_filepath)).Length -eq 7200) 'Borgo map.bin size is incorrect.'
Assert-True ((Test-Path (Join-Path $RepositoryRoot $layout[0].border_filepath))) 'Borgo border.bin is missing.'
$blockdata = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot $layout[0].blockdata_filepath))
Assert-True (@($groups.gMapGroup_TownsAndRoutes | Where-Object { $_ -eq 'BorgoDiCastello' }).Count -eq 1) 'Borgo map group registration is incorrect.'
Assert-True (@($lago.connections | Where-Object { $_.map -eq 'MAP_BORGO_DI_CASTELLO' -and $_.direction -eq 'up' -and [int]$_.offset -eq 87 }).Count -eq 1) 'Lago to Borgo connection is incorrect.'
Assert-True (@($borgo.connections | Where-Object { $_.map -eq 'MAP_LAGO_DI_ALBERA' -and $_.direction -eq 'down' -and [int]$_.offset -eq -87 }).Count -eq 1) 'Borgo to Lago connection is incorrect.'
foreach ($pair in @(@{ lake = 115; borgo = 28 }, @{ lake = 116; borgo = 29 }, @{ lake = 117; borgo = 30 }, @{ lake = 118; borgo = 31 })) {
    Assert-True (($pair.lake - 87) -eq $pair.borgo) "Lago north access $($pair.lake),0 is not aligned to Borgo $($pair.borgo),59."
}
Assert-True (@($borgo.object_events).Count -eq 0 -and @($borgo.warp_events).Count -eq 0 -and @($borgo.coord_events).Count -eq 0 -and @($borgo.bg_events).Count -eq 0) 'Borgo scaffold must not contain events.'
$wildEncounters = Get-Content (Join-Path $RepositoryRoot 'src/data/wild_encounters.json') -Raw
Assert-True ($wildEncounters -notmatch 'MAP_BORGO_DI_CASTELLO') 'Borgo blockout must not contain encounters.'
Assert-True ($scripts -match '^BorgoDiCastello_MapScripts::\r?\n\s*\.byte 0') 'Borgo MapScripts are missing.'
Assert-True ($eventScripts -match 'data/maps/BorgoDiCastello/scripts\.inc') 'Borgo scripts are not included globally.'
Assert-True (@($bosco.warp_events | Where-Object { $_.dest_map -eq 'MAP_BORGO_DI_CASTELLO' }).Count -eq 0 -and @($bosco.connections | Where-Object { $_.map -eq 'MAP_BORGO_DI_CASTELLO' }).Count -eq 0) 'Bosco must not connect directly to Borgo.'
Assert-True ($lago.connections.Count -eq 2) 'Lago has unexpected map connections.'
foreach ($x in 28..31) {
    Assert-True ((Get-Collision (Get-Block $blockdata $x 59)) -eq 0) "Borgo south connection cell $x,59 is blocked."
}
Assert-True ((Get-Collision (Get-Block $blockdata 29 52)) -eq 0 -and (Get-Collision (Get-Block $blockdata 26 38)) -eq 0) 'The Borgo south-to-centre route is blocked.'
Assert-True ((Get-Collision (Get-Block $blockdata 10 50)) -eq 0 -and (Get-Collision (Get-Block $blockdata 4 50)) -eq 1 -and (Get-Collision (Get-Block $blockdata 10 54)) -eq 1) 'The south-west belvedere is not safely blockouted.'
foreach ($anchor in @(@(17, 41), @(37, 39), @(10, 30), @(43, 35), @(37, 18))) {
    Assert-True ((Get-Collision (Get-Block $blockdata $anchor[0] $anchor[1])) -eq 1) "Missing solid building placeholder at $($anchor[0]),$($anchor[1])."
}
Assert-True ((Get-Collision (Get-Block $blockdata 20 10)) -eq 0) 'The north main square is not open.'
Assert-True ((Get-Collision (Get-Block $blockdata 29 3)) -eq 0) 'The future Villa Papale north access is blocked.'
Assert-True ((Get-Collision (Get-Block $blockdata 54 30)) -eq 0 -and (Get-Collision (Get-Block $blockdata 59 30)) -eq 1) 'The future east regional exit is not prepared and secured.'
Write-Output 'Borgo di Castello scaffold: PASS'
