param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)

$ErrorActionPreference = 'Stop'

function Read-Json([string]$RelativePath) {
    Get-Content -LiteralPath (Join-Path $RepositoryRoot $RelativePath) -Raw -Encoding utf8 | ConvertFrom-Json
}

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

$map = Read-Json 'data/maps/Emissario/map.json'
$lago = Read-Json 'data/maps/LagoDiAlbera/map.json'
$layouts = Read-Json 'data/layouts/layouts.json'
$groups = Read-Json 'data/maps/map_groups.json'
$wild = Read-Json 'src/data/wild_encounters.json'
$scripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/Emissario/scripts.inc') -Raw -Encoding utf8
$eventScripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/event_scripts.s') -Raw -Encoding utf8

Assert-True ($map.id -eq 'MAP_EMISSARIO' -and $map.name -eq 'Emissario' -and $map.layout -eq 'LAYOUT_EMISSARIO') 'Emissario identity is incorrect.'
Assert-True ($map.map_type -eq 'MAP_TYPE_UNDERGROUND' -and $map.region_map_section -eq 'MAPSEC_ALBERA_STORICA') 'Emissario map type or region is incorrect.'
Assert-True (-not $map.allow_cycling -and $map.allow_escaping -and $map.allow_running -and -not $map.show_map_name) 'Emissario traversal settings are incorrect.'
Assert-True ($null -eq $map.connections) 'The structural blockout must not have map connections.'
Assert-True (@($map.object_events).Count -eq 0 -and @($map.coord_events).Count -eq 0 -and @($map.bg_events).Count -eq 0) 'The structural blockout must not contain narrative events.'
Assert-True (@($map.warp_events).Count -eq 1) 'Emissario must contain exactly one return warp.'

$exit = @($map.warp_events | Where-Object {
    [int]$_.x -eq 15 -and [int]$_.y -eq 29 -and [int]$_.elevation -eq 3 -and
    $_.dest_map -eq 'MAP_LAGO_DI_ALBERA' -and [int]$_.dest_warp_id -eq 4
})
$entrance = @($lago.warp_events | Where-Object {
    [int]$_.x -eq 81 -and [int]$_.y -eq 3 -and [int]$_.elevation -eq 3 -and
    $_.dest_map -eq 'MAP_EMISSARIO' -and [int]$_.dest_warp_id -eq 0
})
Assert-True ($exit.Count -eq 1 -and $entrance.Count -eq 1) 'Lago and Emissario do not have the approved reciprocal warp pair.'
Assert-True (@($lago.warp_events).Count -eq 5) 'Lago must contain the four existing warps plus the Emissario entrance.'

$layout = @($layouts.layouts | Where-Object { $_.id -eq 'LAYOUT_EMISSARIO' })
Assert-True ($layout.Count -eq 1) 'LAYOUT_EMISSARIO is missing or duplicated.'
Assert-True ([int]$layout[0].width -eq 32 -and [int]$layout[0].height -eq 30) 'Emissario must retain the approved 32x30 blockout dimensions.'
Assert-True ($layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_Cisternoni') 'Emissario must reuse General + Cisternoni.'
Assert-True ($layouts.layouts[-1].id -eq 'LAYOUT_EMISSARIO') 'LAYOUT_EMISSARIO must remain append-only at the end of layouts.json.'
Assert-True (@($groups.gMapGroup_Dungeons | Where-Object { $_ -eq 'Emissario' }).Count -eq 1) 'Emissario is not registered exactly once in gMapGroup_Dungeons.'
Assert-True ($groups.gMapGroup_Dungeons[-1] -eq 'Emissario') 'Emissario must remain append-only at the end of gMapGroup_Dungeons.'

$mapPath = Join-Path $RepositoryRoot 'data/layouts/Emissario/map.bin'
$borderPath = Join-Path $RepositoryRoot 'data/layouts/Emissario/border.bin'
$mapBytes = [IO.File]::ReadAllBytes($mapPath)
$borderBytes = [IO.File]::ReadAllBytes($borderPath)
Assert-True ($mapBytes.Length -eq 32 * 30 * 2) 'Emissario map.bin must be 32x30.'
Assert-True ($borderBytes.Length -eq 8) 'Emissario border.bin must contain four metatiles.'

$approvedRaw = @(0x0491, 0x32C3, 0x1170, 0x10A1, 0x3024)
$expectedCounts = @{}
$expectedCounts[[int]0x0491] = 360
$expectedCounts[[int]0x32C3] = 375
$expectedCounts[[int]0x1170] = 218
$expectedCounts[[int]0x10A1] = 6
$expectedCounts[[int]0x3024] = 1
$actualCounts = @{}
foreach ($raw in $approvedRaw) { $actualCounts[[int]$raw] = 0 }

$generalAttributes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/primary/general/metatile_attributes.bin'))
$cisternoniAttributes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/secondary/cisternoni/metatile_attributes.bin'))
$maxGlobalMetatile = 0x200 + ($cisternoniAttributes.Length / 2) - 1

for ($cell = 0; $cell -lt 32 * 30; $cell++) {
    $raw = [int][BitConverter]::ToUInt16($mapBytes, $cell * 2)
    Assert-True ($approvedRaw -contains $raw) "Unexpected Emissario raw metatile 0x$('{0:X4}' -f $raw) at cell $cell."
    $actualCounts[$raw]++

    $metatile = $raw -band 0x3FF
    Assert-True ($metatile -le $maxGlobalMetatile) "Emissario metatile is outside the loaded tileset at cell $cell."
    if ($metatile -lt 0x200) {
        $attribute = [BitConverter]::ToUInt16($generalAttributes, $metatile * 2)
    } else {
        $attribute = [BitConverter]::ToUInt16($cisternoniAttributes, ($metatile - 0x200) * 2)
    }
    $behavior = $attribute -band 0xFF
    Assert-True ($behavior -notin 0x11, 0x12) 'The visual water marker must not use interior/deep-water behavior before the Dive map exists.'
}

foreach ($raw in $approvedRaw) {
    Assert-True ($actualCounts[[int]$raw] -eq $expectedCounts[[int]$raw]) "Unexpected count for Emissario raw metatile 0x$('{0:X4}' -f $raw)."
}
for ($cell = 0; $cell -lt 4; $cell++) {
    Assert-True ([BitConverter]::ToUInt16($borderBytes, $cell * 2) -eq 0x0091) 'Emissario border must use the cave-rock metatile ID without map-cell collision bits.'
}

function Read-Raw([byte[]]$Bytes, [int]$Width, [int]$X, [int]$Y) {
    [BitConverter]::ToUInt16($Bytes, 2 * (($Y * $Width) + $X))
}

Assert-True ((Read-Raw $mapBytes 32 15 29) -eq 0x3024) 'The south return tile must use the normal MB_SOUTH_ARROW_WARP metatile on the southernmost map row.'
Assert-True ((Read-Raw $mapBytes 32 15 28) -eq 0x32C3) 'The return warp must have a walkable interior approach immediately to the north.'
Assert-True ((Read-Raw $mapBytes 32 15 0) -eq 0x1170) 'The northern water outlet is missing.'
Assert-True ((Read-Raw $mapBytes 32 15 13) -eq 0x1170) 'The central Surf basin is missing.'
Assert-True ((Read-Raw $mapBytes 32 15 6) -eq 0x10A1 -and (Read-Raw $mapBytes 32 16 8) -eq 0x10A1) 'The future-depth visual marker is incomplete.'
Assert-True ([BitConverter]::ToUInt16($generalAttributes, 0x024 * 2) -eq 0x0065) 'The Emissario return tile must retain MB_SOUTH_ARROW_WARP.'
Assert-True ([BitConverter]::ToUInt16($generalAttributes, 0x0A1 * 2) -eq 0x1010) 'The visual marker must remain ordinary pond water.'

$lagoBytes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/LagoDiAlbera/map.bin'))
$lagoDoor = Read-Raw $lagoBytes 120 81 3
Assert-True ($lagoDoor -eq 0x3291) 'The existing Lago Emissario door tile at (81,3) changed.'
$lagoAttributes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/secondary/lago_di_albera/metatile_attributes.bin'))
Assert-True ([BitConverter]::ToUInt16($lagoAttributes, 0x091 * 2) -eq 0x1060) 'The Lago entrance must retain MB_NON_ANIMATED_DOOR.'

Assert-True ($scripts -match '(?m)^Emissario_MapScripts::\s*$' -and $scripts -match '(?m)^\s*\.byte 0\s*$') 'Emissario must retain an empty map-script table in the structural batch.'
Assert-True ($eventScripts.Contains('.include "data/maps/Emissario/scripts.inc"')) 'Emissario scripts must be linked into the Emerald event-script aggregate.'
Assert-True ($scripts -notmatch 'trainerbattle|Aurea|Eco|Dive|setdivewarp') 'Narrative, battle, and Dive scripts are outside the structural blockout.'
Assert-True (@($wild.wild_encounter_groups.encounters | Where-Object { $_.map -eq 'MAP_EMISSARIO' }).Count -eq 0) 'Emissario must not have wild encounters in this version.'

Write-Output 'Emissario structural blockout validation passed.'
