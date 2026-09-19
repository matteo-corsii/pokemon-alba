param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw -Encoding utf8 | ConvertFrom-Json }
function Read-Block([byte[]]$bytes, [int]$width, [int]$x, [int]$y) { [BitConverter]::ToUInt16($bytes, 2 * ($y * $width + $x)) }
function Is-Walkable([UInt16]$raw) { (($raw -band 0x0C00) -eq 0) }
function Elevation([UInt16]$raw) { (($raw -shr 12) -band 15) }

$map = Read-Json 'data/maps/PonteValleLaricia/map.json'
$layouts = (Read-Json 'data/layouts/layouts.json').layouts
$layout = @($layouts | Where-Object { $_.id -eq 'LAYOUT_PONTE_VALLE_LARICIA' })
Assert-True ($layout.Count -eq 1) 'Ponte/Valle layout is missing.'
$blocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot $layout[0].blockdata_filepath))
$expected = @(
    @{ Id='LOCALID_PONTE_VALLE_LARICIA_CONTADINO_CASALE1'; Gfx='OBJ_EVENT_GFX_MAN_4'; Script='PonteValleLaricia_EventScript_ContadinoCasale1'; Text='PonteValleLaricia_Text_ContadinoCasale1' },
    @{ Id='LOCALID_PONTE_VALLE_LARICIA_CONTADINA_CASALE2'; Gfx='OBJ_EVENT_GFX_WOMAN_1'; Script='PonteValleLaricia_EventScript_ContadinaCasale2'; Text='PonteValleLaricia_Text_ContadinaCasale2' },
    @{ Id='LOCALID_PONTE_VALLE_LARICIA_ABITANTE_CASETTA'; Gfx='OBJ_EVENT_GFX_MAN_5'; Script='PonteValleLaricia_EventScript_AbitanteCasetta'; Text='PonteValleLaricia_Text_AbitanteCasetta' },
    @{ Id='LOCALID_PONTE_VALLE_LARICIA_ANZIANO_VALLE'; Gfx='OBJ_EVENT_GFX_EXPERT_M'; Script='PonteValleLaricia_EventScript_AnzianoValle'; Text='PonteValleLaricia_Text_AnzianoValle' },
    @{ Id='LOCALID_PONTE_VALLE_LARICIA_LAVORATRICE_CAMPI'; Gfx='OBJ_EVENT_GFX_WOMAN_2'; Script='PonteValleLaricia_EventScript_LavoratriceCampi'; Text='PonteValleLaricia_Text_LavoratriceCampi' },
    @{ Id='LOCALID_PONTE_VALLE_LARICIA_RAGAZZO_CAMPI'; Gfx='OBJ_EVENT_GFX_YOUNGSTER'; Script='PonteValleLaricia_EventScript_RagazzoCampi'; Text='PonteValleLaricia_Text_RagazzoCampi' }
)
Assert-True (@($map.object_events).Count -eq 6) 'Valle must contain exactly six civilian NPCs.'
Assert-True (@($map.object_events | Group-Object local_id | Where-Object { $_.Count -ne 1 }).Count -eq 0) 'Valle NPC local IDs must be unique.'
$scripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/PonteValleLaricia/scripts.inc') -Raw -Encoding utf8
$blockedCoordinates = @('20,38', '44,35', '43,58', '51,26', '51,8', '58,5')
foreach ($spec in $expected) {
    $npc = @($map.object_events | Where-Object { $_.local_id -eq $spec.Id })
    Assert-True ($npc.Count -eq 1) "Missing civilian $($spec.Id)."
    $event = $npc[0]; $x = [int]$event.x; $y = [int]$event.y
    Assert-True ($event.graphics_id -eq $spec.Gfx -and $event.script -eq $spec.Script) "Civilian $($spec.Id) graphics or script differs."
    Assert-True ($event.trainer_type -eq 'TRAINER_TYPE_NONE' -and $event.flag -eq '0') "Civilian $($spec.Id) must be an ungated non-trainer."
    Assert-True ($x -ge 0 -and $x -lt 64 -and $y -ge 26 -and $y -lt 64 -and $x -ne 63) "Civilian $($spec.Id) is outside the lower Valle."
    Assert-True ($blockedCoordinates -notcontains "$x,$y") "Civilian $($spec.Id) occupies a protected entrance."
    $raw = Read-Block $blocks 64 $x $y
    Assert-True ((Is-Walkable $raw) -and (Elevation $raw) -eq 3) "Civilian $($spec.Id) is not on a walkable elevation-3 tile."
    Assert-True ($scripts.Contains("$($spec.Script)::") -and $scripts.Contains("$($spec.Text):")) "Civilian $($spec.Id) script/text is missing."
}
Assert-True (@($map.coord_events).Count -eq 0 -and @($map.bg_events).Count -eq 0) 'Valle must not add coord or background events.'
Assert-True (@($map.warp_events).Count -eq 2) 'Valle must contain exactly two warps.'
Assert-True (@($map.warp_events | Where-Object { [int]$_.x -eq 51 -and [int]$_.y -eq 26 -and [int]$_.elevation -eq 3 -and $_.dest_map -eq 'MAP_PONTE_VALLE_LARICIA' -and $_.dest_warp_id -eq '1' }).Count -eq 1) 'Valle under-bridge warp is invalid.'
Assert-True (@($map.warp_events | Where-Object { [int]$_.x -eq 51 -and [int]$_.y -eq 8 -and [int]$_.elevation -eq 3 -and $_.dest_map -eq 'MAP_PONTE_VALLE_LARICIA' -and $_.dest_warp_id -eq '0' }).Count -eq 1) 'Upper under-bridge warp is invalid.'
foreach ($coordinate in @('20,38', '44,35', '43,58', '58,5')) { Assert-True (@($map.warp_events | Where-Object { "$($_.x),$($_.y)" -eq $coordinate }).Count -eq 0) "Future entrance $coordinate must not yet have a warp." }
$wild = Read-Json 'src/data/wild_encounters.json'
Assert-True (@($wild.wild_encounter_groups | ForEach-Object { $_.encounters } | Where-Object { $_.map -eq 'MAP_PONTE_VALLE_LARICIA' }).Count -eq 0) 'Valle must not contain wild encounters.'
Assert-True (-not ($scripts -match 'trainerbattle|giveitem|setflag|setvar|special|cutscene')) 'Valle civilian scripts must not contain gameplay commands.'
Write-Output 'Valle Laricia population: PASS'
