param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }

$strada = Read-Json 'data/maps/StradaBorgoCisternoni/map.json'
$wild = Read-Json 'src/data/wild_encounters.json'
$encounters = @($wild.wild_encounter_groups | ForEach-Object { $_.encounters } | Where-Object { $_.map -eq 'MAP_STRADA_BORGO_CISTERNONI' })
Assert-True ($encounters.Count -eq 4) 'Strada must define exactly four time-of-day encounter tables.'

$rates = @(20, 20, 10, 10, 10, 10, 5, 5, 4, 4, 1, 1)
$dayExpected = @(
    'SPECIES_MICIOLO', 'SPECIES_GAZZUOLA', 'SPECIES_BORGOTTO', 'SPECIES_PASTUFO',
    'SPECIES_LUSCINCO', 'SPECIES_TINUNCOL', 'SPECIES_CRISALVIA', 'SPECIES_BORGOTTO',
    'SPECIES_MOLOSPSY', 'SPECIES_CRISALVIA', 'SPECIES_CRISALVIA', 'SPECIES_CRISALVIA')
$nightExpected = @(
    'SPECIES_MICIOLO', 'SPECIES_GAZZUOLA', 'SPECIES_BORGOTTO', 'SPECIES_PASTUFO',
    'SPECIES_LUSCINCO', 'SPECIES_BRILLAZZA', 'SPECIES_CRISALVIA', 'SPECIES_BORGOTTO',
    'SPECIES_LENGHELIS', 'SPECIES_CRISALVIA', 'SPECIES_CRISALVIA', 'SPECIES_CRISALVIA')
foreach ($table in $encounters) {
    Assert-True ($table.land_mons.encounter_rate -eq 20 -and @($table.land_mons.mons).Count -eq 12) "Invalid land encounter layout: $($table.base_label)."
    $expected = if ($table.base_label -eq 'gStradaBorgoCisternoni_Night') { $nightExpected } else { $dayExpected }
    for ($index = 0; $index -lt 12; $index++) {
        $mon = $table.land_mons.mons[$index]
        Assert-True ($mon.species -eq $expected[$index] -and [int]$mon.min_level -ge 22 -and [int]$mon.max_level -le 25) "Invalid encounter slot $index in $($table.base_label)."
    }
    $total = 0
    for ($index = 0; $index -lt 12; $index++) { $total += $rates[$index] }
    Assert-True ($total -eq 100) "Encounter rates do not total 100%: $($table.base_label)."
}
foreach ($label in @('gStradaBorgoCisternoni_Morning', 'gStradaBorgoCisternoni_Day', 'gStradaBorgoCisternoni_Evening')) {
    Assert-True (@($encounters | Where-Object { $_.base_label -eq $label }).Count -eq 1) "Missing daytime table $label."
}
Assert-True (@($encounters | Where-Object { $_.base_label -eq 'gStradaBorgoCisternoni_Night' }).Count -eq 1) 'Missing night encounter table.'

$newNpcs = @(
    @('LOCALID_STRADA_BORGO_CISTERNONI_UPHILL_RESIDENT', 'OBJ_EVENT_GFX_MAN_2', 13, 9, 'StradaBorgoCisternoni_EventScript_UphillResident'),
    @('LOCALID_STRADA_BORGO_CISTERNONI_WALL_MAINTAINER', 'OBJ_EVENT_GFX_HIKER', 15, 20, 'StradaBorgoCisternoni_EventScript_WallMaintainer'),
    @('LOCALID_STRADA_BORGO_CISTERNONI_CISTERNONI_WALKER', 'OBJ_EVENT_GFX_WOMAN_5', 16, 34, 'StradaBorgoCisternoni_EventScript_CisternoniWalker'))
foreach ($expected in $newNpcs) {
    Assert-True (@($strada.object_events | Where-Object { $_.local_id -eq $expected[0] -and $_.graphics_id -eq $expected[1] -and [int]$_.x -eq $expected[2] -and [int]$_.y -eq $expected[3] -and [int]$_.elevation -eq 3 -and $_.movement_range_x -eq 0 -and $_.movement_range_y -eq 0 -and $_.trainer_type -eq 'TRAINER_TYPE_NONE' -and $_.script -eq $expected[4] -and $_.flag -eq '0' }).Count -eq 1) "Invalid ambient NPC $($expected[0])."
}
Assert-True (@($strada.object_events | Where-Object { $_.trainer_type -ne 'TRAINER_TYPE_NONE' }).Count -eq 0) 'Strada must not add trainers.'
$ball = @($strada.object_events | Where-Object { $_.graphics_id -eq 'OBJ_EVENT_GFX_ITEM_BALL' -and [int]$_.x -eq 6 -and [int]$_.y -eq 10 -and [int]$_.elevation -eq 3 -and $_.trainer_sight_or_berry_tree_id -eq 'ITEM_SUPER_POTION' -and $_.script -eq 'Common_EventScript_FindItem' -and $_.flag -eq 'FLAG_ITEM_STRADA_BORGO_CISTERNONI_SUPER_POTION' })
Assert-True ($ball.Count -eq 1) 'Visible Super Potion is incorrect.'
$hidden = @($strada.bg_events | Where-Object { $_.type -eq 'hidden_item' })
Assert-True ($hidden.Count -eq 2) 'Strada must contain exactly two hidden items.'
Assert-True (@($hidden | Where-Object { [int]$_.x -eq 19 -and [int]$_.y -eq 4 -and [int]$_.elevation -eq 3 -and $_.item -eq 'ITEM_ETHER' -and $_.flag -eq 'FLAG_HIDDEN_ITEM_STRADA_BORGO_CISTERNONI_ETHER' }).Count -eq 1) 'Hidden Ether is incorrect.'
Assert-True (@($hidden | Where-Object { [int]$_.x -eq 31 -and [int]$_.y -eq 20 -and [int]$_.elevation -eq 3 -and $_.item -eq 'ITEM_REVIVE' -and $_.flag -eq 'FLAG_HIDDEN_ITEM_STRADA_BORGO_CISTERNONI_REVIVE' }).Count -eq 1) 'Hidden Revive is incorrect.'
foreach ($event in @($strada.object_events) + @($strada.bg_events)) {
    Assert-True (-not (([int]$event.x -eq 8 -and [int]$event.y -eq 29) -or ([int]$event.y -eq 23 -and [int]$event.x -ge 22 -and [int]$event.x -le 30))) 'Population conflicts with the Itemfinder house or Nico/Lia trigger.'
}
$scripts = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'data/maps/StradaBorgoCisternoni/scripts.inc') -Raw
foreach ($label in @('StradaBorgoCisternoni_EventScript_StartNicoLiaScene', 'StradaBorgoCisternoni_EventScript_UphillResident', 'StradaBorgoCisternoni_EventScript_WallMaintainer', 'StradaBorgoCisternoni_EventScript_CisternoniWalker')) { Assert-True ($scripts.Contains($label)) "Missing script $label." }
git -C $RepositoryRoot diff --quiet -- data/layouts/StradaBorgoCisternoni/map.bin
Assert-True ($LASTEXITCODE -eq 0) 'Strada map.bin must remain unchanged.'
Write-Output 'Strada Borgo-Cisternoni population: PASS'
