param(
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) {
        throw $Message
    }
}

function Assert-Contains([string]$Text, [string]$Expected, [string]$Message) {
    Assert-True ($Text.Contains($Expected)) $Message
}

$strings = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/strings.c') -Raw
$partyMenu = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/party_menu.h') -Raw
$summary = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/pokemon_summary_screen.c') -Raw
$moves = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/moves_info.h') -Raw
$species = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/pokemon/species_info.h') -Raw
$starter = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/starter_choose.c') -Raw

$sharedLabels = [ordered]@{
    'gText_MenuBag' = 'BORSA'
    'gText_MenuSave' = 'SALVA'
    'gText_MenuOption' = 'OPZIONI'
    'gText_MenuExit' = 'ESCI'
    'gMenuText_Give' = 'DAI'
    'gText_Cancel2' = 'ANNULLA'
    'gText_PkmnInfo' = 'INFO POKéMON'
    'gText_PkmnSkills' = 'STATISTICHE'
    'gText_BattleMoves' = 'MOSSE LOTTA'
}

foreach ($entry in $sharedLabels.GetEnumerator()) {
    $declaration = "const u8 $($entry.Key)[] = _(`"$($entry.Value)`");"
    Assert-Contains $strings $declaration "Etichetta italiana mancante o inattesa: $($entry.Key)."
}

$partyLabels = [ordered]@{
    'MENU_SUMMARY' = 'RIEPILOGO'
    'MENU_SWITCH' = 'SPOSTA'
    'MENU_ITEM' = 'STRUMENTO'
    'MENU_TAKE_ITEM' = 'PRENDI'
    'MENU_MOVE_ITEM' = 'SPOSTA'
    'MENU_MAIL' = 'POSTA'
    'MENU_READ' = 'LEGGI'
}

foreach ($entry in $partyLabels.GetEnumerator()) {
    $pattern = "\[$([regex]::Escape($entry.Key))\]\s*=\s*\{COMPOUND_STRING\(`"$([regex]::Escape($entry.Value))`"\)"
    Assert-True ([regex]::IsMatch($partyMenu, $pattern)) "Voce contestuale italiana mancante o inattesa: $($entry.Key)."
}

Assert-Contains $summary 'const u8* gText_SkillPageStats = COMPOUND_STRING("STAT.");' 'Etichetta STAT. mancante dal riepilogo.'
Assert-Contains $summary 'const u8* gText_Rename = COMPOUND_STRING("RINOMINA");' 'Etichetta RINOMINA mancante dal riepilogo.'

$mudSlapMatch = [regex]::Match($moves, '(?s)\[MOVE_MUD_SLAP\]\s*=\s*\{(?<body>.*?)\n\s*\},\n\n\s*\[MOVE_OCTAZOOKA\]')
Assert-True $mudSlapMatch.Success 'Record MOVE_MUD_SLAP non individuato.'
$mudSlap = $mudSlapMatch.Groups['body'].Value

foreach ($expected in @(
    '.name = COMPOUND_STRING("Fangosberla")',
    '"Scaglia fango sul bersaglio\n"',
    '"riducendone la precisione.")',
    '.effect = EFFECT_HIT',
    '.power = 20',
    '.type = TYPE_GROUND',
    '.accuracy = 100',
    '.pp = 10',
    '.target = TARGET_SELECTED',
    '.priority = 0',
    '.category = DAMAGE_CATEGORY_SPECIAL',
    '.moveEffect = MOVE_EFFECT_STAT_MINUS',
    '.accuracy = 1',
    '.chance = 100',
    '.battleAnimScript = gBattleAnimMove_MudSlap',
    '.validApprenticeMove = TRUE'
)) {
    Assert-Contains $mudSlap $expected "MOVE_MUD_SLAP non conserva il campo previsto: $expected."
}

Assert-True (($moves | Select-String -Pattern 'COMPOUND_STRING\("Fangosberla"\)' -AllMatches).Matches.Count -eq 1) 'Fangosberla deve comparire in un solo record mossa.'
Assert-True ($species -match '(?s)static const struct LevelUpMove sCingermLevelUpLearnset\[\].*?LEVEL_UP_MOVE\(\s*7, MOVE_MUD_SLAP\)') 'Cingerm non impara più MOVE_MUD_SLAP al livello 7.'
Assert-True ($starter -match '#define GRASS_STARTER \(IS_FRLG \? SPECIES_BULBASAUR\s+: SPECIES_CINGERM\)') 'Lo starter Erba Emerald non è più Cingerm o lo starter FRLG è cambiato.'

Write-Output 'Italian pause menu and Mud-Slap localization validation passed.'
