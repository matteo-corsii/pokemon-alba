param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Read-Json([string]$path) { Get-Content (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }

$borgo = Read-Json 'data/maps/BorgoDiCastello/map.json'
$layouts = Read-Json 'data/layouts/layouts.json'
$groups = Read-Json 'data/maps/map_groups.json'
$eventScripts = Get-Content (Join-Path $RepositoryRoot 'data/event_scripts.s') -Raw

$interiors = @(
    @{ Name = 'BorgoDiCastello_PokemonCenter'; Map = 'MAP_BORGO_DI_CASTELLO_POKEMON_CENTER'; Layout = 'LAYOUT_BORGO_DI_CASTELLO_POKEMON_CENTER'; X = 22; Y = 43; Warp = 0; ReturnY = 8 },
    @{ Name = 'BorgoDiCastello_Mart'; Map = 'MAP_BORGO_DI_CASTELLO_MART'; Layout = 'LAYOUT_BORGO_DI_CASTELLO_MART'; X = 34; Y = 43; Warp = 1; ReturnY = 7 },
    @{ Name = 'BorgoDiCastello_House1'; Map = 'MAP_BORGO_DI_CASTELLO_HOUSE1'; Layout = 'LAYOUT_BORGO_DI_CASTELLO_HOUSE1'; X = 40; Y = 43; Warp = 2; ReturnY = 8 },
    @{ Name = 'BorgoDiCastello_House2'; Map = 'MAP_BORGO_DI_CASTELLO_HOUSE2'; Layout = 'LAYOUT_BORGO_DI_CASTELLO_HOUSE2'; X = 38; Y = 33; Warp = 3; ReturnY = 8 },
    @{ Name = 'BorgoDiCastello_House3'; Map = 'MAP_BORGO_DI_CASTELLO_HOUSE3'; Layout = 'LAYOUT_BORGO_DI_CASTELLO_HOUSE3'; X = 11; Y = 28; Warp = 4; ReturnY = 8 },
    @{ Name = 'BorgoDiCastello_House4'; Map = 'MAP_BORGO_DI_CASTELLO_HOUSE4'; Layout = 'LAYOUT_BORGO_DI_CASTELLO_HOUSE4'; X = 11; Y = 20; Warp = 5; ReturnY = 8 },
    @{ Name = 'BorgoDiCastello_RistoranteBelvedere'; Map = 'MAP_BORGO_DI_CASTELLO_RISTORANTE_BELVEDERE'; Layout = 'LAYOUT_BORGO_DI_CASTELLO_RISTORANTE_BELVEDERE'; X = 40; Y = 17; Warp = 6; ReturnY = 9 }
)

Assert-True (@($borgo.connections | Where-Object { $_.map -eq 'MAP_LAGO_DI_ALBERA' -and $_.direction -eq 'down' -and [int]$_.offset -eq -87 }).Count -eq 1) 'Lago/Borgo connection changed.'
Assert-True (@($borgo.warp_events).Count -eq $interiors.Count) 'Unexpected Borgo warp count.'
foreach ($expected in $interiors) {
    $map = Read-Json ("data/maps/{0}/map.json" -f $expected.Name)
    $layout = @($layouts.layouts | Where-Object { $_.id -eq $expected.Layout })
    Assert-True ($map.id -eq $expected.Map -and $map.layout -eq $expected.Layout -and $map.connections -eq $null) "$($expected.Name) map metadata is incorrect."
    Assert-True ($layout.Count -eq 1) "$($expected.Name) layout is not uniquely registered."
    $mapBin = Join-Path $RepositoryRoot $layout[0].blockdata_filepath
    $borderBin = Join-Path $RepositoryRoot $layout[0].border_filepath
    Assert-True ((Test-Path $mapBin) -and ((Get-Item $mapBin).Length -eq (2 * [int]$layout[0].width * [int]$layout[0].height))) "$($expected.Name) map.bin is missing or has the wrong size."
    Assert-True (Test-Path $borderBin) "$($expected.Name) border.bin is missing."
    Assert-True (@($groups.gMapGroup_IndoorOldale | Where-Object { $_ -eq $expected.Name }).Count -eq 1) "$($expected.Name) map group registration is incorrect."
    Assert-True ($eventScripts -match ([regex]::Escape("data/maps/$($expected.Name)/scripts.inc"))) "$($expected.Name) scripts are not globally included."
    Assert-True (@($borgo.warp_events | Where-Object { [int]$_.x -eq $expected.X -and [int]$_.y -eq $expected.Y -and $_.dest_map -eq $expected.Map -and [int]$_.dest_warp_id -eq 0 }).Count -eq 1) "$($expected.Name) external warp is incorrect."
    Assert-True (@($map.warp_events | Where-Object { $_.dest_map -eq 'MAP_BORGO_DI_CASTELLO' -and [int]$_.dest_warp_id -eq $expected.Warp }).Count -ge 1) "$($expected.Name) return warp is incorrect."
    Assert-True (@($map.coord_events).Count -eq 0 -and @($map.bg_events).Count -eq 0) "$($expected.Name) has unexpected non-warp events."
}

$pcScripts = Get-Content (Join-Path $RepositoryRoot 'data/maps/BorgoDiCastello_PokemonCenter/scripts.inc') -Raw
$martScripts = Get-Content (Join-Path $RepositoryRoot 'data/maps/BorgoDiCastello_Mart/scripts.inc') -Raw
Assert-True ($pcScripts -match 'CableClub_OnResume' -and $pcScripts -match 'Common_EventScript_PkmnCenterNurse') 'Pokemon Center vanilla services are incomplete.'
Assert-True ($martScripts -match 'pokemart BorgoDiCastello_Mart_Pokemart') 'Mart vanilla service is incomplete.'
Assert-True (@($borgo.bg_events | Where-Object { $_.type -eq 'sign' -and [int]$_.x -eq 26 -and [int]$_.y -eq 12 }).Count -eq 1) 'Villa Papale sign is missing.'
Assert-True (@($borgo.warp_events | Where-Object { $_.dest_map -match 'VILLA|CISTERNONI' }).Count -eq 0) 'Future Villa or Cisternoni exits must remain inactive.'
Write-Output 'Borgo di Castello interiors: PASS'
