param(
    [string]$BaseRef = "develop"
)

$ErrorActionPreference = "Stop"

function Assert-True {
    param([bool]$Condition, [string]$Message)
    if (-not $Condition) {
        throw $Message
    }
}

function Read-JsonFile {
    param([string]$Path)
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

$allowedPaths = @(
    "data/event_scripts.s",
    "data/layouts/AlberaBassa_School/border.bin",
    "data/layouts/AlberaBassa_School/map.bin",
    "data/layouts/LittlerootTown/map.bin",
    "data/layouts/Route101/map.bin",
    "data/layouts/layouts.json",
    "data/maps/AlberaBassa_School/map.json",
    "data/maps/AlberaBassa_School/scripts.inc",
    "data/maps/AlberaBassa_Condominium1/map.json",
    "data/maps/AlberaBassa_Condominium1/scripts.inc",
    "data/maps/AlberaBassa_Condominium2/map.json",
    "data/maps/AlberaBassa_Condominium2/scripts.inc",
    "data/maps/AlberaBassa_Condominium3/map.json",
    "data/maps/AlberaBassa_Condominium3/scripts.inc",
    "data/maps/LittlerootTown/map.json",
    "data/maps/LittlerootTown/scripts.inc",
    "data/maps/OldaleTown/map.json",
    "data/maps/Route101/map.json",
    "data/maps/map_groups.json",
    "test/validate_albera_bassa_school.ps1",
    "test/validate_albera_bassa_condominiums.ps1",
    "test/validate_albera_bassa_residential_blockout.ps1",
    "test/validate_porta_pretoria_dedicated_tileset.ps1",
    "test/validate_porta_pretoria_localization.ps1",
    "test/validate_via_verdi_ambient_npc.ps1"
)
$allowedPaths += @(
    'docs/AUSONIA_REGIONAL_DEX_PLAN.md', 'include/constants/pokedex.h', 'include/constants/species.h',
    'src/data/graphics/pokemon.h', 'src/data/pokemon/all_learnables.json', 'src/data/pokemon/egg_moves.h',
    'src/data/pokemon/pokedex_orders.h', 'src/data/pokemon/species_info.h', 'test/species.c',
    'test/validate_albera_amphitheatre_gym.ps1', 'test/validate_early_ausonia_fauna_batch_b.ps1',
    'test/validate_early_ausonia_fauna_batch_c.ps1', 'test/validate_early_ausonia_fauna_batch_d.ps1',
    'test/validate_early_ausonia_graphics_batch_a.ps1', 'test/validate_early_ausonia_graphics_batch_b.ps1',
    'test/validate_early_ausonia_graphics_batch_c.ps1', 'test/validate_early_ausonia_graphics_batch_d.ps1',
    'test/validate_lago_di_albera_tileset.ps1', 'test/validate_lenghelis.ps1', 'test/validate_lumella_omphalux.ps1',
    'test/validate_luscinco_luscerp.ps1', 'test/validate_molospsy.ps1', 'test/validate_paludix_sanguilex.ps1',
    'test/validate_salampolla_alchimandra.ps1', 'test/validate_tritino_tricrest.ps1',
    'test/validate_via_verdi_first_investigation.ps1', 'test/validate_cingerm_graphics.ps1'
)
$allowedPaths += @(Get-ChildItem 'graphics/pokemon/tritino','graphics/pokemon/tricrest','graphics/pokemon/salampolla','graphics/pokemon/alchimandra' -File | ForEach-Object { $_.FullName.Substring((Get-Location).Path.Length + 1).Replace('\','/') })

$allowedPaths += @('test/validate_albera_first_playable_segment.ps1','test/validate_cingerm_starter.ps1','test/validate_full_ausonia_starter_trio.ps1','test/validate_italian_menu_localization.ps1','test/validate_cisternide_calcistern.ps1','graphics/pokemon/cisternide/anim_front.png','graphics/pokemon/cisternide/back.png','graphics/pokemon/cisternide/icon.png','graphics/pokemon/cisternide/normal.pal','graphics/pokemon/cisternide/shiny.pal','graphics/pokemon/calcistern/anim_front.png','graphics/pokemon/calcistern/back.png','graphics/pokemon/calcistern/icon.png','graphics/pokemon/calcistern/normal.pal','graphics/pokemon/calcistern/shiny.pal','graphics/pokemon/luscinco/anim_front.png','graphics/pokemon/luscinco/back.png','graphics/pokemon/luscinco/icon.png','graphics/pokemon/luscinco/normal.pal','graphics/pokemon/luscinco/shiny.pal','graphics/pokemon/luscerp/anim_front.png','graphics/pokemon/luscerp/back.png','graphics/pokemon/luscerp/icon.png','graphics/pokemon/luscerp/normal.pal','graphics/pokemon/luscerp/shiny.pal')
$unexpectedPaths = @(Get-ChangedPaths | Where-Object { $_ -notin $allowedPaths })
Assert-True ($unexpectedPaths.Count -eq 0) ("Out-of-scope files changed: " + ($unexpectedPaths -join ", "))

$groups = Read-JsonFile "data/maps/map_groups.json"
$schoolGroup = @($groups.gMapGroup_IndoorLittleroot | Where-Object { $_ -eq "AlberaBassa_School" })
Assert-True ($schoolGroup.Count -eq 1) "AlberaBassa_School must appear exactly once in gMapGroup_IndoorLittleroot."

$layouts = Read-JsonFile "data/layouts/layouts.json"
$schoolLayout = @($layouts.layouts | Where-Object { $_.id -eq "LAYOUT_ALBERA_BASSA_SCHOOL" })
Assert-True ($schoolLayout.Count -eq 1) "LAYOUT_ALBERA_BASSA_SCHOOL must be unique."
$schoolLayout = $schoolLayout[0]
Assert-True ($schoolLayout.width -eq 12 -and $schoolLayout.height -eq 11) "The school layout must remain 12x11."
Assert-True ($schoolLayout.primary_tileset -eq "gTileset_Building") "The school must use the Building primary tileset."
Assert-True ($schoolLayout.secondary_tileset -eq "gTileset_PokemonSchool") "The school must use the PokemonSchool secondary tileset."
Assert-True ($schoolLayout.layout_version -eq "emerald") "The school layout must be Emerald-only."

$schoolMap = Read-JsonFile "data/maps/AlberaBassa_School/map.json"
Assert-True ($schoolMap.id -eq "MAP_ALBERA_BASSA_SCHOOL") "Unexpected school map ID."
Assert-True ($schoolMap.layout -eq "LAYOUT_ALBERA_BASSA_SCHOOL") "The school map must use its dedicated layout."
Assert-True ($schoolMap.allow_running -eq $false -and $schoolMap.allow_cycling -eq $false) "The school must remain an indoor map."
Assert-True ($null -eq $schoolMap.connections -or @($schoolMap.connections).Count -eq 0) "The school must not add map connections."
Assert-True (@($schoolMap.object_events).Count -eq 3) "The school must contain exactly teacher plus two students."
Assert-True (@($schoolMap.coord_events).Count -eq 0 -and @($schoolMap.bg_events).Count -eq 0) "The school must not add coordinate or background events."
Assert-True ((@($schoolMap.object_events.local_id) -contains "LOCALID_ALBERA_BASSA_SCHOOL_TEACHER")) "Teacher local ID missing."
Assert-True ((@($schoolMap.object_events.script) -contains "AlberaBassa_School_EventScript_Teacher")) "Teacher script missing."
Assert-True ((@($schoolMap.object_events.script) -contains "AlberaBassa_School_EventScript_StudentOne")) "First student script missing."
Assert-True ((@($schoolMap.object_events.script) -contains "AlberaBassa_School_EventScript_StudentTwo")) "Second student script missing."
Assert-True (@($schoolMap.warp_events).Count -eq 2) "The school must have exactly two return warps."
foreach ($warp in $schoolMap.warp_events) {
    Assert-True ($warp.dest_map -eq "MAP_LITTLEROOT_TOWN" -and $warp.dest_warp_id -eq "3") "School return warps must return to the Albèra Bassa school entrance."
}
Assert-True ((@($schoolMap.warp_events | Where-Object { $_.x -eq 5 -and $_.y -eq 10 })).Count -eq 1) "Missing left school exit warp."
Assert-True ((@($schoolMap.warp_events | Where-Object { $_.x -eq 6 -and $_.y -eq 10 })).Count -eq 1) "Missing right school exit warp."

$mapBytes = [IO.File]::ReadAllBytes("data/layouts/AlberaBassa_School/map.bin")
$borderBytes = [IO.File]::ReadAllBytes("data/layouts/AlberaBassa_School/border.bin")
Assert-True ($mapBytes.Length -eq (12 * 11 * 2)) "School map.bin must match the 12x11 layout."
Assert-True ($borderBytes.Length -eq 8) "School border.bin must retain the standard 2x2 border."

$townMap = Read-JsonFile "data/maps/LittlerootTown/map.json"
$externalSchoolWarp = @($townMap.warp_events | Where-Object {
    $_.x -eq 28 -and $_.y -eq 7 -and $_.elevation -eq 0 -and $_.dest_map -eq "MAP_ALBERA_BASSA_SCHOOL" -and $_.dest_warp_id -eq "0"
})
Assert-True ($externalSchoolWarp.Count -eq 1) "Albèra Bassa must have one school entrance warp at (28,7)."

$centralScripts = Get-Content -Raw -Encoding utf8 "data/event_scripts.s"
Assert-True (([regex]::Matches($centralScripts, [regex]::Escape('.include "data/maps/AlberaBassa_School/scripts.inc"'))).Count -eq 1) "School scripts must be centrally included exactly once."

$schoolScripts = Get-Content -Raw -Encoding utf8 "data/maps/AlberaBassa_School/scripts.inc"
Assert-True ($schoolScripts -notmatch "Rustboro|Viridian") "No vanilla school scripts or texts may be inherited."
Assert-True ($schoolScripts -notmatch "trainerbattle|giveitem|FLAG_ALBERA|VAR_ALBERA") "The school must not add battles, item IDs, or custom progression state."
Assert-True ($schoolScripts -match "goto_if_set FLAG_RECEIVED_RUNNING_SHOES, AlberaBassa_School_EventScript_TeacherAfterReward") "Teacher must guard the reward with FLAG_RECEIVED_RUNNING_SHOES."
Assert-True (([regex]::Matches($schoolScripts, "setflag FLAG_RECEIVED_RUNNING_SHOES")).Count -eq 1) "The school must set FLAG_RECEIVED_RUNNING_SHOES exactly once."
Assert-True (([regex]::Matches($schoolScripts, "setflag FLAG_SYS_B_DASH")).Count -eq 1) "The school must set FLAG_SYS_B_DASH exactly once."
Assert-True ($schoolScripts -match "premuto B") "The teacher must explain the B-button running tutorial."

$townScripts = Get-Content -Raw -Encoding utf8 "data/maps/LittlerootTown/scripts.inc"
Assert-True ($townScripts -match "LittlerootTown_EventScript_GiveRunningShoesTrigger::\s*\r?\n\s*goto_if_set FLAG_RECEIVED_RUNNING_SHOES, LittlerootTown_EventScript_RunningShoesAlreadyReceived") "Legacy running-shoes trigger must avoid duplicate delivery."
Assert-True ($townScripts -match "LittlerootTown_EventScript_Mom::\s*\r?\n\s*lock\s*\r?\n\s*faceplayer\s*\r?\n\s*goto_if_set FLAG_RECEIVED_RUNNING_SHOES, LittlerootTown_EventScript_RunningShoesAlreadyReceived") "Legacy Mom delivery must avoid duplicate delivery."
Assert-True ($townScripts -match "LittlerootTown_EventScript_RunningShoesAlreadyReceived::\s*\r?\n\s*releaseall\s*\r?\n\s*end") "Legacy duplicate-delivery exit must release the player."

Write-Host "Albèra Bassa school validation passed."
