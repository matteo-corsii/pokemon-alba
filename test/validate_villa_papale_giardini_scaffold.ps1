param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Read-Json([string]$path) { Get-Content (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Get-Block([byte[]]$blockdata, [int]$x, [int]$y) { $offset = 2 * ($y * 60 + $x); return ([int]$blockdata[$offset]) -bor (([int]$blockdata[$offset + 1]) -shl 8) }
function Get-Collision([int]$block) { return (($block -shr 10) -band 1) }
function Get-MetatileId([int]$block) { return ($block -band 0x3ff) }
function Get-Behavior([byte[]]$attributes, [int]$metatileId) { $offset = 2 * $metatileId; return (([int]$attributes[$offset]) -band 0xff) }

$villa = Read-Json 'data/maps/VillaPapaleGiardini/map.json'
$borgo = Read-Json 'data/maps/BorgoDiCastello/map.json'
$layouts = Read-Json 'data/layouts/layouts.json'
$groups = Read-Json 'data/maps/map_groups.json'
$eventScripts = Get-Content (Join-Path $RepositoryRoot 'data/event_scripts.s') -Raw
$layout = @($layouts.layouts | Where-Object { $_.id -eq 'LAYOUT_VILLA_PAPALE_GIARDINI' })

Assert-True ($villa.id -eq 'MAP_VILLA_PAPALE_GIARDINI' -and $villa.layout -eq 'LAYOUT_VILLA_PAPALE_GIARDINI') 'Villa Papale map identity is incorrect.'
Assert-True ($villa.map_type -eq 'MAP_TYPE_TOWN' -and $villa.region_map_section -eq 'MAPSEC_ALBERA_STORICA') 'Villa Papale metadata is incorrect.'
Assert-True ($layout.Count -eq 1 -and [int]$layout[0].width -eq 60 -and [int]$layout[0].height -eq 60 -and $layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_Sootopolis') 'Villa Papale layout or tilesets are incorrect.'
$mapBin = Join-Path $RepositoryRoot $layout[0].blockdata_filepath
$borderBin = Join-Path $RepositoryRoot $layout[0].border_filepath
Assert-True ((Test-Path $mapBin) -and ((Get-Item $mapBin).Length -eq 7200)) 'Villa Papale map.bin is missing or has the wrong size.'
Assert-True ((Test-Path $borderBin) -and ((Get-Item $borderBin).Length -eq 8)) 'Villa Papale border.bin is missing or has the wrong size.'
Assert-True (@($groups.gMapGroup_TownsAndRoutes | Where-Object { $_ -eq 'VillaPapaleGiardini' }).Count -eq 1) 'Villa Papale map group registration is incorrect.'
Assert-True (@($borgo.connections | Where-Object { $_.map -eq 'MAP_VILLA_PAPALE_GIARDINI' -and $_.direction -eq 'up' -and [int]$_.offset -eq 0 }).Count -eq 1) 'Borgo to Villa connection is incorrect.'
Assert-True (@($villa.connections | Where-Object { $_.map -eq 'MAP_BORGO_DI_CASTELLO' -and $_.direction -eq 'down' -and [int]$_.offset -eq 0 }).Count -eq 1) 'Villa to Borgo connection is incorrect.'
Assert-True (@($villa.object_events).Count -eq 0 -and @($villa.warp_events).Count -eq 0 -and @($villa.coord_events).Count -eq 0 -and @($villa.bg_events).Count -eq 0) 'Villa scaffold must not have events yet.'
Assert-True ($eventScripts -match 'data/maps/VillaPapaleGiardini/scripts\.inc') 'Villa Papale scripts are not globally included.'
Assert-True ((Get-Content (Join-Path $RepositoryRoot 'data/maps/VillaPapaleGiardini/scripts.inc') -Raw) -match '^VillaPapaleGiardini_MapScripts::\r?\n\s*\.byte 0') 'Villa Papale MapScripts are not minimal.'
Assert-True ((Get-Content (Join-Path $RepositoryRoot 'src/data/wild_encounters.json') -Raw) -notmatch 'MAP_VILLA_PAPALE_GIARDINI') 'Villa scaffold must not have encounters.'
$blockdata = [IO.File]::ReadAllBytes($mapBin)
$generalAttributes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/primary/general/metatile_attributes.bin'))
$sootopolisAttributes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/secondary/sootopolis/metatile_attributes.bin'))
foreach ($x in 28..30) {
    Assert-True ((Get-Collision (Get-Block $blockdata $x 59)) -eq 0) "Villa south connection cell $x,59 is blocked."
}
Assert-True ((Get-Collision (Get-Block $blockdata 29 45)) -eq 0 -and (Get-Collision (Get-Block $blockdata 12 35)) -eq 0 -and (Get-Collision (Get-Block $blockdata 46 35)) -eq 0) 'Villa scaffold does not keep the south approach and garden areas open.'
foreach ($y in 13..59) { foreach ($x in 28..30) { Assert-True ((Get-Block $blockdata $x $y) -eq 0x30AF) "Villa avenue cell $x,$y is not the approved shared metatile." } }
foreach ($y in 6..12) { foreach ($x in 21..38) { Assert-True ((Get-Block $blockdata $x $y) -eq 0x30AF) "Villa forecourt cell $x,$y is not the approved shared metatile." } }
Assert-True ((Get-Block $blockdata 29 4) -eq 0x3248) 'Villa facade must retain the closed visual entrance at 29,4 without a warp.'
$grassCells = @()
for ($y = 0; $y -lt 60; $y++) {
    for ($x = 0; $x -lt 60; $x++) {
        $block = Get-Block $blockdata $x $y
        $metatileId = Get-MetatileId $block
        $attributes = if ($metatileId -lt 0x200) { $generalAttributes } else { $sootopolisAttributes }
        $attributesId = if ($metatileId -lt 0x200) { $metatileId } else { $metatileId - 0x200 }
        $behavior = Get-Behavior $attributes $attributesId
        Assert-True ($behavior -notin 0x10..0x19) "Villa scaffold must not retain donor water behavior at $x,$y."
        if ($behavior -eq 0x2) { $grassCells += "$x,$y" }
    }
}
$expectedGrass = @()
foreach ($rect in @(@(43,45,25,26), @(48,50,34,35), @(42,43,40,41))) {
    for ($y = $rect[2]; $y -le $rect[3]; $y++) { for ($x = $rect[0]; $x -le $rect[1]; $x++) { $expectedGrass += "$x,$y" } }
}
Assert-True ($grassCells.Count -eq $expectedGrass.Count -and @($grassCells | Where-Object { $_ -notin $expectedGrass }).Count -eq 0) 'Villa tall-grass patches do not match the approved three eastern garden patches.'
Assert-True (@(Get-ChildItem (Join-Path $RepositoryRoot 'data/maps') -Directory | Where-Object { $_.Name -like 'VillaPapale*' }).Count -eq 1) 'Villa scaffold must not add an interior map.'
Write-Output 'Villa Papale Giardini scaffold: PASS'
