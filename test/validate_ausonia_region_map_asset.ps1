param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

function Read-BigEndian32([byte[]]$Bytes, [int]$Offset) {
    return ($Bytes[$Offset] * 16777216) + ($Bytes[$Offset + 1] * 65536) + ($Bytes[$Offset + 2] * 256) + $Bytes[$Offset + 3]
}

$assetRoot = Join-Path $RepositoryRoot 'graphics/pokenav/region_map'
$pngPath = Join-Path $assetRoot 'map_ausonia.png'
$palPath = Join-Path $assetRoot 'map_ausonia.pal'
$binPath = Join-Path $assetRoot 'map_ausonia.bin'
$sourcePath = Join-Path $RepositoryRoot 'src/region_map.c'

foreach ($path in @($pngPath, $palPath, $binPath, $sourcePath)) {
    Assert-True (Test-Path $path) "Required file is missing: $path"
}

$png = [IO.File]::ReadAllBytes($pngPath)
Assert-True ($png.Length -ge 33) 'Ausonia PNG is truncated.'
Assert-True (($png[0] -eq 137) -and ($png[1] -eq 80) -and ($png[2] -eq 78) -and ($png[3] -eq 71)) 'Ausonia asset is not a PNG.'
$width = Read-BigEndian32 $png 16
$height = Read-BigEndian32 $png 20
Assert-True ($width -eq 128 -and $height -eq 120) "Ausonia PNG must be 128x120, got ${width}x${height}."
Assert-True ($png[24] -eq 8 -and $png[25] -eq 3) 'Ausonia PNG must be indexed 8bpp (PNG color type 3).'
Assert-True (($width % 8) -eq 0 -and ($height % 8) -eq 0) 'Ausonia atlas is not aligned to 8x8 tiles.'
Assert-True (($width / 8) -eq 16 -and ($height / 8) -eq 15) 'Ausonia atlas must be 16x15 tiles.'

$paletteLines = Get-Content $palPath
Assert-True ($paletteLines.Count -ge 3 -and $paletteLines[0] -eq 'JASC-PAL' -and $paletteLines[1] -eq '0100') 'Ausonia palette must be JASC-PAL.'
$paletteCount = [int]$paletteLines[2]
$paletteEntries = @($paletteLines | Select-Object -Skip 3 | Where-Object { $_.Trim().Length -gt 0 })
Assert-True ($paletteEntries.Count -eq $paletteCount) 'Ausonia palette entry count does not match its header.'
Assert-True ($paletteCount -le 48) "Ausonia palette exceeds the 48-color BG budget: $paletteCount."

$tilemap = [IO.File]::ReadAllBytes($binPath)
Assert-True ($tilemap.Length -eq 4096) "Ausonia tilemap must be 4096 bytes, got $($tilemap.Length)."
$maxTile = 0
for ($i = 0; $i -lt $tilemap.Length; $i += 2) {
    $entry = $tilemap[$i] + ($tilemap[$i + 1] * 256)
    $tile = $entry -band 0x03ff
    if ($tile -gt $maxTile) { $maxTile = $tile }
    Assert-True ($tile -le 223) "Ausonia tilemap references tile $tile, above the supplied 224-tile asset."
    Assert-True (($entry -band 0xfc00) -eq 0) "Ausonia tilemap entry $($i / 2) uses unsupported flip/palette bits."
}
Assert-True ($maxTile -le 223) "Ausonia tilemap maximum tile index is $maxTile."

$source = Get-Content $sourcePath -Raw
Assert-True ($source -match 'sRegionMapAusonia_Pal') 'Ausonia palette symbol is missing.'
Assert-True ($source -match 'sRegionMapAusonia_Gfx') 'Ausonia graphics symbol is missing.'
Assert-True ($source -match 'sRegionMapAusonia_Tilemap') 'Ausonia tilemap symbol is missing.'
Assert-True ($source -match '-num_tiles 224 -Wnum_tiles') 'Ausonia graphics conversion must request 224 tiles.'
Assert-True ($source -match '(?s)\[REGION_MAP_AUSONIA\].*?\.regionMapPalette\s*=\s*sRegionMapAusonia_Pal.*?\.regionMapGfx\s*=\s*sRegionMapAusonia_Gfx.*?\.regionMapTilemap\s*=\s*sRegionMapAusonia_Tilemap') 'REGION_MAP_AUSONIA is not routed to its dedicated assets.'
Assert-True ($source -match '(?s)\[REGION_MAP_HOENN\].*?\.regionMapPalette\s*=\s*sRegionMapBg_Pal.*?\.regionMapGfx\s*=\s*sRegionMapBg_GfxLZ.*?\.regionMapTilemap\s*=\s*sRegionMapBg_TilemapLZ') 'Hoenn Region Map routing changed unexpectedly.'

Write-Output "Ausonia Region Map asset: PASS (palette=$paletteCount, maxTile=$maxTile)"
