$ErrorActionPreference = 'Stop'

function Assert([bool]$Condition, [string]$Message)
{
    if (-not $Condition) { throw $Message }
}

$data = Get-Content src/data/wild_encounters.json -Raw | ConvertFrom-Json
$group = $data.wild_encounter_groups | Where-Object { $_.label -eq 'gWildMonHeaders' }
$tables = @($group.encounters | Where-Object { $_.map -eq 'MAP_LAGO_DI_ALBERA' })

Assert ($tables.Count -eq 4) 'Lago di Albera must have four time-of-day encounter tables'

$expectedLabels = @(
    'gLagoDiAlbera_Morning',
    'gLagoDiAlbera_Day',
    'gLagoDiAlbera_Evening',
    'gLagoDiAlbera_Night'
)
Assert ((@($tables.base_label) -join '|') -eq ($expectedLabels -join '|')) 'Unexpected Lago time table order'

$rates = @(20,20,10,10,10,10,5,5,4,4,1,1)
$expectedLand = @{
    gLagoDiAlbera_Morning = @('SPECIES_GHEPIO','SPECIES_GHEPIO','SPECIES_CRISALVIA','SPECIES_CRISALVIA','SPECIES_LUSCINCO','SPECIES_LUSCINCO','SPECIES_LUMELLA','SPECIES_LUMELLA','SPECIES_SALAMPOLLA','SPECIES_SALAMPOLLA','SPECIES_PALUDIX','SPECIES_PALUDIX')
    gLagoDiAlbera_Day = @('SPECIES_GHEPIO','SPECIES_GHEPIO','SPECIES_CRISALVIA','SPECIES_CRISALVIA','SPECIES_LUSCINCO','SPECIES_LUSCINCO','SPECIES_LUMELLA','SPECIES_LUMELLA','SPECIES_SALAMPOLLA','SPECIES_SALAMPOLLA','SPECIES_PALUDIX','SPECIES_PALUDIX')
    gLagoDiAlbera_Evening = @('SPECIES_GHEPIO','SPECIES_GAZZUOLA','SPECIES_CRISALVIA','SPECIES_CRISALVIA','SPECIES_LUSCINCO','SPECIES_LUSCINCO','SPECIES_LUMELLA','SPECIES_LUMELLA','SPECIES_SALAMPOLLA','SPECIES_SALAMPOLLA','SPECIES_PALUDIX','SPECIES_PALUDIX')
    gLagoDiAlbera_Night = @('SPECIES_GAZZUOLA','SPECIES_GAZZUOLA','SPECIES_CRISALVIA','SPECIES_CRISALVIA','SPECIES_LUMELLA','SPECIES_LUSCINCO','SPECIES_SALAMPOLLA','SPECIES_SALAMPOLLA','SPECIES_PALUDIX','SPECIES_PALUDIX','SPECIES_LENGHELIS','SPECIES_LENGHELIS')
}
foreach ($table in $tables)
{
    Assert ($table.land_mons.encounter_rate -eq 20) "$($table.base_label) land rate must be 20"
    $mons = @($table.land_mons.mons)
    Assert ($mons.Count -eq 12) "$($table.base_label) must have 12 land slots"
    Assert ((@($mons.species) -join '|') -eq ($expectedLand[$table.base_label] -join '|')) "$($table.base_label) land species mismatch"
    foreach ($mon in $mons)
    {
        Assert ($mon.min_level -ge 13 -and $mon.max_level -le 16) "$($table.base_label) land levels outside 13-16"
    }
}

$morning = $tables | Where-Object { $_.base_label -eq 'gLagoDiAlbera_Morning' }
Assert ($morning.water_mons.encounter_rate -eq 20) 'Surf encounter rate must be 20'
Assert ((@($morning.water_mons.mons.species) -join '|') -eq (@('SPECIES_CARPULUS','SPECIES_TRITINO','SPECIES_LUCINUS','SPECIES_LUCINUS','SPECIES_NAUFRAGUS') -join '|')) 'Surf slots must be Carpulus 60%, Tritino 30%, Lucinus 9%, Naufragus 1%'
Assert ($morning.fishing_mons.encounter_rate -eq 30) 'Fishing encounter rate must be 30'
Assert ((@($morning.fishing_mons.mons.species) -join '|') -eq (@('SPECIES_CARPULUS','SPECIES_CARPULUS','SPECIES_CARPULUS','SPECIES_TRITINO','SPECIES_LUCINUS','SPECIES_CARPULUS','SPECIES_LUCINUS','SPECIES_TRITINO','SPECIES_NAUFRAGUS','SPECIES_NAUFRAGUS') -join '|')) 'Fishing slots do not match the approved rods'

$forbidden = @('SPECIES_CISTERNIDE','SPECIES_CALCISTERN','SPECIES_TRICREST','SPECIES_ARDEINO','SPECIES_VELAIRONE','SPECIES_CODAIRONE')
$allSpecies = @($tables.land_mons.mons.species) + @($morning.water_mons.mons.species) + @($morning.fishing_mons.mons.species)
foreach ($species in $forbidden)
{
    Assert ($allSpecies -notcontains $species) "$species must not appear in Lago di Albera encounters"
}

Write-Host 'Lago di Albera wild fauna validation passed.'
