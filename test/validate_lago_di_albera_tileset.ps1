param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'
function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Read-Bytes([string]$path) { [IO.File]::ReadAllBytes($path) }
function Read-Palette([string]$path) { @((Get-Content -LiteralPath $path | Select-Object -Skip 3) | ForEach-Object { $_.Trim() }) }
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
        if ($process.ExitCode -ne 0) { throw "Unable to read Git blob $spec." }
        return [IO.File]::ReadAllBytes($temp)
    } finally { Remove-Item -LiteralPath $temp -Force -ErrorAction SilentlyContinue }
}

$lago = Join-Path $RepositoryRoot 'data/tilesets/secondary/lago_di_albera'
$pacifidlog = Join-Path $RepositoryRoot 'data/tilesets/secondary/pacifidlog'
$porta = Join-Path $RepositoryRoot 'data/tilesets/secondary/porta_pretoria'
$via = Join-Path $RepositoryRoot 'data/tilesets/secondary/via_consolare'
$layouts = (Read-Json 'data/layouts/layouts.json').layouts
$layout = @($layouts | Where-Object id -eq 'LAYOUT_LAGO_DI_ALBERA')
$viaLayout = @($layouts | Where-Object id -eq 'LAYOUT_VIA_CONSOLARE')
$map = Read-Json 'data/maps/LagoDiAlbera/map.json'
$graphics = Get-Content (Join-Path $RepositoryRoot 'src/data/tilesets/graphics.h') -Raw
$metatilesHeader = Get-Content (Join-Path $RepositoryRoot 'src/data/tilesets/metatiles.h') -Raw
$headers = Get-Content (Join-Path $RepositoryRoot 'src/data/tilesets/headers.h') -Raw
$includes = Get-Content (Join-Path $RepositoryRoot 'include/tilesets.h') -Raw

Assert-True ($layout.Count -eq 1 -and $layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_LagoDiAlbera') 'Lago must use General + LagoDiAlbera.'
Assert-True (Test-Path -LiteralPath $lago) 'Lago clone directory is missing.'
Assert-True ($graphics.Contains('gTilesetTiles_LagoDiAlbera') -and $graphics.Contains('-num_tiles 512') -and $metatilesHeader.Contains('gMetatiles_LagoDiAlbera') -and $headers.Contains('gTileset_LagoDiAlbera') -and $headers.Contains('.callback = InitTilesetAnim_Pacifidlog') -and $includes.Contains('gTileset_LagoDiAlbera')) 'Lago clone registration is incomplete.'
Assert-True ($viaLayout.Count -eq 1 -and $viaLayout[0].primary_tileset -eq 'gTileset_General' -and $viaLayout[0].secondary_tileset -eq 'gTileset_ViaConsolare') 'Via must use its connection-compatible secondary tileset.'
Assert-True (Test-Path -LiteralPath $via) 'Via connection-compatible tileset directory is missing.'
Assert-True ($graphics.Contains('gTilesetTiles_ViaConsolare[] = INCGFX_U32("data/tilesets/secondary/via_consolare/tiles.png", ".4bpp.fastSmol", "-num_tiles 409 -Wnum_tiles")') -and $metatilesHeader.Contains('gMetatiles_ViaConsolare') -and $headers.Contains('const struct Tileset gTileset_ViaConsolare') -and $includes.Contains('gTileset_ViaConsolare')) 'Via connection-compatible tileset registration is incomplete.'
Assert-True ([int]$layout[0].width -eq 120 -and [int]$layout[0].height -eq 120) 'Lago dimensions changed.'
Assert-True (@($map.object_events).Count -eq 15 -and @($map.warp_events).Count -eq 5 -and @($map.coord_events).Count -eq 7 -and @($map.bg_events).Count -eq 8) 'Lago event counts are incorrect.'
Assert-True (@($map.warp_events | Where-Object { [int]$_.x -eq 81 -and [int]$_.y -eq 3 -and [int]$_.elevation -eq 3 -and $_.dest_map -eq 'MAP_EMISSARIO' -and [int]$_.dest_warp_id -eq 0 }).Count -eq 1) 'Lago Emissario entrance warp is incorrect.'
Assert-True (@($map.connections | Where-Object { $_.direction -eq 'down' -and $_.map -eq 'MAP_VIA_CONSOLARE' -and [int]$_.offset -eq 31 }).Count -eq 1) 'Lago south connection changed.'
$wild = Read-Json 'src/data/wild_encounters.json'
Assert-True (@($wild.wild_encounter_groups.encounters | Where-Object map -eq 'MAP_LAGO_DI_ALBERA').Count -eq 4) 'Lago must retain four time-of-day encounter tables.'

$mapBin = Join-Path $RepositoryRoot 'data/layouts/LagoDiAlbera/map.bin'
Assert-True ((Get-Item -LiteralPath $mapBin).Length -eq 120 * 120 * 2) 'Lago map.bin must be 120x120.'
$lagoCurrent = Read-Bytes $mapBin
git -C $RepositoryRoot diff --quiet develop -- data/tilesets/primary/general data/tilesets/secondary/pacifidlog data/tilesets/secondary/porta_pretoria data/maps/PacifidlogTown data/layouts/PacifidlogTown
Assert-True ($LASTEXITCODE -eq 0) 'A protected map or source tileset changed.'
$viaExpected = @{}
$viaBase = [byte[]](Read-GitBlob 'develop:data/layouts/ViaConsolare/map.bin')
$viaCurrent = Read-Bytes (Join-Path $RepositoryRoot 'data/layouts/ViaConsolare/map.bin')
$viaDeltaCount = 0
for ($cell = 0; $cell -lt $viaBase.Length / 2; $cell++) {
    $baseRaw = [BitConverter]::ToUInt16($viaBase, $cell * 2)
    $currentRaw = [BitConverter]::ToUInt16($viaCurrent, $cell * 2)
    if ($baseRaw -ne $currentRaw) {
        $key = "{0},{1}" -f ($cell % 60), [int][Math]::Floor($cell / 60)
        Assert-True $viaExpected.ContainsKey($key) "Unexpected ViaConsolare delta at ($key)."
        Assert-True ($currentRaw -eq $viaExpected[$key]) "Unexpected ViaConsolare raw value at ($key)."
        $viaDeltaCount++
    }
}
Assert-True ($viaDeltaCount -eq $viaExpected.Count) 'ViaConsolare has an unexpected number of manual connection deltas.'
$connectionSecondaryIds = @{}
foreach ($playerX in 27..30) {
    foreach ($y in 0..5) {
        foreach ($x in ($playerX - 7)..($playerX + 7)) {
            $raw = [BitConverter]::ToUInt16($viaCurrent, (($y * 60) + $x) * 2)
            $metatile = $raw -band 0x3FF
            if ($metatile -ge 0x200) {
                Assert-True ($metatile -in 0x296, 0x2C9) "Unexpected secondary metatile 0x$('{0:X3}' -f $metatile) in the preserved Via-Lago transition view."
                $connectionSecondaryIds[$metatile] = $true
            }
        }
    }
}
Assert-True ($connectionSecondaryIds.Count -eq 2 -and $connectionSecondaryIds.ContainsKey(0x296) -and $connectionSecondaryIds.ContainsKey(0x2C9)) 'Via-Lago transition footprint changed.'

$pacMeta = Read-Bytes (Join-Path $pacifidlog 'metatiles.bin')
$pacAttrs = Read-Bytes (Join-Path $pacifidlog 'metatile_attributes.bin')
$lagoMeta = Read-Bytes (Join-Path $lago 'metatiles.bin')
$lagoAttrs = Read-Bytes (Join-Path $lago 'metatile_attributes.bin')
$portaMeta = Read-Bytes (Join-Path $porta 'metatiles.bin')
$portaAttrs = Read-Bytes (Join-Path $porta 'metatile_attributes.bin')
$viaMeta = Read-Bytes (Join-Path $via 'metatiles.bin')
$viaAttrs = Read-Bytes (Join-Path $via 'metatile_attributes.bin')
Assert-True ($pacMeta.Length -eq $lagoMeta.Length -and $pacAttrs.Length -eq $lagoAttrs.Length -and $lagoMeta.Length / 16 -eq 203) 'Lago clone metatile capacity differs from Pacifidlog.'
Assert-True ($viaMeta.Length -eq $portaMeta.Length -and $viaAttrs.Length -eq $portaAttrs.Length -and $viaMeta.Length / 16 -eq 317) 'Via clone capacity differs from PortaPretoria.'
Assert-True ([Convert]::ToBase64String($viaAttrs) -eq [Convert]::ToBase64String($portaAttrs)) 'Via clone must preserve every PortaPretoria metatile attribute.'
for ($entry = 0; $entry -lt $lagoMeta.Length / 2; $entry++) {
    $palette = ([BitConverter]::ToUInt16($lagoMeta, $entry * 2) -shr 12) -band 0xF
    Assert-True ($palette -lt 13) "Lago metatile entry $entry references unloaded palette $palette."
}
$patchedMetatiles = @(0x096, 0x0C9)
$patchedAttributes = @(0x096, 0x099, 0x0C9)
for ($index = 0; $index -lt 203; $index++) {
    if ($patchedMetatiles -notcontains $index) {
        $source = New-Object byte[] 16; $clone = New-Object byte[] 16
        [Array]::Copy($pacMeta, $index * 16, $source, 0, 16); [Array]::Copy($lagoMeta, $index * 16, $clone, 0, 16)
        Assert-True ([Convert]::ToBase64String($source) -eq [Convert]::ToBase64String($clone)) "Unexpected Lago metatile change at 0x$('{0:X3}' -f $index)."
    }
    if ($patchedAttributes -notcontains $index) {
        Assert-True ([BitConverter]::ToUInt16($pacAttrs, $index * 2) -eq [BitConverter]::ToUInt16($lagoAttrs, $index * 2)) "Unexpected Lago attribute change at 0x$('{0:X3}' -f $index)."
    }
}
Assert-True ([BitConverter]::ToUInt16($lagoAttrs, 0x099 * 2) -eq 0x0065) 'Lago metatile 0x299 must use MB_SOUTH_ARROW_WARP on layer 0.'
$tileMap = @{ 184 = 384; 185 = 385; 186 = 386; 187 = 387; 315 = 388; 317 = 389; 340 = 390; 341 = 391; 342 = 392 }
$viaRelocatedTiles = @{ 384 = 405; 385 = 406; 388 = 407; 389 = 408 }
$viaCompatibilityMetatiles = @{ 0x096 = 0x096; 0x0C9 = 0x0C9; 0x102 = 0x0C9 }
foreach ($index in 0..316) {
    foreach ($entryIndex in 0..7) {
        $actual = [BitConverter]::ToUInt16($viaMeta, $index * 16 + $entryIndex * 2)
        if ($viaCompatibilityMetatiles.ContainsKey($index)) {
            $expected = [BitConverter]::ToUInt16($lagoMeta, $viaCompatibilityMetatiles[$index] * 16 + $entryIndex * 2)
        } else {
            $expected = [BitConverter]::ToUInt16($portaMeta, $index * 16 + $entryIndex * 2)
            $sourceTile = $expected -band 0x3FF
            if ($sourceTile -ge 512 -and $viaRelocatedTiles.ContainsKey($sourceTile - 512)) {
                $expected = ($expected -band 0xFC00) -bor (512 + $viaRelocatedTiles[$sourceTile - 512])
            }
        }
        Assert-True ($actual -eq $expected) "Unexpected Via compatibility metatile change at 0x$('{0:X3}' -f $index), entry $entryIndex."
    }
}
foreach ($index in 0x096, 0x0C9) {
    $viaBlock = New-Object byte[] 16; $lagoBlock = New-Object byte[] 16
    [Array]::Copy($viaMeta, $index * 16, $viaBlock, 0, 16); [Array]::Copy($lagoMeta, $index * 16, $lagoBlock, 0, 16)
    Assert-True ([Convert]::ToBase64String($viaBlock) -eq [Convert]::ToBase64String($lagoBlock)) "Via/Lago transition metatile 0x$('{0:X3}' -f ($index + 0x200)) is not slot-identical."
}
$portaWall = New-Object byte[] 16; $portaWallAlias = New-Object byte[] 16
[Array]::Copy($portaMeta, 0x0C9 * 16, $portaWall, 0, 16); [Array]::Copy($portaMeta, 0x102 * 16, $portaWallAlias, 0, 16)
Assert-True ([Convert]::ToBase64String($portaWall) -eq [Convert]::ToBase64String($portaWallAlias)) 'PortaPretoria wall aliases unexpectedly differ.'
$fieldDoor = Get-Content (Join-Path $RepositoryRoot 'src/field_door.c') -Raw
Assert-True ($fieldDoor.Contains('#define DOOR_TILE_START_SIZE2 (NUM_TILES_TOTAL - 16)')) 'Door animation reserved tile range changed.'
Assert-True ($fieldDoor.Contains('{METATILE_Petalburg_Door_Oldale,                        &gTileset_ViaConsolare')) 'Via must retain the normal Oldale door animation.'
Assert-True (@($tileMap.Values | Where-Object { $_ -ge 496 }).Count -eq 0) 'Imported Lago tiles must not overlap door animation slots 496-511.'
Assert-True (@($tileMap.Values | Where-Object { ($_ -ge 464 -and $_ -le 493) -or ($_ -ge 496 -and $_ -le 503) }).Count -eq 0) 'Imported Lago tiles must not overlap Pacifidlog animation slots.'
$paletteMap = @{ 2 = 2; 6 = 11; 7 = 12; 11 = 12 }
foreach ($index in $patchedMetatiles) {
    Assert-True ([BitConverter]::ToUInt16($lagoAttrs, $index * 2) -eq [BitConverter]::ToUInt16($portaAttrs, $index * 2)) "Lago attribute mismatch for PortaPretoria metatile 0x$('{0:X3}' -f $index)."
    foreach ($entryIndex in 0..7) {
        $sourceEntry = [BitConverter]::ToUInt16($portaMeta, $index * 16 + $entryIndex * 2)
        $sourceTile = $sourceEntry -band 0x3FF
        $sourcePalette = ($sourceEntry -shr 12) -band 0xF
        Assert-True ($paletteMap.ContainsKey($sourcePalette)) "Unexpected PortaPretoria palette $sourcePalette."
        if ($sourceTile -ge 512) { Assert-True ($tileMap.ContainsKey($sourceTile - 512)) "Unexpected PortaPretoria secondary tile $($sourceTile - 512)."; $sourceTile = 512 + $tileMap[$sourceTile - 512] }
        $expectedPalette = $paletteMap[$sourcePalette]
        $expected = ($sourceEntry -band 0x0C00) -bor $sourceTile -bor ($expectedPalette -shl 12)
        Assert-True ([BitConverter]::ToUInt16($lagoMeta, $index * 16 + $entryIndex * 2) -eq $expected) "Lago metatile 0x$('{0:X3}' -f $index) is not the required PortaPretoria compatibility clone."
    }
}
foreach ($pair in @(@(6, 11), @(11, 13))) {
    $portaPalette = Read-Bytes (Join-Path $porta ('palettes/{0:D2}.pal' -f $pair[0]))
    $lagoPalette = Read-Bytes (Join-Path $lago ('palettes/{0:D2}.pal' -f $pair[1]))
    Assert-True ([Convert]::ToBase64String($portaPalette) -eq [Convert]::ToBase64String($lagoPalette)) "Lago palette $($pair[1]) must contain PortaPretoria palette $($pair[0])."
}
$portaPalette7 = Read-Palette (Join-Path $porta 'palettes/07.pal')
$portaPalette11 = Read-Palette (Join-Path $porta 'palettes/11.pal')
$lagoPalette12 = Read-Palette (Join-Path $lago 'palettes/12.pal')
Assert-True ($portaPalette7.Count -eq 16 -and $portaPalette11.Count -eq 16 -and $lagoPalette12.Count -eq 16) 'Compatibility palette sizes are incorrect.'
$wallPaletteOverrides = @{ 4 = $portaPalette11[2]; 7 = $portaPalette11[3]; 8 = $portaPalette11[4] }
foreach ($paletteIndex in 0..15) {
    $expectedColor = if ($wallPaletteOverrides.ContainsKey($paletteIndex)) { $wallPaletteOverrides[$paletteIndex] } else { $portaPalette7[$paletteIndex] }
    Assert-True ($lagoPalette12[$paletteIndex] -eq $expectedColor) "Lago Roman-wall compatibility palette mismatch at color $paletteIndex."
}
foreach ($paletteIndex in @(0..15 | Where-Object { $_ -notin 11, 12, 13 })) {
    $pacifidlogPalette = Read-Bytes (Join-Path $pacifidlog ('palettes/{0:D2}.pal' -f $paletteIndex))
    $lagoPalette = Read-Bytes (Join-Path $lago ('palettes/{0:D2}.pal' -f $paletteIndex))
    Assert-True ([Convert]::ToBase64String($pacifidlogPalette) -eq [Convert]::ToBase64String($lagoPalette)) "Unexpected Lago palette change in slot $paletteIndex."
}
$viaPaletteBlock = [regex]::Match($graphics, '(?ms)const u16 gTilesetPalettes_ViaConsolare\[\]\[16\]\s*=\s*\{(?<body>.*?)\};')
Assert-True ($viaPaletteBlock.Success) 'Via palette table is missing.'
$viaPalettePaths = @([regex]::Matches($viaPaletteBlock.Groups['body'].Value, 'INCGFX_U16\("([^"]+)"') | ForEach-Object { $_.Groups[1].Value })
$expectedViaPalettePaths = @(0..10 | ForEach-Object { 'data/tilesets/secondary/porta_pretoria/palettes/{0:D2}.pal' -f $_ })
$expectedViaPalettePaths += 'data/tilesets/secondary/lago_di_albera/palettes/11.pal', 'data/tilesets/secondary/lago_di_albera/palettes/12.pal'
$expectedViaPalettePaths += @(13..15 | ForEach-Object { 'data/tilesets/secondary/porta_pretoria/palettes/{0:D2}.pal' -f $_ })
Assert-True ($viaPalettePaths.Count -eq 16) 'Via palette table must contain exactly 16 palettes.'
foreach ($paletteIndex in 0..15) { Assert-True ($viaPalettePaths[$paletteIndex] -eq $expectedViaPalettePaths[$paletteIndex]) "Unexpected Via palette source in slot $paletteIndex." }
$viaHeaderBlock = [regex]::Match($headers, '(?ms)const struct Tileset gTileset_ViaConsolare\s*=\s*\{(?<body>.*?)\};')
Assert-True ($viaHeaderBlock.Success -and $viaHeaderBlock.Groups['body'].Value.Contains('.callback = InitTilesetAnim_Petalburg')) 'Via must preserve the PortaPretoria animation callback.'
Add-Type -AssemblyName System.Drawing
$pacifidlogTiles = [System.Drawing.Bitmap]::FromFile((Join-Path $pacifidlog 'tiles.png'))
$portaTiles = [System.Drawing.Bitmap]::FromFile((Join-Path $porta 'tiles.png'))
$lagoTiles = [System.Drawing.Bitmap]::FromFile((Join-Path $lago 'tiles.png'))
$viaTiles = [System.Drawing.Bitmap]::FromFile((Join-Path $via 'tiles.png'))
try {
    Assert-True ($pacifidlogTiles.Width -eq 128 -and $pacifidlogTiles.Height -eq 256 -and $lagoTiles.Width -eq 128 -and $lagoTiles.Height -eq 256) 'Lago tile sheet dimensions are incorrect.'
    Assert-True ($portaTiles.Width -eq 128 -and $portaTiles.Height -eq 208 -and $viaTiles.Width -eq 128 -and $viaTiles.Height -eq 208) 'Via tile sheet dimensions are incorrect.'
    $viaDestinationToSource = @{ 405 = 384; 406 = 385; 407 = 388; 408 = 389 }
    foreach ($tile in 0..408) {
        if ($tile -ge 384 -and $tile -le 392) { $sourceTiles = $lagoTiles; $sourceTile = $tile }
        elseif ($viaDestinationToSource.ContainsKey($tile)) { $sourceTiles = $portaTiles; $sourceTile = $viaDestinationToSource[$tile] }
        else { $sourceTiles = $portaTiles; $sourceTile = $tile }
        $sourceX = ($sourceTile % 16) * 8; $sourceY = [int][Math]::Floor($sourceTile / 16) * 8
        $destinationX = ($tile % 16) * 8; $destinationY = [int][Math]::Floor($tile / 16) * 8
        foreach ($x in 0..7) { foreach ($y in 0..7) { Assert-True ($sourceTiles.GetPixel($sourceX + $x, $sourceY + $y).ToArgb() -eq $viaTiles.GetPixel($destinationX + $x, $destinationY + $y).ToArgb()) "Unexpected Via tile pixels at slot $tile." } }
    }
    foreach ($tile in 0..511) {
        if ($tileMap.Values -notcontains $tile) {
            $tileX = ($tile % 16) * 8
            $tileY = [int][Math]::Floor($tile / 16) * 8
            foreach ($x in 0..7) { foreach ($y in 0..7) { Assert-True ($pacifidlogTiles.GetPixel($tileX + $x, $tileY + $y).ToArgb() -eq $lagoTiles.GetPixel($tileX + $x, $tileY + $y).ToArgb()) "Unexpected Lago tile change at $tile." } }
        }
    }
    $grayscaleByIndex = @(255, 238, 222, 205, 189, 172, 156, 139, 115, 98, 82, 65, 49, 32, 16, 0)
    $wallPixelRemap = @{ 2 = 4; 3 = 7; 4 = 8 }
    $sourcePalettes = @{ 184 = 6; 185 = 6; 186 = 6; 187 = 6; 315 = 7; 317 = 7; 340 = 11; 341 = 11; 342 = 11 }
    $destinationPalettes = @{ 384 = 11; 385 = 11; 386 = 11; 387 = 11; 388 = 12; 389 = 12; 390 = 12; 391 = 12; 392 = 12 }
    foreach ($sourceTile in $tileMap.Keys) {
        $destinationTile = $tileMap[$sourceTile]
        $sourceX = ($sourceTile % 16) * 8; $sourceY = [int][Math]::Floor($sourceTile / 16) * 8
        $destinationX = ($destinationTile % 16) * 8; $destinationY = [int][Math]::Floor($destinationTile / 16) * 8
        $sourcePalette = Read-Palette (Join-Path $porta ('palettes/{0:D2}.pal' -f $sourcePalettes[$sourceTile]))
        $destinationPalette = Read-Palette (Join-Path $lago ('palettes/{0:D2}.pal' -f $destinationPalettes[$destinationTile]))
        foreach ($x in 0..7) {
            foreach ($y in 0..7) {
                $sourcePixel = $portaTiles.GetPixel($sourceX + $x, $sourceY + $y)
                $destinationPixel = $lagoTiles.GetPixel($destinationX + $x, $destinationY + $y)
                $sourcePixelIndex = [Array]::IndexOf($grayscaleByIndex, [int]$sourcePixel.R)
                $destinationPixelIndex = [Array]::IndexOf($grayscaleByIndex, [int]$destinationPixel.R)
                Assert-True ($sourcePixelIndex -ge 0 -and $destinationPixelIndex -ge 0) "Unexpected indexed PNG color in imported tile $sourceTile."
                $expectedPixelIndex = $sourcePixelIndex
                if (($sourceTile -in 340, 341, 342) -and $wallPixelRemap.ContainsKey($sourcePixelIndex)) { $expectedPixelIndex = $wallPixelRemap[$sourcePixelIndex] }
                Assert-True ($destinationPixelIndex -eq $expectedPixelIndex) "PortaPretoria tile $sourceTile pixel index was not remapped correctly."
                Assert-True ($sourcePalette[$sourcePixelIndex] -eq $destinationPalette[$destinationPixelIndex]) "PortaPretoria tile $sourceTile does not render identically in the Lago tileset."
            }
        }
    }
} finally {
    $pacifidlogTiles.Dispose()
    $portaTiles.Dispose()
    $lagoTiles.Dispose()
    $viaTiles.Dispose()
}
Write-Output 'Lago di Albera connection tileset validation: PASS'
