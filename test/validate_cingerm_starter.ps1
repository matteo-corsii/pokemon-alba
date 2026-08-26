param(
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) {
        throw $Message
    }
}

$starterSource = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/starter_choose.c') -Raw
$battleSetup = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/battle_setup.c') -Raw
$trainerPath = Join-Path $RepositoryRoot 'src/data/trainers.party'
$trainerLines = Get-Content -LiteralPath $trainerPath

Assert-True ($starterSource -match '#define GRASS_STARTER \(IS_FRLG \? SPECIES_BULBASAUR\s+: SPECIES_CINGERM\)') 'Lo slot Erba Emerald non restituisce Cingerm mantenendo Bulbasaur in FRLG.'
Assert-True ($starterSource -match '#define FIRE_STARTER\s+\(IS_FRLG \? SPECIES_CHARMANDER : SPECIES_SERBRACE\)') 'Lo slot Fuoco Emerald non restituisce Serbrace mantenendo Charmander in FRLG.'
Assert-True ($starterSource -match '#define WATER_STARTER \(IS_FRLG \? SPECIES_SQUIRTLE\s+: SPECIES_ARDEINO\s+\)') 'Lo slot Acqua Emerald non restituisce Ardeino mantenendo Squirtle in FRLG.'
Assert-True ($battleSetup -match 'ScriptGiveMon\(starterMon, 5, ITEM_NONE\);') 'La creazione iniziale non usa piÃƒÂ¹ il livello 5 standard.'

$expected = [ordered]@{
    TRAINER_BRENDAN_ROUTE_103_MUDKIP = 'Cingerm:3'
    TRAINER_BRENDAN_ROUTE_110_MUDKIP = 'Rovasco:20'
    TRAINER_BRENDAN_ROUTE_119_MUDKIP = 'Rovasco:31'
    TRAINER_MAY_ROUTE_103_MUDKIP = 'Cingerm:5'
    TRAINER_MAY_ROUTE_110_MUDKIP = 'Rovasco:20'
    TRAINER_MAY_ROUTE_119_MUDKIP = 'Rovasco:31'
    TRAINER_BRENDAN_RUSTBORO_MUDKIP = 'Cingerm:15'
    TRAINER_MAY_RUSTBORO_MUDKIP = 'Cingerm:15'
    TRAINER_BRENDAN_LILYCOVE_MUDKIP = 'Rovasco:34'
    TRAINER_MAY_LILYCOVE_MUDKIP = 'Rovasco:34'
}

$found = [ordered]@{}
$currentTrainer = $null
$pendingSpecies = $null
foreach ($line in $trainerLines) {
    if ($line -match '^=== (TRAINER_(?:BRENDAN|MAY)_[A-Z0-9_]+) ===$') {
        $currentTrainer = $Matches[1]
        $pendingSpecies = $null
        continue
    }
    if ($line -match '^=== ') {
        $currentTrainer = $null
        $pendingSpecies = $null
        continue
    }
    if ($currentTrainer -and $line -match '^(Cingerm|Rovasco|Selvazanna|Treecko|Grovyle|Sceptile)$') {
        $pendingSpecies = $line
        continue
    }
    if ($currentTrainer -and $pendingSpecies -and $line -match '^Level: ([0-9]+)$') {
        if (-not $found.Contains($currentTrainer)) {
            $found[$currentTrainer] = @()
        }
        $found[$currentTrainer] += "$pendingSpecies`:$($Matches[1])"
        $pendingSpecies = $null
    }
}

foreach ($entry in $expected.GetEnumerator()) {
    Assert-True ($found.Contains($entry.Key)) "Manca la linea Erba prevista per $($entry.Key)."
    Assert-True ($found[$entry.Key].Count -eq 1 -and $found[$entry.Key][0] -eq $entry.Value) "Progressione Erba inattesa per $($entry.Key)."
}

$unexpected = @($found.Keys | Where-Object { -not $expected.Contains($_) })
Assert-True ($unexpected.Count -eq 0) "Riferimenti inattesi alla linea Cingerm/Treecko nelle squadre di Nico o Lia: $($unexpected -join ', ')."

$forbiddenSpecies = @('Rovasco', 'Selvazanna', 'Serbrace', 'Vipercen', 'Tossivampa', 'Ardeino', 'Velairone', 'Codairone')
$gameplayRoots = @(
    (Join-Path $RepositoryRoot 'data/maps'),
    (Join-Path $RepositoryRoot 'data/scripts'),
    (Join-Path $RepositoryRoot 'src/data/wild_encounters.json')
)

foreach ($root in $gameplayRoots) {
    $files = if (Test-Path -LiteralPath $root -PathType Container) {
        Get-ChildItem -LiteralPath $root -Recurse -File
    } elseif (Test-Path -LiteralPath $root -PathType Leaf) {
        Get-Item -LiteralPath $root
    } else {
        @()
    }
    foreach ($file in $files) {
        if ($file.FullName -eq (Join-Path $RepositoryRoot 'data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc')) {
            continue
        }
        $content = Get-Content -LiteralPath $file.FullName -Raw
        Assert-True ($content -notmatch '\bCingerm\b|SPECIES_CINGERM') "Cingerm compare in un incontro, regalo, evento o mappa non consentiti: $($file.FullName)."
        foreach ($species in $forbiddenSpecies) {
            Assert-True ($content -notmatch "\b$species\b|SPECIES_$($species.ToUpperInvariant())") "$species ÃƒÂ¨ ottenibile o referenziato nel gameplay: $($file.FullName)."
        }
    }
}

Write-Output 'Cingerm starter prototype validation passed.'
