param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'
function J($path) { Get-Content (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function A($condition, $message) { if (-not $condition) { throw $message } }
function T($path) { Get-Content (Join-Path $RepositoryRoot $path) -Raw }

$names = '1F', '2F', '3F', '4F'; $maps = @{}
foreach ($name in $names) { $map = J "data/maps/LagoDiAlbera_WaterGym_${name}/map.json"; A ($map.id -eq "MAP_LAGO_DI_ALBERA_WATER_GYM_${name}") "Map ID $name"; A ($map.layout -eq "LAYOUT_LAGO_DI_ALBERA_WATER_GYM_${name}") "Layout $name"; $maps[$name] = $map }
$lake = J 'data/maps/LagoDiAlbera/map.json'
A (@($lake.warp_events | Where-Object { $_.x -eq 71 -and $_.y -eq 72 -and $_.dest_map -eq 'MAP_LAGO_DI_ALBERA_WATER_GYM_1F' -and $_.dest_warp_id -eq '0' }).Count -eq 1) 'Gym entrance warp'
A (@($maps['1F'].warp_events | Where-Object { $_.x -eq 9 -and $_.y -eq 39 -and $_.dest_map -eq 'MAP_LAGO_DI_ALBERA' -and $_.dest_warp_id -eq '3' }).Count -eq 1) 'Gym exit warp'
A (@($maps['4F'].coord_events | Where-Object { $_.x -eq 9 -and $_.y -eq 2 -and $_.script -eq 'LagoDiAlbera_WaterGym_4F_EventScript_Slide' }).Count -eq 1) 'Slide trigger'
A (@($maps['1F'].warp_events | Where-Object { $_.x -eq 17 -and $_.y -eq 31 }).Count -eq 0) 'No slide return warp'

$expected = @(@{Map='1F';Id='LOCALID_LAGO_WATER_GYM_TRAINER_1';X=7;Y=21;Gfx='OBJ_EVENT_GFX_FISHERMAN';Trainer='TRAINER_TYPE_NORMAL'}, @{Map='2F';Id='LOCALID_LAGO_WATER_GYM_TRAINER_2';X=13;Y=17;Gfx='OBJ_EVENT_GFX_SWIMMER_F';Trainer='TRAINER_TYPE_NORMAL'}, @{Map='3F';Id='LOCALID_LAGO_WATER_GYM_TRAINER_3';X=6;Y=16;Gfx='OBJ_EVENT_GFX_SAILOR';Trainer='TRAINER_TYPE_NORMAL'}, @{Map='4F';Id='LOCALID_LAGO_WATER_GYM_LEADER';X=9;Y=5;Gfx='OBJ_EVENT_GFX_WINONA';Trainer='TRAINER_TYPE_NONE'})
foreach ($entry in $expected) { $event = @($maps[$entry.Map].object_events | Where-Object { $_.local_id -eq $entry.Id }); A ($event.Count -eq 1) "NPC $($entry.Id)"; A ($event[0].x -eq $entry.X -and $event[0].y -eq $entry.Y -and $event[0].graphics_id -eq $entry.Gfx -and $event[0].trainer_type -eq $entry.Trainer) "NPC data $($entry.Id)" }
$graphicsPointers = T 'src/data/object_events/object_event_graphics_info_pointers.h'
$commonGraphicsPointers = ($graphicsPointers -split '(?m)^#if IS_FRLG\s*$', 2)[0]
A ($commonGraphicsPointers -match '(?m)^\s*\[OBJ_EVENT_GFX_SWIMMER_F\]\s*=') 'Dalia sprite must be registered for Emerald.'
A ($commonGraphicsPointers -match '(?m)^\s*\[OBJ_EVENT_GFX_WINONA\]\s*=') 'Marina sprite must be registered for Emerald.'
A (-not ($commonGraphicsPointers -match '(?m)^\s*\[OBJ_EVENT_GFX_SWIMMER_F_LAND\]\s*=')) 'FRLG-only Dalia sprite leaked into the common graphics table.'

$scripts = @('1F','2F','3F','4F' | ForEach-Object { T "data/maps/LagoDiAlbera_WaterGym_${_}/scripts.inc" }) -join "`n"
foreach ($symbol in 'TRAINER_LAGO_WATER_GYM_REMO','TRAINER_LAGO_WATER_GYM_DALIA','TRAINER_LAGO_WATER_GYM_NEREO','TRAINER_LAGO_WATER_GYM_MARINA','FLAG_BADGE02_GET','ITEM_TM_WATER_PULSE','FLAG_RECEIVED_TM_WATER_PULSE') { A ($scripts.Contains($symbol)) "Gym script missing $symbol" }
$lakeScripts = T 'data/maps/LagoDiAlbera/scripts.inc'
foreach ($symbol in 'ITEM_HM_SURF','FLAG_RECEIVED_HM_SURF','FLAG_BADGE02_GET','LagoDiAlbera_EventScript_LauroSurfScene') { A ($lakeScripts.Contains($symbol)) "Lauro scene missing $symbol" }
A (@($lake.object_events | Where-Object { $_.local_id -eq 'LOCALID_LAGO_DI_ALBERA_LAURO_SURF' -and $_.x -eq 70 -and $_.y -eq 75 }).Count -eq 1) 'Lauro object'
A (@($lake.coord_events | Where-Object { $_.x -eq 71 -and $_.y -eq 75 -and $_.script -eq 'LagoDiAlbera_EventScript_LauroSurfScene' }).Count -eq 1) 'Lauro scene trigger'
$fieldMove = T 'src/field_move.c'; A ($fieldMove -match 'IsFieldMoveUnlocked_Surf\(void\)[\s\S]*?FLAG_BADGE02_GET') 'Surf uses badge 2'
foreach ($party in 'src/data/trainers.party','src/data/trainers_frlg.party') { $data = T $party; foreach ($trainer in 'TRAINER_LAGO_WATER_GYM_REMO','TRAINER_LAGO_WATER_GYM_DALIA','TRAINER_LAGO_WATER_GYM_NEREO','TRAINER_LAGO_WATER_GYM_MARINA') { A ($data.Contains("=== $trainer ===")) "$party missing $trainer" } }
$emeraldOpponents = T 'include/constants/opponents.h'
foreach ($entry in @(@('TRAINER_LAGO_WATER_GYM_REMO',568),@('TRAINER_LAGO_WATER_GYM_DALIA',851),@('TRAINER_LAGO_WATER_GYM_NEREO',852),@('TRAINER_LAGO_WATER_GYM_MARINA',854))) { A ($emeraldOpponents -match "(?m)^#define\s+$($entry[0])\s+$($entry[1])$") "Emerald reused trainer ID $($entry[0])" }
A ($emeraldOpponents -match '(?m)^#define\s+TRAINERS_COUNT_EMERALD\s+864$') 'Emerald trainer count preserves the save layout'
A ($emeraldOpponents -match '(?m)^#define\s+MAX_TRAINERS_COUNT_EMERALD\s+864$') 'Emerald max trainer count preserves the save layout'
$frlgOpponents = T 'include/constants/opponents_frlg.h'
foreach ($entry in @(@('TRAINER_LAGO_WATER_GYM_REMO',633),@('TRAINER_LAGO_WATER_GYM_DALIA',634),@('TRAINER_LAGO_WATER_GYM_NEREO',635),@('TRAINER_LAGO_WATER_GYM_MARINA',636))) { A ($frlgOpponents -match "(?m)^#define\s+$($entry[0])\s+$($entry[1])$") "FRLG trainer ID $($entry[0])" }
A ($frlgOpponents -match '(?m)^#define\s+TRAINERS_COUNT_FRLG\s+637$') 'FRLG trainer count'
A ($frlgOpponents -match '(?m)^#define\s+MAX_TRAINERS_COUNT_FRLG\s+768$') 'FRLG max trainer count'
Write-Output 'Lago di Albera Water Gym validation passed.'
