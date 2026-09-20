Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot

function Assert-True([bool]$condition, [string]$message) {
    if (-not $condition) { throw $message }
}

$speciesPath = Join-Path $root 'include/constants/species.h'
$dexPath = Join-Path $root 'include/constants/pokedex.h'
$infoPath = Join-Path $root 'src/data/pokemon/species_info.h'
$graphicsPath = Join-Path $root 'src/data/graphics/pokemon.h'
$learnablesPath = Join-Path $root 'src/data/pokemon/all_learnables.json'
$eggPath = Join-Path $root 'src/data/pokemon/egg_moves.h'
$wildPath = Join-Path $root 'src/data/wild_encounters.json'

$species = Get-Content -Raw $speciesPath
$dex = Get-Content -Raw $dexPath
$info = Get-Content -Raw $infoPath
$graphics = Get-Content -Raw $graphicsPath
$eggs = Get-Content -Raw $eggPath
$learnables = Get-Content -Raw $learnablesPath | ConvertFrom-Json
$wild = Get-Content -Raw $wildPath
$wildData = $wild | ConvertFrom-Json
$valleEntries = @($wildData.wild_encounter_groups | ForEach-Object { $_.encounters } | Where-Object { $null -ne $_ -and $_.PSObject.Properties.Name -contains 'map' -and $_.map -eq 'MAP_PONTE_VALLE_LARICIA' })
$valleJson = $valleEntries | ConvertTo-Json -Depth 20
$nativeWild = @('SPECIES_VITEMOSTO', 'SPECIES_PORCHIGNIS', 'SPECIES_FRASCHIETTO')
$nonWildEvolutions = @('SPECIES_BRONZOVERRO', 'SPECIES_FRASCOTTO')
foreach ($native in $nativeWild) {
    Assert-True ($valleJson -match "\b$native\b") "$native must appear in Valle di Laricia encounters"
    $otherEntries = @($wildData.wild_encounter_groups | ForEach-Object { $_.encounters } | Where-Object { $null -ne $_ -and $_.PSObject.Properties.Name -contains 'map' -and $_.map -ne 'MAP_PONTE_VALLE_LARICIA' } | ConvertTo-Json -Depth 20)
    Assert-True (-not ($otherEntries -match "\b$native\b")) "$native is only authorized in Valle di Laricia encounters"
}
foreach ($evolved in $nonWildEvolutions) {
    Assert-True (-not ($valleJson -match "\b$evolved\b")) "$evolved must not appear in Valle di Laricia encounters"
}

$expected = @{
    VITEMOSTO   = @{ Stats = @(75,55,70,80,100,65); Types = 'TYPE_GRASS, TYPE_FIRE'; Evo = $null }
    PORCHIGNIS  = @{ Stats = @(65,75,65,45,45,30); Types = 'TYPE_FIRE'; Evo = 'SPECIES_BRONZOVERRO' }
    BRONZOVERRO = @{ Stats = @(95,115,110,55,55,75); Types = 'TYPE_FIRE, TYPE_STEEL'; Evo = $null }
    FRASCHIETTO = @{ Stats = @(55,75,50,70,35,35); Types = 'TYPE_FIGHTING'; Evo = 'SPECIES_FRASCOTTO' }
    FRASCOTTO   = @{ Stats = @(85,110,75,85,65,80); Types = 'TYPE_FIGHTING, TYPE_FIRE'; Evo = $null }
}

foreach ($name in $expected.Keys) {
    $constant = "SPECIES_$name"
    $dexConstant = "NATIONAL_DEX_$name"
    Assert-True (([regex]::Matches($species, "\b$constant\b")).Count -eq 1) "$constant must be unique"
    Assert-True (([regex]::Matches($dex, "\b$dexConstant,\s*$", [System.Text.RegularExpressions.RegexOptions]::Multiline)).Count -eq 1) "$dexConstant must be unique in the National Dex enum"
    $entry = [regex]::Match($info, "(?s)\[$constant\]\s*=\s*\{(.*?)\n\s*\},")
    Assert-True $entry.Success "Missing species info for $name"
    foreach ($stat in @('HP','Attack','Defense','Speed','SpAttack','SpDefense')) {
        $expectedValue = $expected[$name].Stats[@('HP','Attack','Defense','Speed','SpAttack','SpDefense').IndexOf($stat)]
        Assert-True ($entry.Value -match "\.base$stat\s*=\s*$expectedValue") "$name base$stat is incorrect"
    }
    Assert-True ($entry.Value -match [regex]::Escape($expected[$name].Types)) "$name types are incorrect"
    Assert-True ($entry.Value -match "\.levelUpLearnset\s*=\s*s$($name.Substring(0,1) + $name.Substring(1).ToLower())LevelUpLearnset") "$name level-up learnset is missing"
    Assert-True ($entry.Value -match 'EXPLICIT_TEACHABLES') "$name must use explicit teachables"
    Assert-True ($entry.Value -match "\.teachableLearnset\s*=\s*s$($name.Substring(0,1) + $name.Substring(1).ToLower())TeachableLearnset") "$name teachable registration is missing"
    Assert-True ($learnables.PSObject.Properties.Name -contains $name) "$name missing from all_learnables.json"
    Assert-True ($graphics -match "gMonFrontPic_$($name.Substring(0,1) + $name.Substring(1).ToLower())") "$name battle graphics registration is missing"
    foreach ($asset in @('anim_front.png','back.png','icon.png','normal.pal','shiny.pal')) {
        Assert-True (Test-Path (Join-Path $root "graphics/pokemon/$($name.ToLower())/$asset")) "$name graphics asset missing: $asset"
    }
    Assert-True ($eggs -match "s$($name.Substring(0,1) + $name.Substring(1).ToLower())EggMoveLearnset") "$name egg-move registration is missing"
    Assert-True (-not ((Get-ChildItem (Join-Path $root 'src/data') -Recurse -File | Where-Object { $_.Name -match 'trainer' } | Get-Content -Raw) -match "\b$constant\b")) "$name must not yet appear in trainer data"
}

Assert-True ($info -match 'EVO_LEVEL, 30, SPECIES_BRONZOVERRO') 'Porchignis evolution must be level 30'
Assert-True ($info -match 'EVO_LEVEL, 30, SPECIES_FRASCOTTO') 'Fraschietto evolution must be level 30'
$vitemostoEntry = [regex]::Match($info, '(?s)\[SPECIES_VITEMOSTO\]\s*=\s*\{(.*?)\n\s*\},')
Assert-True (-not ($vitemostoEntry.Value -match '\.evolutions\s*=')) 'Vitemosto must not evolve'
Write-Output 'Laricia species validator: PASS'
