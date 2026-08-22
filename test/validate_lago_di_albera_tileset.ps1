param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'
function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }

$layout = @((Read-Json 'data/layouts/layouts.json').layouts | Where-Object id -eq 'LAYOUT_LAGO_DI_ALBERA')
$map = Read-Json 'data/maps/LagoDiAlbera/map.json'
$root = Join-Path $RepositoryRoot 'data/tilesets/secondary/lago_di_albera'
$graphics = Get-Content (Join-Path $RepositoryRoot 'src/data/tilesets/graphics.h') -Raw
$metatiles = Get-Content (Join-Path $RepositoryRoot 'src/data/tilesets/metatiles.h') -Raw
$headers = Get-Content (Join-Path $RepositoryRoot 'src/data/tilesets/headers.h') -Raw
$includes = Get-Content (Join-Path $RepositoryRoot 'include/tilesets.h') -Raw

Assert-True ($layout.Count -eq 1 -and $layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq 'gTileset_Pacifidlog') 'Lago must use General + Pacifidlog directly.'
Assert-True (-not (Test-Path -LiteralPath $root)) 'The custom Lago tileset directory must not exist.'
Assert-True (-not $graphics.Contains('LagoDiAlbera') -and -not $metatiles.Contains('LagoDiAlbera') -and -not $headers.Contains('LagoDiAlbera') -and -not $includes.Contains('LagoDiAlbera')) 'Custom Lago tileset registrations remain.'
Assert-True ($graphics.Contains('gTilesetTiles_Pacifidlog') -and $metatiles.Contains('gMetatiles_Pacifidlog') -and $headers.Contains('gTileset_Pacifidlog')) 'Pacifidlog registrations are missing.'
Assert-True ([int]$layout[0].width -eq 120 -and [int]$layout[0].height -eq 120) 'Lago dimensions changed.'
Assert-True (@($map.object_events).Count -eq 0 -and @($map.warp_events).Count -eq 0 -and @($map.coord_events).Count -eq 0 -and @($map.bg_events).Count -eq 0) 'Lago must not contain events.'
Assert-True (@($map.connections | Where-Object { $_.direction -eq 'down' -and $_.map -eq 'MAP_VIA_CONSOLARE' -and [int]$_.offset -eq 31 }).Count -eq 1) 'Lago south connection changed.'
$wild = Read-Json 'src/data/wild_encounters.json'
Assert-True (@($wild.wild_encounter_groups.encounters | Where-Object map -eq 'MAP_LAGO_DI_ALBERA').Count -eq 0) 'Lago must not have encounters.'

$mapBin = Join-Path $RepositoryRoot 'data/layouts/LagoDiAlbera/map.bin'
Assert-True ((Get-Item -LiteralPath $mapBin).Length -eq 120 * 120 * 2) 'Lago map.bin must be 120x120.'
$mapBytes = [IO.File]::ReadAllBytes($mapBin)
for ($i = 0; $i -lt $mapBytes.Length; $i += 2) {
    $metatile = [BitConverter]::ToUInt16($mapBytes, $i) -band 0x3FF
    Assert-True ($metatile -lt 512) "Lago map.bin contains secondary metatile 0x$('{0:X3}' -f $metatile)."
}
git -C $RepositoryRoot diff --quiet develop -- data/layouts/LagoDiAlbera/map.bin data/tilesets/primary/general data/tilesets/secondary/pacifidlog data/tilesets/secondary/porta_pretoria data/maps/PacifidlogTown data/layouts/PacifidlogTown
Assert-True ($LASTEXITCODE -eq 0) 'A protected map or source tileset changed.'
Write-Output 'Lago di Albera direct Pacifidlog tileset validation: PASS'
