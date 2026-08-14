param(
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

function Read-Json([string]$RelativePath) {
    return [IO.File]::ReadAllText((Join-Path $RepositoryRoot $RelativePath), [Text.Encoding]::UTF8) | ConvertFrom-Json
}

$flags = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/flags.h') -Raw
$flagsFrlg = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'include/constants/flags_frlg.h') -Raw
$route = Read-Json 'data/maps/Route101/map.json'
$baseRoute = (& git -C $RepositoryRoot show 'develop:data/maps/Route101/map.json') -join "`n" | ConvertFrom-Json

Assert-True ($flags -match '#define\s+FLAG_ITEM_ROUTE101_POTION\s+0x8E9\b') 'Missing Emerald Potion flag.'
Assert-True ($flags -match '#define\s+FLAG_HIDDEN_ITEM_ROUTE101_ANTIDOTE\s+0x8EA\b') 'Missing Emerald Antidote flag.'
Assert-True ($flagsFrlg -match '#define\s+FLAG_ITEM_ROUTE101_POTION\s+0x8E9\b') 'Missing FRLG Potion flag.'
Assert-True ($flagsFrlg -match '#define\s+FLAG_HIDDEN_ITEM_ROUTE101_ANTIDOTE\s+0x8EA\b') 'Missing FRLG Antidote flag.'

$potion = @($route.object_events | Where-Object { $_.flag -eq 'FLAG_ITEM_ROUTE101_POTION' })
Assert-True ($potion.Count -eq 1) 'Route101 must contain exactly one visible Potion.'
Assert-True ($potion[0].graphics_id -eq 'OBJ_EVENT_GFX_ITEM_BALL') 'Potion must use the standard item ball graphic.'
Assert-True ($potion[0].x -eq 24 -and $potion[0].y -eq 13 -and $potion[0].elevation -eq 3) 'Potion coordinates are invalid.'
Assert-True ($potion[0].trainer_sight_or_berry_tree_id -eq 'ITEM_POTION') 'Potion item is invalid.'
Assert-True ($potion[0].script -eq 'Common_EventScript_FindItem') 'Potion must use the standard persistent item script.'
Assert-True ($route.object_events.Count -eq ($baseRoute.object_events.Count + 1)) 'Route101 object-event count is invalid.'
Assert-True ((@($route.object_events | Where-Object { $_.flag -ne 'FLAG_ITEM_ROUTE101_POTION' } | ConvertTo-Json -Depth 10 -Compress) -join "`n") -eq (@($baseRoute.object_events | ConvertTo-Json -Depth 10 -Compress) -join "`n")) 'An existing Route101 object event changed unexpectedly.'

$antidote = @($route.bg_events | Where-Object { $_.flag -eq 'FLAG_HIDDEN_ITEM_ROUTE101_ANTIDOTE' })
Assert-True ($antidote.Count -eq 1) 'Route101 must contain exactly one hidden Antidote.'
Assert-True ($antidote[0].type -eq 'hidden_item') 'Antidote must be a hidden item event.'
Assert-True ($antidote[0].x -eq 32 -and $antidote[0].y -eq 12 -and $antidote[0].elevation -eq 3) 'Antidote coordinates are invalid.'
Assert-True ($antidote[0].item -eq 'ITEM_ANTIDOTE') 'Antidote item is invalid.'
Assert-True ($route.bg_events.Count -eq ($baseRoute.bg_events.Count + 1)) 'Route101 background-event count is invalid.'
Assert-True ((@($route.bg_events | Where-Object { $_.flag -ne 'FLAG_HIDDEN_ITEM_ROUTE101_ANTIDOTE' } | ConvertTo-Json -Depth 10 -Compress) -join "`n") -eq (@($baseRoute.bg_events | ConvertTo-Json -Depth 10 -Compress) -join "`n")) 'An existing Route101 background event changed unexpectedly.'

$protectedTriggers = @(
    @(10, 16, 'Route101_EventScript_InvestigateSource'),
    @(11, 16, 'Route101_EventScript_InvestigateSource'),
    @(21, 10, 'Route101_EventScript_InvestigatePokemonBehavior'),
    @(22, 10, 'Route101_EventScript_InvestigatePokemonBehavior'),
    @(10, 4, 'Route101_EventScript_InvestigateAncientCanal'),
    @(11, 4, 'Route101_EventScript_InvestigateAncientCanal')
)
foreach ($trigger in $protectedTriggers) {
    $current = @($route.coord_events | Where-Object { $_.x -eq $trigger[0] -and $_.y -eq $trigger[1] -and $_.script -eq $trigger[2] })
    $base = @($baseRoute.coord_events | Where-Object { $_.x -eq $trigger[0] -and $_.y -eq $trigger[1] -and $_.script -eq $trigger[2] })
    Assert-True ($current.Count -eq 1 -and (($current | ConvertTo-Json -Compress) -eq ($base | ConvertTo-Json -Compress))) "Investigation trigger changed: $($trigger[0]),$($trigger[1])."
}

Assert-True ((@($route.coord_events | ConvertTo-Json -Depth 10 -Compress) -join "`n") -eq (@($baseRoute.coord_events | ConvertTo-Json -Depth 10 -Compress) -join "`n")) 'Route101 triggers changed unexpectedly.'
Assert-True (($route.connections | ConvertTo-Json -Depth 10 -Compress) -eq ($baseRoute.connections | ConvertTo-Json -Depth 10 -Compress)) 'Route101 connections changed unexpectedly.'
Assert-True ($route.warp_events.Count -eq $baseRoute.warp_events.Count) 'Route101 warps changed unexpectedly.'

Write-Output 'Via Verdi field items validation passed.'
