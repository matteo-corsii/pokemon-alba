param(
    [string]$BaseRef = "develop"
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

$allowedPaths = @(
    'data/event_scripts.s',
    'data/maps/AlberaStorica/map.json',
    'data/maps/AlberaStorica/scripts.inc',
    'data/maps/AlberaStorica_MentorsHouse/map.json',
    'data/maps/AlberaStorica_MentorsHouse/scripts.inc',
    'data/maps/AlberaStorica_CivicArchive/map.json',
    'data/maps/AlberaStorica_CivicArchive/scripts.inc',
    'data/maps/AlberaStorica_SecretBaseShop/map.json',
    'data/maps/AlberaStorica_SecretBaseShop/scripts.inc',
    'data/maps/map_groups.json',
    'data/scripts/secret_base.inc',
    'data/scripts/shared_secret_base.inc',
    'data/specials.inc',
    'data/layouts/AlberaStorica/map.bin',
    'include/constants/flags.h',
    'include/constants/flags_frlg.h',
    'include/constants/secret_bases.h',
    'include/constants/script_menu.h',
    'src/data/script_menu.h',
    'src/secret_base.c',
    'test/validate_porta_pretoria_dedicated_tileset.ps1',
    'test/validate_porta_pretoria_localization.ps1',
    'test/validate_albera_storica_secret_base_mentor.ps1'
    ,'test/validate_albera_storica_blockout.ps1'
    ,'test/validate_albera_storica_civic_life.ps1'
    # User-approved Porymap state retained with the Albera civic-life batch.
    ,'data/layouts/layouts.json'
    ,'data/maps/AlberaStorica_Anfiteatro/map.json'
    ,'data/maps/OldaleTown/map.json'
    ,'src/data/heal_locations.json'
    ,'src/data/region_map/region_map_sections.json'
)

$unexpectedPaths = @(Get-ChangedPaths | Where-Object { $_ -notin $allowedPaths })
Assert-True ($unexpectedPaths.Count -eq 0) ('Out-of-scope files changed: ' + ($unexpectedPaths -join ', '))

$flags = Get-Content -Raw -Encoding utf8 'include/constants/flags.h'
$flagsFrlg = Get-Content -Raw -Encoding utf8 'include/constants/flags_frlg.h'
Assert-True ($flags -match '#define\s+FLAG_ALBERA_SECRET_BASES_UNLOCKED\s+0x8EB\b') 'Missing Emerald Secret Base mentor unlock flag at 0x8EB.'
Assert-True ($flagsFrlg -match '#define\s+FLAG_ALBERA_SECRET_BASES_UNLOCKED\s+0x8EB\b') 'Missing FRLG Secret Base mentor unlock flag at 0x8EB.'

$secretBaseConstants = Get-Content -Raw -Encoding utf8 'include/constants/secret_bases.h'
Assert-True ($secretBaseConstants -match '#define\s+SECRET_BASE_ALBERA_TREE_1\s+241\b') 'Albera tree Secret Base ID must be append-only ID 241.'
Assert-True ($secretBaseConstants -match '#define\s+SECRET_BASE_ALBERA_TREE\s+SECRET_BASE_GROUP\(24\)') 'Albera tree must use its own Secret Base group.'
Assert-True ($secretBaseConstants -match '#define\s+NUM_SECRET_BASE_GROUPS\s+25\b') 'Secret Base group count must include Albera tree.'

$secretBaseCode = Get-Content -Raw -Encoding utf8 'src/secret_base.c'
Assert-True ($secretBaseCode -match '\[SECRET_BASE_ALBERA_TREE\]\s*=\s*MAP_NUM\(MAP_SECRET_BASE_TREE1\),\s*0,\s*2,\s*3') 'Albera tree must reuse the SecretBase_Tree1 interior entry.'

$specials = Get-Content -Raw -Encoding utf8 'data/specials.inc'
Assert-True (([regex]::Matches($specials, 'def_special ToggleSecretBaseEntranceMetatile')).Count -eq 1) 'Secret Base opening special must be registered once.'

$secretBaseScripts = Get-Content -Raw -Encoding utf8 'data/scripts/secret_base.inc'
Assert-True ($secretBaseScripts -match 'goto_if_unset FLAG_ALBERA_SECRET_BASES_UNLOCKED, SecretBase_EventScript_UnlockRequired') 'New base creation must be gated by the mentor unlock.'
Assert-True ($secretBaseScripts -match 'SecretBase_EventScript_CreateWithoutSecretPower::[\s\S]*?special ToggleSecretBaseEntranceMetatile[\s\S]*?goto SecretBase_EventScript_InitSecretBase') 'Unlocked base creation must use the existing Secret Base opening and initialization flow without Secret Power.'
Assert-True ($secretBaseScripts -match 'SecretBase_EventScript_AlreadyHasSecretBase::\s*goto_if_set FLAG_ALBERA_SECRET_BASES_UNLOCKED, SecretBase_EventScript_MoveWithoutSecretPower') 'Moving an existing base must use the unlocked path.'
Assert-True ($secretBaseScripts -match 'SecretBase_EventScript_MoveWithoutSecretPower::[\s\S]*?special ToggleSecretBaseEntranceMetatile[\s\S]*?goto SecretBase_EventScript_InitSecretBase') 'Unlocked base movement must not require Secret Power.'
Assert-True ($secretBaseScripts -match 'SecretBase_EventScript_UnlockRequired::[\s\S]*?releaseall\s*\r?\n\s*end') 'Locked Secret Base spots must release player controls.'

$sharedSecretBaseScripts = Get-Content -Raw -Encoding utf8 'data/scripts/shared_secret_base.inc'
Assert-True ($sharedSecretBaseScripts -match 'SecretBase_EventScript_PCSelectMainMenu::[\s\S]*?special IsCurSecretBaseOwnedByAnotherPlayer[\s\S]*?goto_if_eq VAR_RESULT, TRUE, SecretBase_EventScript_PCShowMainMenu[\s\S]*?goto SecretBase_EventScript_PCOwnerShowMainMenu') 'Secret Base personal utilities must be gated by the existing ownership check.'
Assert-True ($sharedSecretBaseScripts -match 'SecretBase_EventScript_PCAccessStorage::[\s\S]*?special ShowPokemonStorageSystemPC[\s\S]*?goto SecretBase_EventScript_PCOwnerShowMainMenu') 'Own Secret Base storage must use the direct Storage System special and return safely.'
Assert-True ($sharedSecretBaseScripts -match 'SecretBase_EventScript_PCRest::[\s\S]*?call Common_EventScript_OutOfCenterPartyHeal[\s\S]*?goto SecretBase_EventScript_PCOwnerShowMainMenu') 'Own Secret Base healing must use the out-of-center party-heal flow and return safely.'
Assert-True ($sharedSecretBaseScripts -notmatch 'sethealplace|setrespawn') 'Secret Base utilities must not change the respawn location.'

$scriptMenuConstants = Get-Content -Raw -Encoding utf8 'include/constants/script_menu.h'
$scriptMenus = Get-Content -Raw -Encoding utf8 'src/data/script_menu.h'
Assert-True ($scriptMenuConstants -match 'MULTI_SECRET_BASE_OWNER_PC_NO_REGISTRY') 'Missing own-base PC menu constant.'
Assert-True ($scriptMenuConstants -match 'MULTI_SECRET_BASE_OWNER_PC_WITH_REGISTRY') 'Missing own-base registered-PC menu constant.'
Assert-True ($scriptMenus -match 'MultichoiceList_SecretBaseOwnerPCNoRegistry[\s\S]*?gText_Storage[\s\S]*?gText_Rest') 'Own-base PC menu must expose Storage and Rest.'

$groups = Read-JsonFile 'data/maps/map_groups.json'
Assert-True ((@($groups.gMapGroup_IndoorOldale | Where-Object { $_ -eq 'AlberaStorica_MentorsHouse' })).Count -eq 1) 'Mentor house must be appended exactly once to the indoor map group.'

$albera = Read-JsonFile 'data/maps/AlberaStorica/map.json'
$mentorHouse = Read-JsonFile 'data/maps/AlberaStorica_MentorsHouse/map.json'
Assert-True (@($albera.object_events | Where-Object { $_.local_id -eq 'LOCALID_ALBERA_SECRET_BASE_MENTOR' }).Count -eq 0) 'Mentor must remain inside the house.'
Assert-True ((@($albera.warp_events | Where-Object { $_.x -eq 17 -and $_.y -eq 5 -and $_.dest_map -eq 'MAP_ALBERA_STORICA_ANFITEATRO' })).Count -eq 1) 'Amphitheatre warp changed.'
Assert-True ((@($albera.warp_events | Where-Object { $_.x -eq 15 -and $_.y -eq 26 -and $_.dest_map -eq 'MAP_ALBERA_STORICA_MENTORS_HOUSE' -and $_.dest_warp_id -eq '0' })).Count -eq 1) 'Mentor house entrance must remain at (15,26).'
Assert-True (@($albera.coord_events).Count -eq 2) 'No new Albera Storica coordinate events are allowed.'
Assert-True ((@($albera.coord_events | Where-Object { $_.x -in @(17, 18) -and $_.y -eq 4 -and $_.script -eq 'AlberaStorica_EventScript_EnterAnfiteatro' })).Count -eq 2) 'Amphitheatre triggers changed.'
Assert-True ((@($albera.bg_events | Where-Object { $_.type -eq 'secret_base' -and $_.x -eq 26 -and $_.y -eq 25 -and $_.elevation -eq 3 -and $_.secret_base_id -eq 'SECRET_BASE_ALBERA_TREE_1' })).Count -eq 1) 'Missing Albera Secret Base tree event at (26,25).'
Assert-True (@($albera.bg_events).Count -eq 1) 'No unrelated Albera Storica background events are allowed.'

Assert-True ($mentorHouse.id -eq 'MAP_ALBERA_STORICA_MENTORS_HOUSE') 'Unexpected mentor house map ID.'
Assert-True ($mentorHouse.layout -eq 'LAYOUT_HOUSE_WITH_BED') 'Mentor house must reuse the compatible HouseWithBed layout.'
Assert-True (@($mentorHouse.object_events).Count -eq 1) 'Mentor house must contain exactly one mentor NPC.'
$mentor = @($mentorHouse.object_events | Where-Object { $_.local_id -eq 'LOCALID_ALBERA_SECRET_BASE_MENTOR' -and $_.graphics_id -eq 'OBJ_EVENT_GFX_CAMPER' -and $_.x -eq 5 -and $_.y -eq 3 })
Assert-True ($mentor.Count -eq 1) 'Mentor object placement is invalid.'
Assert-True (@($mentorHouse.warp_events).Count -eq 2) 'Mentor house must have two return door warps.'
foreach ($warp in @($mentorHouse.warp_events)) {
    Assert-True ($warp.dest_map -eq 'MAP_ALBERA_STORICA' -and $warp.dest_warp_id -eq '1') 'Mentor house return warp is invalid.'
}

$mentorScripts = Get-Content -Raw -Encoding utf8 'data/maps/AlberaStorica_MentorsHouse/scripts.inc'
Assert-True ($mentorScripts -match 'goto_if_set FLAG_ALBERA_SECRET_BASES_UNLOCKED') 'Mentor repeat dialogue must be gated by the unlock flag.'
Assert-True (([regex]::Matches($mentorScripts, 'setflag FLAG_ALBERA_SECRET_BASES_UNLOCKED')).Count -eq 1) 'Mentor must set the unlock flag exactly once.'
Assert-True ($mentorScripts -notmatch 'giveitem|trainerbattle|MOVE_SECRET_POWER|FLAG_RECEIVED_SECRET_POWER') 'Mentor may not grant items, battles, or Secret Power.'

$eventScripts = Get-Content -Raw -Encoding utf8 'data/event_scripts.s'
Assert-True (([regex]::Matches($eventScripts, [regex]::Escape('.include "data/maps/AlberaStorica_MentorsHouse/scripts.inc"'))).Count -eq 1) 'Mentor house scripts must be centrally included once.'

$mapBytes = [IO.File]::ReadAllBytes('data/layouts/AlberaStorica/map.bin')
 $mentorDoor = [BitConverter]::ToUInt16($mapBytes, 2 * ((26 * 36) + 15))
Assert-True ($mentorDoor -eq 0x0687) 'Mentor house entrance must use the passable PortaPretoria-compatible Oldale door metatile at (15,26).'
$mentorFacade = [BitConverter]::ToUInt16($mapBytes, 2 * ((26 * 36) + 14))
Assert-True ($mentorFacade -ne 0x0687) 'Mentor house may have only one functional entrance; the adjacent facade tile at (14,26) must not duplicate the warp-door metatile.'
foreach ($expected in @(@(26, 25, 0x3426), @(27, 25, 0x3427))) {
    $raw = [BitConverter]::ToUInt16($mapBytes, 2 * (($expected[1] * 36) + $expected[0]))
    Assert-True ($raw -eq $expected[2]) ('Secret Base tree metatile is invalid at (' + $expected[0] + ',' + $expected[1] + ').')
}

$saveStructureChanges = @((Get-ChangedPaths) | Where-Object { $_ -match '^(include/global\.h|src/save\.c|src/save\.h|src/data/save|test/save\.c)$' })
Assert-True ($saveStructureChanges.Count -eq 0) 'Secret Base mentor batch must not change save structures.'

Write-Output 'Albera Storica Secret Base mentor validation passed.'
