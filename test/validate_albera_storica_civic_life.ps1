param([string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot))
$ErrorActionPreference = 'Stop'
function Assert-True([bool]$Condition, [string]$Message) { if (-not $Condition) { throw $Message } }
function Read-JsonFile([string]$Path) { Get-Content -Raw -Encoding utf8 (Join-Path $RepositoryRoot $Path) | ConvertFrom-Json }
function Read-Text([string]$Path) { Get-Content -Raw -Encoding utf8 (Join-Path $RepositoryRoot $Path) }
function Get-MapCell([byte[]]$MapBin, [int]$X, [int]$Y) { [BitConverter]::ToUInt16($MapBin, 2 * (($Y * 36) + $X)) }

$albera = Read-JsonFile 'data/maps/AlberaStorica/map.json'
$archive = Read-JsonFile 'data/maps/AlberaStorica_CivicArchive/map.json'
$shop = Read-JsonFile 'data/maps/AlberaStorica_SecretBaseShop/map.json'
$groups = Read-JsonFile 'data/maps/map_groups.json'
$flags = Read-Text 'include/constants/flags.h'
$flagsFrlg = Read-Text 'include/constants/flags_frlg.h'
$archiveScripts = Read-Text 'data/maps/AlberaStorica_CivicArchive/scripts.inc'
$shopScripts = Read-Text 'data/maps/AlberaStorica_SecretBaseShop/scripts.inc'
$alberaScripts = Read-Text 'data/maps/AlberaStorica/scripts.inc'

Assert-True (($groups.gMapGroup_IndoorOldale | Where-Object { $_ -eq 'AlberaStorica_CivicArchive' }).Count -eq 1) 'Archive map is not appended exactly once.'
Assert-True (($groups.gMapGroup_IndoorOldale | Where-Object { $_ -eq 'AlberaStorica_SecretBaseShop' }).Count -eq 1) 'Shop map is not appended exactly once.'
Assert-True ($archive.layout -eq 'LAYOUT_LILYCOVE_CITY_LILYCOVE_MUSEUM_1F') 'Archive must reuse the Lilycove Museum civic-archive donor layout.'
Assert-True ($shop.layout -eq 'LAYOUT_FORTREE_CITY_DECORATION_SHOP') 'Shop must reuse the decoration-shop layout.'
Assert-True ((@($albera.warp_events | Where-Object { $_.x -eq 6 -and $_.y -eq 7 -and $_.dest_map -eq 'MAP_ALBERA_STORICA_CIVIC_ARCHIVE' })).Count -eq 1) 'Archive exterior warp is invalid.'
Assert-True ((@($albera.warp_events | Where-Object { $_.x -eq 28 -and $_.y -eq 7 -and $_.dest_map -eq 'MAP_ALBERA_STORICA_SECRET_BASE_SHOP' })).Count -eq 1) 'Shop exterior warp is invalid.'
Assert-True (@($archive.warp_events | Where-Object { $_.dest_map -eq 'MAP_ALBERA_STORICA' -and $_.dest_warp_id -eq '2' }).Count -eq 2) 'Archive return warps are invalid.'
Assert-True ((@($archive.warp_events | Where-Object { $_.x -eq 9 -and $_.y -eq 13 -and $_.dest_map -eq 'MAP_ALBERA_STORICA' -and $_.dest_warp_id -eq '2' }).Count -eq 1) -and (@($archive.warp_events | Where-Object { $_.x -eq 10 -and $_.y -eq 13 -and $_.dest_map -eq 'MAP_ALBERA_STORICA' -and $_.dest_warp_id -eq '2' }).Count -eq 1)) 'Archive Museum donor entrance/return warps are invalid.'
Assert-True (@($archive.warp_events | Where-Object { $_.x -eq 16 -and $_.y -eq 1 }).Count -eq 0) 'Archive must not create a functional second-floor stair warp.'
Assert-True (@($shop.warp_events | Where-Object { $_.dest_map -eq 'MAP_ALBERA_STORICA' -and $_.dest_warp_id -eq '3' }).Count -eq 2) 'Shop return warps are invalid.'

$alberaMapBin = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/AlberaStorica/map.bin'))
Assert-True ($alberaMapBin.Length -eq (36 * 30 * 2)) 'Albera Storica layout size is invalid.'
$archiveEntrance = Get-MapCell $alberaMapBin 6 7
Assert-True (($archiveEntrance -band 0x03FF) -eq 0x287 -and ((($archiveEntrance -shr 10) -band 3) -eq 1) -and ((($archiveEntrance -shr 12) -band 0xF) -eq 0)) 'Archive door must use the walkable Porta Pretoria-compatible entrance metatile at (6,7).'
$archiveFacadeCells = @()
foreach ($y in 3..7) { foreach ($x in 4..9) { $archiveFacadeCells += Get-MapCell $alberaMapBin $x $y } }
Assert-True (@($archiveFacadeCells | Where-Object { (($_ -band 0x03FF) -ge 0x050) -and (($_ -band 0x03FF) -le 0x063) }).Count -eq 0) 'Archive exterior must not retain the Pokemon Center facade metatiles.'
Assert-True (@($archiveFacadeCells | Where-Object { ($_ -band 0x03FF) -in @(0x274, 0x275, 0x276, 0x27F, 0x287, 0x2F3, 0x2F4, 0x2FB) }).Count -ge 12) 'Archive exterior must use the approved Rustboro-derived civic facade family.'

$inventory = [regex]::Matches($shopScripts, '\.2byte (DECOR_[A-Z_]+)') | ForEach-Object { $_.Groups[1].Value }
$expectedInventory = @('DECOR_RAGGED_CHAIR','DECOR_RAGGED_DESK','DECOR_RED_PLANT','DECOR_TROPICAL_PLANT','DECOR_PRETTY_FLOWERS','DECOR_YELLOW_BRICK','DECOR_RED_BRICK')
Assert-True (($inventory -join ';') -eq ($expectedInventory -join ';')) 'Shop inventory must contain exactly the approved seven decorations.'
Assert-True ($shopScripts -notmatch 'C_LOW_NOTE_MAT|SAND_ORNAMENT|MUD_BALL|CAMP_DESK') 'Unapproved decoration added to Albera shop.'
Assert-True (@($shop.object_events).Count -eq 3) 'Shop must contain one shopkeeper and two flavor customers.'
Assert-True (@($archive.object_events).Count -eq 4) 'Archive must contain Lia and three civic NPCs.'
Assert-True ((@($archive.object_events | Where-Object { $_.local_id -eq 'LOCALID_ALBERA_ARCHIVE_LIA' -and $_.flag -eq 'FLAG_BADGE01_GET' })).Count -eq 1) 'Lia must be present before the Gym and absent after the Badge.'
Assert-True (@($archive.bg_events).Count -eq 3) 'Archive must contain exactly three optional document interactions.'
Assert-True ((@($archive.object_events | Where-Object { $_.x -eq 13 -and $_.y -eq 10 })).Count -eq 1) 'Lia must occupy the Museum consultation area.'
Assert-True ((@($archive.object_events | Where-Object { $_.x -eq 5 -and $_.y -eq 12 })).Count -eq 1) 'Archivist must occupy the public-facing Museum reception area.'
Assert-True ((@($archive.object_events | Where-Object { $_.x -eq 13 -and $_.y -eq 7 })).Count -eq 1) 'Scholar must occupy the Museum document area.'
Assert-True ((@($archive.object_events | Where-Object { $_.x -eq 3 -and $_.y -eq 8 })).Count -eq 1) 'Resident must occupy the separate Museum consultation area.'
Assert-True ((@($archive.bg_events | Where-Object { $_.x -eq 9 -and $_.y -eq 1 }).Count -eq 1) -and (@($archive.bg_events | Where-Object { $_.x -eq 17 -and $_.y -eq 9 }).Count -eq 1) -and (@($archive.bg_events | Where-Object { $_.x -eq 6 -and $_.y -eq 6 }).Count -eq 1)) 'Archive document interactions must map to Museum display points.'
Assert-True ($archiveScripts -match 'Lia' -and $archiveScripts -match 'Lirio' -and $archiveScripts -match 'prossimi rilevamenti') 'Lia archive handoff is incomplete.'

Assert-True ($flags -match '#define\s+FLAG_ALBERA_NICO_GYM_ENCOUNTER_COMPLETE\s+0x8EC\b') 'Missing Emerald Nico post-Gym completion flag.'
Assert-True ($flagsFrlg -match '#define\s+FLAG_ALBERA_NICO_GYM_ENCOUNTER_COMPLETE\s+0x8EC\b') 'Missing FRLG Nico post-Gym completion flag.'
$nico = @($albera.object_events | Where-Object { $_.local_id -eq 'LOCALID_ALBERA_STORICA_NICO' -and $_.flag -eq 'FLAG_ALBERA_NICO_GYM_ENCOUNTER_COMPLETE' })
Assert-True ($nico.Count -eq 1) 'Nico post-Gym object is missing or has the wrong persistence flag.'
Assert-True ((@($albera.object_events | Where-Object { $_.local_id -eq 'LOCALID_ALBERA_STORICA_ORIENTATION_NPC' -and $_.x -eq 33 -and $_.y -eq 11 }).Count -eq 1) -and (@($albera.object_events | Where-Object { $_.local_id -eq 'LOCALID_ALBERA_STORICA_PLAZA_NPC' -and $_.x -eq 18 -and $_.y -eq 14 }).Count -eq 1) -and (@($albera.object_events | Where-Object { $_.local_id -eq 'LOCALID_ALBERA_STORICA_GYM_NPC' -and $_.x -eq 19 -and $_.y -eq 9 }).Count -eq 1) -and ($nico[0].x -eq 18 -and $nico[0].y -eq 6)) 'Approved Albera exterior NPC repositioning changed.'
Assert-True ($alberaScripts -match '(?s)AlberaStorica_OnTransition:\s*goto_if_unset FLAG_BADGE01_GET, AlberaStorica_OnTransition_HideNico\s*goto_if_set FLAG_ALBERA_NICO_GYM_ENCOUNTER_COMPLETE, AlberaStorica_OnTransition_HideNico\s*showobjectat LOCALID_ALBERA_STORICA_NICO, MAP_ALBERA_STORICA\s*end\s*AlberaStorica_OnTransition_HideNico:\s*hideobjectat LOCALID_ALBERA_STORICA_NICO, MAP_ALBERA_STORICA\s*end') 'Nico transition visibility must be Badge-gated, one-time, and reversible before the encounter.'
Assert-True ($alberaScripts -match 'setflag FLAG_ALBERA_NICO_GYM_ENCOUNTER_COMPLETE' -and $alberaScripts -match 'removeobject LOCALID_ALBERA_STORICA_NICO') 'Nico encounter must be one-time.'
Assert-True ($alberaScripts -notmatch 'trainerbattle.*Nico') 'Nico post-Gym event must not introduce a battle.'
Assert-True (@($albera.coord_events | Where-Object { $_.x -in @(17,18) -and $_.y -eq 4 -and $_.script -eq 'AlberaStorica_EventScript_EnterAnfiteatro' }).Count -eq 2) 'Amphitheatre entrance triggers changed.'
Assert-True ((@($albera.bg_events | Where-Object { $_.type -eq 'secret_base' -and $_.x -eq 26 -and $_.y -eq 25 })).Count -eq 1) 'Secret Base tutorial tree changed.'
Assert-True ((@($albera.warp_events | Where-Object { $_.x -eq 15 -and $_.y -eq 26 -and $_.dest_map -eq 'MAP_ALBERA_STORICA_MENTORS_HOUSE' })).Count -eq 1) 'Mentor house entrance changed.'
Write-Output 'Albera Storica civic life validation passed.'
