param(
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

function Read-Json([string]$RelativePath) {
    return [IO.File]::ReadAllText((Join-Path $RepositoryRoot $RelativePath), [Text.Encoding]::UTF8) | ConvertFrom-Json
}

function Get-BaseJson([string]$RelativePath) {
    return ((& git -C $RepositoryRoot show "develop:$RelativePath") -join "`n") | ConvertFrom-Json
}

function Get-MapCell([byte[]]$MapBin, [int]$X, [int]$Y) {
    return [BitConverter]::ToUInt16($MapBin, 2 * (($Y * 20) + $X))
}

$palettePaths = 0..15 | ForEach-Object { "data/tilesets/secondary/porta_pretoria/palettes/{0:D2}.pal" -f $_ }
$allowedPaths = @(
    'data/layouts/OldaleTown/map.bin',
    'data/layouts/layouts.json',
    'data/tilesets/secondary/porta_pretoria/tiles.png',
    'data/tilesets/secondary/porta_pretoria/metatiles.bin',
    'data/tilesets/secondary/porta_pretoria/metatile_attributes.bin',
    'include/tilesets.h',
    'src/data/tilesets/graphics.h',
    'src/data/tilesets/headers.h',
    'src/data/tilesets/metatiles.h',
    'src/field_door.c',
    'test/validate_porta_pretoria_dedicated_tileset.ps1',
    'test/validate_porta_pretoria_localization.ps1'
) + $palettePaths

$changedPaths = @(& git -C $RepositoryRoot diff --name-only develop)
$changedPaths += @(& git -C $RepositoryRoot ls-files --others --exclude-standard)
foreach ($changedPath in $changedPaths) {
    Assert-True ($allowedPaths -contains $changedPath) "Unexpected file changed by Porta Pretoria dedicated tileset batch: $changedPath"
}

$layouts = Read-Json 'data/layouts/layouts.json'
$oldaleLayout = @($layouts.layouts | Where-Object id -eq 'LAYOUT_OLDALE_TOWN')
Assert-True ($oldaleLayout.Count -eq 1) 'LAYOUT_OLDALE_TOWN is missing or duplicated.'
Assert-True ($oldaleLayout[0].primary_tileset -eq 'gTileset_General') 'OldaleTown primary tileset changed unexpectedly.'
Assert-True ($oldaleLayout[0].secondary_tileset -eq 'gTileset_PortaPretoria') 'OldaleTown must use the dedicated Porta Pretoria secondary tileset.'
Assert-True ($oldaleLayout[0].width -eq 20 -and $oldaleLayout[0].height -eq 20) 'OldaleTown dimensions changed unexpectedly.'

foreach ($path in @('data/tilesets/primary/general', 'data/tilesets/secondary/petalburg', 'data/tilesets/secondary/rustboro')) {
    & git -C $RepositoryRoot diff --quiet develop -- $path
    Assert-True ($LASTEXITCODE -eq 0) "Shared tileset changed unexpectedly: $path"
}

foreach ($relativePath in @('tiles.png', 'metatiles.bin', 'metatile_attributes.bin')) {
    Assert-True (Test-Path (Join-Path $RepositoryRoot "data/tilesets/secondary/porta_pretoria/$relativePath")) "Dedicated tileset asset missing: $relativePath"
}
foreach ($relativePath in $palettePaths) {
    Assert-True (Test-Path (Join-Path $RepositoryRoot $relativePath)) "Dedicated tileset palette missing: $relativePath"
}
$sourceTilesHash = (Get-FileHash (Join-Path $RepositoryRoot 'data/tilesets/secondary/petalburg/tiles.png') -Algorithm SHA256).Hash
$dedicatedTilesHash = (Get-FileHash (Join-Path $RepositoryRoot 'data/tilesets/secondary/porta_pretoria/tiles.png') -Algorithm SHA256).Hash
Assert-True ($sourceTilesHash -ne $dedicatedTilesHash) 'Dedicated Porta Pretoria tile graphics must differ from shared Petalburg graphics.'
$sharedMetatiles = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/secondary/petalburg/metatiles.bin'))
$dedicatedMetatiles = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/secondary/porta_pretoria/metatiles.bin'))
$sharedAttributes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/secondary/petalburg/metatile_attributes.bin'))
$dedicatedAttributes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/secondary/porta_pretoria/metatile_attributes.bin'))
Assert-True ($dedicatedMetatiles.Length -eq ($sharedMetatiles.Length + (69 * 16))) 'Dedicated Porta Pretoria metatile count must add exactly 69 append-only entries.'
Assert-True ($dedicatedAttributes.Length -eq ($sharedAttributes.Length + (69 * 2))) 'Dedicated Porta Pretoria attribute count must add exactly 69 append-only entries.'
for ($index = 0; $index -lt $sharedMetatiles.Length; $index++) {
    Assert-True ($dedicatedMetatiles[$index] -eq $sharedMetatiles[$index]) 'Existing Petalburg-compatible metatile data changed unexpectedly.'
}
$expectedAttributes = @{
    0x298 = 0x1000; 0x299 = 0x1000; 0x29A = 0x1000
    0x29B = 0x0000; 0x29C = 0x0000; 0x29D = 0x0000
    0x29E = 0x1000; 0x29F = 0x1000; 0x2A0 = 0x1000; 0x2A1 = 0x1000
    0x2A2 = 0x0000; 0x2A3 = 0x0000; 0x2A4 = 0x0000; 0x2A5 = 0x0000
    0x2A6 = 0x0000; 0x2A7 = 0x0000; 0x2A8 = 0x0000; 0x2A9 = 0x0000
    0x2AA = 0x0000; 0x2AB = 0x0000; 0x2AC = 0x1000; 0x2AD = 0x0000
    0x2AE = 0x1000; 0x2AF = 0x0000; 0x2B0 = 0x0000; 0x2B1 = 0x0000
    0x2B2 = 0x0000; 0x2B3 = 0x0000; 0x2B4 = 0x0000; 0x2B5 = 0x0000
    0x2B6 = 0x0000; 0x2B7 = 0x0000; 0x2B8 = 0x0000; 0x2B9 = 0x0000
    0x2BA = 0x1000; 0x2BB = 0x1000; 0x2BC = 0x1000; 0x2BD = 0x0000
    0x2BE = 0x0000
}
foreach ($id in $expectedAttributes.Keys) {
    $attribute = [BitConverter]::ToUInt16($dedicatedAttributes, 2 * ($id - 0x200))
    Assert-True ($attribute -eq $expectedAttributes[$id]) ("Unexpected behavior/elevation attribute for Porta Pretoria metatile 0x{0:X3}." -f $id)
}
foreach ($id in 0x298..0x2B1) {
    $containsDedicatedGraphic = $false
    for ($word = 0; $word -lt 8; $word++) {
        $tileId = [BitConverter]::ToUInt16($dedicatedMetatiles, ((($id - 0x200) * 16) + ($word * 2))) -band 0x03FF
        if ($tileId -ge 704 -and $tileId -le 721) { $containsDedicatedGraphic = $true }
    }
    Assert-True $containsDedicatedGraphic ("Porta Pretoria metatile 0x{0:X3} must reference a dedicated 8x8 graphic." -f $id)
}
foreach ($id in 0x2B2..0x2BE) {
    for ($word = 0; $word -lt 8; $word++) {
        $tileId = [BitConverter]::ToUInt16($dedicatedMetatiles, ((($id - 0x200) * 16) + ($word * 2))) -band 0x03FF
        Assert-True ($tileId -ge 0x334 -and $tileId -le 0x353) ("Rustboro donor metatile 0x{0:X3} must use only cloned Porta Pretoria tile IDs." -f $id)
    }
}
foreach ($id in 0x2BF..0x2DC) {
    $attribute = [BitConverter]::ToUInt16($dedicatedAttributes, 2 * ($id - 0x200))
    Assert-True ($attribute -eq 0x0000) ("Unexpected behavior/layer attribute for donor metatile 0x{0:X3}." -f $id)
    for ($word = 0; $word -lt 8; $word++) {
        $tileId = [BitConverter]::ToUInt16($dedicatedMetatiles, ((($id - 0x200) * 16) + ($word * 2))) -band 0x03FF
        Assert-True ($tileId -eq 0 -or ($tileId -ge 0x334 -and $tileId -le 0x385)) ("Completed donor metatile 0x{0:X3} uses an unexpected tile ID." -f $id)
    }
}

$graphics = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/tilesets/graphics.h') -Raw
$headers = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/tilesets/headers.h') -Raw
$metatiles = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/tilesets/metatiles.h') -Raw
$doors = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/field_door.c') -Raw
Assert-True ($graphics.Contains('gTilesetTiles_PortaPretoria')) 'Dedicated tileset graphics are not registered.'
Assert-True ($headers.Contains('const struct Tileset gTileset_PortaPretoria')) 'Dedicated tileset header is not registered.'
Assert-True ($metatiles.Contains('gMetatiles_PortaPretoria')) 'Dedicated metatiles are not registered.'
Assert-True ($doors.Contains('{METATILE_Petalburg_Door_Oldale,                        &gTileset_PortaPretoria')) 'Oldale house door animation is not registered for the dedicated tileset.'
Assert-True ($graphics.Contains('gTilesetTiles_PortaPretoria[] = INCGFX_U32("data/tilesets/secondary/porta_pretoria/tiles.png", ".4bpp.fastSmol", "-num_tiles 390 -Wnum_tiles")')) 'Porta Pretoria tile capacity must load 390 tiles.'

$maximumReferencedTileId = 0
for ($offset = 0; $offset -lt $dedicatedMetatiles.Length; $offset += 2) {
    $tileId = [BitConverter]::ToUInt16($dedicatedMetatiles, $offset) -band 0x03FF
    if ($tileId -gt $maximumReferencedTileId) { $maximumReferencedTileId = $tileId }
}
Assert-True ($maximumReferencedTileId -eq 0x385) 'Unexpected highest Porta Pretoria tile reference.'
Assert-True ($maximumReferencedTileId -le (0x200 + 390 - 1)) 'Porta Pretoria references a tile beyond its declared capacity.'

Add-Type -AssemblyName System.Drawing
$tileImage = [Drawing.Image]::FromFile((Join-Path $RepositoryRoot 'data/tilesets/secondary/porta_pretoria/tiles.png'))
Assert-True ($tileImage.Width -eq 128 -and $tileImage.Height -eq 200) 'Porta Pretoria tile image dimensions must provide the donor tile capacity.'
$tileImage.Dispose()
foreach ($pair in @(@('12.pal', '10.pal'), @('13.pal', '11.pal'), @('14.pal', '08.pal'))) {
    $actualPalette = Get-Content -LiteralPath (Join-Path $RepositoryRoot "data/tilesets/secondary/porta_pretoria/palettes/$($pair[0])")
    $sourcePalette = Get-Content -LiteralPath (Join-Path $RepositoryRoot "data/tilesets/secondary/rustboro/palettes/$($pair[1])")
    Assert-True ($actualPalette.Count -eq $sourcePalette.Count) "Rustboro donor palette $($pair[1]) length differs from Porta Pretoria slot $($pair[0])."
    for ($index = 0; $index -lt $actualPalette.Count; $index++) {
        Assert-True ($actualPalette[$index] -eq $sourcePalette[$index]) "Rustboro donor palette $($pair[1]) was not cloned exactly to Porta Pretoria slot $($pair[0])."
    }
}

$oldale = Read-Json 'data/maps/OldaleTown/map.json'
$baseOldale = Get-BaseJson 'data/maps/OldaleTown/map.json'
foreach ($property in @('object_events', 'warp_events', 'coord_events', 'bg_events', 'connections')) {
    $actual = ($oldale.$property | ConvertTo-Json -Depth 20 -Compress)
    $expected = ($baseOldale.$property | ConvertTo-Json -Depth 20 -Compress)
    Assert-True ($actual -eq $expected) "OldaleTown $property changed unexpectedly."
}

$mapBin = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/OldaleTown/map.bin'))
Assert-True ($mapBin.Length -eq 800) 'OldaleTown map.bin size changed unexpectedly.'
& git -C $RepositoryRoot diff --quiet HEAD -- data/layouts/OldaleTown/map.bin
Assert-True ($LASTEXITCODE -eq 0) 'OldaleTown map.bin must not change during the donor-kit pass.'
$expectedDedicatedPlacements = [ordered]@{
    '4,6' = 0x2A9; '6,6' = 0x2AA; '7,6' = 0x2AB; '4,7' = 0x298; '6,7' = 0x299; '7,7' = 0x29A
    '14,15' = 0x2A9; '16,15' = 0x2AA; '17,15' = 0x2AB; '14,16' = 0x298; '16,16' = 0x299; '17,16' = 0x29A
    '5,15' = 0x2AC; '5,16' = 0x29E; '7,16' = 0x29F; '13,5' = 0x2AE; '13,6' = 0x2A0; '15,6' = 0x2A1; '16,6' = 0x2A1
}
foreach ($placement in $expectedDedicatedPlacements.GetEnumerator()) {
    $coordinate = $placement.Key.Split(',')
    $x = [int]$coordinate[0]
    $y = [int]$coordinate[1]
    $cell = Get-MapCell $mapBin $x $y
    Assert-True (($cell -band 0x03FF) -eq $placement.Value) "Dedicated facade metatile missing at $($placement.Key)."
}
$newMetatileCells = 0
for ($y = 0; $y -lt 20; $y++) {
    for ($x = 0; $x -lt 20; $x++) {
        $id = (Get-MapCell $mapBin $x $y) -band 0x03FF
        if ($id -ge 0x298 -and $id -le 0x2B1) { $newMetatileCells++ }
    }
}
Assert-True ($newMetatileCells -ge 90) 'OldaleTown must use the new Porta Pretoria metatiles extensively.'
$polishedCells = @(
    @(9,8), @(10,8), @(9,9), @(10,9), @(9,10), @(10,10), @(11,10),
    @(2,12), @(3,12), @(4,12), @(15,12), @(16,12), @(17,12),
    @(2,13), @(3,13), @(4,13), @(13,14), @(13,15), @(13,16),
    @(4,17), @(5,17), @(6,17), @(12,17), @(13,17), @(14,17), @(15,17),
    @(2,4), @(3,4), @(17,4), @(17,5), @(2,14), @(3,14),
    @(2,5), @(2,8), @(3,8), @(4,8), @(19,8), @(2,9), @(19,9),
    @(1,10), @(18,10), @(19,10), @(4,14), @(4,15), @(4,16)
)
foreach ($coordinate in $polishedCells) {
    $cell = Get-MapCell $mapBin $coordinate[0] $coordinate[1]
    Assert-True (($cell -band 0xFC00) -eq 0x3000) "Collision or elevation changed at polished cell ($($coordinate[0]),$($coordinate[1]))."
}

Write-Output 'Porta Pretoria dedicated tileset validation passed.'
