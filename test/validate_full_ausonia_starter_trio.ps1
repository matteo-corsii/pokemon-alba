param(
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

function Assert-Contains([string]$Text, [string]$Expected, [string]$Message) {
    Assert-True ($Text.Contains($Expected)) $Message
}

$starter = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/starter_choose.c') -Raw
$battleSetup = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/battle_setup.c') -Raw
$trainerLines = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/trainers.party')
$species = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/pokemon/species_info.h') -Raw
$summary = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/pokemon_summary_screen.c') -Raw
$pokemon = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/pokemon.c') -Raw
$strings = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/strings.c') -Raw
$abilities = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/abilities.h') -Raw

Assert-True ($starter -match '#define GRASS_STARTER \(IS_FRLG \? SPECIES_BULBASAUR\s+: SPECIES_CINGERM\)') 'Mapping Erba Emerald/FRLG inatteso.'
Assert-True ($starter -match '#define FIRE_STARTER\s+\(IS_FRLG \? SPECIES_CHARMANDER : SPECIES_SERBRACE\)') 'Mapping Fuoco Emerald/FRLG inatteso.'
Assert-True ($starter -match '#define WATER_STARTER \(IS_FRLG \? SPECIES_SQUIRTLE\s+: SPECIES_ARDEINO\s+\)') 'Mapping Acqua Emerald/FRLG inatteso.'
Assert-Contains $battleSetup 'ScriptGiveMon(starterMon, 5, ITEM_NONE);' 'La creazione standard dello starter al livello 5 è cambiata.'

$expected = [ordered]@{
    TRAINER_BRENDAN_ROUTE_103_MUDKIP='Cingerm:3'; TRAINER_BRENDAN_ROUTE_110_MUDKIP='Rovasco:20'; TRAINER_BRENDAN_ROUTE_119_MUDKIP='Rovasco:31'
    TRAINER_MAY_ROUTE_103_MUDKIP='Cingerm:5'; TRAINER_MAY_ROUTE_110_MUDKIP='Rovasco:20'; TRAINER_MAY_ROUTE_119_MUDKIP='Rovasco:31'
    TRAINER_BRENDAN_RUSTBORO_MUDKIP='Cingerm:15'; TRAINER_MAY_RUSTBORO_MUDKIP='Cingerm:15'
    TRAINER_BRENDAN_LILYCOVE_MUDKIP='Rovasco:34'; TRAINER_MAY_LILYCOVE_MUDKIP='Rovasco:34'
    TRAINER_BRENDAN_ROUTE_103_TREECKO='Serbrace:3'; TRAINER_BRENDAN_ROUTE_110_TREECKO='Vipercen:20'; TRAINER_BRENDAN_ROUTE_119_TREECKO='Vipercen:31'
    TRAINER_MAY_ROUTE_103_TREECKO='Serbrace:5'; TRAINER_MAY_ROUTE_110_TREECKO='Vipercen:20'; TRAINER_MAY_ROUTE_119_TREECKO='Vipercen:31'
    TRAINER_BRENDAN_RUSTBORO_TREECKO='Serbrace:15'; TRAINER_MAY_RUSTBORO_TREECKO='Serbrace:15'
    TRAINER_BRENDAN_LILYCOVE_TREECKO='Vipercen:34'; TRAINER_MAY_LILYCOVE_TREECKO='Vipercen:34'
    TRAINER_BRENDAN_ROUTE_103_TORCHIC='Ardeino:3'; TRAINER_BRENDAN_ROUTE_110_TORCHIC='Velairone:20'; TRAINER_BRENDAN_ROUTE_119_TORCHIC='Velairone:31'
    TRAINER_MAY_ROUTE_103_TORCHIC='Ardeino:5'; TRAINER_MAY_ROUTE_110_TORCHIC='Velairone:20'; TRAINER_MAY_ROUTE_119_TORCHIC='Velairone:31'
    TRAINER_BRENDAN_RUSTBORO_TORCHIC='Ardeino:15'; TRAINER_MAY_RUSTBORO_TORCHIC='Ardeino:15'
    TRAINER_BRENDAN_LILYCOVE_TORCHIC='Velairone:34'; TRAINER_MAY_LILYCOVE_TORCHIC='Velairone:34'
}

$ausoniaNames = 'Cingerm|Rovasco|Selvazanna|Serbrace|Vipercen|Tossivampa|Ardeino|Velairone|Codairone'
$found = [ordered]@{}
$currentTrainer = $null
$pendingSpecies = $null
foreach ($line in $trainerLines) {
    if ($line -match '^=== (TRAINER_(?:BRENDAN|MAY)_[A-Z0-9_]+) ===$') { $currentTrainer = $Matches[1]; $pendingSpecies = $null; continue }
    if ($line -match '^=== ') { $currentTrainer = $null; $pendingSpecies = $null; continue }
    if ($currentTrainer -and $line -match "^($ausoniaNames)$") { $pendingSpecies = $line; continue }
    if ($currentTrainer -and $pendingSpecies -and $line -match '^Level: ([0-9]+)$') {
        if (-not $found.Contains($currentTrainer)) { $found[$currentTrainer] = @() }
        $found[$currentTrainer] += "$pendingSpecies`:$($Matches[1])"
        $pendingSpecies = $null
    }
}
foreach ($entry in $expected.GetEnumerator()) {
    Assert-True ($found.Contains($entry.Key)) "Starter rivale mancante per $($entry.Key)."
    Assert-True ($found[$entry.Key].Count -eq 1 -and $found[$entry.Key][0] -eq $entry.Value) "Starter rivale inatteso per $($entry.Key)."
}
Assert-True ($found.Count -eq $expected.Count) 'Una specie di Ausonia compare in una squadra di Nico/Lia non prevista.'

foreach ($evolution in @(
    '.evolutions = EVOLUTION({EVO_LEVEL, 16, SPECIES_ROVASCO})',
    '.evolutions = EVOLUTION({EVO_LEVEL, 36, SPECIES_SELVAZANNA})',
    '.evolutions = EVOLUTION({EVO_LEVEL, 16, SPECIES_VIPERCEN})',
    '.evolutions = EVOLUTION({EVO_LEVEL, 36, SPECIES_TOSSIVAMPA})',
    '.evolutions = EVOLUTION({EVO_LEVEL, 16, SPECIES_VELAIRONE})',
    '.evolutions = EVOLUTION({EVO_LEVEL, 36, SPECIES_CODAIRONE})'
)) { Assert-Contains $species $evolution "Evoluzione registrata inattesa: $evolution" }

Assert-Contains $species '.frontPic = gMonFrontPic_Cingerm' 'Cingerm non usa più il front sprite originale.'
Assert-Contains $species '.backPic = gMonBackPic_Cingerm' 'Cingerm non usa più il back sprite originale.'
Assert-Contains $species '.frontPic = gMonFrontPic_Serbrace' 'Serbrace non usa il front sprite originale.'
Assert-Contains $species '.backPic = gMonBackPic_Serbrace' 'Serbrace non usa il back sprite originale.'
Assert-Contains $species '.frontPic = gMonFrontPic_Ardeino' 'Ardeino non usa il front sprite originale.'
Assert-Contains $species '.backPic = gMonBackPic_Ardeino' 'Ardeino non usa il back sprite originale.'

foreach ($heading in @('static const u8 sTextAbilityTitle[] = _("ABILITÀ");', 'static const u8 sTextTrainerMemoTitle[] = _("MEMO ALLENATORE");')) {
    Assert-Contains $summary $heading "Intestazione italiana mancante: $heading"
}
Assert-True ($summary -match 'ClearInfoPageHeading\(.+?, 8\)') 'L’intestazione grafica ABILITY non viene neutralizzata in memoria.'
Assert-True ($summary -match 'ClearInfoPageHeading\(.+?, 13\)') 'L’intestazione grafica TRAINER MEMO non viene neutralizzata in memoria.'

$natureNames = @('Ardita','Schiva','Audace','Decisa','Birbona','Sicura','Docile','Placida','Scaltra','Fiacca','Timida','Lesta','Seria','Allegra','Ingenua','Modesta','Mite','Quieta','Ritrosa','Ardente','Calma','Gentile','Vivace','Cauta','Furba')
foreach ($nature in $natureNames) { Assert-Contains $pokemon ".name = COMPOUND_STRING(`"$nature`")" "Natura italiana mancante: $nature" }

$memoBlock = [regex]::Match($strings, '(?s)const u8 gText_XNatureMetAtYZ\[\].*?const u8 gText_XNatureHatchedSomewhereAt\[\].*?;').Value
Assert-True ($memoBlock.Length -gt 0) 'Blocco dei modelli del Memo Allenatore non individuato.'
foreach ($english in @(' nature','met at','probably met','hatched','obtained in a trade','fateful')) {
    Assert-True (-not $memoBlock.Contains($english)) "Il Memo Allenatore contiene ancora testo inglese: $english"
}
foreach ($italian in @('Natura {DYNAMIC 0}{DYNAMIC 2}', 'incontrato al {LV_2}', 'schiuso al {LV_2}', 'ottenuto con uno scambio', 'in un luogo ignoto')) {
    Assert-Contains $strings $italian "Modello italiano del Memo Allenatore mancante: $italian"
}

$abilityTranslations = [ordered]@{ Overgrow='Erbaiuto'; Defiant='Agonismo'; Blaze='Aiutofuoco'; Corrosion='Corrosione'; Torrent='Acquaiuto'; Hydration='Idratazione' }
foreach ($entry in $abilityTranslations.GetEnumerator()) {
    Assert-True ($abilities -match "(?s)\[ABILITY_$($entry.Key.ToUpperInvariant())\].*?\.name = _\(`"$($entry.Value)`"\).*?\.description = COMPOUND_STRING\(`"[^`"]+`"\)") "Traduzione abilità mancante o incompleta: $($entry.Key)."
}

$gameplayRoots = @('data/maps', 'data/scripts', 'src/data/wild_encounters.json')
$canonicalStarterScript = [IO.Path]::GetFullPath((Join-Path $RepositoryRoot 'data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc'))
foreach ($relativeRoot in $gameplayRoots) {
    $root = Join-Path $RepositoryRoot $relativeRoot
    $files = if (Test-Path -LiteralPath $root -PathType Container) { Get-ChildItem -LiteralPath $root -Recurse -File } elseif (Test-Path -LiteralPath $root) { Get-Item -LiteralPath $root } else { @() }
    foreach ($file in $files) {
        if ([IO.Path]::GetFullPath($file.FullName) -eq $canonicalStarterScript) { continue }
        $content = Get-Content -LiteralPath $file.FullName -Raw
        foreach ($speciesName in @('Cingerm','Rovasco','Selvazanna','Serbrace','Vipercen','Tossivampa','Ardeino','Velairone','Codairone')) {
            Assert-True ($content -notmatch "\b$speciesName\b|SPECIES_$($speciesName.ToUpperInvariant())") "$speciesName compare in gameplay non consentito: $($file.FullName)."
        }
    }
}

$canonicalStarterContent = Get-Content -LiteralPath $canonicalStarterScript -Raw
foreach ($starter in @('CINGERM', 'SERBRACE', 'ARDEINO')) {
    Assert-Contains $canonicalStarterContent "SPECIES_$starter" "$starter non compare nella selezione canonica del laboratorio."
}
Assert-True (-not $canonicalStarterContent.Contains('TRAINER_MAY_ROUTE_103')) 'La milestone canonica non deve introdurre una battaglia contro Lia.'

Write-Output 'Full Ausonia starter trio validation passed.'
