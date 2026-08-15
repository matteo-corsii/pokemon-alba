param(
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

function Read-Json([string]$RelativePath) {
    return [IO.File]::ReadAllText((Join-Path $RepositoryRoot $RelativePath), [Text.Encoding]::UTF8) | ConvertFrom-Json
}

function Read-Text([string]$RelativePath) {
    return [IO.File]::ReadAllText((Join-Path $RepositoryRoot $RelativePath), [Text.Encoding]::UTF8)
}

function Get-BaseJson([string]$RelativePath) {
    return ((& git -C $RepositoryRoot show "develop:$RelativePath") -join "`n") | ConvertFrom-Json
}

function Get-Route101MapCell([int]$X, [int]$Y) {
    $mapBin = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/Route101/map.bin'))
    Assert-True ($mapBin.Length -eq 1440) 'Route101 map.bin size changed unexpectedly.'
    return [BitConverter]::ToUInt16($mapBin, 2 * (($Y * 36) + $X))
}

$allowedPaths = @(
    'data/maps/OldaleTown/scripts.inc',
    'data/maps/OldaleTown_House1/scripts.inc',
    'data/maps/OldaleTown_House2/scripts.inc',
    'data/maps/OldaleTown_Mart/scripts.inc',
    'data/maps/OldaleTown_PokemonCenter_1F/scripts.inc',
    'data/layouts/Route101/map.bin',
    'data/maps/Route101/map.json',
    'data/maps/Route101/scripts.inc',
    'data/layouts/OldaleTown/map.bin',
    'data/layouts/layouts.json',
    'data/tilesets/secondary/porta_pretoria/tiles.png',
    'data/tilesets/secondary/porta_pretoria/metatiles.bin',
    'data/tilesets/secondary/porta_pretoria/metatile_attributes.bin',
    'data/tilesets/secondary/porta_pretoria/palettes/00.pal',
    'data/tilesets/secondary/porta_pretoria/palettes/01.pal',
    'data/tilesets/secondary/porta_pretoria/palettes/02.pal',
    'data/tilesets/secondary/porta_pretoria/palettes/03.pal',
    'data/tilesets/secondary/porta_pretoria/palettes/04.pal',
    'data/tilesets/secondary/porta_pretoria/palettes/05.pal',
    'data/tilesets/secondary/porta_pretoria/palettes/06.pal',
    'data/tilesets/secondary/porta_pretoria/palettes/07.pal',
    'data/tilesets/secondary/porta_pretoria/palettes/08.pal',
    'data/tilesets/secondary/porta_pretoria/palettes/09.pal',
    'data/tilesets/secondary/porta_pretoria/palettes/10.pal',
    'data/tilesets/secondary/porta_pretoria/palettes/11.pal',
    'data/tilesets/secondary/porta_pretoria/palettes/12.pal',
    'data/tilesets/secondary/porta_pretoria/palettes/13.pal',
    'data/tilesets/secondary/porta_pretoria/palettes/14.pal',
    'data/tilesets/secondary/porta_pretoria/palettes/15.pal',
    'include/tilesets.h',
    'src/data/tilesets/graphics.h',
    'src/data/tilesets/headers.h',
    'src/data/tilesets/metatiles.h',
    'src/field_door.c',
    'test/validate_albera_storica_blockout.ps1',
    'test/validate_porta_pretoria_dedicated_tileset.ps1',
    'test/validate_porta_pretoria_localization.ps1'
)
$changedPaths = @(& git -C $RepositoryRoot diff --name-only develop)
$changedPaths += @(& git -C $RepositoryRoot ls-files --others --exclude-standard)
foreach ($changedPath in $changedPaths) {
    Assert-True ($allowedPaths -contains $changedPath) "Unexpected file changed by Porta Pretoria localization: $changedPath"
}

$house1 = Read-Text 'data/maps/OldaleTown_House1/scripts.inc'
$house2 = Read-Text 'data/maps/OldaleTown_House2/scripts.inc'
$center = Read-Text 'data/maps/OldaleTown_PokemonCenter_1F/scripts.inc'
$mart = Read-Text 'data/maps/OldaleTown_Mart/scripts.inc'
$town = Read-Text 'data/maps/OldaleTown/scripts.inc'
$routeScript = Read-Text 'data/maps/Route101/scripts.inc'

foreach ($entry in @(
    @($house1, "Prima di proseguire, controlla l'ordine", 'House 1 text missing.'),
    @($house1, 'Il primo POK', 'House 1 battle-order text missing.'),
    @($house2, 'Le pietre di Porta Pretoria conservano', 'House 2 Roman works text missing.'),
    @($house2, 'Da qui si passa verso Alb', 'House 2 Albèra Storica text missing.'),
    @($center, 'Molti viaggiatori si fermano qui prima', 'Pokémon Center traveler text missing.'),
    @($center, 'Da Via Verdi arrivano sempre scarpe', 'Pokémon Center Via Verdi text missing.'),
    @($mart, "Tra l'erba di Via Verdi, un ANTIDOTO", 'Mart Antidote text missing.'),
    @($town, 'Ad Alb', 'Secret Base mentor foreshadowing missing.'),
    @($town, 'crearsi un posto tutto loro.', 'Secret Base mentor foreshadowing is incomplete.'),
    @($routeScript, 'Route101_EventScript_AnticaVillaSign::', 'Villa sign script missing.'),
    @($routeScript, 'RESTI ROMANI', 'Villa sign title missing.'),
    @($routeScript, 'Antica Villa dei Cavallacci', 'Villa sign text missing.')
)) {
    Assert-True $entry[0].Contains($entry[1]) $entry[2]
}

$route = Read-Json 'data/maps/Route101/map.json'
$baseRoute = Get-BaseJson 'data/maps/Route101/map.json'
$villaSigns = @($route.bg_events | Where-Object {
    $_.type -eq 'sign' -and $_.x -eq 28 -and $_.y -eq 5 -and $_.elevation -eq 0 -and
    $_.player_facing_dir -eq 'BG_EVENT_PLAYER_FACING_ANY' -and $_.script -eq 'Route101_EventScript_AnticaVillaSign'
})
Assert-True ($villaSigns.Count -eq 1) 'Villa dei Cavallacci sign is missing or invalid.'
Assert-True ($route.bg_events.Count -eq $baseRoute.bg_events.Count) 'Route101 background event count changed unexpectedly.'
$villaSignMetatile = Get-Route101MapCell 28 5
Assert-True (($villaSignMetatile -band 0x03FF) -eq 0x0003) 'Villa dei Cavallacci must use the visible Route101 sign metatile.'
Assert-True ((($villaSignMetatile -shr 10) -band 3) -eq 0) 'Villa sign collision changed unexpectedly.'
Assert-True ((($villaSignMetatile -shr 12) -band 0xF) -eq 3) 'Villa sign elevation must match the surrounding path.'
Assert-True ((@($route.object_events | ConvertTo-Json -Depth 10 -Compress) -join "`n") -eq (@($baseRoute.object_events | ConvertTo-Json -Depth 10 -Compress) -join "`n")) 'Route101 object events changed unexpectedly.'
Assert-True ((@($route.coord_events | ConvertTo-Json -Depth 10 -Compress) -join "`n") -eq (@($baseRoute.coord_events | ConvertTo-Json -Depth 10 -Compress) -join "`n")) 'Route101 triggers changed unexpectedly.'
Assert-True (($route.connections | ConvertTo-Json -Depth 10 -Compress) -eq ($baseRoute.connections | ConvertTo-Json -Depth 10 -Compress)) 'Route101 connections changed unexpectedly.'
Assert-True (($route.warp_events | ConvertTo-Json -Depth 10 -Compress) -eq ($baseRoute.warp_events | ConvertTo-Json -Depth 10 -Compress)) 'Route101 warps changed unexpectedly.'

foreach ($path in @('include/constants/flags.h', 'include/constants/flags_frlg.h', 'include/constants/vars.h', 'include/constants/vars_frlg.h')) {
    & git -C $RepositoryRoot diff --quiet develop -- $path
    Assert-True ($LASTEXITCODE -eq 0) "Flag or variable file changed: $path"
}
Assert-True ($center.Contains('call Common_EventScript_PkmnCenterNurse')) 'Pokémon Center nurse service changed.'
Assert-True ($center.Contains('setrespawn HEAL_LOCATION_OLDALE_TOWN')) 'Pokémon Center respawn changed.'
Assert-True ($center.Contains('CableClub_OnResume')) 'Pokémon Center Wireless handling changed.'
Assert-True ($center.Contains('FLAG_SYS_POKEDEX_GET')) 'Pokémon Center conditional girl changed.'
Assert-True ($mart.Contains('pokemart OldaleTown_Mart_Pokemart_Basic')) 'Mart basic inventory changed.'
Assert-True ($mart.Contains('pokemart OldaleTown_Mart_Pokemart_Expanded')) 'Mart expanded inventory changed.'
Assert-True ($mart.Contains('FLAG_ADVENTURE_STARTED')) 'Mart progression handling changed.'

Write-Output 'Porta Pretoria localization validation passed.'
