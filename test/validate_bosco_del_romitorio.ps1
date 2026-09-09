$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$mapPath = Join-Path $root 'data/maps/BoscoDelRomitorio/map.json'
$scriptsPath = Join-Path $root 'data/maps/BoscoDelRomitorio/scripts.inc'
$map = Get-Content -Raw $mapPath | ConvertFrom-Json
$scripts = Get-Content -Raw $scriptsPath

function Assert-True([bool]$condition, [string]$message) {
    if (-not $condition) { throw "FAIL: $message" }
}

Assert-True ($map.id -eq 'MAP_BOSCO_DEL_ROMITORIO') 'map id'
Assert-True (@($map.warp_events | Where-Object { $_.x -eq 31 -and $_.y -eq 63 -and $_.dest_map -eq 'MAP_LAGO_DI_ALBERA' -and $_.dest_warp_id -eq 12 }).Count -eq 1) 'west warp'
Assert-True (@($map.warp_events | Where-Object { $_.x -eq 32 -and $_.y -eq 63 -and $_.dest_map -eq 'MAP_LAGO_DI_ALBERA' -and $_.dest_warp_id -eq 13 }).Count -eq 1) 'east warp'

$encounters = @(
    @{ id = 'LOCALID_BOSCO_DEL_ROMITORIO_CINGERM'; gfx = 'OBJ_EVENT_GFX_CINGERM'; x = 14; y = 24; script = 'BoscoDelRomitorio_EventScript_Cingerm'; flag = 'FLAG_HIDE_BOSCO_DEL_ROMITORIO_CINGERM' },
    @{ id = 'LOCALID_BOSCO_DEL_ROMITORIO_ARDEINO'; gfx = 'OBJ_EVENT_GFX_ARDEINO'; x = 29; y = 37; script = 'BoscoDelRomitorio_EventScript_Ardeino'; flag = 'FLAG_HIDE_BOSCO_DEL_ROMITORIO_ARDEINO' },
    @{ id = 'LOCALID_BOSCO_DEL_ROMITORIO_SERBRACE'; gfx = 'OBJ_EVENT_GFX_SERBRACE'; x = 50; y = 25; script = 'BoscoDelRomitorio_EventScript_Serbrace'; flag = 'FLAG_HIDE_BOSCO_DEL_ROMITORIO_SERBRACE' }
)
foreach ($expected in $encounters) {
    $event = @($map.object_events | Where-Object { $_.local_id -eq $expected.id })
    Assert-True ($event.Count -eq 1) "$($expected.id) count"
    Assert-True ($event[0].graphics_id -eq $expected.gfx -and $event[0].x -eq $expected.x -and $event[0].y -eq $expected.y -and $event[0].script -eq $expected.script -and $event[0].flag -eq $expected.flag) "$($expected.id) placement"
    Assert-True ($event[0].trainer_type -eq 'TRAINER_TYPE_NONE') "$($expected.id) trainer type"
}

$items = @(
    @{ x = 58; y = 56; item = 'ITEM_MIRACLE_SEED'; flag = 'FLAG_ITEM_BOSCO_DEL_ROMITORIO_MIRACLE_SEED' },
    @{ x = 16; y = 43; item = 'ITEM_TM09'; flag = 'FLAG_ITEM_BOSCO_DEL_ROMITORIO_TM09' },
    @{ x = 54; y = 20; item = 'ITEM_CHARCOAL'; flag = 'FLAG_ITEM_BOSCO_DEL_ROMITORIO_CHARCOAL' },
    @{ x = 27; y = 31; item = 'ITEM_MYSTIC_WATER'; flag = 'FLAG_ITEM_BOSCO_DEL_ROMITORIO_MYSTIC_WATER' }
)
foreach ($expected in $items) {
    $event = @($map.object_events | Where-Object { $_.x -eq $expected.x -and $_.y -eq $expected.y })
    Assert-True ($event.Count -eq 1 -and $event[0].graphics_id -eq 'OBJ_EVENT_GFX_ITEM_BALL' -and $event[0].trainer_sight_or_berry_tree_id -eq $expected.item -and $event[0].flag -eq $expected.flag -and $event[0].script -eq 'Common_EventScript_FindItem') "$($expected.item)"
}

$hermit = @($map.object_events | Where-Object { $_.x -eq 32 -and $_.y -eq 6 })
Assert-True ($hermit.Count -eq 1 -and $hermit[0].graphics_id -eq 'OBJ_EVENT_GFX_OLD_MAN' -and $hermit[0].movement_type -eq 'MOVEMENT_TYPE_FACE_DOWN' -and $hermit[0].script -eq 'BoscoDelRomitorio_EventScript_Hermit') 'hermit'
Assert-True (@($map.object_events | Where-Object { $_.trainer_type -ne 'TRAINER_TYPE_NONE' }).Count -eq 0) 'no trainers'
Assert-True ($map.bg_events.Count -eq 2) 'sign count'
Assert-True (@($map.bg_events | Where-Object { $_.x -eq 33 -and $_.y -eq 56 }).Count -eq 1) 'entrance sign'
Assert-True (@($map.bg_events | Where-Object { $_.x -eq 40 -and $_.y -eq 10 }).Count -eq 1) 'retreat sign'
Assert-True ($scripts -match 'VAR_STARTER_MON' -and $scripts -match 'FLAG_HIDE_BOSCO_DEL_ROMITORIO') 'starter visibility logic'
Assert-True ($scripts -notmatch 'wild_encounters') 'no fauna changes in scripts'
Write-Output 'PASS: BoscoDelRomitorio starter encounters, items, signs and NPC'
