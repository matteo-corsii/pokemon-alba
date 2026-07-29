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
                $value = $packed[$y * $stride + $x]
            }
            else
            {
                $packedByte = $packed[$y * $stride + [int]($x / 2)]
                $value = if (($x % 2) -eq 0) { $packedByte -shr 4 } else { $packedByte -band 15 }
            }
            $indices[$y * $image.PixelWidth + $x] = $value
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
    [Array]::Copy($Image.Indices, $Frame * $Image.Width * $FrameHeight, $frameData, 0, $frameData.Length)
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
            $value = $Image.Indices[(($Frame * $FrameHeight + $y) * $Image.Width) + $x]
            if ($value -ne 0)
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

function Assert-Png($Image, [int]$Width, [int]$Height, [int]$FrameHeight, [string]$Name, [double]$MaximumDifference)
{
    Assert-Condition ($Image.Width -eq $Width -and $Image.Height -eq $Height) "$Name has invalid dimensions"
    Assert-Condition (($Width % 8) -eq 0 -and ($Height % 8) -eq 0) "$Name is not tile-aligned"
    Assert-Condition (($Image.Indices | Measure-Object -Maximum).Maximum -le 15) "$Name uses an index above 15"
    Assert-Condition ($Image.Palette[0].A -eq 0) "$Name palette index 0 is not transparent"

    $frameCount = $Height / $FrameHeight
    for ($frame = 0; $frame -lt $frameCount; $frame++)
    {
        $data = Get-FrameIndices $Image $frame $FrameHeight
        Assert-Condition (($data | Where-Object { $_ -ne 0 }).Count -gt 0) "$Name frame $frame is empty"
    }
    if ($frameCount -eq 2)
    {
        $first = Get-FrameIndices $Image 0 $FrameHeight
        $second = Get-FrameIndices $Image 1 $FrameHeight
        $different = 0
        for ($index = 0; $index -lt $first.Count; $index++)
        {
            if ($first[$index] -ne $second[$index])
            {
                $different++
            }
        }
        Assert-Condition ($different -gt 0) "$Name frames are identical"
        Assert-Condition ($different -lt ($first.Count * $MaximumDifference)) "$Name frame variation is unexpectedly large"
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
        $colors += ,@($parts | ForEach-Object { [int]$_ })
    }
    return $colors
}

$assetRoot = Join-Path $RepositoryRoot 'graphics/pokemon/ardeino'
$front = Read-IndexedPng (Join-Path $assetRoot 'anim_front.png')
$back = Read-IndexedPng (Join-Path $assetRoot 'back.png')
$icon = Read-IndexedPng (Join-Path $assetRoot 'icon.png')

Assert-Png $front 64 128 64 'anim_front.png' 0.4
Assert-Png $back 64 64 64 'back.png' 0.4
Assert-Png $icon 32 64 32 'icon.png' 0.5
Assert-Condition (Test-EqualArrays (Get-BoundingBox $front 0 64) @(7, 3, 48, 58)) 'anim_front.png frame 0 bounding box changed'
Assert-Condition (Test-EqualArrays (Get-BoundingBox $front 1 64) @(5, 3, 54, 58)) 'anim_front.png frame 1 bounding box changed'
Assert-Condition (Test-EqualArrays (Get-BoundingBox $back 0 64) @(17, 3, 33, 57)) 'back.png bounding box changed'
Assert-Condition (Test-EqualArrays (Get-BoundingBox $icon 0 32) @(1, 6, 30, 25)) 'icon.png frame 0 bounding box changed'
Assert-Condition (Test-EqualArrays (Get-BoundingBox $icon 1 32) @(1, 7, 30, 25)) 'icon.png frame 1 bounding box changed'
Assert-Condition (Test-EqualArrays ($icon.Indices | Sort-Object -Unique) @(0, 1, 3, 4, 5, 6, 9, 10, 15)) 'icon.png contains indices outside the documented pal3 remapping'

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
$ardeino = [regex]::Match($species, '(?s)\[SPECIES_ARDEINO\]\s*=\s*\{.*?\n    \},').Value
Assert-Condition ($ardeino.Length -gt 0) 'SPECIES_ARDEINO record was not found'

foreach ($declaration in @(
    'gMonFrontPic_Ardeino[] = INCGFX_U32("graphics/pokemon/ardeino/anim_front.png", ".4bpp.smol")',
    'gMonBackPic_Ardeino[] = INCGFX_U32("graphics/pokemon/ardeino/back.png", ".4bpp.smol")',
    'gMonPalette_Ardeino[] = INCGFX_U16("graphics/pokemon/ardeino/normal.pal", ".gbapal")',
    'gMonShinyPalette_Ardeino[] = INCGFX_U16("graphics/pokemon/ardeino/shiny.pal", ".gbapal")',
    'gMonIcon_Ardeino[] = INCGFX_U8("graphics/pokemon/ardeino/icon.png", ".4bpp")'
))
{
    Assert-Condition ($graphics.Contains($declaration)) "Missing Ardeino graphics declaration: $declaration"
}

foreach ($reference in @(
    '.frontPic = gMonFrontPic_Ardeino', '.backPic = gMonBackPic_Ardeino',
    '.palette = gMonPalette_Ardeino', '.shinyPalette = gMonShinyPalette_Ardeino',
    '.iconSprite = gMonIcon_Ardeino', '.frontPicSize = MON_COORDS_SIZE(56, 64)',
    '.backPicSize = MON_COORDS_SIZE(40, 64)', '.frontPicYOffset = 3',
    '.backPicYOffset = 4', '.iconPalIndex = 3', 'ANIMCMD_FRAME(1, 12)',
    'ANIMCMD_FRAME(0, 8)', '.cryId = CRY_DUCKLETT', 'FOOTPRINT(Ducklett)',
    'sPicTable_Ducklett', 'gOverworldPalette_Ducklett', 'BACK_ANIM_CONCAVE_ARC_SMALL'
))
{
    Assert-Condition ($ardeino.Contains($reference)) "Unexpected SPECIES_ARDEINO graphics state: $reference"
}

foreach ($oldReference in @(
    '.frontPic = gMonFrontPic_Ducklett', '.backPic = gMonBackPic_Ducklett',
    '.palette = gMonPalette_Ducklett', '.shinyPalette = gMonShinyPalette_Ducklett',
    '.iconSprite = gMonIcon_Ducklett'
))
{
    Assert-Condition (-not $ardeino.Contains($oldReference)) "Ardeino still uses a Ducklett battle placeholder: $oldReference"
}

Assert-Condition (([regex]::Matches($ardeino, 'ANIMCMD_FRAME\(')).Count -eq 2) 'Ardeino front animation must use exactly two commands'
Assert-Condition ($starter -match '#define WATER_STARTER\s+\(IS_FRLG \? SPECIES_SQUIRTLE\s+: SPECIES_ARDEINO\s+\)') 'Ardeino is no longer the Emerald Water starter'
Assert-Condition ($species.Contains('.frontPic = gMonFrontPic_Cingerm')) 'Cingerm original front sprite reference changed'
Assert-Condition ($species.Contains('.frontPic = gMonFrontPic_Serbrace')) 'Serbrace original front sprite reference changed'

foreach ($placeholder in @(
    @('ROVASCO', 'OinkologneM'), @('SELVAZANNA', 'Mamoswine'),
    @('VIPERCEN', 'Arbok'), @('TOSSIVAMPA', 'Seviper'),
    @('VELAIRONE', 'Swanna'), @('CODAIRONE', 'Bombirdier')
))
{
    $record = [regex]::Match($species, "(?s)\[SPECIES_$($placeholder[0])\]\s*=\s*\{.*?\n    \},").Value
    Assert-Condition ($record.Contains(".frontPic = gMonFrontPic_$($placeholder[1])")) "$($placeholder[0]) placeholder changed"
}

$wildEncounters = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/wild_encounters.json') -Raw
Assert-Condition (-not $wildEncounters.Contains('SPECIES_ARDEINO')) 'Ardeino was added to wild encounters'

Write-Output 'Ardeino graphics validation passed.'
