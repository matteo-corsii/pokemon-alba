param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Get-Block([byte[]]$data, [int]$width, [int]$x, [int]$y) { [BitConverter]::ToUInt16($data, 2 * ($y * $width + $x)) }

$map = Read-Json 'data/maps/BorgoDiCastello/map.json'
$scripts = Get-Content (Join-Path $RepositoryRoot 'data/maps/BorgoDiCastello/scripts.inc') -Raw
$flags = Get-Content (Join-Path $RepositoryRoot 'include/constants/flags.h') -Raw
$flagsFrlg = Get-Content (Join-Path $RepositoryRoot 'include/constants/flags_frlg.h') -Raw
$blocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/BorgoDiCastello/map.bin'))
$attrs = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/primary/general/metatile_attributes.bin'))

Assert-True ($map.id -eq 'MAP_BORGO_DI_CASTELLO') 'Wrong map.'
Assert-True ($flags -match '(?m)^#define FLAG_HIDE_BORGO_DI_CASTELLO_NICO_LIA 0x90F$') 'Emerald hide flag is incorrect.'
Assert-True ($flags -match '(?m)^#define FLAG_BORGO_NICO_LIA_INTRO_DONE 0x910$') 'Emerald completion flag is incorrect.'
Assert-True ($flagsFrlg -match '(?m)^#define FLAG_HIDE_BORGO_DI_CASTELLO_NICO_LIA\s+0x90F$') 'FRLG hide flag is incorrect.'
Assert-True ($flagsFrlg -match '(?m)^#define FLAG_BORGO_NICO_LIA_INTRO_DONE\s+0x910$') 'FRLG completion flag is incorrect.'

$expectedObjects = @(
    @{ id = 'LOCALID_BORGO_DI_CASTELLO_LIA_INTRO'; gfx = 'OBJ_EVENT_GFX_MAY_NORMAL'; x = 28; y = 55; facing = 'MOVEMENT_TYPE_FACE_RIGHT' },
    @{ id = 'LOCALID_BORGO_DI_CASTELLO_NICO_INTRO'; gfx = 'OBJ_EVENT_GFX_BRENDAN_NORMAL'; x = 31; y = 55; facing = 'MOVEMENT_TYPE_FACE_LEFT' }
)
foreach ($expected in $expectedObjects) {
    $event = @($map.object_events | Where-Object { $_.local_id -eq $expected.id })
    Assert-True ($event.Count -eq 1) "Missing or duplicate intro object $($expected.id)."
    $event = $event[0]
    Assert-True ($event.graphics_id -eq $expected.gfx -and [int]$event.x -eq $expected.x -and [int]$event.y -eq $expected.y -and [int]$event.elevation -eq 3 -and $event.movement_type -eq $expected.facing -and [int]$event.movement_range_x -eq 0 -and [int]$event.movement_range_y -eq 0) "Intro object $($expected.id) has incorrect placement or movement."
    Assert-True ($event.trainer_type -eq 'TRAINER_TYPE_NONE' -and $event.script -eq 'BorgoDiCastello_EventScript_StartNicoLiaIntro' -and $event.flag -eq 'FLAG_HIDE_BORGO_DI_CASTELLO_NICO_LIA') "Intro object $($expected.id) has incorrect scene binding."
    foreach ($tileY in @([int]$expected['y'], ([int]$expected['y'] - 1))) {
        $block = Get-Block $blocks 60 ([int]$expected['x']) $tileY
        Assert-True ((($block -shr 10) -band 1) -eq 0 -and (($block -shr 12) -band 0xF) -eq 3) "Intro object $($expected.id) or its northward exit is not walkable at ($($expected.x),$tileY)."
    }
}

$triggers = @($map.coord_events | Where-Object { $_.script -eq 'BorgoDiCastello_EventScript_StartNicoLiaIntro' })
Assert-True ($triggers.Count -eq 4) 'The intro must have exactly four south-approach triggers.'
foreach ($x in 28..31) {
    $event = @($triggers | Where-Object { [int]$_.x -eq $x -and [int]$_.y -eq 57 -and [int]$_.elevation -eq 3 -and $_.type -eq 'trigger' })
    Assert-True ($event.Count -eq 1) "Missing intro trigger at ($x,57)."
}
Assert-True (@($map.coord_events | Where-Object { [int]$_.y -eq 59 }).Count -eq 0) 'The intro must not trigger in the map-connection row.'

foreach ($required in @(
    'map_script MAP_SCRIPT_ON_TRANSITION, BorgoDiCastello_OnTransition',
    'goto_if_set FLAG_BORGO_NICO_LIA_INTRO_DONE, BorgoDiCastello_OnTransition_HideIntroParty',
    'goto_if_unset FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE, BorgoDiCastello_OnTransition_HideIntroParty',
    'clearflag FLAG_HIDE_BORGO_DI_CASTELLO_NICO_LIA',
    'setvar VAR_TEMP_0, 0',
    'lockall',
    'setflag FLAG_BORGO_NICO_LIA_INTRO_DONE',
    'setvar VAR_TEMP_0, 1',
    'setflag FLAG_HIDE_BORGO_DI_CASTELLO_NICO_LIA',
    'removeobject LOCALID_BORGO_DI_CASTELLO_LIA_INTRO',
    'removeobject LOCALID_BORGO_DI_CASTELLO_NICO_INTRO',
    'releaseall')) {
    Assert-True ($scripts.Contains($required)) "Missing intro-scene instruction: $required"
}

$pages = @(
    @('NICO: Eccoti.', 'Abbiamo chiesto in giro.'),
    @('LIA: Alla Villa conservano', 'vecchie mappe.'),
    @('E documenti sul', 'territorio.'),
    @('NICO: Per capire le falde,', 'partiamo da quei documenti.'),
    @('LIA: Magari ci aiuteranno', "con l'Emissario..."),
    @('NICO: Una cosa alla volta.'),
    @('Prima leggiamo', 'quei documenti.'),
    @('NICO: Ci troviamo', 'alla Villa.')
)
$expectedSceneLines = @($pages | ForEach-Object { $_ })
foreach ($line in $expectedSceneLines) { Assert-True ($scripts.Contains($line)) "Missing approved intro text line: $line" }

$widthBlock = [regex]::Match((Get-Content (Join-Path $RepositoryRoot 'src/fonts.c') -Raw), '(?s)gFontNormalLatinGlyphWidths\[\]\s*=\s*\{(.*?)\};').Groups[1].Value
$widths = @([regex]::Matches($widthBlock, '\d+') | ForEach-Object { [int]$_.Value })
function Get-NormalGlyphWidth([char]$character) {
    $code = if ($character -eq ' ') { 0 } elseif ($character -eq ',') { 0xB8 } elseif ($character -eq '.') { 0xAD } elseif ($character -eq ':') { 0xF0 } elseif ($character -eq "'") { 0xB4 } elseif ($character -ge 'A' -and $character -le 'Z') { 0xBB + ([int]$character - [int][char]'A') } elseif ($character -ge 'a' -and $character -le 'z') { 0xD5 + ([int]$character - [int][char]'a') } else { throw "Unsupported scene character: $character" }
    return $widths[$code]
}
foreach ($page in $pages) {
    Assert-True ($page.Count -le 2) 'An intro text page has more than two lines.'
    foreach ($line in $page) {
        $width = 0
        foreach ($character in $line.ToCharArray()) { $width += Get-NormalGlyphWidth $character }
        Assert-True ($width -le 180) "Intro text line exceeds the conservative 180-pixel field-message limit ($width): $line"
    }
}

$sceneText = [string]::Join("`n", $expectedSceneLines)
foreach ($forbidden in @('ECO', 'RIFLESSO', 'AUREA', 'LARICIA')) { Assert-True ($sceneText -notmatch "(?i)\b$forbidden\b") "Intro text must not mention $forbidden." }
Assert-True ($sceneText -notmatch '(?i)Emissario.*falde|falde.*Emissario') 'Intro text must not assert a causal link between the Emissario and the aquifers.'

$changedBinaries = & git -C $RepositoryRoot diff --name-only -- data/layouts/BorgoDiCastello/map.bin data/layouts/VillaPapaleGiardini/map.bin data/layouts/VillaPapaleInterno/map.bin
Assert-True (@($changedBinaries).Count -eq 0) 'The intro scene must not modify map.bin files.'
$changedTrainerData = & git -C $RepositoryRoot diff --name-only -- src/data/trainers.party src/data/trainers_frlg.party
Assert-True (@($changedTrainerData).Count -eq 0) 'The intro scene must not modify trainer data.'
Write-Output 'Borgo di Castello intro scene: PASS'
