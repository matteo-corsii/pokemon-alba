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
    $frameData = [byte[]]::new($Image.Width * $FrameHeight)
    [Array]::Copy(
        $Image.Indices,
        $Frame * $Image.Width * $FrameHeight,
        $frameData,
        0,
        $frameData.Length
    )
    return ,$frameData
}

function Get-BoundingBox($Image, [int]$Frame, [int]$FrameHeight)
{
    $minimumX = $Image.Width
    $minimumY = $FrameHeight
    $maximumX = -1
    $maximumY = -1
    for ($y = 0; $y -lt $FrameHeight; $y++)
    {
        for ($x = 0; $x -lt $Image.Width; $x++)
        {
            $index = $Image.Indices[(($Frame * $FrameHeight + $y) * $Image.Width) + $x]
            if ($index -ne 0)
            {
                $minimumX = [Math]::Min($minimumX, $x)
                $minimumY = [Math]::Min($minimumY, $y)
                $maximumX = [Math]::Max($maximumX, $x)
                $maximumY = [Math]::Max($maximumY, $y)
            }
        }
    }
    Assert-Condition ($maximumX -ge 0) "Frame $Frame is empty"
    return @($minimumX, $minimumY, ($maximumX - $minimumX + 1), ($maximumY - $minimumY + 1))
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
        $differentPixels = 0
        for ($index = 0; $index -lt $first.Count; $index++)
        {
            if ($first[$index] -ne $second[$index])
            {
                $differentPixels++
            }
        }
        Assert-Condition ($differentPixels -gt 0) "$Name frames are identical"
        Assert-Condition ($differentPixels -lt ($first.Count * 0.4)) "$Name frame variation is unexpectedly large"
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

$assetRoot = Join-Path $RepositoryRoot 'graphics/pokemon/serbrace'
$front = Read-IndexedPng (Join-Path $assetRoot 'anim_front.png')
$back = Read-IndexedPng (Join-Path $assetRoot 'back.png')
$icon = Read-IndexedPng (Join-Path $assetRoot 'icon.png')

Assert-Png $front 64 128 64 'anim_front.png'
Assert-Png $back 64 64 64 'back.png'
Assert-Png $icon 32 64 32 'icon.png'
Assert-Condition (Test-EqualArrays ($icon.Indices | Sort-Object -Unique) @(0, 1, 2, 3, 8, 12, 13, 14, 15)) 'icon.png contains indices outside the documented pal3 remapping'
Assert-Condition (Test-EqualArrays (Get-BoundingBox $front 0 64) @(2, 6, 60, 54)) 'anim_front.png frame 0 bounding box changed'
Assert-Condition (Test-EqualArrays (Get-BoundingBox $front 1 64) @(2, 6, 59, 54)) 'anim_front.png frame 1 bounding box changed'
Assert-Condition (Test-EqualArrays (Get-BoundingBox $back 0 64) @(3, 9, 58, 51)) 'back.png bounding box changed'
Assert-Condition (Test-EqualArrays (Get-BoundingBox $icon 0 32) @(2, 3, 28, 28)) 'icon.png frame 0 bounding box changed'
Assert-Condition (Test-EqualArrays (Get-BoundingBox $icon 1 32) @(2, 3, 28, 28)) 'icon.png frame 1 bounding box changed'

$normal = Read-JascPalette (Join-Path $assetRoot 'normal.pal')
$shiny = Read-JascPalette (Join-Path $assetRoot 'shiny.pal')
Assert-Condition (Test-EqualArrays $normal[0] @(255, 0, 255)) 'normal.pal index 0 is not the transparent color'
Assert-Condition (Test-EqualArrays $shiny[0] @(255, 0, 255)) 'shiny.pal index 0 is not the transparent color'

$globalIconPalette = Read-JascPalette (Join-Path $RepositoryRoot 'graphics/pokemon/icon_palettes/pal3.pal')
for ($index = 0; $index -lt 16; $index++)
{
    $actual = @($icon.Palette[$index].R, $icon.Palette[$index].G, $icon.Palette[$index].B)
    Assert-Condition (Test-EqualArrays $actual $globalIconPalette[$index]) "icon.png palette index $index does not match pal3"
}

$graphics = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/graphics/pokemon.h') -Raw
$species = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/pokemon/species_info.h') -Raw
$starter = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/starter_choose.c') -Raw
$serbrace = [regex]::Match($species, '(?s)\[SPECIES_SERBRACE\]\s*=\s*\{.*?\n    \},').Value
Assert-Condition ($serbrace.Length -gt 0) 'SPECIES_SERBRACE record was not found'

foreach ($declaration in @(
    'gMonFrontPic_Serbrace[] = INCGFX_U32("graphics/pokemon/serbrace/anim_front.png", ".4bpp.smol")',
    'gMonBackPic_Serbrace[] = INCGFX_U32("graphics/pokemon/serbrace/back.png", ".4bpp.smol")',
    'gMonPalette_Serbrace[] = INCGFX_U16("graphics/pokemon/serbrace/normal.pal", ".gbapal")',
    'gMonShinyPalette_Serbrace[] = INCGFX_U16("graphics/pokemon/serbrace/shiny.pal", ".gbapal")',
    'gMonIcon_Serbrace[] = INCGFX_U8("graphics/pokemon/serbrace/icon.png", ".4bpp")'
))
{
    Assert-Condition ($graphics.Contains($declaration)) "Missing Serbrace graphics declaration: $declaration"
}

foreach ($reference in @(
    '.frontPic = gMonFrontPic_Serbrace',
    '.backPic = gMonBackPic_Serbrace',
    '.palette = gMonPalette_Serbrace',
    '.shinyPalette = gMonShinyPalette_Serbrace',
    '.iconSprite = gMonIcon_Serbrace',
    '.frontPicSize = MON_COORDS_SIZE(64, 56)',
    '.backPicSize = MON_COORDS_SIZE(64, 56)',
    '.frontPicYOffset = 4',
    '.backPicYOffset = 4',
    '.iconPalIndex = 3',
    'ANIMCMD_FRAME(1, 12)',
    'ANIMCMD_FRAME(0, 8)',
    '.cryId = CRY_EKANS',
    'FOOTPRINT(Ekans)',
    'sPicTable_Ekans'
))
{
    Assert-Condition ($serbrace.Contains($reference)) "Unexpected SPECIES_SERBRACE graphics state: $reference"
}

Assert-Condition (-not $serbrace.Contains('.frontPic = gMonFrontPic_Ekans')) 'Serbrace still uses the Ekans front sprite'
Assert-Condition (-not $serbrace.Contains('.backPic = gMonBackPic_Ekans')) 'Serbrace still uses the Ekans back sprite'
Assert-Condition (([regex]::Matches($serbrace, 'ANIMCMD_FRAME\(')).Count -eq 2) 'Serbrace front animation must use exactly two commands'
Assert-Condition ($starter -match '#define FIRE_STARTER\s+\(IS_FRLG \? SPECIES_CHARMANDER : SPECIES_SERBRACE\)') 'Serbrace is no longer the Emerald Fire starter'
Assert-Condition ($species.Contains('.frontPic = gMonFrontPic_Cingerm')) 'Cingerm original front sprite reference changed'
Assert-Condition ($species.Contains('.frontPic = gMonFrontPic_Ducklett')) 'Ardeino no longer uses its intended placeholder'

$wildEncounters = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/wild_encounters.json') -Raw
Assert-Condition (-not $wildEncounters.Contains('SPECIES_SERBRACE')) 'Serbrace was added to wild encounters'

Write-Output 'Serbrace graphics validation passed.'
