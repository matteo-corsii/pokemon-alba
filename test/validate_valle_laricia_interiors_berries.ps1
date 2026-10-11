param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw -Encoding utf8 | ConvertFrom-Json }

$layouts = (Read-Json 'data/layouts/layouts.json').layouts
$groups = Read-Json 'data/maps/map_groups.json'
$outer = Read-Json 'data/maps/PonteValleLaricia/map.json'
$specs = @(
    @{ Name='ValleLaricia_Casale1'; Id='MAP_VALLE_LARICIA_CASALE1'; Layout='LAYOUT_VALLE_LARICIA_CASALE1'; Return='2'; X=20; Y=38; Resident='LOCALID_VALLE_LARICIA_CASALE1_RESIDENT'; Script='ValleLaricia_Casale1_EventScript_Resident'; Reward='ITEM_ORAN_BERRY'; Flag='FLAG_RECEIVED_VALLE_LARICIA_ORAN_BERRIES' },
    @{ Name='ValleLaricia_Casale2'; Id='MAP_VALLE_LARICIA_CASALE2'; Layout='LAYOUT_VALLE_LARICIA_CASALE2'; Return='3'; X=44; Y=35; Resident='LOCALID_VALLE_LARICIA_CASALE2_RESIDENT'; Script='ValleLaricia_Casale2_EventScript_Resident'; Reward='ITEM_PECHA_BERRY'; Flag='FLAG_RECEIVED_VALLE_LARICIA_PECHA_BERRIES' },
    @{ Name='ValleLaricia_CasettaAgricola'; Id='MAP_VALLE_LARICIA_CASETTA_AGRICOLA'; Layout='LAYOUT_VALLE_LARICIA_CASETTA_AGRICOLA'; Return='4'; X=43; Y=58; Resident='LOCALID_VALLE_LARICIA_CASETTA_AGRICOLA_RESIDENT'; Script='ValleLaricia_CasettaAgricola_EventScript_Resident'; Reward='ITEM_CHERI_BERRY'; Flag='FLAG_RECEIVED_VALLE_LARICIA_CHERI_BERRIES' }
)
foreach ($spec in $specs) {
    $map = Read-Json "data/maps/$($spec.Name)/map.json"
    $layout = @($layouts | Where-Object { $_.id -eq $spec.Layout })
    Assert-True ($map.id -eq $spec.Id -and $map.layout -eq $spec.Layout) "$($spec.Name) identity is invalid."
    Assert-True ($map.region_map_section -eq 'MAPSEC_PONTE_VALLE_LARICIA' -and $map.weather -eq 'WEATHER_NONE' -and $map.map_type -eq 'MAP_TYPE_INDOOR' -and $null -eq $map.connections) "$($spec.Name) metadata is invalid."
    Assert-True ($map.allow_cycling -eq $false -and $map.allow_escaping -eq $false -and $map.allow_running -eq $false -and $map.show_map_name -eq $false -and $map.battle_scene -eq 'MAP_BATTLE_SCENE_NORMAL') "$($spec.Name) indoor settings are invalid."
    Assert-True ($layout.Count -eq 1 -and [int]$layout[0].width -eq 10 -and [int]$layout[0].height -eq 9 -and $layout[0].primary_tileset -eq 'gTileset_Building' -and $layout[0].secondary_tileset -eq 'gTileset_GenericBuilding') "$($spec.Name) layout is invalid."
    Assert-True (([IO.File]::ReadAllBytes((Join-Path $RepositoryRoot $layout[0].blockdata_filepath))).Length -eq 180 -and ([IO.File]::ReadAllBytes((Join-Path $RepositoryRoot $layout[0].border_filepath))).Length -eq 8) "$($spec.Name) binary layout is invalid."
    Assert-True (@($groups.gMapGroup_IndoorOldale | Where-Object { $_ -eq $spec.Name }).Count -eq 1) "$($spec.Name) is not appended exactly once to IndoorOldale."
    Assert-True (@($map.object_events).Count -eq 1 -and $map.object_events[0].local_id -eq $spec.Resident -and $map.object_events[0].script -eq $spec.Script -and $map.object_events[0].trainer_type -eq 'TRAINER_TYPE_NONE') "$($spec.Name) resident is invalid."
    Assert-True (@($map.warp_events).Count -eq 2 -and @($map.warp_events | Where-Object { [int]$_.x -in 3,4 -and [int]$_.y -eq 8 -and [int]$_.elevation -eq 0 -and $_.dest_map -eq 'MAP_PONTE_VALLE_LARICIA' -and $_.dest_warp_id -eq $spec.Return }).Count -eq 2) "$($spec.Name) return warps are invalid."
    Assert-True (@($outer.warp_events | Where-Object { [int]$_.x -eq $spec.X -and [int]$_.y -eq $spec.Y -and [int]$_.elevation -eq 3 -and $_.dest_map -eq $map.id -and $_.dest_warp_id -eq '0' }).Count -eq 1) "$($spec.Name) external warp is invalid."
    $scripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot "data/maps/$($spec.Name)/scripts.inc") -Raw -Encoding utf8
    Assert-True ($scripts.Contains("$($spec.Script)::") -and $scripts.Contains("checkitemspace $($spec.Reward), 2") -and $scripts.Contains("giveitem $($spec.Reward), 2") -and $scripts.Contains("setflag $($spec.Flag)")) "$($spec.Name) reward script is incomplete."
    Assert-True ($scripts.IndexOf("checkitemspace $($spec.Reward), 2") -lt $scripts.IndexOf("giveitem $($spec.Reward), 2") -and $scripts.IndexOf("giveitem $($spec.Reward), 2") -lt $scripts.IndexOf("setflag $($spec.Flag)")) "$($spec.Name) reward flag ordering is unsafe."
}
$casale1 = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/ValleLaricia_Casale1/scripts.inc') -Raw -Encoding utf8
Assert-True ($casale1.Contains('FLAG_RECEIVED_WAILMER_PAIL') -and $casale1.Contains('checkitem ITEM_WAILMER_PAIL') -and $casale1.Contains('giveitem ITEM_WAILMER_PAIL')) 'Casale 1 does not safely handle the Wailmer Pail.'
$flags = @('include/constants/flags.h','include/constants/flags_frlg.h') | ForEach-Object { Get-Content -LiteralPath (Join-Path $RepositoryRoot $_) -Raw -Encoding utf8 }
foreach ($flag in @('FLAG_RECEIVED_VALLE_LARICIA_ORAN_BERRIES','FLAG_RECEIVED_VALLE_LARICIA_PECHA_BERRIES','FLAG_RECEIVED_VALLE_LARICIA_CHERI_BERRIES')) { Assert-True (@($flags | Where-Object { $_.Contains($flag) }).Count -eq 2) "$flag is not defined in both configurations." }
$berryHeader = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/berry.h') -Raw -Encoding utf8
foreach ($id in 1..22) { Assert-True ($berryHeader -match "BERRY_TREE_VALLE_LARICIA_SOIL_$id\s+$(89 + $id)(\D|$)") "Berry tree ID $id is invalid." }
$trees = @($outer.object_events | Where-Object { $_.graphics_id -eq 'OBJ_EVENT_GFX_BERRY_TREE' })
Assert-True ($trees.Count -eq 22 -and @($trees | Group-Object trainer_sight_or_berry_tree_id | Where-Object { $_.Count -ne 1 }).Count -eq 0) 'Berry Tree events must be unique.'
foreach ($tree in $trees) { Assert-True ($tree.script -eq 'BerryTreeScript' -and $tree.movement_type -eq 'MOVEMENT_TYPE_BERRY_TREE_GROWTH' -and $tree.flag -eq '0' -and $tree.trainer_type -eq 'TRAINER_TYPE_NONE') 'Berry Tree event is not initially empty/plantable.' }
Assert-True (@($outer.warp_events | Where-Object { [int]$_.x -eq 58 -and [int]$_.y -eq 5 }).Count -eq 0) 'Nemora must not receive a warp.'
Write-Output 'Valle Laricia interiors and berry farming: PASS'
