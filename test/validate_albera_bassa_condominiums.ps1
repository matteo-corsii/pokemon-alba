param(
    [string]$BaseRef = 'develop'
)

$ErrorActionPreference = 'Stop'

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

function Read-JsonFile([string]$Path) {
    return Get-Content -Raw -Encoding utf8 $Path | ConvertFrom-Json
}

function Get-ChangedPaths {
    $paths = @()
    $paths += git diff --name-only "$BaseRef...HEAD"
    $paths += git diff --name-only
    $paths += git diff --cached --name-only
    $paths += git ls-files --others --exclude-standard
    return @($paths | Where-Object { $_ } | Sort-Object -Unique)
}

$condominiums = @(
    @{ Name = 'AlberaBassa_Condominium1'; Id = 'MAP_ALBERA_BASSA_CONDOMINIUM1'; Layout = 'LAYOUT_HOUSE_WITH_BED'; ExternalX = 5; ExternalY = 26; ExternalWarp = '4'; ExitA = @(3, 7); ExitB = @(4, 7); Resident = 'AlberaBassa_Condominium1_EventScript_Resident' },
    @{ Name = 'AlberaBassa_Condominium2'; Id = 'MAP_ALBERA_BASSA_CONDOMINIUM2'; Layout = 'LAYOUT_LILYCOVE_CITY_HOUSE2'; ExternalX = 19; ExternalY = 26; ExternalWarp = '5'; ExitA = @(2, 7); ExitB = @(3, 7); Resident = 'AlberaBassa_Condominium2_EventScript_Resident' },
    @{ Name = 'AlberaBassa_Condominium3'; Id = 'MAP_ALBERA_BASSA_CONDOMINIUM3'; Layout = 'LAYOUT_RUSTBORO_CITY_HOUSE'; ExternalX = 31; ExternalY = 26; ExternalWarp = '6'; ExitA = @(5, 8); ExitB = @(6, 8); Resident = 'AlberaBassa_Condominium3_EventScript_Resident' }
)

$allowedPaths = @(
    'data/event_scripts.s',
    'data/maps/LittlerootTown/map.json',
    'data/maps/map_groups.json',
    'data/maps/AlberaBassa_Condominium1/map.json',
    'data/maps/AlberaBassa_Condominium1/scripts.inc',
    'data/maps/AlberaBassa_Condominium2/map.json',
    'data/maps/AlberaBassa_Condominium2/scripts.inc',
    'data/maps/AlberaBassa_Condominium3/map.json',
    'data/maps/AlberaBassa_Condominium3/scripts.inc',
    'test/validate_albera_bassa_condominiums.ps1',
    'test/validate_albera_bassa_residential_blockout.ps1',
    'test/validate_albera_bassa_school.ps1'
)
$unexpectedPaths = @(Get-ChangedPaths | Where-Object { $_ -notin $allowedPaths })
Assert-True ($unexpectedPaths.Count -eq 0) ('Out-of-scope files changed: ' + ($unexpectedPaths -join ', '))

$town = Read-JsonFile 'data/maps/LittlerootTown/map.json'
$baseTown = (git show "${BaseRef}:data/maps/LittlerootTown/map.json" | ConvertFrom-Json)
$townWarpCount = @($town.warp_events).Count
$baseTownWarpCount = @($baseTown.warp_events).Count
Assert-True ($townWarpCount -eq ($baseTownWarpCount + 3)) 'Only the three condominium entrance warps may be appended.'
foreach ($eventCollection in @('object_events', 'coord_events', 'bg_events', 'connections')) {
    Assert-True ((@($town.$eventCollection | ConvertTo-Json -Depth 20 -Compress) -join '') -eq (@($baseTown.$eventCollection | ConvertTo-Json -Depth 20 -Compress) -join '')) "LittlerootTown $eventCollection changed outside the condominium entrances."
}

$groups = Read-JsonFile 'data/maps/map_groups.json'
$group = @($groups.gMapGroup_IndoorLittleroot)
foreach ($condo in $condominiums) {
    Assert-True ((@($group | Where-Object { $_ -eq $condo.Name }).Count -eq 1)) "$($condo.Name) must appear once in gMapGroup_IndoorLittleroot."
    $external = @($town.warp_events | Where-Object {
        $_.x -eq $condo.ExternalX -and $_.y -eq $condo.ExternalY -and $_.elevation -eq 0 -and $_.dest_map -eq $condo.Id -and $_.dest_warp_id -eq '0'
    })
    Assert-True ($external.Count -eq 1) "$($condo.Name) exterior entrance warp is missing or changed."

    $mapPath = "data/maps/$($condo.Name)/map.json"
    $scriptPath = "data/maps/$($condo.Name)/scripts.inc"
    $map = Read-JsonFile $mapPath
    Assert-True ($map.id -eq $condo.Id -and $map.layout -eq $condo.Layout) "$($condo.Name) must use its approved donor layout."
    Assert-True ($map.map_type -eq 'MAP_TYPE_INDOOR' -and -not $map.allow_cycling -and -not $map.allow_running) "$($condo.Name) must remain an indoor residence."
    Assert-True ($null -eq $map.connections -or @($map.connections).Count -eq 0) "$($condo.Name) must not add map connections."
    Assert-True (@($map.object_events).Count -ge 1 -and @($map.object_events).Count -le 3) "$($condo.Name) must contain one to three ambient residents."
    Assert-True (@($map.coord_events).Count -eq 0 -and @($map.bg_events).Count -eq 0) "$($condo.Name) must not add coordinate or background events."
    Assert-True (@($map.warp_events).Count -eq 2) "$($condo.Name) must have two return warps."
    foreach ($warp in @($map.warp_events)) {
        Assert-True ($warp.dest_map -eq 'MAP_LITTLEROOT_TOWN' -and $warp.dest_warp_id -eq $condo.ExternalWarp) "$($condo.Name) return warp is incorrect."
    }
    Assert-True ((@($map.warp_events | Where-Object { $_.x -eq $condo.ExitA[0] -and $_.y -eq $condo.ExitA[1] }).Count -eq 1)) "$($condo.Name) first exit coordinate is incorrect."
    Assert-True ((@($map.warp_events | Where-Object { $_.x -eq $condo.ExitB[0] -and $_.y -eq $condo.ExitB[1] }).Count -eq 1)) "$($condo.Name) second exit coordinate is incorrect."
    foreach ($resident in @($map.object_events)) {
        Assert-True ($resident.flag -eq '0' -and $resident.trainer_type -eq 'TRAINER_TYPE_NONE') "$($condo.Name) residents must remain unconditional ambient NPCs."
    }

    $scripts = Get-Content -Raw -Encoding utf8 $scriptPath
    Assert-True ($scripts -match [regex]::Escape($condo.Resident)) "$($condo.Name) resident script is missing."
    Assert-True ($scripts -notmatch 'setflag|clearflag|setvar|addvar|giveitem|trainerbattle|special') "$($condo.Name) must not add progression, rewards, battles, or services."
    Assert-True ($scripts -notmatch 'Rustboro|Lilycove|Celadon|Viridian') "$($condo.Name) must not inherit donor-specific text."
}

$centralScripts = Get-Content -Raw -Encoding utf8 'data/event_scripts.s'
foreach ($condo in $condominiums) {
    $include = ".include `"data/maps/$($condo.Name)/scripts.inc`""
    Assert-True (([regex]::Matches($centralScripts, [regex]::Escape($include))).Count -eq 1) "$($condo.Name) scripts must be included exactly once."
}

git diff --quiet "$BaseRef...HEAD" -- data/layouts/LittlerootTown/map.bin
Assert-True ($LASTEXITCODE -eq 0) 'The approved Albera Bassa exterior layout must not change for condominium access.'

Write-Output 'Albera Bassa condominium validation passed.'
