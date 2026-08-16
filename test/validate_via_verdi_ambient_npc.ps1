param(
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

$mapPath = Join-Path $RepositoryRoot 'data/maps/Route101/map.json'
$scriptPath = Join-Path $RepositoryRoot 'data/maps/Route101/scripts.inc'
$routeMap = [IO.File]::ReadAllText($mapPath, [Text.Encoding]::UTF8) | ConvertFrom-Json
$routeScript = Get-Content -LiteralPath $scriptPath -Raw
$baseRouteMap = ((& git -C $RepositoryRoot show 'develop:data/maps/Route101/map.json') -join "`n") | ConvertFrom-Json

$hiker = @($routeMap.object_events | Where-Object {
    $_.graphics_id -eq 'OBJ_EVENT_GFX_HIKER' -and $_.x -eq 18 -and $_.y -eq 12
})
Assert-True ($hiker.Count -eq 1) 'Expected exactly one ambient Via Verdi Hiker at (18,12).'
Assert-True ($hiker[0].elevation -eq 3) 'The ambient Hiker must remain on elevation 3.'
Assert-True ($hiker[0].movement_type -eq 'MOVEMENT_TYPE_LOOK_AROUND') 'The ambient Hiker must use LOOK_AROUND.'
Assert-True ($hiker[0].trainer_type -eq 'TRAINER_TYPE_NONE') 'The ambient Hiker must not be a trainer.'
Assert-True ($hiker[0].flag -eq '0') 'The ambient Hiker must not use a visibility flag.'
Assert-True ($hiker[0].script -eq 'Route101_EventScript_AmbientHiker') 'The ambient Hiker script changed unexpectedly.'

Assert-True ($routeScript.Contains('Route101_EventScript_AmbientHiker::')) 'Missing ambient Hiker script.'
Assert-True ($routeScript.Contains('Route101_Text_AmbientHiker:')) 'Missing ambient Hiker text.'
Assert-True ($routeScript.Contains('Da queste parti il terreno resta umido\n')) 'Ambient Hiker text changed unexpectedly.'
Assert-True ($routeScript.Contains('anche quando il sole picchia.$')) 'Ambient Hiker text changed unexpectedly.'

foreach ($trigger in @(@(10, 16), @(11, 16), @(21, 10), @(22, 10), @(10, 4), @(11, 4))) {
    $matching = @($routeMap.coord_events | Where-Object { $_.x -eq $trigger[0] -and $_.y -eq $trigger[1] })
    Assert-True ($matching.Count -eq 1) "Missing investigation trigger at ($($trigger[0]),$($trigger[1]))."
}

& git -C $RepositoryRoot diff --quiet develop -- 'src/data/wild_encounters.json'
Assert-True ($LASTEXITCODE -eq 0) 'Via Verdi ambient NPC work must not change wild encounters.'

$youngster = @($routeMap.object_events | Where-Object { $_.graphics_id -eq 'OBJ_EVENT_GFX_YOUNGSTER' -and $_.script -eq 'Route101_EventScript_Youngster' })
$baseYoungster = @($baseRouteMap.object_events | Where-Object { $_.graphics_id -eq 'OBJ_EVENT_GFX_YOUNGSTER' -and $_.script -eq 'Route101_EventScript_Youngster' })
Assert-True ($youngster.Count -eq 1 -and $baseYoungster.Count -eq 1) 'Expected exactly one Via Verdi Youngster.'
Assert-True ($youngster[0].x -eq 16 -and $youngster[0].y -eq 9 -and $youngster[0].elevation -eq 3 -and $youngster[0].movement_type -eq 'MOVEMENT_TYPE_LOOK_AROUND' -and $youngster[0].movement_range_x -eq 0 -and $youngster[0].movement_range_y -eq 0 -and $youngster[0].trainer_type -eq 'TRAINER_TYPE_NONE' -and $youngster[0].flag -eq '0') 'Approved Via Verdi Youngster move must be limited to (16,8) -> (16,9).'
$otherObjects = @($routeMap.object_events | Where-Object { $_ -ne $youngster[0] })
$baseOtherObjects = @($baseRouteMap.object_events | Where-Object { $_ -ne $baseYoungster[0] })
Assert-True (($otherObjects | ConvertTo-Json -Depth 20 -Compress) -eq ($baseOtherObjects | ConvertTo-Json -Depth 20 -Compress)) 'Via Verdi object events other than the approved Youngster move changed unexpectedly.'

Write-Output 'Via Verdi ambient NPC validation passed.'
