param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing

function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw -Encoding utf8 | ConvertFrom-Json }
function Read-Block([byte[]]$bytes, [int]$width, [int]$x, [int]$y) { [BitConverter]::ToUInt16($bytes, 2 * ($y * $width + $x)) }
function Collision([UInt16]$raw) { return (($raw -band 0x0C00) -shr 10) }
function Is-Walkable([UInt16]$raw) { return (Collision $raw) -eq 0 }
function Copy-Bytes([byte[]]$bytes, [int]$offset, [int]$count) {
    $copy = [byte[]]::new($count)
    [Array]::Copy($bytes, $offset, $copy, 0, $count)
    return $copy
}
function Copy-IndexedTile([System.Drawing.Bitmap]$bitmap, [int]$tileIndex) {
    $columns = [int]($bitmap.Width / 8)
    $tileX = ($tileIndex % $columns) * 8
    $tileY = [int]([Math]::Floor($tileIndex / $columns)) * 8
    $bits = $bitmap.LockBits([Drawing.Rectangle]::new(0, 0, $bitmap.Width, $bitmap.Height), [Drawing.Imaging.ImageLockMode]::ReadOnly, [Drawing.Imaging.PixelFormat]::Format4bppIndexed)
    try {
        $stride = [Math]::Abs($bits.Stride)
        $copy = [byte[]]::new(32)
        foreach ($row in 0..7) {
            [Runtime.InteropServices.Marshal]::Copy([IntPtr]::new([int64]$bits.Scan0.ToInt64() + [int64]($tileY + $row) * $stride + [int64]($tileX / 2)), $copy, $row * 4, 4)
        }
        return $copy
    } finally {
        $bitmap.UnlockBits($bits)
    }
}
function Test-Reachable([byte[]]$bytes, [int]$width, [int]$height, [string[]]$starts, [string[]]$targets) {
    $targetsByKey = @{}; foreach ($target in $targets) { $targetsByKey[$target] = $true }
    $seen = @{}; $queue = New-Object System.Collections.Generic.Queue[string]
    foreach ($start in $starts) { $p = $start.Split(','); if (Is-Walkable (Read-Block $bytes $width ([int]$p[0]) ([int]$p[1]))) { $seen[$start] = $true; $queue.Enqueue($start) } }
    while ($queue.Count -gt 0) {
        $key = $queue.Dequeue(); if ($targetsByKey.ContainsKey($key)) { return $true }
        $p = $key.Split(','); $x = [int]$p[0]; $y = [int]$p[1]
        foreach ($d in @(@(1,0), @(-1,0), @(0,1), @(0,-1))) {
            $nx=$x+$d[0]; $ny=$y+$d[1]; if ($nx -lt 0 -or $nx -ge $width -or $ny -lt 0 -or $ny -ge $height) { continue }
            if (Is-Walkable (Read-Block $bytes $width $nx $ny)) { $next="$nx,$ny"; if (-not $seen.ContainsKey($next)) { $seen[$next]=$true; $queue.Enqueue($next) } }
        }
    }
    return $false
}

$layouts = (Read-Json 'data/layouts/layouts.json').layouts
$groups = Read-Json 'data/maps/map_groups.json'
$map = Read-Json 'data/maps/Laricia/map.json'
$ponte = Read-Json 'data/maps/PonteValleLaricia/map.json'
$layout = @($layouts | Where-Object { $_.id -eq 'LAYOUT_LARICIA' })
Assert-True ($map.id -eq 'MAP_LARICIA' -and $map.name -eq 'Laricia' -and $map.layout -eq 'LAYOUT_LARICIA') 'Laricia map identity is incorrect.'
Assert-True ($map.region_map_section -eq 'MAPSEC_LARICIA' -and $map.show_map_name -eq $true -and $map.map_type -eq 'MAP_TYPE_TOWN') 'Laricia map metadata is incorrect.'
Assert-True ($layout.Count -eq 1 -and [int]$layout[0].width -eq 64 -and [int]$layout[0].height -eq 64) 'Laricia must have one 64x64 layout.'
Assert-True ($layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_Laricia') 'Laricia tilesets are incorrect.'
Assert-True (@($groups.gMapGroup_TownsAndRoutes | Where-Object { $_ -eq 'Laricia' }).Count -eq 1) 'Laricia must be registered once in TownsAndRoutes.'
$connection = @($map.connections | Where-Object { $_.direction -eq 'left' -and $_.map -eq 'MAP_PONTE_VALLE_LARICIA' -and [int]$_.offset -eq 0 })
$return = @($ponte.connections | Where-Object { $_.direction -eq 'right' -and $_.map -eq 'MAP_LARICIA' -and [int]$_.offset -eq 0 })
Assert-True ($connection.Count -eq 1 -and $return.Count -eq 1 -and @($map.connections).Count -eq 1) 'Laricia/Ponte connection must be the sole reciprocal connection.'
$border = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot $layout[0].border_filepath)); $reference = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/AlberaStorica/border.bin'))
Assert-True ($border.Length -eq 8 -and [Linq.Enumerable]::SequenceEqual([byte[]]$border, [byte[]]$reference)) 'Laricia must use the approved shared forest border.'
$blocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot $layout[0].blockdata_filepath)); $ponteBlocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/PonteValleLaricia/map.bin'))
Assert-True ($blocks.Length -eq 64*64*2) 'Laricia map.bin size is incorrect.'
foreach ($y in @(14..17) + @(38..41)) {
    Assert-True (Is-Walkable (Read-Block $blocks 64 0 $y)) "Laricia west entry 0,$y is not walkable."
    Assert-True (Is-Walkable (Read-Block $ponteBlocks 64 63 $y)) "Ponte east entry 63,$y is not walkable."
    foreach ($x in 0..7) { Assert-True (((Read-Block $blocks 64 $x $y) -band 0x03FF) -lt 0x200) "Laricia shared-edge strip $x,$y must use a primary General metatile." }
}
$requiredBridgeMetatiles = @(0x293, 0x294, 0x2EE, 0x2F0, 0x2F9, 0x2FA, 0x302, 0x309)
$sootopolisRoot = Join-Path $RepositoryRoot 'data/tilesets/secondary/sootopolis'
$portaPretoriaRoot = Join-Path $RepositoryRoot 'data/tilesets/secondary/porta_pretoria'
$lariciaTilesetRoot = Join-Path $RepositoryRoot 'data/tilesets/secondary/laricia'
$sootopolisMetatiles = [IO.File]::ReadAllBytes((Join-Path $sootopolisRoot 'metatiles.bin'))
$sootopolisAttributes = [IO.File]::ReadAllBytes((Join-Path $sootopolisRoot 'metatile_attributes.bin'))
$portaPretoriaMetatiles = [IO.File]::ReadAllBytes((Join-Path $portaPretoriaRoot 'metatiles.bin'))
$portaPretoriaAttributes = [IO.File]::ReadAllBytes((Join-Path $portaPretoriaRoot 'metatile_attributes.bin'))
$lariciaMetatiles = [IO.File]::ReadAllBytes((Join-Path $lariciaTilesetRoot 'metatiles.bin'))
$lariciaAttributes = [IO.File]::ReadAllBytes((Join-Path $lariciaTilesetRoot 'metatile_attributes.bin'))
Assert-True ($lariciaMetatiles.Length -eq 0x10A * 16 -and $lariciaAttributes.Length -eq 0x10A * 2) 'Laricia tileset must contain coherent entries through secondary metatile 0x309.'
$portaPretoriaImage = [System.Drawing.Bitmap]::FromFile((Join-Path $portaPretoriaRoot 'tiles.png'))
$lariciaImage = [System.Drawing.Bitmap]::FromFile((Join-Path $lariciaTilesetRoot 'tiles.png'))
$sourceTileCount = [int](($portaPretoriaImage.Width * $portaPretoriaImage.Height) / 64)
$lariciaTileCount = [int](($lariciaImage.Width * $lariciaImage.Height) / 64)
Assert-True ($lariciaTileCount -ge 347) 'Laricia tiles.png does not contain the registered 347 tiles.'
$paletteMap = @{ 0 = 0; 2 = 2; 6 = 12; 7 = 13; 11 = 14 }
foreach ($slot in 0..15) {
    $lariciaPalette = [IO.File]::ReadAllBytes((Join-Path $lariciaTilesetRoot ('palettes/{0:D2}.pal' -f $slot)))
    if ($slot -eq 12) { $expectedPalette = [IO.File]::ReadAllBytes((Join-Path $portaPretoriaRoot 'palettes/06.pal')) }
    elseif ($slot -eq 13) { $expectedPalette = [IO.File]::ReadAllBytes((Join-Path $portaPretoriaRoot 'palettes/07.pal')) }
    elseif ($slot -eq 14) { $expectedPalette = [IO.File]::ReadAllBytes((Join-Path $portaPretoriaRoot 'palettes/11.pal')) }
    else { $expectedPalette = [IO.File]::ReadAllBytes((Join-Path $sootopolisRoot ('palettes/{0:D2}.pal' -f $slot))) }
    Assert-True ([Linq.Enumerable]::SequenceEqual([byte[]]$lariciaPalette, [byte[]]$expectedPalette)) "Laricia palette $slot is not the required Sootopolis/PortaPretoria copy."
}
foreach ($i in 0..253) {
    if ($requiredBridgeMetatiles -contains ($i + 0x200)) { continue }
    Assert-True ([Linq.Enumerable]::SequenceEqual([byte[]](Copy-Bytes $sootopolisMetatiles ($i * 16) 16), [byte[]](Copy-Bytes $lariciaMetatiles ($i * 16) 16))) "Laricia unexpectedly changes Sootopolis metatile 0x$('{0:X3}' -f ($i + 0x200))."
    Assert-True ([Linq.Enumerable]::SequenceEqual([byte[]](Copy-Bytes $sootopolisAttributes ($i * 2) 2), [byte[]](Copy-Bytes $lariciaAttributes ($i * 2) 2))) "Laricia unexpectedly changes Sootopolis attributes for 0x$('{0:X3}' -f ($i + 0x200))."
}
foreach ($id in $requiredBridgeMetatiles) {
    $offset = ($id - 0x200) * 16
    Assert-True ([BitConverter]::ToUInt16($lariciaAttributes, ($id - 0x200) * 2) -eq [BitConverter]::ToUInt16($portaPretoriaAttributes, ($id - 0x200) * 2)) "Laricia attributes for 0x$('{0:X3}' -f $id) differ from PortaPretoria."
    foreach ($component in 0..7) {
        $source = [BitConverter]::ToUInt16($portaPretoriaMetatiles, $offset + $component * 2)
        $target = [BitConverter]::ToUInt16($lariciaMetatiles, $offset + $component * 2)
        $sourcePalette = ($source -shr 12) -band 15
        Assert-True ($paletteMap.ContainsKey($sourcePalette)) "Unexpected PortaPretoria palette $sourcePalette in 0x$('{0:X3}' -f $id)."
        Assert-True (($target -band 0x0C00) -eq ($source -band 0x0C00)) "Laricia flips differ for 0x$('{0:X3}' -f $id), component $component."
        Assert-True ((($target -shr 12) -band 15) -eq $paletteMap[$sourcePalette]) "Laricia palette remap differs for 0x$('{0:X3}' -f $id), component $component."
        $sourceTile = $source -band 0x03FF
        $targetTile = $target -band 0x03FF
        if ($sourceTile -lt 0x200) {
            Assert-True ($targetTile -eq $sourceTile) "Laricia primary tile reference differs for 0x$('{0:X3}' -f $id), component $component."
        } else {
            Assert-True ($targetTile -ge 0x200 -and ($targetTile - 0x200) -lt $lariciaTileCount) "Laricia tile for 0x$('{0:X3}' -f $id), component $component is not a valid secondary tile."
            Assert-True (($sourceTile - 0x200) -lt $sourceTileCount) "PortaPretoria source tile for 0x$('{0:X3}' -f $id), component $component is out of range."
            Assert-True ([Linq.Enumerable]::SequenceEqual([byte[]](Copy-IndexedTile $portaPretoriaImage ($sourceTile - 0x200)), [byte[]](Copy-IndexedTile $lariciaImage ($targetTile - 0x200)))) "Laricia graphics differ from PortaPretoria for 0x$('{0:X3}' -f $id), component $component."
        }
    }
}
$portaPretoriaImage.Dispose()
$lariciaImage.Dispose()
Assert-True ([BitConverter]::ToUInt16($lariciaAttributes, (0x309 - 0x200) * 2) -eq [BitConverter]::ToUInt16($portaPretoriaAttributes, (0x309 - 0x200) * 2)) 'Laricia 0x309 bridge pavement is not semantically equivalent to PortaPretoria.'
foreach ($y in 0..63) { if ($y -notin @(14..17) + @(38..41)) { Assert-True (-not (Is-Walkable (Read-Block $blocks 64 0 $y))) "Laricia west edge has an unintended opening at 0,$y." } }
$upper = @(14..17 | ForEach-Object { "0,$_" }); $lower = @(38..41 | ForEach-Object { "0,$_" })
Assert-True (Test-Reachable $blocks 64 64 $upper @('30,22')) 'Upper bridge entry cannot reach Laricia piazza.'
Assert-True (Test-Reachable $blocks 64 64 @('30,22') @('30,8')) 'Piazza cannot reach Via dell Uccelliera.'
Assert-True (Test-Reachable $blocks 64 64 @('30,22') @('16,47')) 'Piazza cannot reach the lower alleys and Fraschetta 3 approach.'
Assert-True (Test-Reachable $blocks 64 64 @('30,22') $lower) 'Piazza cannot reach the lower Valle exit.'
Assert-True (Test-Reachable $blocks 64 64 @('30,22') @('51,24')) 'Piazza cannot reach the east road/Sagra approach.'
Assert-True (-not (Test-Reachable $blocks 64 64 @('30,22') @('63,24'))) 'The Sagra block is bypassable to Laricia east edge.'
Assert-True (@($map.warp_events).Count -eq 0 -and @($map.coord_events).Count -eq 0) 'Laricia scaffold must not add warps or coord events.'
Assert-True (@($map.object_events).Count -eq 3 -and @($map.object_events | Where-Object { $_.trainer_type -ne 'TRAINER_TYPE_NONE' }).Count -eq 0) 'Laricia must contain only the non-trainer Sagra setup objects.'
Assert-True (@($map.object_events | Where-Object { $_.graphics_id -eq 'OBJ_EVENT_GFX_TRUCK' }).Count -eq 2) 'Laricia Sagra must visibly use two existing truck object graphics.'
$wild = Read-Json 'src/data/wild_encounters.json'
Assert-True (@($wild.wild_encounter_groups | ForEach-Object { $_.encounters } | Where-Object { $_.map -eq 'MAP_LARICIA' }).Count -eq 0) 'Laricia scaffold must not add encounters.'
$scripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/Laricia/scripts.inc') -Raw
Assert-True ($scripts.Contains('Sagra') -and $scripts.Contains('Porchetta') -and $scripts.Contains('GALLORO / GENZALIA')) 'Laricia Sagra blocker or eastern direction text is missing.'
Write-Output 'Laricia scaffold: PASS'
