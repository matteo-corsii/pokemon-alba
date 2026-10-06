param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'
function Assert-True([bool]$Condition, [string]$Message) { if (-not $Condition) { throw $Message } }
function Read-Text([string]$Path) { Get-Content (Join-Path $RepositoryRoot $Path) -Raw }

$pokenav = Read-Text 'src/pokenav_region_map.c'
$regionMap = Read-Text 'src/region_map.c'
$help = Read-Text 'src/pokenav_main_menu.c'

Assert-True ($pokenav -match '\.height = 3,') 'Ausonia info window must be three tiles high.'
Assert-True ($help -match '\[HELPBAR_MAP_AUSONIA\].*COMPOUND_STRING\("\{B_BUTTON\}CANCEL"\)') 'Ausonia help bar must not advertise zoom.'
Assert-True ($pokenav -match 'if \(!state->zoomDisabled \|\| GetRegionMapType\(gMapHeader\.regionMapSectionId\) == REGION_MAP_AUSONIA\)') 'Ausonia must retain the standard Region Map input callback.'
Assert-True ($pokenav -match 'if \(IsAusoniaRegionMap\(\)\)\s*return POKENAV_MAP_FUNC_NONE;') 'Ausonia A input must not start zoom.'
Assert-True ($pokenav -match 'if \(!GetZoomDisabled\(\) \|\| IsAusoniaRegionMap\(\)\)') 'Ausonia cursor and player icon creation must remain enabled.'
Assert-True ($pokenav -match 'GetRegionMapInfoWindowHeight\(\)') 'Ausonia tilemap clearing must use the compact window height.'
Assert-True ($pokenav -match 'GetFontIdToFit\(name, FONT_NARROW, 0, 12 \* 8\)') 'Ausonia info names must use the compact-window fit font without truncation.'
Assert-True ($help -match '\[HELPBAR_MAP_AUSONIA\]') 'Ausonia help bar text is not registered.'
Assert-True ($regionMap -match 'BG_ATTR_WRAPAROUND,\s*GetRegionMapType\(gMapHeader\.regionMapSectionId\) == REGION_MAP_AUSONIA \? 0 : 1') 'Ausonia must disable affine wraparound while vanilla maps retain it.'
Assert-True ($regionMap -match 'REGION_MAP_AUSONIA\)\s*\r?\n\s*CalcZoomScrollParams\(8, 16, 0, 0, 0x100, 0x100, 0\)') 'Ausonia full-map viewport must skip the technical tilemap border at (1,2).'
Assert-True ($regionMap -match 'GetRegionMapType\(gMapHeader\.regionMapSectionId\) == REGION_MAP_AUSONIA\)\s*\r?\n\s*sRegionMap->playerIconSprite->oam\.priority = 1;') 'Ausonia player icon must render above detail BG1.'

& (Join-Path $PSScriptRoot 'validate_ausonia_region_map_asset.ps1') -RepositoryRoot $RepositoryRoot
& (Join-Path $PSScriptRoot 'validate_ausonia_region_map_scaffold.ps1') -RepositoryRoot $RepositoryRoot
Write-Output 'Ausonia Region Map interaction validation passed.'
