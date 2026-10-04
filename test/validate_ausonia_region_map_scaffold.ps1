param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'
function Read-Json([string]$path) { Get-Content (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
$sections = Read-Json 'src/data/region_map/region_map_sections.json'
$expected = @{
    MAPSEC_AUSONIA_ALBERA_BASSA = @(8, 12, 'ALBÈRA BASSA')
    MAPSEC_AUSONIA_VIA_VERDI = @(8, 11, 'VIA VERDI')
    MAPSEC_AUSONIA_PORTA_PRETORIA = @(8, 10, 'PORTA PRETORIA')
    MAPSEC_AUSONIA_VIA_CISTERNONI = @(9, 10, 'VIA DEI CISTERNONI')
    MAPSEC_ALBERA_STORICA = @(7, 10, 'ALBÈRA STORICA')
    MAPSEC_VIA_CONSOLARE = @(9, 9, 'VIA CONSOLARE')
    MAPSEC_LAGO_DI_ALBERA = @(9, 8, 'LAGO DI ALBÈRA')
    MAPSEC_BORGO_DI_CASTELLO = @(8, 7, 'BORGO DI CASTELLO')
    MAPSEC_VILLA_PAPALE = @(8, 6, 'VILLA PAPALE')
    MAPSEC_GALLERIE_DI_SOPRA = @(8, 8, 'GALLERIE DI SOPRA')
    MAPSEC_PONTE_VALLE_LARICIA = @(10, 10, 'PONTE/VALLE LARICIA')
    MAPSEC_LARICIA = @(12, 10, 'LARICIA')
}
foreach ($id in $expected.Keys) {
    $entry = @($sections.map_sections | Where-Object id -eq $id)
    Assert-True ($entry.Count -eq 1) "$id must have exactly one section entry."
    Assert-True ([int]$entry[0].x -eq $expected[$id][0] -and [int]$entry[0].y -eq $expected[$id][1]) "$id has incorrect coordinates."
    Assert-True ($entry[0].name -eq $expected[$id][2]) "$id has incorrect user-facing name."
}
$seen = @{}
foreach ($entry in $sections.map_sections | Where-Object { $expected.ContainsKey($_.id) }) {
    $key = "$($entry.x),$($entry.y)"
    Assert-True (-not $seen.ContainsKey($key)) "Ausonia coordinate collision at $key."
    $seen[$key] = $entry.id
}
foreach ($path in @('data/maps/LittlerootTown/map.json','data/maps/Route101/map.json','data/maps/OldaleTown/map.json','data/maps/Route103/map.json')) {
    $map = Read-Json $path
    Assert-True ($map.region_map_section -like 'MAPSEC_AUSONIA_*') "$path is not assigned an Ausonia section."
}
$regionMap = Get-Content (Join-Path $RepositoryRoot 'src/region_map.c') -Raw
Assert-True ($regionMap -match 'REGION_MAP_AUSONIA') 'Ausonia Region Map type is missing.'
Assert-True ($regionMap -match 'case REGION_AUSONIA:\s*return REGION_MAP_AUSONIA') 'Ausonia routing is missing.'
Assert-True (-not ($regionMap -match 'MAPSEC_AUSONIA.*sFlyLocations')) 'No Ausonia Fly destination may be added by this scaffold.'
Write-Output 'Ausonia Region Map scaffold: PASS'