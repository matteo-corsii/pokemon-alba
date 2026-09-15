param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Get-NormalGlyphWidth([char]$character, [int[]]$widths) {
    $code = if ($character -eq ' ') { 0 } elseif ($character -eq ',') { 0xB8 } elseif ($character -eq '.') { 0xAD } elseif ($character -eq ':') { 0xF0 } elseif ($character -eq "'") { 0xB4 } elseif ($character -ge 'A' -and $character -le 'Z') { 0xBB + ([int]$character - [int][char]'A') } elseif ($character -ge 'a' -and $character -le 'z') { 0xD5 + ([int]$character - [int][char]'a') } else { throw "Unsupported house-text character: $character" }
    return $widths[$code]
}

$strada = Read-Json 'data/maps/StradaBorgoCisternoni/map.json'
$house = Read-Json 'data/maps/StradaBorgoCisternoni_Casa/map.json'
$layouts = (Read-Json 'data/layouts/layouts.json').layouts
$groups = Read-Json 'data/maps/map_groups.json'
$layout = @($layouts | Where-Object { $_.id -eq 'LAYOUT_STRADA_BORGO_CISTERNONI_CASA' })
$scripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/StradaBorgoCisternoni_Casa/scripts.inc') -Raw

Assert-True ($house.id -eq 'MAP_STRADA_BORGO_CISTERNONI_CASA') 'House map ID is incorrect.'
Assert-True ($house.layout -eq 'LAYOUT_STRADA_BORGO_CISTERNONI_CASA' -and $house.region_map_section -eq 'MAPSEC_ALBERA_STORICA' -and $house.map_type -eq 'MAP_TYPE_INDOOR') 'House map metadata is incorrect.'
Assert-True ($layout.Count -eq 1 -and [int]$layout[0].width -eq 10 -and [int]$layout[0].height -eq 8 -and $layout[0].primary_tileset -eq 'gTileset_Building' -and $layout[0].secondary_tileset -eq 'gTileset_GenericBuilding') 'House layout or tilesets are incorrect.'
$mapBin = Join-Path $RepositoryRoot $layout[0].blockdata_filepath
$borderBin = Join-Path $RepositoryRoot $layout[0].border_filepath
Assert-True ((Get-Item -LiteralPath $mapBin).Length -eq 160) 'House map.bin must be 10 x 8 x 2 bytes.'
Assert-True ((Get-Item -LiteralPath $borderBin).Length -eq 8) 'House border.bin size is incorrect.'
Assert-True (@($groups.gMapGroup_IndoorOldale | Where-Object { $_ -eq 'StradaBorgoCisternoni_Casa' }).Count -eq 1) 'House is not registered exactly once in IndoorOldale.'
Assert-True ((Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/event_scripts.s') -Raw).Contains('.include "data/maps/StradaBorgoCisternoni_Casa/scripts.inc"')) 'House scripts are not included.'

$outside = @($strada.warp_events | Where-Object { [int]$_.x -eq 8 -and [int]$_.y -eq 29 -and $_.dest_map -eq 'MAP_STRADA_BORGO_CISTERNONI_CASA' -and [int]$_.dest_warp_id -eq 0 })
Assert-True ($outside.Count -eq 1 -and [int]$outside[0].elevation -eq 0) 'Exterior Itemfinder-house warp is incorrect.'
$stradaBlocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/layouts/StradaBorgoCisternoni/map.bin'))
$doorRaw = [BitConverter]::ToUInt16($stradaBlocks, 2 * (29 * 36 + 8))
$generalAttributes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot 'data/tilesets/primary/general/metatile_attributes.bin'))
$doorBehavior = [BitConverter]::ToUInt16($generalAttributes, 2 * ($doorRaw -band 0x3FF)) -band 0xFF
Assert-True ((($doorRaw -shr 10) -band 1) -eq 0 -and $doorBehavior -eq 0x69) 'Exterior warp must remain on the walkable animated-door tile.'
$inside = @($house.warp_events)
Assert-True ($inside.Count -eq 2) 'House must retain its two standard south exit cells.'
foreach ($warp in $inside) {
    Assert-True ([int]$warp.y -eq 7 -and [int]$warp.x -in 3, 4 -and [int]$warp.elevation -eq 0 -and $warp.dest_map -eq 'MAP_STRADA_BORGO_CISTERNONI' -and [int]$warp.dest_warp_id -eq 0) 'House return warp is incorrect.'
}

Assert-True (@($house.object_events).Count -eq 1) 'House must contain exactly one NPC.'
$npc = $house.object_events[0]
Assert-True ($npc.local_id -eq 'LOCALID_STRADA_BORGO_CISTERNONI_CASA_RILEVATORE' -and $npc.graphics_id -eq 'OBJ_EVENT_GFX_HIKER' -and [int]$npc.x -eq 6 -and [int]$npc.y -eq 3 -and [int]$npc.elevation -eq 3) 'House NPC identity, graphic, or placement is incorrect.'
Assert-True ($npc.trainer_type -eq 'TRAINER_TYPE_NONE' -and $npc.flag -eq '0') 'House NPC must not be a trainer or use a visibility flag.'
Assert-True (@($house.coord_events).Count -eq 0 -and @($house.bg_events).Count -eq 0) 'House must not add coordinate or background events.'

foreach ($required in @('checkitem ITEM_ITEMFINDER', 'checkitemspace ITEM_ITEMFINDER', 'giveitem_msg StradaBorgoCisternoni_Casa_Text_ReceivedItemfinder, ITEM_ITEMFINDER', 'goto_if_eq VAR_RESULT, TRUE, StradaBorgoCisternoni_Casa_EventScript_PostItemfinder', 'Common_EventScript_ShowBagIsFull')) {
    Assert-True ($scripts.Contains($required)) "Missing one-shot Itemfinder instruction: $required"
}
Assert-True ($scripts -notmatch 'FLAG_.*ITEMFINDER') 'Itemfinder gift must use the existing key-item possession check, not a new flag.'
$wild = Read-Json 'src/data/wild_encounters.json'
Assert-True (@($wild.wild_encounter_groups | ForEach-Object { $_.encounters } | Where-Object { $_.map -eq 'MAP_STRADA_BORGO_CISTERNONI_CASA' }).Count -eq 0) 'House must not add encounters.'

$pages = @(
    @('Chi cammina su queste strade', 'impara a guardare bene.'),
    @('Ci sono cose che passano', 'inosservate ogni giorno.'),
    @('Questo potrebbe esserti utile.'),
    @('Hai ricevuto il', 'CERCASOGGETTI.'),
    @('Usalo quando hai la sensazione', 'che ci sia qualcosa nei dintorni.')
)
$widthBlock = [regex]::Match((Get-Content (Join-Path $RepositoryRoot 'src/fonts.c') -Raw), '(?s)gFontNormalLatinGlyphWidths\[\]\s*=\s*\{(.*?)\};').Groups[1].Value
$widths = @([regex]::Matches($widthBlock, '\d+') | ForEach-Object { [int]$_.Value })
foreach ($page in $pages) {
    Assert-True ($page.Count -le 2) 'House text page has more than two lines.'
    foreach ($line in $page) {
        Assert-True ($scripts.Contains($line)) "Missing approved house text line: $line"
        $width = 0
        foreach ($character in $line.ToCharArray()) { $width += Get-NormalGlyphWidth $character $widths }
        Assert-True ($width -le 180) "House text line exceeds the 180-pixel limit ($width): $line"
    }
}

git -C $RepositoryRoot diff --quiet -- data/layouts/StradaBorgoCisternoni/map.bin data/layouts/BorgoDiCastello/map.bin data/layouts/Route103/map.bin
Assert-True ($LASTEXITCODE -eq 0) 'This task must not modify external map.bin files.'
Write-Output 'Strada Borgo-Cisternoni Itemfinder house: PASS'
