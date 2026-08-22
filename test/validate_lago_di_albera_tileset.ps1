param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'
function Assert-True([bool]$c,[string]$m) { if (-not $c) { throw $m } }
function Read-Json([string]$p) { Get-Content (Join-Path $RepositoryRoot $p) -Raw | ConvertFrom-Json }
$root = Join-Path $RepositoryRoot 'data/tilesets/secondary/lago_di_albera'
$porta = Join-Path $RepositoryRoot 'data/tilesets/secondary/porta_pretoria'
foreach ($file in 'tiles.png','metatiles.bin','metatile_attributes.bin') { Assert-True (Test-Path (Join-Path $root $file)) "Missing Lago tileset file: $file" }
Assert-True ((Get-ChildItem (Join-Path $root 'palettes') -Filter '*.pal').Count -eq 16) 'Lago tileset must contain sixteen palettes.'
$meta = [IO.File]::ReadAllBytes((Join-Path $root 'metatiles.bin'))
$attrs = [IO.File]::ReadAllBytes((Join-Path $root 'metatile_attributes.bin'))
$portaMeta = [IO.File]::ReadAllBytes((Join-Path $porta 'metatiles.bin'))
$portaAttrs = [IO.File]::ReadAllBytes((Join-Path $porta 'metatile_attributes.bin'))
Assert-True (($meta.Length / 16) -eq 332 -and ($attrs.Length / 2) -eq 332) 'Lago metatile or attribute count is incorrect.'
Assert-True ($meta.Length -le (512 * 16)) 'Lago exceeds the secondary metatile limit.'
Assert-True ($attrs.Length -eq (($meta.Length / 16) * 2)) 'Lago attribute count does not match metatile count.'
$metaPrefix = New-Object byte[] $portaMeta.Length
$attrsPrefix = New-Object byte[] $portaAttrs.Length
[Array]::Copy($meta, $metaPrefix, $portaMeta.Length)
[Array]::Copy($attrs, $attrsPrefix, $portaAttrs.Length)
Assert-True ([Convert]::ToBase64String($metaPrefix) -eq [Convert]::ToBase64String($portaMeta)) 'PortaPretoria metatiles were not preserved as a prefix.'
Assert-True ([Convert]::ToBase64String($attrsPrefix) -eq [Convert]::ToBase64String($portaAttrs)) 'PortaPretoria attributes were not preserved as a prefix.'
$graphics = Get-Content (Join-Path $RepositoryRoot 'src/data/tilesets/graphics.h') -Raw
$headers = Get-Content (Join-Path $RepositoryRoot 'src/data/tilesets/headers.h') -Raw
$includes = Get-Content (Join-Path $RepositoryRoot 'include/tilesets.h') -Raw
Assert-True (($graphics -split 'gTilesetTiles_LagoDiAlbera').Count -eq 2 -and $graphics.Contains('-num_tiles 443')) 'Lago graphics registration is incorrect.'
Assert-True (($headers -split 'gTileset_LagoDiAlbera').Count -eq 2 -and $headers.Contains('.callback = InitTilesetAnim_Petalburg')) 'Lago tileset header is incorrect.'
Assert-True (($includes -split 'gTileset_LagoDiAlbera').Count -eq 2) 'Lago tileset declaration is incorrect.'
$layout = @((Read-Json 'data/layouts/layouts.json').layouts | Where-Object id -eq 'LAYOUT_LAGO_DI_ALBERA')
$map = Read-Json 'data/maps/LagoDiAlbera/map.json'
Assert-True ($layout.Count -eq 1 -and $layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_LagoDiAlbera') 'Lago layout tilesets are incorrect.'
Assert-True ([int]$layout[0].width -eq 120 -and [int]$layout[0].height -eq 120 -and @($map.object_events).Count -eq 0 -and @($map.warp_events).Count -eq 0 -and @($map.coord_events).Count -eq 0 -and @($map.bg_events).Count -eq 0) 'Lago map structure changed.'
Assert-True ($map.connections.Count -eq 1 -and $map.connections[0].direction -eq 'down' -and $map.connections[0].map -eq 'MAP_VIA_CONSOLARE' -and [int]$map.connections[0].offset -eq 31) 'Lago connection changed.'
git -C $RepositoryRoot diff --quiet develop -- data/layouts/LagoDiAlbera/map.bin data/maps/PacifidlogTown data/layouts/PacifidlogTown data/tilesets/secondary/pacifidlog data/tilesets/secondary/porta_pretoria data/tilesets/primary/general data/maps/ViaConsolare data/layouts/ViaConsolare
Assert-True ($LASTEXITCODE -eq 0) 'A source tileset or protected map changed.'
Assert-True (-not (Test-Path (Join-Path $root 'anim'))) 'Lago must not import Pacifidlog animations.'
$wild = Read-Json 'src/data/wild_encounters.json'
Assert-True (@($wild.wild_encounter_groups.encounters | Where-Object map -eq 'MAP_LAGO_DI_ALBERA').Count -eq 0) 'Lago must not have encounters.'
Write-Output 'Lago di Albera tileset validation: PASS'
