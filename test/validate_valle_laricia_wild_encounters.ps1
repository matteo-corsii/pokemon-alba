param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw -Encoding utf8 | ConvertFrom-Json }

$wild = Read-Json 'src/data/wild_encounters.json'
$entries = @($wild.wild_encounter_groups | ForEach-Object { $_.encounters } | Where-Object { $_.map -eq 'MAP_PONTE_VALLE_LARICIA' })
Assert-True ($entries.Count -eq 4) 'Valle di Laricia must have exactly four time-of-day encounter tables.'

$expectedLabels = @('gPonteValleLaricia_Morning', 'gPonteValleLaricia_Day', 'gPonteValleLaricia_Evening', 'gPonteValleLaricia_Night')
$expectedRates = @(20, 20, 10, 10, 10, 10, 5, 5, 4, 4, 1, 1)
$landRates = @($wild.wild_encounter_groups | Where-Object { $_.label -eq 'gWildMonHeaders' } | Select-Object -ExpandProperty fields | Where-Object { $_.type -eq 'land_mons' } | Select-Object -ExpandProperty encounter_rates)
Assert-True (($landRates -join ',') -eq ($expectedRates -join ',')) 'Standard land encounter slot probabilities are incorrect.'
$expectedSpecies = @('SPECIES_VITEMOSTO','SPECIES_VITEMOSTO','SPECIES_CRISALVIA','SPECIES_PASTUFO','SPECIES_CRISALVIA','SPECIES_PASTUFO','SPECIES_PORCHIGNIS','SPECIES_PORCHIGNIS','SPECIES_FRASCHIETTO','SPECIES_FRASCHIETTO','SPECIES_FRASCHIETTO','SPECIES_FRASCHIETTO')
$expectedLevels = @(@(23,25),@(24,26),@(24,25),@(24,25),@(25,26),@(25,26),@(25,26),@(26,27),@(25,26),@(26,27),@(27,28),@(27,28))

for ($tableIndex = 0; $tableIndex -lt $entries.Count; $tableIndex++) {
    $entry = $entries[$tableIndex]
    Assert-True ($entry.base_label -eq $expectedLabels[$tableIndex]) "Unexpected Valle encounter label at index $tableIndex."
    Assert-True ($null -ne $entry.land_mons) "Missing Valle land_mons for $($entry.base_label)."
    Assert-True ([int]$entry.land_mons.encounter_rate -eq 20) "Valle encounter rate must be 20 for $($entry.base_label)."
    Assert-True (@($entry.land_mons.mons).Count -eq 12) "Valle must have exactly 12 land slots for $($entry.base_label)."
    Assert-True ($null -eq $entry.water_mons -and $null -eq $entry.fishing_mons -and $null -eq $entry.rock_smash_mons) "Valle must not define water, fishing, or Rock Smash encounters."
    for ($slot = 0; $slot -lt 12; $slot++) {
        $mon = @($entry.land_mons.mons)[$slot]
        $expectedSlotSpecies = $expectedSpecies[$slot]
        if ($tableIndex -eq 3 -and $slot -ge 10) { $expectedSlotSpecies = 'SPECIES_OMPHALUX' }
        Assert-True ($mon.species -eq $expectedSlotSpecies) "Unexpected species in $($entry.base_label) slot $($slot + 1)."
        $expectedRange = $expectedLevels[$slot]
        if ($tableIndex -eq 3 -and $slot -eq 9) { $expectedRange = @(26, 28) }
        if ($tableIndex -eq 3 -and $slot -eq 10) { $expectedRange = @(26, 27) }
        if ($tableIndex -eq 3 -and $slot -eq 11) { $expectedRange = @(27, 28) }
        Assert-True ([int]$mon.min_level -eq $expectedRange[0] -and [int]$mon.max_level -eq $expectedRange[1]) "Unexpected level range in $($entry.base_label) slot $($slot + 1)."
    }
    if ($tableIndex -lt 3) {
        Assert-True (@($entry.land_mons.mons | Where-Object { $_.species -eq 'SPECIES_OMPHALUX' }).Count -eq 0) 'Omphalux is night-only.'
    } else {
        Assert-True (@($entry.land_mons.mons | Where-Object { $_.species -eq 'SPECIES_OMPHALUX' }).Count -eq 2) 'Night must contain exactly two Omphalux slots.'
        Assert-True (@($entry.land_mons.mons | Where-Object { $_.species -eq 'SPECIES_FRASCHIETTO' }).Count -eq 2) 'Night must contain exactly two Fraschietto slots.'
        $nightLevels = @($entry.land_mons.mons)[9]
        Assert-True ([int]$nightLevels.min_level -eq 26 -and [int]$nightLevels.max_level -eq 28) 'Night Fraschietto slot 10 level range is incorrect.'
    }
    Assert-True (@($entry.land_mons.mons | Where-Object { $_.species -in @('SPECIES_BRONZOVERRO','SPECIES_FRASCOTTO') }).Count -eq 0) 'Bronzoverro and Frascotto must never be wild in Valle.'
}

$serialized = ($entries | ConvertTo-Json -Depth 20)
Assert-True (-not ($serialized -match 'FLAG_|flag')) 'Valle encounter tables must not depend on flags.'
Write-Output 'Valle Laricia wild encounters: PASS'
