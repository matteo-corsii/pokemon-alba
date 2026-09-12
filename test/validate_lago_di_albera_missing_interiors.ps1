param([string]$RepositoryRoot = (Resolve-Path "$PSScriptRoot\\.."))
$ErrorActionPreference = 'Stop'
function J([string]$Path) { Get-Content (Join-Path $RepositoryRoot $Path) -Raw -Encoding utf8 | ConvertFrom-Json }
function A([bool]$Condition, [string]$Message) { if (-not $Condition) { throw $Message } }

$outer = J 'data/maps/LagoDiAlbera/map.json'
$layouts = (J 'data/layouts/layouts.json').layouts
$groups = J 'data/maps/map_groups.json'
$eventScripts = Get-Content (Join-Path $RepositoryRoot 'data/event_scripts.s') -Raw -Encoding utf8
$specs = @(
    @{ Name='LagoDiAlbera_PokemonCenterWest';  Map='MAP_LAGO_DI_ALBERA_POKEMON_CENTER_WEST';  Layout='LAYOUT_LAGO_DI_ALBERA_POKEMON_CENTER_WEST';  X=3;   Y=97; Template='PokemonCenter_1F'; Width=14; Height=9; Primary='gTileset_Building'; Secondary='gTileset_PokemonCenter'; DoorX=7; DoorY=8; OuterWarp=5; Nurse=$true },
    @{ Name='LagoDiAlbera_PokemonCenterEast';  Map='MAP_LAGO_DI_ALBERA_POKEMON_CENTER_EAST';  Layout='LAYOUT_LAGO_DI_ALBERA_POKEMON_CENTER_EAST';  X=85;  Y=58; Template='PokemonCenter_1F'; Width=14; Height=9; Primary='gTileset_Building'; Secondary='gTileset_PokemonCenter'; DoorX=7; DoorY=8; OuterWarp=6; Nurse=$true },
    @{ Name='LagoDiAlbera_Mart';                Map='MAP_LAGO_DI_ALBERA_MART';                 Layout='LAYOUT_LAGO_DI_ALBERA_MART';                 X=114; Y=57; Template='Mart';             Width=11; Height=8; Primary='gTileset_Building'; Secondary='gTileset_Shop';          DoorX=3; DoorY=7; OuterWarp=7; Mart=$true },
    @{ Name='LagoDiAlbera_StiltHouse1';         Map='MAP_LAGO_DI_ALBERA_STILT_HOUSE_1';        Layout='LAYOUT_LAGO_DI_ALBERA_STILT_HOUSE_1';        X=86;  Y=79; Template='House1';           Width=10; Height=9; Primary='gTileset_Building'; Secondary='gTileset_GenericBuilding'; DoorX=3; DoorY=8; OuterWarp=8 },
    @{ Name='LagoDiAlbera_StiltHouse2';         Map='MAP_LAGO_DI_ALBERA_STILT_HOUSE_2';        Layout='LAYOUT_LAGO_DI_ALBERA_STILT_HOUSE_2';        X=86;  Y=68; Template='House1';           Width=10; Height=9; Primary='gTileset_Building'; Secondary='gTileset_GenericBuilding'; DoorX=3; DoorY=8; OuterWarp=9 },
    @{ Name='LagoDiAlbera_EastHouse1';          Map='MAP_LAGO_DI_ALBERA_EAST_HOUSE_1';         Layout='LAYOUT_LAGO_DI_ALBERA_EAST_HOUSE_1';         X=109; Y=36; Template='House1';           Width=10; Height=9; Primary='gTileset_Building'; Secondary='gTileset_GenericBuilding'; DoorX=3; DoorY=8; OuterWarp=10 },
    @{ Name='LagoDiAlbera_EastHouse2';          Map='MAP_LAGO_DI_ALBERA_EAST_HOUSE_2';         Layout='LAYOUT_LAGO_DI_ALBERA_EAST_HOUSE_2';         X=109; Y=24; Template='House1';           Width=10; Height=9; Primary='gTileset_Building'; Secondary='gTileset_GenericBuilding'; DoorX=3; DoorY=8; OuterWarp=11 }
)
$expectedNpcCounts = @{
    LagoDiAlbera_PokemonCenterWest = 3
    LagoDiAlbera_PokemonCenterEast = 3
    LagoDiAlbera_Mart = 3
    LagoDiAlbera_StiltHouse1 = 2
    LagoDiAlbera_StiltHouse2 = 2
    LagoDiAlbera_EastHouse1 = 2
    LagoDiAlbera_EastHouse2 = 2
}

A (@($outer.warp_events).Count -eq 12) 'Lago must retain five existing warps and add exactly seven interior warps.'
foreach ($s in $specs) {
    $map = J "data/maps/$($s.Name)/map.json"
    $layout = @($layouts | Where-Object id -eq $s.Layout)
    A ($map.id -eq $s.Map -and $map.layout -eq $s.Layout -and $map.map_type -eq 'MAP_TYPE_INDOOR') "$($s.Name) identity is incorrect."
    A ($map.region_map_section -eq 'MAPSEC_ALBERA_STORICA' -and $map.connections -eq $null) "$($s.Name) map metadata is incorrect."
    A ($layout.Count -eq 1 -and [int]$layout[0].width -eq $s.Width -and [int]$layout[0].height -eq $s.Height -and $layout[0].primary_tileset -eq $s.Primary -and $layout[0].secondary_tileset -eq $s.Secondary) "$($s.Name) layout differs from its vanilla template."
    A (@($groups.gMapGroup_IndoorOldale | Where-Object { $_ -eq $s.Name }).Count -eq 1) "$($s.Name) is not registered exactly once."
    $outerWarp = @($outer.warp_events | Where-Object { [int]$_.x -eq $s.X -and [int]$_.y -eq $s.Y -and $_.dest_map -eq $s.Map -and [int]$_.dest_warp_id -eq 0 })
    A ($outerWarp.Count -eq 1) "$($s.Name) outer warp is incorrect."
    A (@($map.warp_events | Where-Object { [int]$_.x -eq $s.DoorX -and [int]$_.y -eq $s.DoorY -and $_.dest_map -eq 'MAP_LAGO_DI_ALBERA' -and [int]$_.dest_warp_id -eq $s.OuterWarp }).Count -eq 1) "$($s.Name) primary return warp is incorrect."
    A (@($map.warp_events | Where-Object { $_.dest_map -eq 'MAP_LAGO_DI_ALBERA' -and [int]$_.dest_warp_id -eq $s.OuterWarp }).Count -eq 2) "$($s.Name) must retain both normal door tiles."
    $mapBin = Join-Path $RepositoryRoot "data/layouts/$($s.Name)/map.bin"
    $borderBin = Join-Path $RepositoryRoot "data/layouts/$($s.Name)/border.bin"
    $templateBin = Join-Path $RepositoryRoot "data/layouts/$($s.Template)/map.bin"
    $templateBorder = Join-Path $RepositoryRoot "data/layouts/$($s.Template)/border.bin"
    A ((Get-Item -LiteralPath $mapBin).Length -eq $s.Width * $s.Height * 2) "$($s.Name) map.bin has an invalid size."
    A ((Get-FileHash -LiteralPath $mapBin -Algorithm SHA256).Hash -eq (Get-FileHash -LiteralPath $templateBin -Algorithm SHA256).Hash) "$($s.Name) map.bin is not the declared vanilla template copy."
    A ((Get-FileHash -LiteralPath $borderBin -Algorithm SHA256).Hash -eq (Get-FileHash -LiteralPath $templateBorder -Algorithm SHA256).Hash) "$($s.Name) border.bin is not the declared vanilla template copy."
    $scriptPath = "data/maps/$($s.Name)/scripts.inc"
    $script = Get-Content (Join-Path $RepositoryRoot $scriptPath) -Raw -Encoding utf8
    A ($script -match "(?m)^$([regex]::Escape($s.Name))_MapScripts::") "$($s.Name) MapScripts label is missing."
    A ($eventScripts.Contains("data/maps/$($s.Name)/scripts.inc")) "$($s.Name) scripts are not included globally."
    A (@($map.object_events).Count -eq $expectedNpcCounts[$s.Name]) "$($s.Name) NPC count is incorrect."
    foreach ($object in @($map.object_events)) {
        A ([int]$object.x -ge 0 -and [int]$object.x -lt $s.Width -and [int]$object.y -ge 0 -and [int]$object.y -lt $s.Height) "$($s.Name) NPC is outside the layout."
        A (@($map.warp_events | Where-Object { [int]$_.x -eq [int]$object.x -and [int]$_.y -eq [int]$object.y }).Count -eq 0) "$($s.Name) NPC blocks a warp."
        A ($script -match "(?m)^$([regex]::Escape([string]$object.script))::") "$($s.Name) NPC script label is missing: $($object.script)."
        A ($object.trainer_type -eq 'TRAINER_TYPE_NONE' -and [int]$object.flag -eq 0) "$($s.Name) ambient NPC has gameplay state."
    }
}

$west = Get-Content (Join-Path $RepositoryRoot 'data/maps/LagoDiAlbera_PokemonCenterWest/scripts.inc') -Raw
$east = Get-Content (Join-Path $RepositoryRoot 'data/maps/LagoDiAlbera_PokemonCenterEast/scripts.inc') -Raw
$mart = Get-Content (Join-Path $RepositoryRoot 'data/maps/LagoDiAlbera_Mart/scripts.inc') -Raw
A ($west.Contains('Common_EventScript_PkmnCenterNurse') -and $east.Contains('Common_EventScript_PkmnCenterNurse')) 'Both Lago Pokemon Centers must retain the native nurse flow.'
A ($mart.Contains('pokemart LagoDiAlbera_Mart_Pokemart') -and $mart.Contains('pokemartlistend')) 'Lago Mart must retain a native Pokemart list.'

$lagoMetatiles = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/secondary/lago_di_albera/metatiles.bin'))
$lagoAttributes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/secondary/lago_di_albera/metatile_attributes.bin'))
foreach ($index in 0x96, 0xC9) {
    $metatileOffset = $index * 16
    $attributeOffset = $index * 2
    A (($lagoMetatiles[$metatileOffset..($metatileOffset + 15)] | Where-Object { $_ -ne 0 }).Count -gt 0) "Lago compatibility metatile 0x2$('{0:X2}' -f $index) is unavailable."
    A ($attributeOffset + 1 -lt $lagoAttributes.Length) "Lago compatibility metatile 0x2$('{0:X2}' -f $index) attributes are unavailable."
}
Write-Output 'Lago di Albera missing interiors: PASS'
