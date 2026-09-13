param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Read-Json([string]$path) { Get-Content (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }

$borgo = Read-Json 'data/maps/BorgoDiCastello/map.json'
$external = @($borgo.object_events)
$expected = @(
    @('LOCALID_BORGO_DI_CASTELLO_ANZIANO_BELVEDERE', 'OBJ_EVENT_GFX_EXPERT_M', 10, 50, 'MOVEMENT_TYPE_FACE_RIGHT'),
    @('LOCALID_BORGO_DI_CASTELLO_TURISTA_BELVEDERE', 'OBJ_EVENT_GFX_MAN_3', 16, 52, 'MOVEMENT_TYPE_FACE_LEFT'),
    @('LOCALID_BORGO_DI_CASTELLO_RESIDENTE_VICOLI', 'OBJ_EVENT_GFX_WOMAN_1', 16, 32, 'MOVEMENT_TYPE_FACE_DOWN'),
    @('LOCALID_BORGO_DI_CASTELLO_VIAGGIATORE_LARICIA', 'OBJ_EVENT_GFX_HIKER', 52, 51, 'MOVEMENT_TYPE_FACE_LEFT'),
    @('LOCALID_BORGO_DI_CASTELLO_BAMBINO', 'OBJ_EVENT_GFX_BOY_1', 15, 36, 'MOVEMENT_TYPE_FACE_DOWN'),
    @('LOCALID_BORGO_DI_CASTELLO_DONNA_VILLA', 'OBJ_EVENT_GFX_WOMAN_2', 27, 8, 'MOVEMENT_TYPE_FACE_RIGHT'),
    @('LOCALID_BORGO_DI_CASTELLO_VIAGGIATORE_PIAZZA', 'OBJ_EVENT_GFX_GENTLEMAN', 35, 20, 'MOVEMENT_TYPE_FACE_LEFT'),
    @('LOCALID_BORGO_DI_CASTELLO_ABITANTE_FONTANA', 'OBJ_EVENT_GFX_MAN_5', 28, 16, 'MOVEMENT_TYPE_FACE_DOWN')
)

Assert-True ($external.Count -eq 8) 'Borgo must have exactly eight ambient external NPCs.'
foreach ($item in $expected) {
    Assert-True (@($external | Where-Object { $_.local_id -eq $item[0] -and $_.graphics_id -eq $item[1] -and [int]$_.x -eq $item[2] -and [int]$_.y -eq $item[3] -and $_.movement_type -eq $item[4] -and $_.trainer_type -eq 'TRAINER_TYPE_NONE' -and $_.flag -eq '0' }).Count -eq 1) "Invalid external NPC $($item[0])."
}

$reserved = @(@(22,43), @(34,43), @(40,43), @(38,33), @(11,28), @(11,20), @(40,17), @(26,12), @(28,0), @(29,0), @(30,0), @(59,50), @(59,51), @(59,52), @(59,53), @(28,59), @(29,59), @(30,59), @(31,59))
foreach ($cell in $reserved) {
    Assert-True (@($external | Where-Object { [int]$_.x -eq $cell[0] -and [int]$_.y -eq $cell[1] }).Count -eq 0) "NPC blocks reserved Borgo cell $($cell[0]),$($cell[1])."
}

$interiorCounts = @{
    'BorgoDiCastello_PokemonCenter' = 2
    'BorgoDiCastello_Mart' = 2
    'BorgoDiCastello_House1' = 1
    'BorgoDiCastello_House2' = 1
    'BorgoDiCastello_House3' = 1
    'BorgoDiCastello_House4' = 1
    'BorgoDiCastello_RistoranteBelvedere' = 3
}
foreach ($name in $interiorCounts.Keys) {
    $map = Read-Json ("data/maps/$name/map.json")
    Assert-True (@($map.object_events).Count -eq $interiorCounts[$name]) "$name NPC count is incorrect."
    Assert-True (@($map.object_events | Where-Object { $_.trainer_type -ne 'TRAINER_TYPE_NONE' -or $_.flag -ne '0' }).Count -eq 0) "$name has a trainer or a persistence flag."
}

$scriptPaths = @('data/maps/BorgoDiCastello/scripts.inc') + @($interiorCounts.Keys | ForEach-Object { "data/maps/$_/scripts.inc" })
$scriptText = ($scriptPaths | ForEach-Object { Get-Content (Join-Path $RepositoryRoot $_) -Raw }) -join "`n"
foreach ($forbidden in @('Nico', 'Lia', 'Aurea', 'Eco', 'Riflesso', 'falde')) {
    Assert-True ($scriptText -cnotmatch $forbidden) "Forbidden narrative reference: $forbidden."
}
Assert-True ((Get-Content (Join-Path $RepositoryRoot 'src/data/wild_encounters.json') -Raw) -notmatch 'MAP_BORGO_DI_CASTELLO') 'Borgo must not receive encounters.'
Write-Output 'Borgo di Castello ambient NPCs: PASS'
