param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw -Encoding utf8 | ConvertFrom-Json }
function Read-Block([byte[]]$bytes, [int]$width, [int]$x, [int]$y) { [BitConverter]::ToUInt16($bytes, 2 * ($y * $width + $x)) }
function Collision([UInt16]$raw) { return (($raw -band 0x0C00) -shr 10) }
function Is-Walkable([UInt16]$raw) { return (Collision $raw) -eq 0 }
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
Assert-True ($layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_Sootopolis') 'Laricia tilesets are incorrect.'
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
