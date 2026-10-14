param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Read-Block([byte[]]$bytes, [int]$width, [int]$x, [int]$y) { [BitConverter]::ToUInt16($bytes, 2 * ($y * $width + $x)) }
function Get-NormalGlyphWidth([char]$character, [int[]]$widths) {
    $code = if ($character -eq ' ') { 0 } elseif ($character -eq ',') { 0xB8 } elseif ($character -eq '.') { 0xAD } elseif ($character -eq ':') { 0xF0 } elseif ($character -ge 'A' -and $character -le 'Z') { 0xBB + ([int]$character - [int][char]'A') } elseif ($character -ge 'a' -and $character -le 'z') { 0xD5 + ([int]$character - [int][char]'a') } else { throw "Unsupported scene-text character: $character" }
    return $widths[$code]
}

$strada = Read-Json 'data/maps/StradaBorgoCisternoni/map.json'
$scripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/StradaBorgoCisternoni/scripts.inc') -Raw
$flagsEmerald = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/flags.h') -Raw
$flagsFrlg = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/flags_frlg.h') -Raw

foreach ($definition in @(
    @{ name = 'FLAG_STRADA_BORGO_CISTERNONI_NICO_LIA_DONE'; value = '0x913' },
    @{ name = 'FLAG_HIDE_STRADA_BORGO_CISTERNONI_NICO_LIA'; value = '0x914' }
)) {
    $pattern = "(?m)^#define\s+$($definition.name)\s+$($definition.value)\b"
    Assert-True ($flagsEmerald -match $pattern) "Missing Emerald scene flag: $($definition.name)"
    Assert-True ($flagsFrlg -match $pattern) "Missing FRLG scene flag: $($definition.name)"
}

$expectedNpcs = @(
    @{ id = 'LOCALID_STRADA_BORGO_CISTERNONI_NICO'; gfx = 'OBJ_EVENT_GFX_BRENDAN_NORMAL'; x = 23; y = 21 },
    @{ id = 'LOCALID_STRADA_BORGO_CISTERNONI_LIA'; gfx = 'OBJ_EVENT_GFX_MAY_NORMAL'; x = 26; y = 21 }
)
Assert-True (@($strada.object_events).Count -eq 6) 'Strada must retain Nico, Lia, and the three ambient NPCs plus the visible item.'
foreach ($expected in $expectedNpcs) {
    $npc = @($strada.object_events | Where-Object { $_.local_id -eq $expected.id })
    Assert-True ($npc.Count -eq 1) "Missing scene NPC $($expected.id)."
    Assert-True ($npc[0].graphics_id -eq $expected.gfx -and [int]$npc[0].x -eq $expected.x -and [int]$npc[0].y -eq $expected.y -and [int]$npc[0].elevation -eq 3 -and $npc[0].movement_type -eq 'MOVEMENT_TYPE_FACE_DOWN') "NPC placement is incorrect for $($expected.id)."
    Assert-True ($npc[0].trainer_type -eq 'TRAINER_TYPE_NONE' -and $npc[0].flag -eq 'FLAG_HIDE_STRADA_BORGO_CISTERNONI_NICO_LIA') "NPC gating is incorrect for $($expected.id)."
}

$triggerXs = @(22..30)
$triggers = @($strada.coord_events | Where-Object { $_.script -eq 'StradaBorgoCisternoni_EventScript_StartNicoLiaScene' })
Assert-True ($triggers.Count -eq $triggerXs.Count) 'Scene trigger count is incorrect.'
foreach ($x in $triggerXs) {
    $trigger = @($triggers | Where-Object { [int]$_.x -eq $x -and [int]$_.y -eq 23 -and [int]$_.elevation -eq 3 -and $_.var -eq 'VAR_TEMP_0' -and [int]$_.var_value -eq 0 })
    Assert-True ($trigger.Count -eq 1) "Missing or incorrect scene trigger $x,23."
}
Assert-True (@($strada.warp_events | Where-Object { [int]$_.x -eq 8 -and [int]$_.y -eq 29 }).Count -eq 1) 'The Itemfinder-house warp must remain unchanged.'
Assert-True (@($strada.bg_events).Count -eq 2) 'Route population must retain exactly its two hidden items.'

$blocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/StradaBorgoCisternoni/map.bin'))
$walkable = @{}
for ($y = 0; $y -lt 44; $y++) {
    for ($x = 0; $x -lt 36; $x++) {
        $raw = Read-Block $blocks 36 $x $y
        if ((($raw -shr 10) -band 1) -eq 0) { $walkable["$x,$y"] = $true }
    }
}
foreach ($point in @('23,21', '26,21', '23,22', '26,22')) { Assert-True $walkable.ContainsKey($point) "NPC start or exit cell is blocked: $point" }
foreach ($point in @('22,23', '30,23')) { Assert-True $walkable.ContainsKey($point) "Scene trigger is blocked: $point" }

# Removing the full trigger row disconnects Borgo from Route 103: the required scene cannot be bypassed.
$blocked = @{}
foreach ($x in $triggerXs) { $blocked["$x,23"] = $true }
$queue = New-Object 'System.Collections.Generic.Queue[string]'
$seen = @{}
foreach ($y in 0..3) { $key = "0,$y"; if ($walkable.ContainsKey($key)) { $seen[$key] = $true; $queue.Enqueue($key) } }
while ($queue.Count -gt 0) {
    $key = $queue.Dequeue(); $parts = $key.Split(','); $x = [int]$parts[0]; $y = [int]$parts[1]
    foreach ($delta in @(@(1,0), @(-1,0), @(0,1), @(0,-1))) {
        $next = "{0},{1}" -f ($x + $delta[0]), ($y + $delta[1])
        if ($walkable.ContainsKey($next) -and -not $blocked.ContainsKey($next) -and -not $seen.ContainsKey($next)) { $seen[$next] = $true; $queue.Enqueue($next) }
    }
}
foreach ($x in 16..19) { Assert-True (-not $seen.ContainsKey("$x,43")) 'Scene trigger can be bypassed on the Borgo-to-Route103 path.' }

foreach ($required in @(
    'map_script MAP_SCRIPT_ON_TRANSITION, StradaBorgoCisternoni_OnTransition',
    'goto_if_unset FLAG_VILLA_PAPALE_ARCHIVIO_DONE, StradaBorgoCisternoni_OnTransition_HideNicoLia',
    'goto_if_set FLAG_STRADA_BORGO_CISTERNONI_NICO_LIA_DONE, StradaBorgoCisternoni_OnTransition_HideNicoLia',
    'setflag FLAG_STRADA_BORGO_CISTERNONI_NICO_LIA_DONE',
    'setflag FLAG_HIDE_STRADA_BORGO_CISTERNONI_NICO_LIA',
    'removeobject LOCALID_STRADA_BORGO_CISTERNONI_NICO',
    'removeobject LOCALID_STRADA_BORGO_CISTERNONI_LIA'
)) { Assert-True ($scripts.Contains($required)) "Missing scene lifecycle instruction: $required" }

$pages = @(
    @('NICO: Le carte indicano', 'questa direzione.'),
    @('NICO: Se vogliamo capire', 'cosa sta succedendo,'),
    @('dobbiamo arrivare', 'a Laricia.'),
    @('LIA: Io continuo a pensare', 'a quella cronaca...'),
    @('NICO: Lo so.'),
    @('Ma prima vediamo', 'cosa troviamo sul posto.'),
    @('LIA: Allora andiamo.'),
    @('NICO: Scendiamo fino', 'alla Via dei Cisternoni.'),
    @('Da li proseguiamo', 'verso Laricia.')
)
$widthBlock = [regex]::Match((Get-Content (Join-Path $RepositoryRoot 'src/fonts.c') -Raw), '(?s)gFontNormalLatinGlyphWidths\[\]\s*=\s*\{(.*?)\};').Groups[1].Value
$widths = @([regex]::Matches($widthBlock, '\d+') | ForEach-Object { [int]$_.Value })
foreach ($page in $pages) {
    Assert-True ($page.Count -le 2) 'Scene text page has more than two lines.'
    foreach ($line in $page) {
        Assert-True ($scripts.Contains($line)) "Missing approved scene line: $line"
        $width = 0; foreach ($character in $line.ToCharArray()) { $width += Get-NormalGlyphWidth $character $widths }
        Assert-True ($width -le 180) "Scene line exceeds 180 pixels ($width): $line"
    }
}
$sceneOnly = [string]::Join("`n", @($pages | ForEach-Object { $_ }))
$cisternoniIndex = $scripts.IndexOf('alla Via dei Cisternoni.')
$routeIndex = $scripts.IndexOf('Da li proseguiamo')
$lariciaIndex = $scripts.IndexOf('verso Laricia.')
Assert-True ($cisternoniIndex -ge 0 -and $routeIndex -gt $cisternoniIndex -and $lariciaIndex -gt $routeIndex) 'Scene must direct the player to Cisternoni before Laricia.'
foreach ($forbidden in @('Ponte di Laricia', 'Valle di Laricia', 'ECO', 'RIFLESSO', 'AUREA')) { Assert-True ($sceneOnly -notmatch "(?i)$forbidden") "Scene contains forbidden premature lore: $forbidden" }

git -C $RepositoryRoot diff --quiet -- data/layouts/StradaBorgoCisternoni/map.bin data/layouts/BorgoDiCastello/map.bin data/layouts/Route103/map.bin data/maps/StradaBorgoCisternoni_Casa
Assert-True ($LASTEXITCODE -eq 0) 'Scene must not modify map.bin files or the Itemfinder house.'
Write-Output 'Strada Borgo-Cisternoni Nico/Lia scene: PASS'
