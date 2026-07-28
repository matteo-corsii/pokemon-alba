param(
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName PresentationCore

function Assert-Condition([bool]$Condition, [string]$Message)
{
    if (-not $Condition)
    {
        throw $Message
    }
}

function Test-EqualArrays($First, $Second)
{
    if ($First.Count -ne $Second.Count)
    {
        return $false
    }
    for ($index = 0; $index -lt $First.Count; $index++)
    {
        if ($First[$index] -ne $Second[$index])
        {
            return $false
        }
    }
    return $true
}

function Read-IndexedPng([string]$Path)
{
    $stream = [IO.File]::OpenRead($Path)
    try
    {
        $decoder = New-Object Windows.Media.Imaging.PngBitmapDecoder(
            $stream,
            [Windows.Media.Imaging.BitmapCreateOptions]::PreservePixelFormat,
            [Windows.Media.Imaging.BitmapCacheOption]::OnLoad
        )
        $image = $decoder.Frames[0]
    }
    finally
    {
        $stream.Dispose()
    }

    $bitsPerPixel = $image.Format.BitsPerPixel
    Assert-Condition ($image.Format.ToString().StartsWith('Indexed')) "$Path is not an indexed PNG"
    Assert-Condition ($bitsPerPixel -in 4, 8) "$Path uses unsupported indexed depth $bitsPerPixel"

    $stride = [int](($image.PixelWidth * $bitsPerPixel + 7) / 8)
    $packed = New-Object byte[] ($stride * $image.PixelHeight)
    $image.CopyPixels($packed, $stride, 0)
    $indices = New-Object byte[] ($image.PixelWidth * $image.PixelHeight)

    for ($y = 0; $y -lt $image.PixelHeight; $y++)
    {
        for ($x = 0; $x -lt $image.PixelWidth; $x++)
        {
            if ($bitsPerPixel -eq 8)
            {
                $index = $packed[$y * $stride + $x]
            }
            else
            {
                $packedByte = $packed[$y * $stride + [int]($x / 2)]
                $index = if (($x % 2) -eq 0) { $packedByte -shr 4 } else { $packedByte -band 15 }
            }
            $indices[$y * $image.PixelWidth + $x] = $index
        }
    }

    return [pscustomobject]@{
        Width = $image.PixelWidth
        Height = $image.PixelHeight
        Palette = $image.Palette.Colors
        Indices = $indices
    }
}

function Get-FrameIndices($Image, [int]$Frame, [int]$FrameHeight)
{
    $frameLength = [int]$Image.Width * $FrameHeight
    $frameData = [byte[]]::new($frameLength)
    [Array]::Copy(
        $Image.Indices,
        $Frame * $Image.Width * $FrameHeight,
        $frameData,
        0,
        $frameData.Length
    )
    return ,$frameData
}

function Assert-Png($Image, [int]$Width, [int]$Height, [int]$FrameHeight, [string]$Name)
{
    Assert-Condition ($Image.Width -eq $Width -and $Image.Height -eq $Height) "$Name has invalid dimensions"
    Assert-Condition (($Width % 8) -eq 0 -and ($Height % 8) -eq 0) "$Name is not tile-aligned"
    Assert-Condition (($Image.Indices | Measure-Object -Maximum).Maximum -le 15) "$Name uses an index above 15"
    Assert-Condition ($Image.Palette[0].A -eq 0) "$Name palette index 0 is not transparent"

    $frameCount = $Height / $FrameHeight
    for ($frame = 0; $frame -lt $frameCount; $frame++)
    {
        $indices = Get-FrameIndices -Image $Image -Frame $frame -FrameHeight $FrameHeight
        Assert-Condition (($indices | Where-Object { $_ -ne 0 }).Count -gt 0) "$Name frame $frame is empty"
    }

    if ($frameCount -eq 2)
    {
        $first = Get-FrameIndices -Image $Image -Frame 0 -FrameHeight $FrameHeight
        $second = Get-FrameIndices -Image $Image -Frame 1 -FrameHeight $FrameHeight
        Assert-Condition (-not (Test-EqualArrays $first $second)) "$Name frames are identical"
    }
}

function Read-JascPalette([string]$Path)
{
    $lines = Get-Content -LiteralPath $Path
    Assert-Condition ($lines.Count -eq 19) "$Path must contain a 3-line header and 16 colors"
    Assert-Condition ($lines[0] -eq 'JASC-PAL') "$Path has an invalid JASC header"
    Assert-Condition ($lines[1] -eq '0100') "$Path has an invalid JASC version"
    Assert-Condition ($lines[2] -eq '16') "$Path must declare exactly 16 colors"

    $colors = @()
    foreach ($line in $lines[3..18])
    {
        $parts = $line -split ' '
        Assert-Condition ($parts.Count -eq 3) "$Path contains an invalid color row"
        $rgb = @($parts | ForEach-Object { [int]$_ })
        Assert-Condition (($rgb | Measure-Object -Minimum).Minimum -ge 0) "$Path contains a negative color component"
        Assert-Condition (($rgb | Measure-Object -Maximum).Maximum -le 255) "$Path contains a color component above 255"
        $colors += ,$rgb
    }
    return $colors
}

$assetRoot = Join-Path $RepositoryRoot 'graphics/pokemon/cingerm'
$front = Read-IndexedPng (Join-Path $assetRoot 'anim_front.png')
$back = Read-IndexedPng (Join-Path $assetRoot 'back.png')
$icon = Read-IndexedPng (Join-Path $assetRoot 'icon.png')

Assert-Png $front 64 128 64 'anim_front.png'
Assert-Png $back 64 64 64 'back.png'
Assert-Png $icon 32 64 32 'icon.png'

$normal = Read-JascPalette (Join-Path $assetRoot 'normal.pal')
$shiny = Read-JascPalette (Join-Path $assetRoot 'shiny.pal')
Assert-Condition (Test-EqualArrays $normal[0] @(255, 0, 255)) 'normal.pal index 0 is not the transparent color'
Assert-Condition (Test-EqualArrays $shiny[0] @(255, 0, 255)) 'shiny.pal index 0 is not the transparent color'

$globalIconPalette = Read-JascPalette (Join-Path $RepositoryRoot 'graphics/pokemon/icon_palettes/pal5.pal')
for ($index = 0; $index -lt 16; $index++)
{
    $actual = @($icon.Palette[$index].R, $icon.Palette[$index].G, $icon.Palette[$index].B)
    Assert-Condition (Test-EqualArrays $actual $globalIconPalette[$index]) "icon.png palette index $index does not match pal5"
}

$gameplayPaths = @(
    (Join-Path $RepositoryRoot 'data/maps'),
    (Join-Path $RepositoryRoot 'data/scripts'),
    (Join-Path $RepositoryRoot 'src/data/wild_encounters.json'),
    (Join-Path $RepositoryRoot 'src/data/trainers.h'),
    (Join-Path $RepositoryRoot 'src/data/trainers_frlg.h'),
    (Join-Path $RepositoryRoot 'src/data/debug_trainers.h'),
    (Join-Path $RepositoryRoot 'src/data/battle_partners.h')
)
$gameplayFiles = foreach ($path in $gameplayPaths)
{
    if (Test-Path -LiteralPath $path -PathType Container)
    {
        Get-ChildItem -LiteralPath $path -Recurse -File
    }
    elseif (Test-Path -LiteralPath $path -PathType Leaf)
    {
        Get-Item -LiteralPath $path
    }
}
$references = $gameplayFiles | Select-String -SimpleMatch 'SPECIES_CINGERM'
Assert-Condition ($null -eq $references) 'Cingerm is referenced by gameplay data and is no longer unobtainable'

Write-Output 'Cingerm graphics validation passed.'
