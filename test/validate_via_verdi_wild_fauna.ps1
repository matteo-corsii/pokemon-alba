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

function Get-Route101Entry($WildData) {
    $entries = @($WildData.wild_encounter_groups.encounters | Where-Object { $_.map -eq 'MAP_ROUTE101' })
    Assert-True ($entries.Count -eq 1) 'MAP_ROUTE101 must have exactly one encounter entry.'
    return $entries[0]
}

$wild = Read-Json 'src/data/wild_encounters.json'
$route101 = Get-Route101Entry $wild
$mons = @($route101.land_mons.mons)

Assert-True ($route101.base_label -eq 'gRoute101') 'MAP_ROUTE101 base label changed.'
Assert-True ($route101.land_mons.encounter_rate -eq 20) 'MAP_ROUTE101 encounter rate must remain 20.'
Assert-True ($mons.Count -eq 12) 'MAP_ROUTE101 must retain the twelve standard land slots.'

$expectedSlots = @(
    @('SPECIES_FOLIARVA', 2, 4), # 20%
    @('SPECIES_BORGOTTO', 3, 5), # 20%
    @('SPECIES_FOLIARVA', 2, 4), # 10%
    @('SPECIES_BORGOTTO', 3, 5), # 10%
    @('SPECIES_GHEPIO', 3, 5),   # 10%
    @('SPECIES_GAZZUOLA', 4, 5), # 10%
    @('SPECIES_FOLIARVA', 2, 4), # 5%
    @('SPECIES_GHEPIO', 3, 5),   # 5%
    @('SPECIES_FOLIARVA', 2, 4), # 4%
    @('SPECIES_BORGOTTO', 3, 5), # 4%
    @('SPECIES_GHEPIO', 3, 5),   # 1%
    @('SPECIES_GAZZUOLA', 4, 5)  # 1%
)

for ($index = 0; $index -lt $expectedSlots.Count; $index++) {
    $expected = $expectedSlots[$index]
    $actual = $mons[$index]
    Assert-True ($actual.species -eq $expected[0]) "Unexpected species in Route101 slot $index."
    Assert-True ($actual.min_level -eq $expected[1] -and $actual.max_level -eq $expected[2]) "Unexpected levels in Route101 slot $index."
}

$weights = @(20, 20, 10, 10, 10, 10, 5, 5, 4, 4, 1, 1)
$expectedWeights = @{ SPECIES_FOLIARVA = 39; SPECIES_BORGOTTO = 34; SPECIES_GHEPIO = 16; SPECIES_GAZZUOLA = 11 }
foreach ($species in $expectedWeights.Keys) {
    $weight = 0
    for ($index = 0; $index -lt $mons.Count; $index++) {
        if ($mons[$index].species -eq $species) { $weight += $weights[$index] }
    }
    Assert-True ($weight -eq $expectedWeights[$species]) "$species has an unexpected Route101 weight."
}

$allowedSpecies = @('SPECIES_FOLIARVA', 'SPECIES_BORGOTTO', 'SPECIES_GHEPIO', 'SPECIES_GAZZUOLA')
foreach ($mon in $mons) {
    Assert-True ($allowedSpecies -contains $mon.species) "Unexpected Route101 species: $($mon.species)."
}
foreach ($legacySpecies in @('SPECIES_WURMPLE', 'SPECIES_POOCHYENA', 'SPECIES_ZIGZAGOON')) {
    Assert-True ($mons.species -notcontains $legacySpecies) "Legacy Route101 species remains: $legacySpecies."
}

$speciesConstants = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/species.h') -Raw
foreach ($species in $allowedSpecies) {
    Assert-True ($speciesConstants.Contains($species)) "Missing implemented species constant: $species."
}

$changedArtifacts = @(
    & git -C $RepositoryRoot diff --name-only develop...HEAD -- '*.gba' '*.elf' '*.map' '*.zip'
    & git -C $RepositoryRoot diff --name-only -- '*.gba' '*.elf' '*.map' '*.zip'
    & git -C $RepositoryRoot diff --cached --name-only -- '*.gba' '*.elf' '*.map' '*.zip'
    & git -C $RepositoryRoot ls-files --others --exclude-standard -- '*.gba' '*.elf' '*.map' '*.zip'
) | Where-Object { $_ } | Sort-Object -Unique
Assert-True ($changedArtifacts.Count -eq 0) 'Generated artifact detected.'

Write-Output 'Via Verdi wild fauna validation passed.'
