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
$layouts = Read-Json 'data/layouts/layouts.json'
$oldaleLayout = @($layouts.layouts | Where-Object id -eq 'LAYOUT_OLDALE_TOWN')
Assert-True ($oldaleLayout.Count -eq 1) 'LAYOUT_OLDALE_TOWN is missing or duplicated.'
Assert-True ($oldaleLayout[0].primary_tileset -eq 'gTileset_General') 'OldaleTown primary tileset changed unexpectedly.'
Assert-True ($oldaleLayout[0].secondary_tileset -eq 'gTileset_PortaPretoria') 'OldaleTown must use the dedicated Porta Pretoria secondary tileset.'
Assert-True ($oldaleLayout[0].width -eq 20 -and $oldaleLayout[0].height -eq 20) 'OldaleTown dimensions changed unexpectedly.'

# Connected-map blocks are rendered with the active map layout's tilesets.
# Every map directly connected to OldaleTown must therefore share its compatible
# secondary tileset; the Porta Pretoria prefix is byte-identical to Petalburg.
foreach ($layoutId in @('LAYOUT_ROUTE101', 'LAYOUT_ROUTE103', 'LAYOUT_ALBERA_STORICA')) {
    $layout = @($layouts.layouts | Where-Object id -eq $layoutId)
    Assert-True ($layout.Count -eq 1) "$layoutId is missing or duplicated."
    Assert-True ($layout[0].primary_tileset -eq 'gTileset_General') "$layoutId primary tileset changed unexpectedly."
    Assert-True ($layout[0].secondary_tileset -eq 'gTileset_PortaPretoria') "$layoutId must share Porta Pretoria's compatible secondary tileset for seamless connections."
}

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
Assert-True ($dedicatedMetatiles.Length -eq ($sharedMetatiles.Length + (165 * 16))) 'Dedicated Porta Pretoria metatile count must add exactly 165 append-only entries.'
Assert-True ($dedicatedAttributes.Length -eq ($sharedAttributes.Length + (165 * 2))) 'Dedicated Porta Pretoria attribute count must add exactly 165 append-only entries.'
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
foreach ($id in 0x2DD..0x33C) {
    $attribute = [BitConverter]::ToUInt16($dedicatedAttributes, 2 * ($id - 0x200))
    Assert-True ($attribute -eq 0x0000) ("Complete building donor metatile 0x{0:X3} must not have door, animation, or special behavior." -f $id)
    for ($word = 0; $word -lt 8; $word++) {
        $tileId = [BitConverter]::ToUInt16($dedicatedMetatiles, ((($id - 0x200) * 16) + ($word * 2))) -band 0x03FF
        Assert-True ($tileId -eq 0 -or $tileId -lt 0x200 -or ($tileId -ge 0x334 -and $tileId -le 0x394)) ("Complete building donor metatile 0x{0:X3} uses an unexpected tile ID." -f $id)
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
Assert-True ($graphics.Contains('gTilesetTiles_PortaPretoria[] = INCGFX_U32("data/tilesets/secondary/porta_pretoria/tiles.png", ".4bpp.fastSmol", "-num_tiles 405 -Wnum_tiles")')) 'Porta Pretoria tile capacity must load 405 tiles.'

$maximumReferencedTileId = 0
for ($offset = 0; $offset -lt $dedicatedMetatiles.Length; $offset += 2) {
    $tileId = [BitConverter]::ToUInt16($dedicatedMetatiles, $offset) -band 0x03FF
    if ($tileId -gt $maximumReferencedTileId) { $maximumReferencedTileId = $tileId }
}
Assert-True ($maximumReferencedTileId -eq 0x394) 'Unexpected highest Porta Pretoria tile reference.'
Assert-True ($maximumReferencedTileId -le (0x200 + 405 - 1)) 'Porta Pretoria references a tile beyond its declared capacity.'

# General owns palettes 0-5 and the engine loads a secondary tileset only into
# palettes 6-12. The appended Porta Pretoria metatiles must not depend on
# transient, unloaded BG palette slots during camera transitions.
for ($offset = $sharedMetatiles.Length; $offset -lt $dedicatedMetatiles.Length; $offset += 2) {
    $paletteId = ([BitConverter]::ToUInt16($dedicatedMetatiles, $offset) -shr 12) -band 0xF
    Assert-True ($paletteId -le 12) 'Porta Pretoria appended metatile references an unloaded BG palette slot.'
}

Add-Type -AssemblyName System.Drawing
$tileImage = [Drawing.Image]::FromFile((Join-Path $RepositoryRoot 'data/tilesets/secondary/porta_pretoria/tiles.png'))
Assert-True ($tileImage.Width -eq 128 -and $tileImage.Height -eq 208) 'Porta Pretoria tile image dimensions must provide the donor tile capacity.'
$tileImage.Dispose()
foreach ($pair in @(@('07.pal', '11.pal'), @('11.pal', '08.pal'), @('12.pal', '10.pal'))) {
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
$donorMetatileCells = 0
for ($y = 0; $y -lt 20; $y++) {
    for ($x = 0; $x -lt 20; $x++) {
        $id = (Get-MapCell $mapBin $x $y) -band 0x03FF
        if ($id -ge 0x200) {
            Assert-True ($id -le 0x394) "OldaleTown references a Porta Pretoria metatile outside the declared tile capacity at ($x,$y)."
        }
        if ($id -ge 0x2B2 -and $id -le 0x33C) { $donorMetatileCells++ }
        if ($id -ge 0x200) {
            $metatileOffset = 16 * ($id - 0x200)
            for ($word = 0; $word -lt 8; $word++) {
                $paletteId = ([BitConverter]::ToUInt16($dedicatedMetatiles, $metatileOffset + (2 * $word)) -shr 12) -band 0xF
                Assert-True ($paletteId -le 12) "OldaleTown references an unloaded BG palette slot at ($x,$y)."
            }
        }
    }
}
Assert-True ($donorMetatileCells -gt 0) 'OldaleTown must use the imported Porta Pretoria donor metatiles.'

foreach ($coordinate in @(@(5, 7), @(15, 16), @(6, 16), @(14, 6))) {
    $cell = Get-MapCell $mapBin $coordinate[0] $coordinate[1]
    Assert-True ((($cell -shr 10) -band 3) -eq 1) "Building entrance collision changed at ($($coordinate[0]),$($coordinate[1]))."
}

foreach ($coordinate in @(@(8, 0), @(9, 0), @(10, 0), @(11, 0), @(8, 19), @(9, 19), @(10, 19), @(11, 19))) {
    $cell = Get-MapCell $mapBin $coordinate[0] $coordinate[1]
    Assert-True ((($cell -shr 10) -band 3) -eq 0 -and (($cell -shr 12) -band 0xF) -eq 3) "Connection or checkpoint access changed at ($($coordinate[0]),$($coordinate[1]))."
}

Write-Output 'Porta Pretoria dedicated tileset validation passed.'
