param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Read-Json([string]$path) { Get-Content (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Get-Block([byte[]]$blockdata, [int]$x, [int]$y) { $offset = 2 * ($y * 32 + $x); return ([int]$blockdata[$offset]) -bor (([int]$blockdata[$offset + 1]) -shl 8) }
function Get-Collision([int]$block) { return (($block -shr 10) -band 1) }

$interior = Read-Json 'data/maps/VillaPapaleInterno/map.json'
$gardens = Read-Json 'data/maps/VillaPapaleGiardini/map.json'
$layouts = Read-Json 'data/layouts/layouts.json'
$groups = Read-Json 'data/maps/map_groups.json'
$eventScripts = Get-Content (Join-Path $RepositoryRoot 'data/event_scripts.s') -Raw
$layout = @($layouts.layouts | Where-Object { $_.id -eq 'LAYOUT_VILLA_PAPALE_INTERNO' })

Assert-True ($interior.id -eq 'MAP_VILLA_PAPALE_INTERNO' -and $interior.layout -eq 'LAYOUT_VILLA_PAPALE_INTERNO') 'Villa Papale interior identity is incorrect.'
Assert-True ($interior.map_type -eq 'MAP_TYPE_INDOOR' -and $interior.region_map_section -eq 'MAPSEC_ALBERA_STORICA') 'Villa Papale interior metadata is incorrect.'
Assert-True ($layout.Count -eq 1 -and [int]$layout[0].width -eq 32 -and [int]$layout[0].height -eq 26 -and $layout[0].primary_tileset -eq 'gTileset_Building' -and $layout[0].secondary_tileset -eq 'gTileset_LilycoveMuseum') 'Villa Papale interior layout or tilesets are incorrect.'
$mapBin = Join-Path $RepositoryRoot $layout[0].blockdata_filepath
$borderBin = Join-Path $RepositoryRoot $layout[0].border_filepath
Assert-True ((Test-Path $mapBin) -and ((Get-Item $mapBin).Length -eq 1664)) 'Villa Papale interior map.bin is missing or has the wrong size.'
Assert-True ((Test-Path $borderBin) -and ((Get-Item $borderBin).Length -eq 8)) 'Villa Papale interior border.bin is missing or has the wrong size.'
Assert-True (@($groups.gMapGroup_IndoorOldale | Where-Object { $_ -eq 'VillaPapaleInterno' }).Count -eq 1) 'Villa Papale interior map group registration is incorrect.'
Assert-True (@($gardens.warp_events | Where-Object { [int]$_.x -eq 29 -and [int]$_.y -eq 9 -and [int]$_.elevation -eq 0 -and $_.dest_map -eq 'MAP_VILLA_PAPALE_INTERNO' -and [int]$_.dest_warp_id -eq 0 }).Count -eq 1) 'Villa gardens entrance warp is incorrect.'
Assert-True (@($interior.warp_events | Where-Object { [int]$_.x -eq 15 -and [int]$_.y -eq 25 -and [int]$_.elevation -eq 0 -and $_.dest_map -eq 'MAP_VILLA_PAPALE_GIARDINI' -and [int]$_.dest_warp_id -eq 0 }).Count -eq 1) 'Villa interior exit warp is incorrect.'
Assert-True (@($interior.object_events).Count -eq 0 -and @($interior.coord_events).Count -eq 0 -and @($interior.bg_events).Count -eq 0) 'Villa Papale interior must not contain NPCs, trainers, or scripted objects yet.'
Assert-True ((Get-Content (Join-Path $RepositoryRoot 'src/data/wild_encounters.json') -Raw) -notmatch 'MAP_VILLA_PAPALE_INTERNO') 'Villa Papale interior must not have encounters.'
Assert-True ($eventScripts -match 'data/maps/VillaPapaleInterno/scripts\.inc') 'Villa Papale interior scripts are not globally included.'
Assert-True ((Get-Content (Join-Path $RepositoryRoot 'data/maps/VillaPapaleInterno/scripts.inc') -Raw) -match '^VillaPapaleInterno_MapScripts::\r?\n\s*\.byte 0') 'Villa Papale interior MapScripts must remain minimal.'
$blockdata = [IO.File]::ReadAllBytes($mapBin)
foreach ($point in @(@(15,25), @(15,22), @(5,12), @(25,12), @(15,10), @(15,3))) {
    Assert-True ((Get-Collision (Get-Block $blockdata $point[0] $point[1])) -eq 0) "Villa Papale interior circulation point $($point[0]),$($point[1]) is blocked."
}
Write-Output 'Villa Papale interior: PASS'
