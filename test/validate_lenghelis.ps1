$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
Set-Location $repoRoot

function Assert-True([bool]$Condition, [string]$Message) { if (-not $Condition) { throw $Message } }
function Assert-Contains([string]$Text, [string]$Expected, [string]$Context) {
    Assert-True $Text.Contains($Expected) "$Context is missing: $Expected"
}
function Get-Section([string]$Text, [string]$Start, [string]$End) {
    $a = $Text.IndexOf($Start); Assert-True ($a -ge 0) "Missing section: $Start"
    $b = $Text.IndexOf($End, $a + $Start.Length); Assert-True ($b -gt $a) "Missing section end: $End"
    return $Text.Substring($a, $b - $a)
}

$species = (Get-Content 'include/constants/species.h' -Raw) -replace "`r`n", "`n"
$dex = (Get-Content 'include/constants/pokedex.h' -Raw) -replace "`r`n", "`n"
$infoText = (Get-Content 'src/data/pokemon/species_info.h' -Raw -Encoding UTF8) -replace "`r`n", "`n"
$graphics = Get-Content 'src/data/graphics/pokemon.h' -Raw
$egg = Get-Content 'src/data/pokemon/egg_moves.h' -Raw
$learnables = Get-Content 'src/data/pokemon/all_learnables.json' -Raw | ConvertFrom-Json
$wild = Get-Content 'src/data/wild_encounters.json' -Raw
$record = Get-Section $infoText '[SPECIES_LENGHELIS] =' "`n    },"

Assert-Contains $species "SPECIES_MOLOSPSY,`n    SPECIES_LENGHELIS,`n    SPECIES_LUSCINCO,`n    SPECIES_LUSCERP,`n    SPECIES_LUMELLA,`n    SPECIES_OMPHALUX,`n    SPECIES_PALUDIX,`n    SPECIES_SANGUILEX,`n    SPECIES_TRITINO,`n    SPECIES_TRICREST,`n    SPECIES_SALAMPOLLA,`n    SPECIES_ALCHIMANDRA,`n    SPECIES_CUSTOM_END," 'Species append-only order'
Assert-Contains $dex "NATIONAL_DEX_MOLOSPSY,`n    NATIONAL_DEX_LENGHELIS,`n    NATIONAL_DEX_LUSCINCO,`n    NATIONAL_DEX_LUSCERP," 'National Dex append-only order'
Assert-Contains $dex '#define NATIONAL_DEX_COUNT  NATIONAL_DEX_ALCHIMANDRA' 'National Dex count'
Assert-Contains $graphics 'gMonFrontPic_Lenghelis' 'Lenghelis graphics symbols'
Assert-Contains $graphics 'gMonBackPic_Lenghelis' 'Lenghelis graphics symbols'
Assert-Contains $graphics 'gMonPalette_Lenghelis' 'Lenghelis graphics symbols'
Assert-Contains $graphics 'gMonShinyPalette_Lenghelis' 'Lenghelis graphics symbols'
Assert-Contains $graphics 'gMonIcon_Lenghelis' 'Lenghelis graphics symbols'

foreach ($asset in @('anim_front.png','back.png','icon.png','normal.pal','shiny.pal')) {
    Assert-True (Test-Path "graphics/pokemon/lenghelis/$asset") "Missing Lenghelis asset: $asset"
}
Assert-True ((Get-ChildItem graphics/pokemon/lenghelis -File).Count -eq 5) 'Lenghelis must contain exactly five assets'
foreach ($forbidden in @('*.zip','*preview*','*concept*')) {
    Assert-True (@(Get-ChildItem graphics/pokemon/lenghelis -File -Filter $forbidden).Count -eq 0) "Forbidden Lenghelis asset: $forbidden"
}

foreach ($expected in @(
    '.baseHP        = 65', '.baseAttack    = 50', '.baseDefense   = 65',
    '.baseSpeed     = 85', '.baseSpAttack  = 90', '.baseSpDefense = 85',
    '.types = MON_TYPES(TYPE_GHOST, TYPE_FAIRY)', '.catchRate = 90', '.expYield = 145',
    '.evYield_SpAttack = 1', '.genderRatio = PERCENT_FEMALE(50)', '.eggCycles = 20',
    '.friendship = 70', '.growthRate = GROWTH_MEDIUM_FAST',
    '.eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD, EGG_GROUP_AMORPHOUS)',
    '.abilities = { ABILITY_ILLUMINATE, ABILITY_FRISK, ABILITY_INFILTRATOR }',
    '.bodyColor = BODY_COLOR_PURPLE', '.speciesName = _("Lenghelis")',
    '.cryId = CRY_MISDREAVUS', '.natDexNum = NATIONAL_DEX_LENGHELIS',
    '.categoryName = _("SPIRITELLO")', '.height = 6', '.weight = 72',
    '.frontPic = gMonFrontPic_Lenghelis', '.backPic = gMonBackPic_Lenghelis',
    '.palette = gMonPalette_Lenghelis', '.shinyPalette = gMonShinyPalette_Lenghelis',
    '.iconSprite = gMonIcon_Lenghelis', '.frontPicSize = MON_COORDS_SIZE(64, 64)',
    '.backPicSize = MON_COORDS_SIZE(64, 64)', '.frontAnimId = ANIM_H_JUMPS',
    '.backAnimId = BACK_ANIM_CONCAVE_ARC_SMALL', 'FOOTPRINT(Espeon)',
    'sPicTable_Espeon', 'gOverworldPalette_Espeon', 'gShinyOverworldPalette_Espeon',
    '.levelUpLearnset = sLenghelisLevelUpLearnset',
    '.teachableLearnset = sLenghelisTeachableLearnset',
    '.eggMoveLearnset = sLenghelisEggMoveLearnset'
)) { Assert-Contains $record $expected 'Lenghelis species record' }
Assert-True ($record -notmatch 'EVOLUTION\(') 'Lenghelis must be single-stage'

$levels = [regex]::Matches((Get-Section $infoText 'static const struct LevelUpMove sLenghelisLevelUpLearnset[]' '};'), 'LEVEL_UP_MOVE\(\s*(\d+),\s*(MOVE_[A-Z0-9_]+)\)') | ForEach-Object { "$($_.Groups[1].Value):$($_.Groups[2].Value)" }
$expectedLevels = @('1:MOVE_ASTONISH','1:MOVE_TAIL_WHIP','4:MOVE_BABY_DOLL_EYES','7:MOVE_FAIRY_WIND','10:MOVE_CONFUSE_RAY','13:MOVE_QUICK_ATTACK','16:MOVE_NIGHT_SHADE','20:MOVE_DISARMING_VOICE','24:MOVE_WILL_O_WISP','28:MOVE_HEX','32:MOVE_MOONLIGHT','36:MOVE_SWIFT','40:MOVE_SHADOW_BALL','44:MOVE_DAZZLING_GLEAM','48:MOVE_MYSTICAL_FIRE','52:MOVE_MOONBLAST')
Assert-True (($levels -join ',') -eq ($expectedLevels -join ',')) 'Lenghelis level-up learnset differs'
foreach ($move in @('MOVE_DISABLE','MOVE_WISH','MOVE_YAWN','MOVE_CURSE','MOVE_DESTINY_BOND','MOVE_MISTY_TERRAIN')) { Assert-Contains $egg $move 'Lenghelis Egg Moves' }
$teachExpected = @('MOVE_CALM_MIND','MOVE_PROTECT','MOVE_REST','MOVE_SLEEP_TALK','MOVE_SUBSTITUTE','MOVE_SHADOW_BALL','MOVE_DAZZLING_GLEAM','MOVE_PSYCHIC','MOVE_PSYSHOCK','MOVE_THIEF','MOVE_TAUNT','MOVE_LIGHT_SCREEN','MOVE_REFLECT','MOVE_WILL_O_WISP','MOVE_HEX','MOVE_MYSTICAL_FIRE','MOVE_SWIFT','MOVE_HELPING_HAND','MOVE_TRICK','MOVE_SKILL_SWAP','MOVE_DREAM_EATER')
Assert-True ($learnables.LENGHELIS.Count -eq $teachExpected.Count) 'Lenghelis teachable count differs'
for ($i = 0; $i -lt $teachExpected.Count; $i++) { Assert-True ($learnables.LENGHELIS[$i] -eq $teachExpected[$i]) "Lenghelis teachables differ at index $i" }
Write-Host 'Lenghelis validation passed.' -ForegroundColor Green
