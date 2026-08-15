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

$groups = Read-Json 'data/maps/map_groups.json'
$layouts = Read-Json 'data/layouts/layouts.json'
$regionSections = Read-Json 'src/data/region_map/region_map_sections.json'
$oldale = Read-Json 'data/maps/OldaleTown/map.json'
$alberaStorica = Read-Json 'data/maps/AlberaStorica/map.json'

$townsAndRoutes = @($groups.gMapGroup_TownsAndRoutes)
Assert-True (($townsAndRoutes | Where-Object { $_ -eq 'AlberaStorica' }).Count -eq 1) 'AlberaStorica must appear exactly once in the map group.'

$layout = @($layouts.layouts | Where-Object { $_.id -eq 'LAYOUT_ALBERA_STORICA' })
Assert-True ($layout.Count -eq 1) 'LAYOUT_ALBERA_STORICA missing or duplicated.'
Assert-True ($layout[0].width -eq 36 -and $layout[0].height -eq 30) 'Unexpected layout dimensions.'
Assert-True ($layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_PortaPretoria') 'Unexpected tilesets.'
Assert-True ($layout[0].layout_version -eq 'emerald') 'The layout must remain Emerald-only.'

$section = @($regionSections.map_sections | Where-Object { $_.id -eq 'MAPSEC_ALBERA_STORICA' })
$expectedSectionName = [string]::Concat('ALB', [char]0x00C8, 'RA STORICA')
Assert-True ($section.Count -eq 1 -and $section[0].name -eq $expectedSectionName) 'Invalid regional map section.'

Assert-True ($alberaStorica.id -eq 'MAP_ALBERA_STORICA' -and $alberaStorica.layout -eq 'LAYOUT_ALBERA_STORICA') 'Albera Storica map is not linked to its layout.'
Assert-True (@($alberaStorica.connections).Count -eq 1) 'Albera Storica must have only the Porta Pretoria connection.'
$returnConnection = @($alberaStorica.connections | Where-Object { $_.direction -eq 'right' -and $_.map -eq 'MAP_OLDALE_TOWN' -and $_.offset -eq 0 })
Assert-True ($returnConnection.Count -eq 1) 'Return connection to Porta Pretoria is invalid.'
Assert-True (@($alberaStorica.object_events).Count -eq 0) 'Albera Storica must not gain unrelated NPCs.'
$amphitheatreEntranceTriggers = @($alberaStorica.coord_events | Where-Object { $_.x -in @(17, 18) -and $_.y -eq 4 -and $_.elevation -eq 3 -and $_.var -eq 'VAR_ALBERA_GYM_INPUT' -and $_.var_value -eq '0' -and $_.script -eq 'AlberaStorica_EventScript_EnterAnfiteatro' })
Assert-True ($amphitheatreEntranceTriggers.Count -eq 2 -and @($alberaStorica.coord_events).Count -eq 2) 'Albera Storica must not gain unrelated coordinate events.'

$westConnection = @($oldale.connections | Where-Object { $_.direction -eq 'left' })
Assert-True ($westConnection.Count -eq 1 -and $westConnection[0].map -eq 'MAP_ALBERA_STORICA') 'Porta Pretoria west must connect to Albera Storica.'
Assert-True (-not (@($oldale.connections | Where-Object { $_.direction -eq 'left' -and $_.map -eq 'MAP_ROUTE102' }).Count)) 'The old Route102 destination was not removed.'
Assert-True (@($oldale.coord_events | Where-Object { $_.script -eq 'OldaleTown_EventScript_BlockWestExit' }).Count -eq 0) 'The state-7 west block is still active.'
$northBlocks = @($oldale.coord_events | Where-Object { $_.script -eq 'OldaleTown_EventScript_BlockNorthExit' })
Assert-True ($northBlocks.Count -eq 4) 'The north block must remain unchanged.'
Assert-True ((@($northBlocks | ForEach-Object { "$($_.x),$($_.y)" }) -join ';') -eq '8,0;9,0;10,0;11,0') 'North block coordinates changed.'

$mapBytes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/AlberaStorica/map.bin'))
Assert-True ($mapBytes.Length -eq (36 * 30 * 2)) 'Unexpected map.bin size.'
Assert-True (Test-Path (Join-Path $RepositoryRoot 'data/layouts/AlberaStorica/border.bin')) 'Missing border.bin.'
$mapWords = for ($index = 0; $index -lt 36 * 30; $index++) { [BitConverter]::ToUInt16($mapBytes, $index * 2) }
Assert-True ($mapWords[(10 * 36) + 35] -eq 0x3001 -and $mapWords[(11 * 36) + 35] -eq 0x3001) 'The east entrance to Porta Pretoria is not walkable.'
Assert-True ((@($mapWords[((2 * 36) + 15)..((2 * 36) + 20)] | ForEach-Object { $_ -band 0x03FF }) -join ';') -eq '656;657;658;659;660;661') 'The Amphitheater landmark facade changed unexpectedly.'

$changedArtifacts = @(& git -C $RepositoryRoot diff --name-only develop -- '*.gba' '*.elf' '*.map' '*.zip')
$untrackedArtifacts = @(& git -C $RepositoryRoot ls-files --others --exclude-standard -- '*.gba' '*.elf' '*.map' '*.zip')
Assert-True (($changedArtifacts.Count + $untrackedArtifacts.Count) -eq 0) 'Build artifacts or archives introduced by the blockout were found.'

Write-Output 'Albera Storica blockout validation passed.'
