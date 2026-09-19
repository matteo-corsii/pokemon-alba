param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw -Encoding utf8 | ConvertFrom-Json }
function Read-Block([byte[]]$bytes, [int]$width, [int]$x, [int]$y) { [BitConverter]::ToUInt16($bytes, 2 * ($y * $width + $x)) }
function Is-Walkable([UInt16]$raw) { return (($raw -band 0x0C00) -eq 0) }

$map = Read-Json 'data/maps/Laricia/map.json'
$layouts = (Read-Json 'data/layouts/layouts.json').layouts
$layout = @($layouts | Where-Object { $_.id -eq 'LAYOUT_LARICIA' })
Assert-True ($layout.Count -eq 1 -and [int]$layout[0].width -eq 64 -and [int]$layout[0].height -eq 64) 'Laricia layout is invalid.'
$blocks = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot $layout[0].blockdata_filepath))
Assert-True ($blocks.Length -eq 64 * 64 * 2) 'Laricia map.bin size is invalid.'
Assert-True (@($map.object_events).Count -eq 12) 'Laricia must contain exactly the approved 12 exterior objects.'
Assert-True (@($map.object_events | Group-Object local_id | Where-Object { $_.Count -ne 1 }).Count -eq 0) 'Laricia object local IDs must be unique.'

$sagra = @($map.object_events | Where-Object { $_.local_id -eq 'LOCALID_LARICIA_SAGRA_ADDETTO' })
Assert-True ($sagra.Count -eq 1 -and [int]$sagra[0].x -eq 45 -and [int]$sagra[0].y -eq 16 -and $sagra[0].script -eq 'Laricia_EventScript_SagraBlocker') 'Laricia Sagra attendant is incorrect.'
$trucks = @($map.object_events | Where-Object { $_.graphics_id -eq 'OBJ_EVENT_GFX_TRUCK' })
Assert-True ($trucks.Count -eq 2 -and @($trucks | Where-Object { -not (([int]$_.x -eq 47 -and [int]$_.y -eq 15) -or ([int]$_.x -eq 47 -and [int]$_.y -eq 18)) }).Count -eq 0) 'Laricia Sagra trucks are incorrect.'

$civilians = @(
    @{ Id='LOCALID_LARICIA_ANZIANO_PIAZZA'; Script='Laricia_EventScript_AnzianoPiazza'; Text='Laricia_Text_AnzianoPiazza' },
    @{ Id='LOCALID_LARICIA_VISITATORE_PALAZZO'; Script='Laricia_EventScript_VisitatorePalazzo'; Text='Laricia_Text_VisitatorePalazzo' },
    @{ Id='LOCALID_LARICIA_CLIENTE_FRASCHETTA'; Script='Laricia_EventScript_ClienteFraschetta'; Text='Laricia_Text_ClienteFraschetta' },
    @{ Id='LOCALID_LARICIA_RAGAZZO_CENTRO'; Script='Laricia_EventScript_RagazzoCentro'; Text='Laricia_Text_RagazzoCentro' },
    @{ Id='LOCALID_LARICIA_ABITANTE_MART'; Script='Laricia_EventScript_AbitanteMart'; Text='Laricia_Text_AbitanteMart' },
    @{ Id='LOCALID_LARICIA_RESIDENTE_VICOLI'; Script='Laricia_EventScript_ResidenteVicoli'; Text='Laricia_Text_ResidenteVicoli' },
    @{ Id='LOCALID_LARICIA_CONTADINO_VALLE'; Script='Laricia_EventScript_ContadinoValle'; Text='Laricia_Text_ContadinoValle' },
    @{ Id='LOCALID_LARICIA_VIANDANTE_UCCELLIERA'; Script='Laricia_EventScript_ViandanteUccelliera'; Text='Laricia_Text_ViandanteUccelliera' },
    @{ Id='LOCALID_LARICIA_VISITATORE_SAGRA'; Script='Laricia_EventScript_VisitatoreSagra'; Text='Laricia_Text_VisitatoreSagra' }
)
$warpsByCoordinate = @{}; foreach ($warp in $map.warp_events) { $warpsByCoordinate["$($warp.x),$($warp.y)"] = $true }
$scripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/Laricia/scripts.inc') -Raw -Encoding utf8
foreach ($spec in $civilians) {
    $npc = @($map.object_events | Where-Object { $_.local_id -eq $spec.Id })
    Assert-True ($npc.Count -eq 1) "Laricia civilian $($spec.Id) must occur exactly once."
    $event = $npc[0]; $x = [int]$event.x; $y = [int]$event.y
    Assert-True ($event.trainer_type -eq 'TRAINER_TYPE_NONE' -and $event.flag -eq '0') "Laricia civilian $($spec.Id) must not be gated or a trainer."
    Assert-True ($event.script -eq $spec.Script -and $scripts.Contains("$($spec.Script)::") -and $scripts.Contains("$($spec.Text):")) "Laricia civilian $($spec.Id) script or text is missing."
    $inApprovedArea = ($x -ge 0 -and $x -lt 45 -and $y -ge 0 -and $y -lt 64) -or ($spec.Id -eq 'LOCALID_LARICIA_VIANDANTE_UCCELLIERA' -and $x -ge 49 -and $x -le 51 -and $y -eq 6)
    Assert-True $inApprovedArea "Laricia civilian $($spec.Id) is outside the approved city side."
    Assert-True (Is-Walkable (Read-Block $blocks 64 $x $y)) "Laricia civilian $($spec.Id) is on a blocked tile."
    Assert-True (-not $warpsByCoordinate.ContainsKey("$x,$y")) "Laricia civilian $($spec.Id) is on a warp."
}
Assert-True (@($map.coord_events).Count -eq 0 -and @($map.bg_events).Count -eq 0) 'Laricia civilian population must not add coord or background events.'
Assert-True (($map.connections | Measure-Object).Count -eq 1 -and $map.connections[0].direction -eq 'left' -and $map.connections[0].map -eq 'MAP_PONTE_VALLE_LARICIA') 'Laricia must retain only the Ponte/Valle connection.'
Assert-True (-not ($scripts -match 'trainerbattle|giveitem')) 'Laricia civilian scripts must not add battles or items.'
Write-Output 'Laricia population: PASS'
