param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)

$ErrorActionPreference = 'Stop'

function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw -Encoding utf8 | ConvertFrom-Json }
function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }

$sections = (Read-Json 'src/data/region_map/region_map_sections.json').map_sections
$lagoDisplayName = [string]::Concat('LAGO DI ALB', [char]0x00C8, 'RA')
$expectedSections = @{
    MAPSEC_VIA_CONSOLARE = 'VIA CONSOLARE'
    MAPSEC_LAGO_DI_ALBERA = $lagoDisplayName
    MAPSEC_BORGO_DI_CASTELLO = 'BORGO DI CASTELLO'
    MAPSEC_VILLA_PAPALE = 'VILLA PAPALE'
    MAPSEC_GALLERIE_DI_SOPRA = 'GALLERIE DI SOPRA'
    MAPSEC_PONTE_VALLE_LARICIA = 'PONTE/VALLE LARICIA'
    MAPSEC_LARICIA = 'LARICIA'
}

foreach ($id in $expectedSections.Keys) {
    $section = @($sections | Where-Object { $_.id -eq $id })
    Assert-True ($section.Count -eq 1 -and $section[0].name -eq $expectedSections[$id]) "Map section $id is missing or has the wrong display name."
}
Assert-True (@($sections | Where-Object { $_.id -eq 'MAPSEC_ALBERA_STORICA' }).Count -eq 1) 'MAPSEC_ALBERA_STORICA must remain available.'

$assignments = @{
    'data/maps/ViaConsolare/map.json' = 'MAPSEC_VIA_CONSOLARE'
    'data/maps/LagoDiAlbera/map.json' = 'MAPSEC_LAGO_DI_ALBERA'
    'data/maps/BorgoDiCastello/map.json' = 'MAPSEC_BORGO_DI_CASTELLO'
    'data/maps/VillaPapaleGiardini/map.json' = 'MAPSEC_VILLA_PAPALE'
    'data/maps/VillaPapaleInterno/map.json' = 'MAPSEC_VILLA_PAPALE'
    'data/maps/StradaBorgoCisternoni/map.json' = 'MAPSEC_GALLERIE_DI_SOPRA'
    'data/maps/Route103/map.json' = 'MAPSEC_ROUTE_103'
    'data/maps/PonteValleLaricia/map.json' = 'MAPSEC_PONTE_VALLE_LARICIA'
    'data/maps/Laricia/map.json' = 'MAPSEC_LARICIA'
}
foreach ($path in $assignments.Keys) {
    Assert-True ((Read-Json $path).region_map_section -eq $assignments[$path]) "$path has the wrong map section."
}

$route = Read-Json 'data/maps/Route103/map.json'
$secretBase = @($route.bg_events | Where-Object { $_.type -eq 'secret_base' -and $_.x -eq 26 -and $_.y -eq 5 -and $_.elevation -eq 3 -and $_.secret_base_id -eq 'SECRET_BASE_CISTERNONI_TREE_1' })
Assert-True ($secretBase.Count -eq 1) 'Route103 Secret Base tree at (26,5) must remain unchanged.'

Write-Output 'Albera area map sections: PASS'
