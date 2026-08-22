param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'
function Assert-True([bool]$c,[string]$m) { if (-not $c) { throw $m } }
function Read-Json([string]$p) { Get-Content (Join-Path $RepositoryRoot $p) -Raw | ConvertFrom-Json }
$root = Join-Path $RepositoryRoot 'data/tilesets/secondary/lago_di_albera'
$porta = Join-Path $RepositoryRoot 'data/tilesets/secondary/porta_pretoria'
foreach ($file in 'tiles.png','metatiles.bin','metatile_attributes.bin') { Assert-True (Test-Path (Join-Path $root $file)) "Missing Lago tileset file: $file" }
Assert-True ((Get-ChildItem (Join-Path $root 'palettes') -Filter '*.pal').Count -eq 16) 'Lago tileset must contain sixteen palettes.'
$meta = [IO.File]::ReadAllBytes((Join-Path $root 'metatiles.bin'))
$attrs = [IO.File]::ReadAllBytes((Join-Path $root 'metatile_attributes.bin'))
$portaMeta = [IO.File]::ReadAllBytes((Join-Path $porta 'metatiles.bin'))
$portaAttrs = [IO.File]::ReadAllBytes((Join-Path $porta 'metatile_attributes.bin'))
Assert-True (($meta.Length / 16) -eq 338 -and ($attrs.Length / 2) -eq 338) 'Lago metatile or attribute count is incorrect.'
Assert-True ($meta.Length -le (512 * 16)) 'Lago exceeds the secondary metatile limit.'
Assert-True ($attrs.Length -eq (($meta.Length / 16) * 2)) 'Lago attribute count does not match metatile count.'
$metaPrefix = New-Object byte[] $portaMeta.Length
$attrsPrefix = New-Object byte[] $portaAttrs.Length
[Array]::Copy($meta, $metaPrefix, $portaMeta.Length)
[Array]::Copy($attrs, $attrsPrefix, $portaAttrs.Length)
Assert-True ([Convert]::ToBase64String($metaPrefix) -eq [Convert]::ToBase64String($portaMeta)) 'PortaPretoria metatiles were not preserved as a prefix.'
Assert-True ([Convert]::ToBase64String($attrsPrefix) -eq [Convert]::ToBase64String($portaAttrs)) 'PortaPretoria attributes were not preserved as a prefix.'
$pacifidlog = Join-Path $RepositoryRoot 'data/tilesets/secondary/pacifidlog'
$pacMeta = [IO.File]::ReadAllBytes((Join-Path $pacifidlog 'metatiles.bin'))
$pacAttrs = [IO.File]::ReadAllBytes((Join-Path $pacifidlog 'metatile_attributes.bin'))
$sourceMetatiles = @(0x010, 0x011, 0x012, 0x013, 0x014, 0x018, 0x019, 0x01A, 0x01B, 0x01C, 0x020, 0x021, 0x024, 0x028, 0x029, 0x02A, 0x02C, 0x030, 0x031, 0x032, 0x033)
$sourceTiles = @()
foreach ($sourceMetatile in $sourceMetatiles) {
    foreach ($entryIndex in 0..7) {
        $entry = [BitConverter]::ToUInt16($pacMeta, $sourceMetatile * 16 + $entryIndex * 2)
        $tile = $entry -band 0x3FF
        if ($tile -ge 512) { $sourceTiles += $tile - 512 }
    }
}
$sourceTiles = @($sourceTiles | Sort-Object -Unique)
Assert-True ($sourceTiles.Count -eq 44) 'Lago must import exactly forty-four Pacifidlog tiles.'
$tileMap = @{}
for ($i = 0; $i -lt $sourceTiles.Count; $i++) { $tileMap[$sourceTiles[$i]] = 405 + $i }
$paletteMap = @{ 0 = 0; 1 = 1; 4 = 4; 8 = 13; 9 = 15 }
for ($i = 0; $i -lt $sourceMetatiles.Count; $i++) {
    $sourceMetatile = $sourceMetatiles[$i]
    $destinationMetatile = 317 + $i
    Assert-True ([BitConverter]::ToUInt16($attrs, $destinationMetatile * 2) -eq [BitConverter]::ToUInt16($pacAttrs, $sourceMetatile * 2)) "Lago attribute mismatch for Pacifidlog metatile 0x$('{0:X3}' -f $sourceMetatile)."
    foreach ($entryIndex in 0..7) {
        $sourceEntry = [BitConverter]::ToUInt16($pacMeta, $sourceMetatile * 16 + $entryIndex * 2)
        $expectedEntry = $sourceEntry
        $sourceTile = $sourceEntry -band 0x3FF
        if ($sourceTile -ge 512) { $expectedEntry = ($expectedEntry -band 0xFC00) -bor (512 + $tileMap[$sourceTile - 512]) }
        $sourcePalette = ($expectedEntry -shr 12) -band 0xF
        Assert-True ($paletteMap.ContainsKey($sourcePalette)) "Unsupported Pacifidlog palette $sourcePalette."
        $expectedEntry = ($expectedEntry -band 0x0FFF) -bor ($paletteMap[$sourcePalette] -shl 12)
        $actualEntry = [BitConverter]::ToUInt16($meta, $destinationMetatile * 16 + $entryIndex * 2)
        Assert-True ($actualEntry -eq $expectedEntry) "Lago metatile 0x$('{0:X3}' -f $destinationMetatile) is not a valid Pacifidlog clone."
        $actualTile = $actualEntry -band 0x3FF
        Assert-True ($actualTile -lt 512 -or $actualTile -lt 961) "Lago metatile references a tile outside its declared capacity."
    }
}
$palettePairs = @(@(8, 13), @(9, 15))
foreach ($pair in $palettePairs) {
    $sourcePalettePath = Join-Path $pacifidlog ('palettes/{0:D2}.pal' -f $pair[0])
    $lagoPalettePath = Join-Path $root ('palettes/{0:D2}.pal' -f $pair[1])
    $sourcePaletteBytes = [IO.File]::ReadAllBytes($sourcePalettePath)
    $lagoPaletteBytes = [IO.File]::ReadAllBytes($lagoPalettePath)
    Assert-True ([Convert]::ToBase64String($sourcePaletteBytes) -eq [Convert]::ToBase64String($lagoPaletteBytes)) "Lago palette slot $($pair[1]) does not contain Pacifidlog palette $($pair[0])."
}
Add-Type -AssemblyName System.Drawing
$portaTiles = [System.Drawing.Bitmap]::FromFile((Join-Path $porta 'tiles.png'))
$pacifidlogTiles = [System.Drawing.Bitmap]::FromFile((Join-Path $pacifidlog 'tiles.png'))
$lagoTiles = [System.Drawing.Bitmap]::FromFile((Join-Path $root 'tiles.png'))
try {
    for ($tile = 0; $tile -lt 405; $tile++) {
        $tileX = ($tile % 16) * 8
        $tileY = [int][Math]::Floor($tile / 16) * 8
        foreach ($x in 0..7) { foreach ($y in 0..7) { Assert-True ($portaTiles.GetPixel($tileX + $x, $tileY + $y).ToArgb() -eq $lagoTiles.GetPixel($tileX + $x, $tileY + $y).ToArgb()) "PortaPretoria tile $tile was not preserved." } }
    }
    for ($tile = 0; $tile -lt $sourceTiles.Count; $tile++) {
        $sourceTile = $sourceTiles[$tile]
        $sourceX = ($sourceTile % 16) * 8
        $sourceY = [int][Math]::Floor($sourceTile / 16) * 8
        $destinationTile = 405 + $tile
        $destinationX = ($destinationTile % 16) * 8
        $destinationY = [int][Math]::Floor($destinationTile / 16) * 8
        foreach ($x in 0..7) { foreach ($y in 0..7) { Assert-True ($pacifidlogTiles.GetPixel($sourceX + $x, $sourceY + $y).ToArgb() -eq $lagoTiles.GetPixel($destinationX + $x, $destinationY + $y).ToArgb()) "Pacifidlog tile $sourceTile was not imported correctly." } }
    }
} finally {
    $portaTiles.Dispose()
    $pacifidlogTiles.Dispose()
    $lagoTiles.Dispose()
}
$graphics = Get-Content (Join-Path $RepositoryRoot 'src/data/tilesets/graphics.h') -Raw
$headers = Get-Content (Join-Path $RepositoryRoot 'src/data/tilesets/headers.h') -Raw
$includes = Get-Content (Join-Path $RepositoryRoot 'include/tilesets.h') -Raw
Assert-True (($graphics -split 'gTilesetTiles_LagoDiAlbera').Count -eq 2 -and $graphics.Contains('-num_tiles 449')) 'Lago graphics registration is incorrect.'
Assert-True (($headers -split 'gTileset_LagoDiAlbera').Count -eq 2 -and $headers.Contains('.callback = InitTilesetAnim_Petalburg')) 'Lago tileset header is incorrect.'
Assert-True (($includes -split 'gTileset_LagoDiAlbera').Count -eq 2) 'Lago tileset declaration is incorrect.'
$layout = @((Read-Json 'data/layouts/layouts.json').layouts | Where-Object id -eq 'LAYOUT_LAGO_DI_ALBERA')
$map = Read-Json 'data/maps/LagoDiAlbera/map.json'
Assert-True ($layout.Count -eq 1 -and $layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_LagoDiAlbera') 'Lago layout tilesets are incorrect.'
Assert-True ([int]$layout[0].width -eq 120 -and [int]$layout[0].height -eq 120 -and @($map.object_events).Count -eq 0 -and @($map.warp_events).Count -eq 0 -and @($map.coord_events).Count -eq 0 -and @($map.bg_events).Count -eq 0) 'Lago map structure changed.'
Assert-True ($map.connections.Count -eq 1 -and $map.connections[0].direction -eq 'down' -and $map.connections[0].map -eq 'MAP_VIA_CONSOLARE' -and [int]$map.connections[0].offset -eq 31) 'Lago connection changed.'
git -C $RepositoryRoot diff --quiet develop -- data/layouts/LagoDiAlbera/map.bin data/maps/PacifidlogTown data/layouts/PacifidlogTown data/tilesets/secondary/pacifidlog data/tilesets/secondary/porta_pretoria data/tilesets/primary/general data/maps/ViaConsolare data/layouts/ViaConsolare
Assert-True ($LASTEXITCODE -eq 0) 'A source tileset or protected map changed.'
Assert-True (-not (Test-Path (Join-Path $root 'anim'))) 'Lago must not import Pacifidlog animations.'
$wild = Read-Json 'src/data/wild_encounters.json'
Assert-True (@($wild.wild_encounter_groups.encounters | Where-Object map -eq 'MAP_LAGO_DI_ALBERA').Count -eq 0) 'Lago must not have encounters.'
Write-Output 'Lago di Albera tileset validation: PASS'
