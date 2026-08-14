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

foreach ($path in @('src/data/wild_encounters.json', 'data/layouts/Route101/map.bin')) {
    & git -C $RepositoryRoot diff --quiet develop -- $path
    Assert-True ($LASTEXITCODE -eq 0) "Out-of-scope file changed: $path"
}

Write-Output 'Via Verdi ambient NPC validation passed.'
