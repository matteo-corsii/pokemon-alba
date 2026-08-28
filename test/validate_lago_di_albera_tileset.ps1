param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'
function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Read-Bytes([string]$path) { [IO.File]::ReadAllBytes($path) }
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
$layout = @((Read-Json 'data/layouts/layouts.json').layouts | Where-Object id -eq 'LAYOUT_LAGO_DI_ALBERA')
$map = Read-Json 'data/maps/LagoDiAlbera/map.json'
$graphics = Get-Content (Join-Path $RepositoryRoot 'src/data/tilesets/graphics.h') -Raw
$metatilesHeader = Get-Content (Join-Path $RepositoryRoot 'src/data/tilesets/metatiles.h') -Raw
$headers = Get-Content (Join-Path $RepositoryRoot 'src/data/tilesets/headers.h') -Raw
$includes = Get-Content (Join-Path $RepositoryRoot 'include/tilesets.h') -Raw

Assert-True ($layout.Count -eq 1 -and $layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_LagoDiAlbera') 'Lago must use General + LagoDiAlbera.'
Assert-True (Test-Path -LiteralPath $lago) 'Lago clone directory is missing.'
Assert-True ($graphics.Contains('gTilesetTiles_LagoDiAlbera') -and $graphics.Contains('-num_tiles 512') -and $metatilesHeader.Contains('gMetatiles_LagoDiAlbera') -and $headers.Contains('gTileset_LagoDiAlbera') -and $headers.Contains('.callback = InitTilesetAnim_Pacifidlog') -and $includes.Contains('gTileset_LagoDiAlbera')) 'Lago clone registration is incomplete.'
Assert-True ([int]$layout[0].width -eq 120 -and [int]$layout[0].height -eq 120) 'Lago dimensions changed.'
Assert-True (@($map.object_events).Count -eq 12 -and @($map.warp_events).Count -eq 0 -and @($map.coord_events).Count -eq 0 -and @($map.bg_events).Count -eq 5) 'Lago ambient event counts are incorrect.'
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

$pacMeta = Read-Bytes (Join-Path $pacifidlog 'metatiles.bin')
$pacAttrs = Read-Bytes (Join-Path $pacifidlog 'metatile_attributes.bin')
$lagoMeta = Read-Bytes (Join-Path $lago 'metatiles.bin')
$lagoAttrs = Read-Bytes (Join-Path $lago 'metatile_attributes.bin')
$portaMeta = Read-Bytes (Join-Path $porta 'metatiles.bin')
$portaAttrs = Read-Bytes (Join-Path $porta 'metatile_attributes.bin')
Assert-True ($pacMeta.Length -eq $lagoMeta.Length -and $pacAttrs.Length -eq $lagoAttrs.Length -and $lagoMeta.Length / 16 -eq 203) 'Lago clone metatile capacity differs from Pacifidlog.'
$patchedMetatiles = @(0x096, 0x0C9)
for ($index = 0; $index -lt 203; $index++) {
    if ($patchedMetatiles -notcontains $index) {
        $source = New-Object byte[] 16; $clone = New-Object byte[] 16
        [Array]::Copy($pacMeta, $index * 16, $source, 0, 16); [Array]::Copy($lagoMeta, $index * 16, $clone, 0, 16)
        Assert-True ([Convert]::ToBase64String($source) -eq [Convert]::ToBase64String($clone)) "Unexpected Lago metatile change at 0x$('{0:X3}' -f $index)."
        Assert-True ([BitConverter]::ToUInt16($pacAttrs, $index * 2) -eq [BitConverter]::ToUInt16($lagoAttrs, $index * 2)) "Unexpected Lago attribute change at 0x$('{0:X3}' -f $index)."
    }
}
$tileMap = @{ 184 = 494; 185 = 504; 186 = 505; 187 = 506; 315 = 507; 317 = 508; 340 = 509; 341 = 510; 342 = 511 }
$paletteMap = @{ 2 = 2; 6 = 11; 7 = 12; 11 = 13 }
foreach ($index in $patchedMetatiles) {
    Assert-True ([BitConverter]::ToUInt16($lagoAttrs, $index * 2) -eq [BitConverter]::ToUInt16($portaAttrs, $index * 2)) "Lago attribute mismatch for PortaPretoria metatile 0x$('{0:X3}' -f $index)."
    foreach ($entryIndex in 0..7) {
        $sourceEntry = [BitConverter]::ToUInt16($portaMeta, $index * 16 + $entryIndex * 2)
        $sourceTile = $sourceEntry -band 0x3FF
        $sourcePalette = ($sourceEntry -shr 12) -band 0xF
        Assert-True ($paletteMap.ContainsKey($sourcePalette)) "Unexpected PortaPretoria palette $sourcePalette."
        if ($sourceTile -ge 512) { Assert-True ($tileMap.ContainsKey($sourceTile - 512)) "Unexpected PortaPretoria secondary tile $($sourceTile - 512)."; $sourceTile = 512 + $tileMap[$sourceTile - 512] }
        $expected = ($sourceEntry -band 0x0C00) -bor $sourceTile -bor ($paletteMap[$sourcePalette] -shl 12)
        Assert-True ([BitConverter]::ToUInt16($lagoMeta, $index * 16 + $entryIndex * 2) -eq $expected) "Lago metatile 0x$('{0:X3}' -f $index) is not the required PortaPretoria compatibility clone."
    }
}
foreach ($pair in @(@(6, 11), @(7, 12), @(11, 13))) {
    $portaPalette = Read-Bytes (Join-Path $porta ('palettes/{0:D2}.pal' -f $pair[0]))
    $lagoPalette = Read-Bytes (Join-Path $lago ('palettes/{0:D2}.pal' -f $pair[1]))
    Assert-True ([Convert]::ToBase64String($portaPalette) -eq [Convert]::ToBase64String($lagoPalette)) "Lago palette $($pair[1]) must contain PortaPretoria palette $($pair[0])."
}
foreach ($paletteIndex in @(0..15 | Where-Object { $_ -notin 11, 12, 13 })) {
    $pacifidlogPalette = Read-Bytes (Join-Path $pacifidlog ('palettes/{0:D2}.pal' -f $paletteIndex))
    $lagoPalette = Read-Bytes (Join-Path $lago ('palettes/{0:D2}.pal' -f $paletteIndex))
    Assert-True ([Convert]::ToBase64String($pacifidlogPalette) -eq [Convert]::ToBase64String($lagoPalette)) "Unexpected Lago palette change in slot $paletteIndex."
}
Add-Type -AssemblyName System.Drawing
$pacifidlogTiles = [System.Drawing.Bitmap]::FromFile((Join-Path $pacifidlog 'tiles.png'))
$portaTiles = [System.Drawing.Bitmap]::FromFile((Join-Path $porta 'tiles.png'))
$lagoTiles = [System.Drawing.Bitmap]::FromFile((Join-Path $lago 'tiles.png'))
try {
    Assert-True ($pacifidlogTiles.Width -eq 128 -and $pacifidlogTiles.Height -eq 256 -and $lagoTiles.Width -eq 128 -and $lagoTiles.Height -eq 256) 'Lago tile sheet dimensions are incorrect.'
    foreach ($tile in 0..511) {
        if ($tileMap.Values -notcontains $tile) {
            $tileX = ($tile % 16) * 8
            $tileY = [int][Math]::Floor($tile / 16) * 8
            foreach ($x in 0..7) { foreach ($y in 0..7) { Assert-True ($pacifidlogTiles.GetPixel($tileX + $x, $tileY + $y).ToArgb() -eq $lagoTiles.GetPixel($tileX + $x, $tileY + $y).ToArgb()) "Unexpected Lago tile change at $tile." } }
        }
    }
    foreach ($sourceTile in $tileMap.Keys) {
        $destinationTile = $tileMap[$sourceTile]
        $sourceX = ($sourceTile % 16) * 8; $sourceY = [int][Math]::Floor($sourceTile / 16) * 8
        $destinationX = ($destinationTile % 16) * 8; $destinationY = [int][Math]::Floor($destinationTile / 16) * 8
        foreach ($x in 0..7) { foreach ($y in 0..7) { Assert-True ($portaTiles.GetPixel($sourceX + $x, $sourceY + $y).ToArgb() -eq $lagoTiles.GetPixel($destinationX + $x, $destinationY + $y).ToArgb()) "PortaPretoria tile $sourceTile was not copied correctly." } }
    }
} finally {
    $pacifidlogTiles.Dispose()
    $portaTiles.Dispose()
    $lagoTiles.Dispose()
}
Write-Output 'Lago di Albera connection tileset validation: PASS'
