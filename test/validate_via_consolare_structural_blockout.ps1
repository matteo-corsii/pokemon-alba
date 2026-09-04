param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
)

$ErrorActionPreference = 'Stop'

function Read-Json([string]$RelativePath) {
    $path = Join-Path $RepositoryRoot $RelativePath
    return Get-Content -LiteralPath $path -Raw | ConvertFrom-Json
}

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

$map = Read-Json 'data/maps/ViaConsolare/map.json'
$route = Read-Json 'data/maps/Route103/map.json'
$groups = Read-Json 'data/maps/map_groups.json'
$layouts = Read-Json 'data/layouts/layouts.json'
$wild = Read-Json 'src/data/wild_encounters.json'
$scripts = Get-Content (Join-Path $RepositoryRoot 'data/maps/ViaConsolare/scripts.inc') -Raw

Assert-True ($map.id -eq 'MAP_VIA_CONSOLARE') 'Via Consolare map id is incorrect.'
Assert-True ($map.layout -eq 'LAYOUT_VIA_CONSOLARE') 'Via Consolare layout id is incorrect.'
Assert-True ($map.map_type -eq 'MAP_TYPE_ROUTE') 'Via Consolare must be an outdoor route.'
$routeConnection = @($map.connections | Where-Object { $_.direction -eq 'right' -and $_.map -eq 'MAP_ROUTE103' -and [int]$_.offset -eq 0 })
$lakeConnection = @($map.connections | Where-Object { $_.direction -eq 'up' -and $_.map -eq 'MAP_LAGO_DI_ALBERA' -and [int]$_.offset -eq -31 })
Assert-True ($map.connections.Count -eq 2 -and $routeConnection.Count -eq 1 -and $lakeConnection.Count -eq 1) 'Via Consolare connections are incorrect.'
$leftMansioWarp = @($map.warp_events | Where-Object {
    $_.x -eq 5 -and $_.y -eq 21 -and
    $_.dest_map -eq 'MAP_VIA_CONSOLARE_MANSIO' -and
    $_.dest_warp_id -eq '0'
})
$rightMansioWarp = @($map.warp_events | Where-Object {
    $_.x -eq 12 -and $_.y -eq 21 -and
    $_.dest_map -eq 'MAP_VIA_CONSOLARE_MANSIO' -and
    $_.dest_warp_id -eq '1'
})
Assert-True (@($map.warp_events).Count -eq 2) 'Via Consolare must contain exactly two Mansio warps.'
Assert-True ($leftMansioWarp.Count -eq 1) 'Via Consolare left Mansio warp is incorrect.'
Assert-True ($rightMansioWarp.Count -eq 1) 'Via Consolare right Mansio warp is incorrect.'
Assert-True (@($map.object_events).Count -eq 9) 'Via Consolare must contain exactly nine object events.'
$leadObjects = @($map.object_events | Where-Object { $_.local_id -in 'LOCALID_VIA_CONSOLARE_LIA_EMISSARIO', 'LOCALID_VIA_CONSOLARE_NICO_EMISSARIO' })
Assert-True ($leadObjects.Count -eq 2) 'Via Consolare must contain Lia and Nico for the Emissario lead scene.'
Assert-True (@($leadObjects | Where-Object { $_.local_id -eq 'LOCALID_VIA_CONSOLARE_LIA_EMISSARIO' -and $_.x -eq 27 -and $_.y -eq 7 -and $_.elevation -eq 3 -and $_.movement_type -eq 'MOVEMENT_TYPE_FACE_RIGHT' -and $_.flag -eq 'FLAG_HIDE_VIA_CONSOLARE_LIA' }).Count -eq 1) 'Via Consolare Lia placement is incorrect.'
Assert-True (@($leadObjects | Where-Object { $_.local_id -eq 'LOCALID_VIA_CONSOLARE_NICO_EMISSARIO' -and $_.x -eq 27 -and $_.y -eq 8 -and $_.elevation -eq 3 -and $_.movement_type -eq 'MOVEMENT_TYPE_FACE_RIGHT' -and $_.flag -eq 'FLAG_HIDE_VIA_CONSOLARE_NICO' }).Count -eq 1) 'Via Consolare Nico placement is incorrect.'
$leadTriggers = @($map.coord_events | Where-Object { $_.script -eq 'ViaConsolare_EventScript_StartEmissarioLead' })
Assert-True (@($map.coord_events).Count -eq 6 -and $leadTriggers.Count -eq 6) 'Via Consolare must contain exactly the six Emissario lead triggers.'
foreach ($y in 7, 8) {
    foreach ($x in 28, 29, 30) {
        Assert-True (@($leadTriggers | Where-Object { $_.x -eq $x -and $_.y -eq $y -and $_.elevation -eq 3 }).Count -eq 1) "Missing Via Consolare Emissario trigger at ($x,$y)."
    }
}
Assert-True (@($map.bg_events).Count -eq 12) 'Via Consolare must contain exactly twelve background events.'
$expectedObjects = @(
    @{ x = 30; y = 5; movement = 'MOVEMENT_TYPE_FACE_DOWN'; trainer = 'TRAINER_TYPE_NONE'; script = 'ViaConsolare_EventScript_Custode' },
    @{ x = 41; y = 15; movement = 'MOVEMENT_TYPE_FACE_LEFT'; trainer = 'TRAINER_TYPE_NONE'; script = 'ViaConsolare_EventScript_Olivicoltrice' },
    @{ x = 26; y = 14; movement = 'MOVEMENT_TYPE_FACE_RIGHT'; trainer = 'TRAINER_TYPE_NONE'; script = 'ViaConsolare_EventScript_Caposquadra' },
    @{ x = 9; y = 8; movement = 'MOVEMENT_TYPE_FACE_RIGHT'; trainer = 'TRAINER_TYPE_NORMAL'; script = 'ViaConsolare_EventScript_Livio' },
    @{ x = 47; y = 20; movement = 'MOVEMENT_TYPE_FACE_RIGHT'; trainer = 'TRAINER_TYPE_NORMAL'; script = 'ViaConsolare_EventScript_Elio' }
)
foreach ($expected in $expectedObjects) {
    $found = @($map.object_events | Where-Object {
        $_.x -eq $expected.x -and $_.y -eq $expected.y -and
        $_.movement_type -eq $expected.movement -and $_.trainer_type -eq $expected.trainer -and
        $_.script -eq $expected.script
    })
    Assert-True ($found.Count -eq 1) ("Via Consolare object event is incorrect at $($expected.x),$($expected.y).")
}
$itemObjects = @($map.object_events | Where-Object { $_.graphics_id -eq 'OBJ_EVENT_GFX_ITEM_BALL' })
Assert-True ($itemObjects.Count -eq 2) 'Via Consolare must contain exactly two visible item balls.'
Assert-True (@($itemObjects | Where-Object { $_.x -eq 52 -and $_.y -eq 16 -and $_.trainer_sight_or_berry_tree_id -eq 'ITEM_SUPER_POTION' -and $_.flag -eq 'FLAG_ITEM_VIA_CONSOLARE_SUPER_POTION' }).Count -eq 1) 'Super Potion item is incorrect.'
Assert-True (@($itemObjects | Where-Object { $_.x -eq 13 -and $_.y -eq 11 -and $_.trainer_sight_or_berry_tree_id -eq 'ITEM_REPEL' -and $_.flag -eq 'FLAG_ITEM_VIA_CONSOLARE_REPEL' }).Count -eq 1) 'Repel item is incorrect.'
$hidden = @($map.bg_events | Where-Object { $_.type -eq 'hidden_item' })
Assert-True ($hidden.Count -eq 1 -and $hidden[0].x -eq 56 -and $hidden[0].y -eq 27 -and $hidden[0].item -eq 'ITEM_STARDUST' -and $hidden[0].flag -eq 'FLAG_HIDDEN_ITEM_VIA_CONSOLARE_STARDUST') 'Hidden Stardust is incorrect.'
$expectedBg = @(
    @{ x = 32; y = 5; script = 'ViaConsolare_EventScript_CippoNord' },
    @{ x = 1; y = 5; script = 'ViaConsolare_EventScript_CippoOccidentale' },
    @{ x = 14; y = 22; script = 'ViaConsolare_EventScript_CartelloMansio' },
    @{ x = 5; y = 25; script = 'ViaConsolare_EventScript_VascaMansio' },
    @{ x = 6; y = 25; script = 'ViaConsolare_EventScript_VascaMansio' },
    @{ x = 7; y = 25; script = 'ViaConsolare_EventScript_VascaMansio' },
    @{ x = 8; y = 25; script = 'ViaConsolare_EventScript_VascaMansio' },
    @{ x = 0; y = 6; script = 'ViaConsolare_EventScript_SbarramentoOccidentale' },
    @{ x = 0; y = 7; script = 'ViaConsolare_EventScript_SbarramentoOccidentale' },
    @{ x = 0; y = 8; script = 'ViaConsolare_EventScript_SbarramentoOccidentale' },
    @{ x = 0; y = 9; script = 'ViaConsolare_EventScript_SbarramentoOccidentale' }
)
foreach ($expected in $expectedBg) {
    Assert-True (@($map.bg_events | Where-Object { $_.type -eq 'sign' -and $_.x -eq $expected.x -and $_.y -eq $expected.y -and $_.script -eq $expected.script }).Count -eq 1) ("Via Consolare BG event is incorrect at $($expected.x),$($expected.y).")
}
Assert-True ($scripts.Contains('trainerbattle_single TRAINER_VIA_CONSOLARE_LIVIO') -and $scripts.Contains('trainerbattle_single TRAINER_VIA_CONSOLARE_ELIO')) 'Via Consolare trainer scripts are missing.'
Assert-True ($scripts.Contains('ViaConsolare_Text_Custode') -and $scripts.Contains('ViaConsolare_Text_Olivicoltrice') -and $scripts.Contains('ViaConsolare_Text_Caposquadra')) 'Via Consolare ambient NPC scripts are missing.'
Assert-True ($scripts.Contains('ViaConsolare_Text_VascaMansio') -and $scripts.Contains('ViaConsolare_Text_SbarramentoOccidentale')) 'Via Consolare interaction scripts are missing.'

$reverse = @($route.connections | Where-Object { $_.direction -eq 'left' -and $_.map -eq 'MAP_VIA_CONSOLARE' })
Assert-True ($reverse.Count -eq 1 -and [int]$reverse[0].offset -eq 0) 'Route103 reciprocal connection is missing or misaligned.'
Assert-True (@($route.connections | Where-Object { $_.direction -eq 'down' -and $_.map -eq 'MAP_OLDALE_TOWN' }).Count -eq 1) 'Route103 south connection was not preserved.'
Assert-True (@($route.warp_events | Where-Object { $_.dest_map -eq 'MAP_CISTERNONI' }).Count -eq 2) 'Route103 Cisternoni warps were not preserved.'
Assert-True (@($route.object_events | Where-Object { $_.local_id -eq 'LOCALID_ROUTE103_LIA' }).Count -eq 1) 'Route103 Lia was not preserved.'
Assert-True (@($route.object_events | Where-Object { $_.local_id -eq 'LOCALID_ROUTE103_NICO' }).Count -eq 1) 'Route103 Nico was not preserved.'
Assert-True (@($route.bg_events | Where-Object { $_.secret_base_id -eq 'SECRET_BASE_CISTERNONI_TREE_1' }).Count -eq 1) 'Route103 Secret Base tree was not preserved.'

$layout = @($layouts.layouts | Where-Object { $_.id -eq 'LAYOUT_VIA_CONSOLARE' })
Assert-True ($layout.Count -eq 1 -and [int]$layout[0].width -eq 60 -and [int]$layout[0].height -eq 30) 'Via Consolare layout dimensions are incorrect.'
Assert-True ($layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_ViaConsolare') 'Via Consolare tilesets are incorrect.'

$group = @($groups.gMapGroup_TownsAndRoutes | Where-Object { $_ -eq 'ViaConsolare' })
Assert-True ($group.Count -eq 1) 'ViaConsolare is not registered exactly once in map groups.'

$mapPath = Join-Path $RepositoryRoot 'data/layouts/ViaConsolare/map.bin'
Assert-True ((Get-Item $mapPath).Length -eq (60 * 30 * 2)) 'Via Consolare map.bin size does not match 60x30.'
git -C $RepositoryRoot diff --quiet develop -- data/layouts/ViaConsolare/border.bin
Assert-True ($LASTEXITCODE -eq 0) 'Via Consolare binary layout files changed.'
git -C $RepositoryRoot diff --quiet develop -- data/layouts/Route103/map.bin data/layouts/Route103/border.bin data/maps/Route103/map.json
Assert-True ($LASTEXITCODE -eq 0) 'Route103 was modified.'
# The Mansio exit tiles intentionally use the Condominium's south-warp blocks;
# their exact raw values and behaviors are checked by validate_mansio_consolare_structural_blockout.ps1.

$viaWild = @($wild.wild_encounter_groups.encounters | Where-Object { $_.map -eq 'MAP_VIA_CONSOLARE' })
Assert-True ($viaWild.Count -eq 4) 'Via Consolare must have four time-based encounter tables.'

Write-Output 'Via Consolare structural blockout: PASS'
