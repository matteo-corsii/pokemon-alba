param(
    [string]$BaseRef = 'develop'
)

$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

function Get-ChangedPaths {
    $paths = @()
    $paths += git diff --name-only "$BaseRef...HEAD"
    $paths += git diff --name-only
    $paths += git diff --cached --name-only
    $paths += git ls-files --others --exclude-standard
    return @($paths | Where-Object { $_ } | Sort-Object -Unique)
}

function Read-JsonFile([string]$Path) {
    return Get-Content -Raw -Encoding utf8 $Path | ConvertFrom-Json
}

function Get-Cell([byte[]]$MapBin, [int]$X, [int]$Y) {
    return [BitConverter]::ToUInt16($MapBin, 2 * (($Y * 36) + $X))
}

$allowedPaths = @(
    'data/layouts/LittlerootTown/map.bin',
    'data/layouts/Route101/map.bin',
    'data/maps/OldaleTown/map.json',
    'data/maps/Route101/map.json',
    'data/maps/LittlerootTown/map.json',
    'data/maps/map_groups.json',
    'data/maps/AlberaBassa_Condominium1/map.json',
    'data/maps/AlberaBassa_Condominium1/scripts.inc',
    'data/maps/AlberaBassa_Condominium2/map.json',
    'data/maps/AlberaBassa_Condominium2/scripts.inc',
    'data/maps/AlberaBassa_Condominium3/map.json',
    'data/maps/AlberaBassa_Condominium3/scripts.inc',
    'data/event_scripts.s',
    'test/validate_albera_bassa_school.ps1',
    'test/validate_albera_bassa_condominiums.ps1',
    'test/validate_porta_pretoria_dedicated_tileset.ps1',
    'test/validate_porta_pretoria_localization.ps1',
    'test/validate_via_verdi_ambient_npc.ps1',
    'test/validate_albera_bassa_residential_blockout.ps1'
)
$allowedPaths += @(
    'docs/AUSONIA_REGIONAL_DEX_PLAN.md', 'include/constants/pokedex.h', 'include/constants/species.h',
    'src/data/graphics/pokemon.h', 'src/data/pokemon/all_learnables.json', 'src/data/pokemon/egg_moves.h',
    'src/data/pokemon/pokedex_orders.h', 'src/data/pokemon/species_info.h', 'test/species.c',
    'test/validate_albera_amphitheatre_gym.ps1', 'test/validate_early_ausonia_fauna_batch_b.ps1',
    'test/validate_early_ausonia_fauna_batch_c.ps1', 'test/validate_early_ausonia_fauna_batch_d.ps1',
    'test/validate_early_ausonia_graphics_batch_a.ps1', 'test/validate_early_ausonia_graphics_batch_b.ps1',
    'test/validate_early_ausonia_graphics_batch_c.ps1', 'test/validate_early_ausonia_graphics_batch_d.ps1',
    'test/validate_lago_di_albera_tileset.ps1', 'test/validate_lenghelis.ps1', 'test/validate_lumella_omphalux.ps1',
    'test/validate_luscinco_luscerp.ps1', 'test/validate_molospsy.ps1', 'test/validate_paludix_sanguilex.ps1',
    'test/validate_salampolla_alchimandra.ps1', 'test/validate_tritino_tricrest.ps1',
    'test/validate_via_verdi_first_investigation.ps1', 'test/validate_cingerm_graphics.ps1'
)
$allowedPaths += @(Get-ChildItem 'graphics/pokemon/tritino','graphics/pokemon/tricrest','graphics/pokemon/salampolla','graphics/pokemon/alchimandra' -File | ForEach-Object { $_.FullName.Substring((Get-Location).Path.Length + 1).Replace('\','/') })
$allowedPaths += @('test/validate_albera_first_playable_segment.ps1','test/validate_cingerm_starter.ps1','test/validate_full_ausonia_starter_trio.ps1','test/validate_italian_menu_localization.ps1','test/validate_cisternide_calcistern.ps1','graphics/pokemon/cisternide/anim_front.png','graphics/pokemon/cisternide/back.png','graphics/pokemon/cisternide/icon.png','graphics/pokemon/cisternide/normal.pal','graphics/pokemon/cisternide/shiny.pal','graphics/pokemon/calcistern/anim_front.png','graphics/pokemon/calcistern/back.png','graphics/pokemon/calcistern/icon.png','graphics/pokemon/calcistern/normal.pal','graphics/pokemon/calcistern/shiny.pal','graphics/pokemon/luscinco/anim_front.png','graphics/pokemon/luscinco/back.png','graphics/pokemon/luscinco/icon.png','graphics/pokemon/luscinco/normal.pal','graphics/pokemon/luscinco/shiny.pal','graphics/pokemon/luscerp/anim_front.png','graphics/pokemon/luscerp/back.png','graphics/pokemon/luscerp/icon.png','graphics/pokemon/luscerp/normal.pal','graphics/pokemon/luscerp/shiny.pal')
$unexpectedPaths = @(Get-ChangedPaths | Where-Object { $_ -notin $allowedPaths })
Assert-True ($unexpectedPaths.Count -eq 0) ('Out-of-scope files changed: ' + ($unexpectedPaths -join ', '))

$town = Read-JsonFile 'data/maps/LittlerootTown/map.json'
$oldale = Read-JsonFile 'data/maps/OldaleTown/map.json'
$layouts = Read-JsonFile 'data/layouts/layouts.json'
$layout = @($layouts.layouts | Where-Object { $_.id -eq 'LAYOUT_LITTLEROOT_TOWN' })
Assert-True ($layout.Count -eq 1) 'LittlerootTown layout must be unique.'
Assert-True ($layout[0].width -eq 36 -and $layout[0].height -eq 32) 'Albèra Bassa layout must remain 36x32.'
Assert-True ($layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_Petalburg') 'Albèra Bassa tilesets changed.'

$mapBin = [IO.File]::ReadAllBytes('data/layouts/LittlerootTown/map.bin')
Assert-True ($mapBin.Length -eq (36 * 32 * 2)) 'Albèra Bassa map.bin size changed.'

$housePattern = @(
    @(0x3208, 0x3209, 0x3209, 0x3209, 0x320A),
    @(0x0610, 0x0611, 0x0611, 0x0611, 0x0612),
    @(0x0618, 0x0619, 0x0619, 0x0619, 0x061A),
    @(0x0622, 0x0632, 0x0630, 0x0640, 0x0621),
    @(0x062A, 0x063A, 0x0638, 0x0648, 0x0629)
)
$footprints = @(
    @(2, 22),
    @(16, 22),
    @(28, 22)
)
foreach ($footprint in $footprints) {
    $startX = $footprint[0]
    $startY = $footprint[1]
    for ($row = 0; $row -lt 5; $row++) {
        for ($column = 0; $column -lt 5; $column++) {
            Assert-True ((Get-Cell $mapBin ($startX + $column) ($startY + $row)) -eq $housePattern[$row][$column]) "Residential footprint at ($startX,$startY) is not the proven Petalburg house pattern."
        }
    }
}

foreach ($eventCollection in @('object_events', 'warp_events', 'coord_events', 'bg_events')) {
    foreach ($event in @($town.$eventCollection)) {
        foreach ($footprint in $footprints) {
            $isApprovedCondominiumDoor = $eventCollection -eq 'warp_events' -and (
                ($event.x -eq 5 -and $event.y -eq 26) -or
                ($event.x -eq 19 -and $event.y -eq 26) -or
                ($event.x -eq 31 -and $event.y -eq 26)
            )
            Assert-True ($isApprovedCondominiumDoor -or -not ($event.x -ge $footprint[0] -and $event.x -le ($footprint[0] + 4) -and $event.y -ge $footprint[1] -and $event.y -le ($footprint[1] + 4))) "A $eventCollection event overlaps the residential footprint at ($($footprint[0]),$($footprint[1]))."
        }
    }
}

Assert-True (@($town.warp_events | Where-Object { $_.x -eq 5 -and $_.y -eq 8 }).Count -eq 1) 'Player house warp changed.'
Assert-True (@($town.warp_events | Where-Object { $_.x -eq 14 -and $_.y -eq 8 }).Count -eq 1) 'Lia house warp changed.'
Assert-True (@($town.warp_events | Where-Object { $_.x -eq 7 -and $_.y -eq 16 }).Count -eq 1) 'Laboratory warp changed.'
Assert-True (@($town.warp_events | Where-Object { $_.x -eq 28 -and $_.y -eq 7 }).Count -eq 1) 'School warp changed.'
Assert-True (@($town.coord_events | Where-Object { $_.y -eq 0 }).Count -eq 13) 'North connection trigger corridor changed.'
Assert-True (@($town.connections | Where-Object { $_.map -eq 'MAP_ROUTE101' -and $_.direction -eq 'up' }).Count -eq 1) 'Via Verdi connection changed.'
Assert-True (@($oldale.object_events | Where-Object { $_.graphics_id -eq 'OBJ_EVENT_GFX_GIRL_3' -and $_.script -eq 'OldaleTown_EventScript_Traveler' -and $_.x -eq 10 -and $_.y -eq 15 }).Count -eq 1) 'Approved Porta Pretoria traveler polish was not preserved.'

for ($y = 27; $y -le 31; $y++) {
    for ($x = 0; $x -lt 36; $x++) {
        $cell = Get-Cell $mapBin $x $y
        Assert-True (((($cell -band 0x0C00) -shr 10) -ne 3)) 'Southern expansion corridor must remain walkable.'
    }
}

Write-Output 'Albèra Bassa residential blockout validation passed.'
