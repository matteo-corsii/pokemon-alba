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

$wild = Read-Json 'src/data/wild_encounters.json'
$entries = @($wild.wild_encounter_groups.encounters)
$routeEntries = @($entries | Where-Object { $_.map -eq 'MAP_ROUTE103' })
Assert-True ($routeEntries.Count -eq 1) 'MAP_ROUTE103 must have exactly one encounter entry.'
$route = $routeEntries[0]
Assert-True ($route.base_label -eq 'gRoute103') 'MAP_ROUTE103 base label changed.'
Assert-True ($route.land_mons.encounter_rate -eq 20) 'MAP_ROUTE103 encounter rate must remain 20.'

$mons = @($route.land_mons.mons)
Assert-True ($mons.Count -eq 12) 'MAP_ROUTE103 must have exactly twelve land slots.'
$expectedSlots = @(
    @('SPECIES_FOLIARVA', 6, 7),
    @('SPECIES_MICIOLO', 7, 8),
    @('SPECIES_FOLIARVA', 7, 8),
    @('SPECIES_FOLIARVA', 6, 8),
    @('SPECIES_MICIOLO', 8, 9),
    @('SPECIES_GAZZUOLA', 7, 8),
    @('SPECIES_GAZZUOLA', 8, 9),
    @('SPECIES_GAZZUOLA', 7, 9),
    @('SPECIES_GAZZUOLA', 9, 9),
    @('SPECIES_MOLOSPSY', 9, 10),
    @('SPECIES_MOLOSPSY', 10, 10),
    @('SPECIES_MOLOSPSY', 9, 10)
)
for ($i = 0; $i -lt $expectedSlots.Count; $i++) {
    $expected = $expectedSlots[$i]
    Assert-True ($mons[$i].species -eq $expected[0] -and $mons[$i].min_level -eq $expected[1] -and $mons[$i].max_level -eq $expected[2]) "Unexpected MAP_ROUTE103 slot $i."
}

$weights = @(20, 20, 10, 10, 10, 10, 5, 5, 4, 4, 1, 1)
$expectedWeights = @{
    SPECIES_FOLIARVA = 40
    SPECIES_MICIOLO = 30
    SPECIES_GAZZUOLA = 24
    SPECIES_MOLOSPSY = 6
}
foreach ($species in $expectedWeights.Keys) {
    $weight = 0
    for ($i = 0; $i -lt $mons.Count; $i++) {
        if ($mons[$i].species -eq $species) { $weight += $weights[$i] }
    }
    Assert-True ($weight -eq $expectedWeights[$species]) "$species has an unexpected MAP_ROUTE103 weight."
}

foreach ($legacySpecies in @('SPECIES_WURMPLE', 'SPECIES_POOCHYENA', 'SPECIES_ZIGZAGOON', 'SPECIES_WINGULL', 'SPECIES_TENTACOOL', 'SPECIES_PELIPPER', 'SPECIES_MAGIKARP', 'SPECIES_WAILMER', 'SPECIES_SHARPEDO')) {
    Assert-True ($mons.species -notcontains $legacySpecies) "Legacy MAP_ROUTE103 species remains: $legacySpecies."
}
Assert-True (-not ($route.PSObject.Properties.Name -contains 'water_mons')) 'MAP_ROUTE103 must not define water_mons.'
Assert-True (-not ($route.PSObject.Properties.Name -contains 'fishing_mons')) 'MAP_ROUTE103 must not define fishing_mons.'
Assert-True (-not ($route.PSObject.Properties.Name -contains 'rock_smash_mons')) 'MAP_ROUTE103 must not define rock_smash_mons.'

$cisternoniEntries = @($entries | Where-Object { $_.map -eq 'MAP_CISTERNONI' })
Assert-True ($cisternoniEntries.Count -eq 0) 'MAP_CISTERNONI must not have a wild encounter table.'

$baseJson = (& git -C $RepositoryRoot show 'develop:src/data/wild_encounters.json') -join "`n" | ConvertFrom-Json
$baseOther = @($baseJson.wild_encounter_groups.encounters | Where-Object { $_.map -ne 'MAP_ROUTE103' } | ConvertTo-Json -Depth 30 -Compress)
$currentOther = @($entries | Where-Object { $_.map -ne 'MAP_ROUTE103' } | ConvertTo-Json -Depth 30 -Compress)
Assert-True (($baseOther -join "`n") -eq ($currentOther -join "`n")) 'An encounter table other than MAP_ROUTE103 changed.'

Write-Output 'Via dei Cisternoni wild fauna validation passed.'
