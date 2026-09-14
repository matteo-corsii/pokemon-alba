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
Assert-True (@($villa.coord_events).Count -eq 0 -and @($villa.bg_events).Count -eq 0) 'Villa gardens must not have coord or background events.'
Assert-True (@($villa.warp_events | Where-Object { [int]$_.x -eq 29 -and [int]$_.y -eq 9 -and [int]$_.elevation -eq 0 -and $_.dest_map -eq 'MAP_VILLA_PAPALE_INTERNO' -and [int]$_.dest_warp_id -eq 0 }).Count -eq 1 -and @($villa.warp_events).Count -eq 1) 'Villa gardens must contain only the canonical interior entrance warp.'
Assert-True ($eventScripts -match 'data/maps/VillaPapaleGiardini/scripts\.inc') 'Villa Papale scripts are not globally included.'
Assert-True ((Get-Content (Join-Path $RepositoryRoot 'data/maps/VillaPapaleGiardini/scripts.inc') -Raw) -match '^VillaPapaleGiardini_MapScripts::\r?\n\s*\.byte 0') 'Villa Papale MapScripts are not minimal.'
Assert-True ((Get-Content (Join-Path $RepositoryRoot 'src/data/wild_encounters.json') -Raw) -notmatch 'MAP_VILLA_PAPALE_GIARDINI') 'Villa scaffold must not have encounters.'
$blockdata = [IO.File]::ReadAllBytes($mapBin)
$generalAttributes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/primary/general/metatile_attributes.bin'))
$sootopolisAttributes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/secondary/sootopolis/metatile_attributes.bin'))
foreach ($x in 28..30) {
    $entryBlock = Get-Block $blockdata $x 59
    Assert-True ((Get-Collision $entryBlock) -eq 0) "Villa south connection cell $x,59 is blocked."
    Assert-True ((Get-MetatileId $entryBlock) -ne 0x0A1) "Villa south connection cell $x,59 must not be water."
}
$mainPath = @()
$grassCells = @()
$waterCells = @()
for ($y = 0; $y -lt 60; $y++) {
    for ($x = 0; $x -lt 60; $x++) {
        $block = Get-Block $blockdata $x $y
        $metatileId = Get-MetatileId $block
        $attributes = if ($metatileId -lt 0x200) { $generalAttributes } else { $sootopolisAttributes }
        $attributesId = if ($metatileId -lt 0x200) { $metatileId } else { $metatileId - 0x200 }
        $behavior = Get-Behavior $attributes $attributesId
        if ($x -ge 28 -and $x -le 30 -and $y -ge 13 -and $y -le 59) { $mainPath += "$x,$y"; Assert-True ((Get-Collision $block) -eq 0) "Villa central avenue cell $x,$y is blocked." }
        if ($behavior -eq 0x2) { $grassCells += "$x,$y" }
        if ($behavior -ge 0x10 -and $behavior -le 0x19) { $waterCells += "$x,$y"; Assert-True ((Get-Collision $block) -ne 0) "Villa decorative water cell $x,$y must be non-walkable." }
    }
}
Assert-True ($grassCells.Count -gt 0) 'Villa eastern gardens must contain tall grass.'
Assert-True (@($grassCells | Where-Object { $_ -match '^(28|29|30),(13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31|32|33|34|35|36|37|38|39|40|41|42|43|44|45|46|47|48|49|50|51|52|53|54|55|56|57|58|59)$' }).Count -eq 0) 'Tall grass must not cover the central avenue.'
Assert-True (@($grassCells | Where-Object { $_ -match '^(28|29|30),(9|10|11|12)$' }).Count -eq 0) 'Tall grass must not cover the Villa entrance approach.'
Assert-True (@($waterCells | Where-Object { $_ -match '^(28|29|30),59$' }).Count -eq 0) 'Decorative water must not cover the south connection.'
Assert-True (@($waterCells | Where-Object { $_ -match '^29,9$' }).Count -eq 0) 'Decorative water must not cover the Villa entrance.'
$doorBlock = Get-Block $blockdata 29 9
Assert-True ((Get-Collision $doorBlock) -eq 0) 'Villa entrance at 29,9 must be reachable.'
$doorId = Get-MetatileId $doorBlock
$doorAttrsId = if ($doorId -lt 0x200) { $doorId } else { $doorId - 0x200 }
$doorBehavior = Get-Behavior $(if ($doorId -lt 0x200) { $generalAttributes } else { $sootopolisAttributes }) $doorAttrsId
Assert-True ($doorBehavior -eq 0x69) 'Villa entrance must retain the verified south-arrow warp behavior.'
Assert-True (@(Get-ChildItem (Join-Path $RepositoryRoot 'data/maps') -Directory | Where-Object { $_.Name -like 'VillaPapale*' -and $_.Name -notin @('VillaPapaleGiardini', 'VillaPapaleInterno') }).Count -eq 0) 'Villa map directories must contain only the gardens and the canonical interior.'
Write-Output 'Villa Papale Giardini scaffold: PASS'
