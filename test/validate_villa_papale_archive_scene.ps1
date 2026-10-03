param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Get-Block([byte[]]$data, [int]$width, [int]$x, [int]$y) { [BitConverter]::ToUInt16($data, 2 * ($y * $width + $x)) }

$map = Read-Json 'data/maps/VillaPapaleInterno/map.json'
$scripts = Get-Content (Join-Path $RepositoryRoot 'data/maps/VillaPapaleInterno/scripts.inc') -Raw -Encoding utf8
$flags = Get-Content (Join-Path $RepositoryRoot 'include/constants/flags.h') -Raw
$flagsFrlg = Get-Content (Join-Path $RepositoryRoot 'include/constants/flags_frlg.h') -Raw
$blocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/VillaPapaleInterno/map.bin'))

Assert-True ($map.id -eq 'MAP_VILLA_PAPALE_INTERNO') 'Wrong Villa Papale map.'
Assert-True ($flags -match '(?m)^#define FLAG_HIDE_VILLA_PAPALE_ARCHIVIO_PARTY 0x911$') 'Emerald archive-party flag is incorrect.'
Assert-True ($flags -match '(?m)^#define FLAG_VILLA_PAPALE_ARCHIVIO_DONE 0x912$') 'Emerald archive completion flag is incorrect.'
Assert-True ($flagsFrlg -match '(?m)^#define FLAG_HIDE_VILLA_PAPALE_ARCHIVIO_PARTY\s+0x911$') 'FRLG archive-party flag is incorrect.'
Assert-True ($flagsFrlg -match '(?m)^#define FLAG_VILLA_PAPALE_ARCHIVIO_DONE\s+0x912$') 'FRLG archive completion flag is incorrect.'

$expectedObjects = @(
    @{ id = 'LOCALID_VILLA_PAPALE_INTERNO_ARCHIVISTA'; gfx = 'OBJ_EVENT_GFX_EXPERT_M'; x = 15; y = 7; facing = 'MOVEMENT_TYPE_FACE_DOWN'; flag = '0'; script = 'VillaPapaleInterno_EventScript_Archivista' },
    @{ id = 'LOCALID_VILLA_PAPALE_INTERNO_LIA_ARCHIVIO'; gfx = 'OBJ_EVENT_GFX_MAY_NORMAL'; x = 13; y = 7; facing = 'MOVEMENT_TYPE_FACE_RIGHT'; flag = 'FLAG_HIDE_VILLA_PAPALE_ARCHIVIO_PARTY'; script = 'VillaPapaleInterno_EventScript_LiaArchivio' },
    @{ id = 'LOCALID_VILLA_PAPALE_INTERNO_NICO_ARCHIVIO'; gfx = 'OBJ_EVENT_GFX_BRENDAN_NORMAL'; x = 17; y = 7; facing = 'MOVEMENT_TYPE_FACE_LEFT'; flag = 'FLAG_HIDE_VILLA_PAPALE_ARCHIVIO_PARTY'; script = 'VillaPapaleInterno_EventScript_NicoArchivio' }
)
foreach ($expected in $expectedObjects) {
    $event = @($map.object_events | Where-Object { $_.local_id -eq $expected.id })
    Assert-True ($event.Count -eq 1) "Missing or duplicate archive object $($expected.id)."
    $event = $event[0]
    Assert-True ($event.graphics_id -eq $expected.gfx -and [int]$event.x -eq $expected.x -and [int]$event.y -eq $expected.y -and [int]$event.elevation -eq 3 -and $event.movement_type -eq $expected.facing -and [int]$event.movement_range_x -eq 0 -and [int]$event.movement_range_y -eq 0 -and $event.trainer_type -eq 'TRAINER_TYPE_NONE' -and $event.script -eq $expected.script -and $event.flag -eq $expected.flag) "Archive object $($expected.id) is incorrect."
    $block = Get-Block $blocks 32 ([int]$expected.x) ([int]$expected.y)
    Assert-True ((($block -shr 10) -band 1) -eq 0 -and (($block -shr 12) -band 0xF) -eq 3) "Archive object $($expected.id) is not on an accessible normal-elevation tile."
}
Assert-True (@($map.coord_events).Count -eq 0) 'Archive scene must begin through the Archivista interaction, not a coordinate trigger.'

foreach ($required in @(
    'map_script MAP_SCRIPT_ON_TRANSITION, VillaPapaleInterno_OnTransition',
    'goto_if_unset FLAG_BORGO_NICO_LIA_INTRO_DONE, VillaPapaleInterno_OnTransition_HideArchiveParty',
    'clearflag FLAG_HIDE_VILLA_PAPALE_ARCHIVIO_PARTY',
    'setflag FLAG_HIDE_VILLA_PAPALE_ARCHIVIO_PARTY',
    'goto_if_unset FLAG_BORGO_NICO_LIA_INTRO_DONE, VillaPapaleInterno_EventScript_ArchivistaNotReady',
    'goto_if_set FLAG_VILLA_PAPALE_ARCHIVIO_DONE, VillaPapaleInterno_EventScript_ArchivistaPost',
    'setflag FLAG_VILLA_PAPALE_ARCHIVIO_DONE',
    'VillaPapaleInterno_EventScript_LiaArchivioPost',
    'VillaPapaleInterno_EventScript_NicoArchivioPost')) {
    Assert-True ($scripts.Contains($required)) "Missing archive-scene instruction: $required"
}

$pokemon = 'Pok' + [char]0x00E9 + 'mon'
$pages = @(
    @('ARCHIVISTA: Mi avete chiesto', 'le carte piu antiche.'),
    @('Quelle su sorgenti,', 'laghi e falde.'),
    @('NICO: Le confrontiamo con', 'quanto accade oggi.'),
    @('LIA: Qualcosa cambia troppo', 'in fretta.'),
    @('ARCHIVISTA: Il territorio cambia.', 'Lo ha sempre fatto.'),
    @('Ma certe trasformazioni', 'richiedono molto tempo.'),
    @('NICO: E qui sembra accadere', 'tutto troppo in fretta.'),
    @('ARCHIVISTA:', 'Guardate questa carta.'),
    @("E' molto antica."),
    @('La conca che oggi chiamiamo', 'Valle di Laricia, un tempo,'),
    @('era occupata da un lago.'),
    @('LIA: Un lago?', 'Oggi ci sono campi.'),
    @('ARCHIVISTA: Il territorio', 'cambia piu di quanto'),
    @('immaginiamo.'),
    @('Questa e una cronaca,', 'non un testo scientifico.'),
    @('Riporta testimonianze', 'molto antiche.'),
    @('Quando la terra viene ferita,', "alcuni $pokemon sembrano"),
    @('rispondere al suo richiamo.'),
    @('LIA: Mi ricorda quello', "che abbiamo visto all'Emissario."),
    @('NICO: Somiglia.', 'Ma non sappiamo abbastanza.'),
    @('Rari erano coloro', 'che sostenevano di udire'),
    @('quel richiamo insieme', "ai propri $pokemon."),
    @('LIA: Udirlo?'),
    @('NICO: Potrebbe voler dire', 'qualsiasi cosa.'),
    @('LIA: Oppure no.'),
    @('NICO: Prima servono', 'altri fatti.'),
    @('ARCHIVISTA: Curioso...', 'Non siete i primi'),
    @('a chiedere queste carte.'),
    @('NICO: Chi era?'),
    @('ARCHIVISTA: Non conosco', "l'identita'."),
    @('Chiese vecchie mappe,', 'documenti sulle falde'),
    @('e cronache antiche.'),
    @('NICO: Se vogliamo capire', "quanto c'e di vero..."),
    @('dobbiamo vedere Laricia', 'con i nostri occhi.'),
    @('LIA: Continuo a pensare', 'a quel richiamo.'),
    @('NICO: Ne parliamo quando', 'avremo fatti concreti.'),
    @('Le carte raccontano molto,', 'se sappiamo cosa cercare.'),
    @('NICO: Prima i fatti.', 'A Laricia capiremo di piu.')
)
$sceneLines = @($pages | ForEach-Object { $_ })
foreach ($line in $sceneLines) { Assert-True ($scripts.Contains($line)) "Missing archive scene line: $line" }

$widthBlock = [regex]::Match((Get-Content (Join-Path $RepositoryRoot 'src/fonts.c') -Raw), '(?s)gFontNormalLatinGlyphWidths\[\]\s*=\s*\{(.*?)\};').Groups[1].Value
$widths = @([regex]::Matches($widthBlock, '\d+') | ForEach-Object { [int]$_.Value })
function Get-NormalGlyphWidth([char]$character) {
    $code = if ($character -eq ' ') { 0 } elseif ([int][char]$character -eq 0x00E9) { 0x1B } elseif ($character -eq ',') { 0xB8 } elseif ($character -eq '.') { 0xAD } elseif ($character -eq ':') { 0xF0 } elseif ($character -eq "'") { 0xB4 } elseif ($character -eq '?') { 0xAF } elseif ($character -ge 'A' -and $character -le 'Z') { 0xBB + ([int]$character - [int][char]'A') } elseif ($character -ge 'a' -and $character -le 'z') { 0xD5 + ([int]$character - [int][char]'a') } else { throw "Unsupported archive-scene character: $character" }
    return $widths[$code]
}
foreach ($page in $pages) {
    Assert-True ($page.Count -le 2) 'An archive-scene text page has more than two lines.'
    foreach ($line in $page) {
        $width = 0
        foreach ($character in $line.ToCharArray()) { $width += Get-NormalGlyphWidth $character }
        Assert-True ($width -le 180) "Archive-scene line exceeds the conservative 180-pixel limit ($width): $line"
    }
}

$sceneText = [string]::Join("`n", $sceneLines)
$pokemonPattern = [regex]::Escape($pokemon)
Assert-True ($sceneText -match "Quando la terra viene ferita,\s+alcuni $pokemonPattern sembrano\s+rispondere al suo richiamo\.") 'The canonical ancient-chronicle line is missing.'
Assert-True ($sceneText -match "Rari erano coloro\s+che sostenevano di udire\s+quel richiamo insieme\s+ai propri $pokemonPattern\.") 'The required foreshadow is missing.'
Assert-True ($sceneText -match 'Valle di Laricia, un tempo,\s+era occupata da un lago\.') 'Laricia must be described as the site of an ancient lake.'
Assert-True ($sceneText -notmatch '(?i)era acqua|riflesso') 'Archive scene reveals forbidden terminology.'
Assert-True ($sceneText -notmatch '(?i)aurea.*(responsabile|colpa)|responsabile.*aurea') 'Archive scene must not identify Aurea as responsible.'
Assert-True ($sceneText -notmatch '(?i)(eco.*falde|falde.*eco)') 'Archive scene must not causally link Eco and the aquifers.'

$changedBinaries = & git -C $RepositoryRoot diff --name-only -- data/layouts/VillaPapaleInterno/map.bin data/layouts/VillaPapaleGiardini/map.bin data/layouts/BorgoDiCastello/map.bin data/layouts/LagoDiAlbera/map.bin
Assert-True (@($changedBinaries).Count -eq 0) 'Archive scene must not modify map.bin files.'
$changedTrainerData = & git -C $RepositoryRoot diff --name-only -- src/data/trainers.party src/data/trainers_frlg.party
Assert-True (@($changedTrainerData).Count -eq 0) 'Archive scene must not modify trainer data.'
Assert-True ((Get-Content (Join-Path $RepositoryRoot 'src/data/wild_encounters.json') -Raw) -notmatch 'MAP_VILLA_PAPALE_INTERNO') 'Archive scene must not add encounters.'
Write-Output 'Villa Papale archive scene: PASS'
