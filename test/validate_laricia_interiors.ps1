param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'
function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw -Encoding utf8 | ConvertFrom-Json }

$specs = @(
    @{ Name='Laricia_PokemonCenter'; Map='MAP_LARICIA_POKEMON_CENTER'; Layout='LAYOUT_LARICIA_POKEMON_CENTER'; X=12; Y=37; Secondary='gTileset_PokemonCenter'; W=14; H=9 },
    @{ Name='Laricia_Mart'; Map='MAP_LARICIA_MART'; Layout='LAYOUT_LARICIA_MART'; X=18; Y=47; Secondary='gTileset_Shop'; W=11; H=8 },
    @{ Name='Laricia_FraschettaPiazza'; Map='MAP_LARICIA_FRASCHETTA_PIAZZA'; Layout='LAYOUT_LARICIA_FRASCHETTA_PIAZZA'; X=12; Y=25; Secondary='gTileset_GenericBuilding'; W=17; H=9 },
    @{ Name='Laricia_FraschettaVicoli'; Map='MAP_LARICIA_FRASCHETTA_VICOLI'; Layout='LAYOUT_LARICIA_FRASCHETTA_VICOLI'; X=45; Y=46; Secondary='gTileset_GenericBuilding'; W=13; H=8 },
    @{ Name='Laricia_House1'; Map='MAP_LARICIA_HOUSE1'; Layout='LAYOUT_LARICIA_HOUSE1'; X=48; Y=25; Secondary='gTileset_GenericBuilding'; W=10; H=9 },
    @{ Name='Laricia_House2'; Map='MAP_LARICIA_HOUSE2'; Layout='LAYOUT_LARICIA_HOUSE2'; X=49; Y=37; Secondary='gTileset_GenericBuilding'; W=10; H=9 },
    @{ Name='Laricia_House3'; Map='MAP_LARICIA_HOUSE3'; Layout='LAYOUT_LARICIA_HOUSE3'; X=18; Y=56; Secondary='gTileset_GenericBuilding'; W=10; H=9 },
    @{ Name='Laricia_House4'; Map='MAP_LARICIA_HOUSE4'; Layout='LAYOUT_LARICIA_HOUSE4'; X=44; Y=56; Secondary='gTileset_GenericBuilding'; W=10; H=9 }
)
$layouts = (Read-Json 'data/layouts/layouts.json').layouts
$groups = Read-Json 'data/maps/map_groups.json'
$outside = Read-Json 'data/maps/Laricia/map.json'
Assert-True (@($outside.warp_events).Count -eq 8) 'Laricia must have exactly the eight civilian interior warps.'
Assert-True (@($outside.warp_events | Where-Object { [int]$_.x -eq 32 -and [int]$_.y -eq 10 }).Count -eq 0) 'Palazzo Chigi must not have a warp.'
Assert-True (@($layouts | Where-Object { $_.id -in $specs.Layout }).Count -eq 8) 'Laricia interior layout IDs must be unique and registered.'
Assert-True (@($groups.gMapGroup_IndoorOldale | Where-Object { $_ -in $specs.Name }).Count -eq 8) 'Laricia interiors must be appended once to IndoorOldale.'
foreach ($s in $specs) {
    $map = Read-Json ("data/maps/{0}/map.json" -f $s.Name)
    $layout = @($layouts | Where-Object { $_.id -eq $s.Layout })
    $outsideWarp = @($outside.warp_events | Where-Object { [int]$_.x -eq $s.X -and [int]$_.y -eq $s.Y -and $_.dest_map -eq $s.Map -and $_.dest_warp_id -eq '0' })
    Assert-True ($map.id -eq $s.Map -and $map.layout -eq $s.Layout) "$($s.Name) map identity is invalid."
    Assert-True ($map.region_map_section -eq 'MAPSEC_LARICIA' -and $map.map_type -eq 'MAP_TYPE_INDOOR' -and $map.connections -eq $null) "$($s.Name) map metadata is invalid."
    Assert-True ($layout.Count -eq 1 -and [int]$layout[0].width -eq $s.W -and [int]$layout[0].height -eq $s.H -and $layout[0].primary_tileset -eq 'gTileset_Building' -and $layout[0].secondary_tileset -eq $s.Secondary) "$($s.Name) layout is invalid."
    Assert-True (([IO.File]::ReadAllBytes((Join-Path $RepositoryRoot $layout[0].blockdata_filepath))).Length -eq ($s.W * $s.H * 2)) "$($s.Name) map.bin size is invalid."
    Assert-True ($outsideWarp.Count -eq 1) "$($s.Name) external warp is invalid."
    Assert-True (@($map.warp_events | Where-Object { $_.dest_map -eq 'MAP_LARICIA' -and $_.dest_warp_id -eq [string]$specs.IndexOf($s) }).Count -ge 1) "$($s.Name) return warp is invalid."
    Assert-True (@($map.object_events | Group-Object local_id | Where-Object { $_.Count -gt 1 }).Count -eq 0) "$($s.Name) has duplicate local IDs."
    $scripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot ("data/maps/{0}/scripts.inc" -f $s.Name)) -Raw
    foreach ($event in $map.object_events) { Assert-True ($scripts.Contains($event.script)) "$($s.Name) references a missing object script." }
}
$centerScripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/Laricia_PokemonCenter/scripts.inc') -Raw
$martScripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/Laricia_Mart/scripts.inc') -Raw
Assert-True ($centerScripts.Contains('Common_EventScript_PkmnCenterNurse') -and $centerScripts.Contains('LOCALID_LARICIA_POKEMON_CENTER_NURSE')) 'Pokemon Center nurse/healing is missing.'
Assert-True ($martScripts.Contains('pokemart Laricia_Mart_Pokemart')) 'Mart clerk/shop script is missing.'
Assert-True ($specs[2].Secondary -eq 'gTileset_GenericBuilding' -and $specs[3].Secondary -eq 'gTileset_GenericBuilding' -and $specs[2].Layout -ne $specs[3].Layout) 'Fraschette must use distinct GenericBuilding layouts.'
Write-Output 'Laricia interiors: PASS'
