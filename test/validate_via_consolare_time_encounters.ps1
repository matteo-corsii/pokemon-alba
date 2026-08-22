param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

$wildPath = Join-Path $RepositoryRoot 'src/data/wild_encounters.json'
$wild = Get-Content $wildPath -Raw | ConvertFrom-Json
$groups = @($wild.wild_encounter_groups)
$encounters = @($groups[0].encounters)
$via = @($encounters | Where-Object { $_.map -eq 'MAP_VIA_CONSOLARE' })
Assert-True ($via.Count -eq 4) 'MAP_VIA_CONSOLARE must have exactly four time tables.'

$config = Get-Content (Join-Path $RepositoryRoot 'include/config/overworld.h') -Raw
Assert-True ($config -match '(?m)^#define OW_TIME_OF_DAY_ENCOUNTERS\s+TRUE\b') 'Time-based encounters are not enabled.'
Assert-True ($config -match '(?m)^#define OW_TIME_OF_DAY_DISABLE_FALLBACK\s+FALSE\b') 'Time encounter fallback must remain enabled.'
Assert-True ($config -match '(?m)^#define OW_TIME_OF_DAY_FALLBACK\s+TIME_MORNING\b') 'Unexpected time encounter fallback.'

$weights = @(20,20,10,10,10,10,5,5,4,4,1,1)
$expected = @{
    Morning = @('SPECIES_GHEPIO|12|13','SPECIES_BORGOTTO|13|14','SPECIES_GHEPIO|13|14','SPECIES_BORGOTTO|14|15','SPECIES_CRISALVIA|13|14','SPECIES_CRISALVIA|14|15','SPECIES_LUSCINCO|14|15','SPECIES_LUSCINCO|15|16','SPECIES_LUSCINCO|14|16','SPECIES_PALUDIX|14|16','SPECIES_LUSCINCO|16|16','SPECIES_PALUDIX|15|16')
    Day = @('SPECIES_GHEPIO|12|13','SPECIES_BORGOTTO|13|14','SPECIES_GHEPIO|13|14','SPECIES_BORGOTTO|14|15','SPECIES_CRISALVIA|13|14','SPECIES_CRISALVIA|14|15','SPECIES_LUSCINCO|14|15','SPECIES_LUSCINCO|15|16','SPECIES_LUSCINCO|14|16','SPECIES_PALUDIX|14|16','SPECIES_LUSCINCO|16|16','SPECIES_PALUDIX|15|16')
    Evening = @('SPECIES_BORGOTTO|13|14','SPECIES_GAZZUOLA|13|14','SPECIES_BORGOTTO|14|15','SPECIES_GAZZUOLA|14|15','SPECIES_CRISALVIA|14|15','SPECIES_CRISALVIA|15|16','SPECIES_LUMELLA|14|15','SPECIES_LUMELLA|15|16','SPECIES_LUMELLA|14|16','SPECIES_PALUDIX|14|16','SPECIES_LUMELLA|16|16','SPECIES_PALUDIX|15|16')
    Night = @('SPECIES_GAZZUOLA|14|15','SPECIES_LUMELLA|14|16','SPECIES_GAZZUOLA|15|16','SPECIES_BORGOTTO|13|14','SPECIES_BORGOTTO|14|15','SPECIES_LUSCINCO|14|15','SPECIES_LUSCINCO|15|16','SPECIES_PALUDIX|14|16','SPECIES_LUSCINCO|14|16','SPECIES_LENGHELIS|15|16','SPECIES_LUSCINCO|16|16','SPECIES_LENGHELIS|16|16')
}

foreach ($time in $expected.Keys) {
    $entry = @($via | Where-Object { $_.base_label -eq "gViaConsolare_$time" })
    Assert-True ($entry.Count -eq 1) "Missing Via Consolare $time table."
    Assert-True ([int]$entry[0].land_mons.encounter_rate -eq 20) "$time encounter rate is not 20."
    Assert-True (-not ($entry[0].PSObject.Properties.Name | Where-Object { $_ -in @('water_mons','fishing_mons','rock_smash_mons') })) "$time contains a non-land encounter field."
    $mons = @($entry[0].land_mons.mons)
    Assert-True ($mons.Count -eq 12) "$time must have exactly 12 land slots."
    for ($i = 0; $i -lt 12; $i++) {
        $actual = "$($mons[$i].species)|$($mons[$i].min_level)|$($mons[$i].max_level)"
        Assert-True ($actual -eq $expected[$time][$i]) "$time slot $i is incorrect: $actual"
    }
}

Assert-True ((ConvertTo-Json $expected.Morning -Compress) -eq (ConvertTo-Json $expected.Day -Compress)) 'Morning and Day definitions differ.'
foreach ($entry in $via) {
    $paludixWeight = 0
    for ($i = 0; $i -lt 12; $i++) {
        if ($entry.land_mons.mons[$i].species -eq 'SPECIES_PALUDIX') { $paludixWeight += $weights[$i] }
    }
    Assert-True ($paludixWeight -eq 5) "$($entry.base_label) must contain Paludix at 5%."
}
$lenghelisOther = @($via | Where-Object base_label -ne 'gViaConsolare_Night' | ForEach-Object { @($_.land_mons.mons) } | Where-Object species -eq 'SPECIES_LENGHELIS')
Assert-True ($lenghelisOther.Count -eq 0) 'Lenghelis must be night-only.'
foreach ($excluded in @('SPECIES_SANGUILEX','SPECIES_OMPHALUX','SPECIES_LUSCERP')) {
    Assert-True (@($via | ForEach-Object { @($_.land_mons.mons) } | Where-Object species -eq $excluded).Count -eq 0) "$excluded must not appear on Via Consolare."
}

Assert-True (@($encounters | Where-Object map -eq 'MAP_VIA_CONSOLARE_MANSIO').Count -eq 0) 'Mansio must not have encounters.'
$route103 = @($encounters | Where-Object { $_.map -eq 'MAP_ROUTE103' })
Assert-True ($route103.Count -eq 1 -and $route103[0].base_label -eq 'gRoute103') 'Route103 normal encounter table changed.'
$base = (& git -C $RepositoryRoot show 'develop:src/data/wild_encounters.json') -join "`n" | ConvertFrom-Json
$baseOther = @($base.wild_encounter_groups[0].encounters | Where-Object map -ne 'MAP_VIA_CONSOLARE')
$currentOther = @($encounters | Where-Object map -ne 'MAP_VIA_CONSOLARE')
Assert-True ($baseOther.Count -eq $currentOther.Count) 'The number of non-Via Consolare encounter tables changed.'
for ($i = 0; $i -lt $baseOther.Count; $i++) {
    $baseEntry = ConvertTo-Json -InputObject $baseOther[$i] -Depth 30 -Compress
    $currentEntry = ConvertTo-Json -InputObject $currentOther[$i] -Depth 30 -Compress
    Assert-True ($baseEntry -eq $currentEntry) "Encounter table outside Via Consolare changed at index $i."
}

Write-Output 'Via Consolare time-based encounters validation passed.'
