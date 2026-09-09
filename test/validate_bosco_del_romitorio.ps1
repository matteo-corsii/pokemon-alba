$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$mapPath = Join-Path $root 'data/maps/BoscoDelRomitorio/map.json'
$scriptsPath = Join-Path $root 'data/maps/BoscoDelRomitorio/scripts.inc'
$partyPath = Join-Path $root 'src/data/trainers.party'
$map = Get-Content -Raw $mapPath | ConvertFrom-Json
$scripts = Get-Content -Raw $scriptsPath
$party = Get-Content -Raw $partyPath

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
$trainers = @(
    @{ id = 'LOCALID_BOSCO_DEL_ROMITORIO_GUARDABOSCHI'; gfx = 'OBJ_EVENT_GFX_HIKER'; x = 3; y = 22; dir = 'MOVEMENT_TYPE_FACE_DOWN'; script = 'BoscoDelRomitorio_EventScript_Guardaboschi'; constant = 'TRAINER_MIKE_1'; party = @('Miciolo[\s\S]*?Level: 21', 'Molospsy[\s\S]*?Level: 22', 'Luscinco[\s\S]*?Level: 23') },
    @{ id = 'LOCALID_BOSCO_DEL_ROMITORIO_ESPLORATRICE'; gfx = 'OBJ_EVENT_GFX_LASS'; x = 9; y = 47; dir = 'MOVEMENT_TYPE_FACE_DOWN'; script = 'BoscoDelRomitorio_EventScript_Esploratrice'; constant = 'TRAINER_CINDY_2'; party = @('Molospsy[\s\S]*?Level: 20', 'Paludix[\s\S]*?Level: 21', 'Lumella[\s\S]*?Level: 22') },
    @{ id = 'LOCALID_BOSCO_DEL_ROMITORIO_SORGENTI'; gfx = 'OBJ_EVENT_GFX_FISHERMAN'; x = 36; y = 44; dir = 'MOVEMENT_TYPE_FACE_LEFT'; script = 'BoscoDelRomitorio_EventScript_Sorgenti'; constant = 'TRAINER_ANDREW'; party = @('Carpulus[\s\S]*?Level: 21', 'Lucinus[\s\S]*?Level: 22', 'Tritino[\s\S]*?Level: 23') },
    @{ id = 'LOCALID_BOSCO_DEL_ROMITORIO_CERCAROCHE'; gfx = 'OBJ_EVENT_GFX_HIKER'; x = 59; y = 37; dir = 'MOVEMENT_TYPE_FACE_DOWN'; script = 'BoscoDelRomitorio_EventScript_Cercarocce'; constant = 'TRAINER_LUCAS_2'; party = @('Cisternide[\s\S]*?Level: 21', 'Tinuncol[\s\S]*?Level: 22', 'Salampolla[\s\S]*?Level: 23') },
    @{ id = 'LOCALID_BOSCO_DEL_ROMITORIO_CUSTODE'; gfx = 'OBJ_EVENT_GFX_BLACK_BELT'; x = 59; y = 13; dir = 'MOVEMENT_TYPE_FACE_DOWN'; script = 'BoscoDelRomitorio_EventScript_CustodeRovine'; constant = 'TRAINER_RHETT'; party = @('Lucinus[\s\S]*?Level: 22', 'Salampolla[\s\S]*?Level: 23', 'Molospsy[\s\S]*?Level: 23') }
)
$trainerObjects = @($map.object_events | Where-Object { $_.trainer_type -eq 'TRAINER_TYPE_NORMAL' })
Assert-True ($trainerObjects.Count -eq 5) 'exactly five Bosco trainers'
foreach ($expected in $trainers) {
    $event = @($trainerObjects | Where-Object { $_.local_id -eq $expected.id })
    Assert-True ($event.Count -eq 1 -and $event[0].graphics_id -eq $expected.gfx -and $event[0].x -eq $expected.x -and $event[0].y -eq $expected.y -and $event[0].movement_type -eq $expected.dir -and $event[0].script -eq $expected.script -and $event[0].trainer_sight_or_berry_tree_id -eq '3') "$($expected.id) trainer placement"
    Assert-True ($scripts -match "trainerbattle_single $($expected.constant),") "$($expected.constant) battle script"
    $block = [regex]::Match($party, "(?ms)^=== $($expected.constant) ===\r?\n(.*?)(?=^=== |\z)").Value
    Assert-True ($block -and $block -notmatch 'Items:' -and $block -notmatch '@') "$($expected.constant) must have no custom items"
    foreach ($pattern in $expected.party) { Assert-True ($block -match $pattern) "$($expected.constant) missing $pattern" }
}
Assert-True ($map.bg_events.Count -eq 2) 'sign count'
Assert-True (@($map.bg_events | Where-Object { $_.x -eq 33 -and $_.y -eq 56 }).Count -eq 1) 'entrance sign'
Assert-True (@($map.bg_events | Where-Object { $_.x -eq 40 -and $_.y -eq 10 }).Count -eq 1) 'retreat sign'
Assert-True ($scripts -match 'VAR_STARTER_MON' -and $scripts -match 'FLAG_HIDE_BOSCO_DEL_ROMITORIO') 'starter visibility logic'
Assert-True ($scripts -notmatch 'wild_encounters') 'no fauna changes in scripts'
Write-Output 'PASS: BoscoDelRomitorio starter encounters, items, signs and NPC'
