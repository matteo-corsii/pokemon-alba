param([string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path)
$ErrorActionPreference = 'Stop'

function Assert-True([bool]$condition, [string]$message) { if (-not $condition) { throw $message } }
function Read-Json([string]$path) { Get-Content -LiteralPath (Join-Path $RepositoryRoot $path) -Raw | ConvertFrom-Json }
function Read-RawBlocks([string]$path) {
    $bytes = [IO.File]::ReadAllBytes((Join-Path $RepositoryRoot $path))
    Assert-True ($bytes.Length -eq 8) "$path must be a 2x2 (8-byte) border."
    return @(0..3 | ForEach-Object { [BitConverter]::ToUInt16($bytes, $_ * 2) })
}
function Assert-Connection([object]$map, [string]$direction, [string]$destination, [int]$offset) {
    $matches = @($map.connections | Where-Object { $_.direction -eq $direction -and $_.map -eq $destination -and [int]$_.offset -eq $offset })
    Assert-True ($matches.Count -eq 1) "$($map.id) connection $direction to $destination ($offset) changed."
}

$layouts = (Read-Json 'data/layouts/layouts.json').layouts
$reference = Read-RawBlocks 'data/layouts/AlberaStorica/border.bin'
$expectedForest = @(0x1D4, 0x1D5, 0x1DC, 0x1DD)
Assert-True (($reference -join ',') -eq ($expectedForest -join ',')) 'Albera Storica no longer provides the approved shared forest border pattern.'

$targets = @(
    @{ Directory = 'LagoDiAlbera'; MapFile = 'data/maps/LagoDiAlbera/map.json'; MapId = 'MAP_LAGO_DI_ALBERA'; Layout = 'LAYOUT_LAGO_DI_ALBERA'; Secondary = 'gTileset_LagoDiAlbera' },
    @{ Directory = 'BoscoDelRomitorio'; MapFile = 'data/maps/BoscoDelRomitorio/map.json'; MapId = 'MAP_BOSCO_DEL_ROMITORIO'; Layout = 'LAYOUT_BOSCO_DEL_ROMITORIO'; Secondary = 'gTileset_LagoDiAlbera' },
    @{ Directory = 'ViaConsolare'; MapFile = 'data/maps/ViaConsolare/map.json'; MapId = 'MAP_VIA_CONSOLARE'; Layout = 'LAYOUT_VIA_CONSOLARE'; Secondary = 'gTileset_ViaConsolare' },
    @{ Directory = 'BorgoDiCastello'; MapFile = 'data/maps/BorgoDiCastello/map.json'; MapId = 'MAP_BORGO_DI_CASTELLO'; Layout = 'LAYOUT_BORGO_DI_CASTELLO'; Secondary = 'gTileset_Sootopolis' },
    @{ Directory = 'Route103'; MapFile = 'data/maps/Route103/map.json'; MapId = 'MAP_ROUTE103'; Layout = 'LAYOUT_ROUTE103'; Secondary = 'gTileset_PortaPretoria' },
    @{ Directory = 'VillaPapaleGiardini'; MapFile = 'data/maps/VillaPapaleGiardini/map.json'; MapId = 'MAP_VILLA_PAPALE_GIARDINI'; Layout = 'LAYOUT_VILLA_PAPALE_GIARDINI'; Secondary = 'gTileset_Sootopolis' },
    @{ Directory = 'StradaBorgoCisternoni'; MapFile = 'data/maps/StradaBorgoCisternoni/map.json'; MapId = 'MAP_STRADA_BORGO_CISTERNONI'; Layout = 'LAYOUT_STRADA_BORGO_CISTERNONI'; Secondary = 'gTileset_PortaPretoria' },
    @{ Directory = 'PonteValleLaricia'; MapFile = 'data/maps/PonteValleLaricia/map.json'; MapId = 'MAP_PONTE_VALLE_LARICIA'; Layout = 'LAYOUT_PONTE_VALLE_LARICIA'; Secondary = 'gTileset_PortaPretoria' },
    @{ Directory = 'Laricia'; MapFile = 'data/maps/Laricia/map.json'; MapId = 'MAP_LARICIA'; Layout = 'LAYOUT_LARICIA'; Secondary = 'gTileset_Laricia' }
)

foreach ($target in $targets) {
    $map = Read-Json $target.MapFile
    $layout = @($layouts | Where-Object { $_.id -eq $target.Layout })
    Assert-True ($map.id -eq $target.MapId -and $map.layout -eq $target.Layout) "$($target.Directory) map/layout identity changed."
    Assert-True ($layout.Count -eq 1 -and $layout[0].primary_tileset -eq 'gTileset_General' -and $layout[0].secondary_tileset -eq $target.Secondary) "$($target.Directory) tilesets changed."
    Assert-True ($layout[0].border_filepath -eq "data/layouts/$($target.Directory)/border.bin") "$($target.Directory) border path changed."
    $raw = Read-RawBlocks $layout[0].border_filepath
    Assert-True (($raw -join ',') -eq ($reference -join ',')) "$($target.Directory) must use Albera Storica's shared forest border."
    foreach ($block in $raw) {
        Assert-True (($block -band 0x3FF) -lt 0x200) "$($target.Directory) border must use shared primary metatiles, not secondary-specific IDs."
        Assert-True (($block -band 0xFC00) -eq 0) "$($target.Directory) border raw block has unexpected collision or elevation bits."
    }
}

$lago = Read-Json 'data/maps/LagoDiAlbera/map.json'
$bosco = Read-Json 'data/maps/BoscoDelRomitorio/map.json'
$via = Read-Json 'data/maps/ViaConsolare/map.json'
$borgo = Read-Json 'data/maps/BorgoDiCastello/map.json'
$route103 = Read-Json 'data/maps/Route103/map.json'
$villa = Read-Json 'data/maps/VillaPapaleGiardini/map.json'
$strada = Read-Json 'data/maps/StradaBorgoCisternoni/map.json'
$ponteValle = Read-Json 'data/maps/PonteValleLaricia/map.json'
$laricia = Read-Json 'data/maps/Laricia/map.json'
Assert-Connection $lago 'down' 'MAP_VIA_CONSOLARE' 31
Assert-Connection $lago 'up' 'MAP_BORGO_DI_CASTELLO' 87
Assert-True ($null -eq $bosco.connections -or @($bosco.connections | Where-Object { $null -ne $_ }).Count -eq 0) 'Bosco del Romitorio must not gain map connections.'
Assert-Connection $via 'right' 'MAP_ROUTE103' 0
Assert-Connection $via 'up' 'MAP_LAGO_DI_ALBERA' -31
Assert-Connection $borgo 'down' 'MAP_LAGO_DI_ALBERA' -87
Assert-Connection $borgo 'up' 'MAP_VILLA_PAPALE_GIARDINI' 0
Assert-Connection $borgo 'right' 'MAP_STRADA_BORGO_CISTERNONI' 50
Assert-Connection $route103 'down' 'MAP_OLDALE_TOWN' 0
Assert-Connection $route103 'left' 'MAP_VIA_CONSOLARE' 0
Assert-Connection $route103 'up' 'MAP_STRADA_BORGO_CISTERNONI' 0
Assert-Connection $route103 'right' 'MAP_PONTE_VALLE_LARICIA' 0
Assert-Connection $villa 'down' 'MAP_BORGO_DI_CASTELLO' 0
Assert-Connection $strada 'left' 'MAP_BORGO_DI_CASTELLO' -50
Assert-Connection $strada 'down' 'MAP_ROUTE103' 0
Assert-Connection $ponteValle 'left' 'MAP_ROUTE103' 0
Assert-Connection $ponteValle 'right' 'MAP_LARICIA' 0
Assert-Connection $laricia 'left' 'MAP_PONTE_VALLE_LARICIA' 0
Assert-True (@($lago.connections).Count -eq 2 -and @($via.connections).Count -eq 2 -and @($borgo.connections).Count -eq 3 -and @($route103.connections).Count -eq 4 -and @($villa.connections).Count -eq 1 -and @($strada.connections).Count -eq 2 -and @($ponteValle.connections).Count -eq 2 -and @($laricia.connections).Count -eq 1) 'Custom exterior connection counts changed unexpectedly.'

git -C $RepositoryRoot diff --quiet -- data/layouts/BorgoDiCastello/map.bin
Assert-True ($LASTEXITCODE -eq 0) 'Borgo map.bin must remain unchanged.'

Write-Output 'Custom exterior forest borders: PASS'
