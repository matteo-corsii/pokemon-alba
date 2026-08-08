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

function Assert-Png($Image, [int]$Width, [int]$Height, [string]$Name)
{
    Assert-Condition ($Image.Width -eq $Width -and $Image.Height -eq $Height) "$Name has invalid dimensions"
    Assert-Condition (($Width % 8) -eq 0 -and ($Height % 8) -eq 0) "$Name is not tile-aligned"
    Assert-Condition (($Image.Indices | Measure-Object -Maximum).Maximum -le 15) "$Name uses an index above 15"
    Assert-Condition ($Image.Palette[0].A -eq 0) "$Name palette index 0 is not transparent"
    Assert-Condition (
        $Image.Palette[0].R -eq 255 -and $Image.Palette[0].G -eq 0 -and $Image.Palette[0].B -eq 255
    ) "$Name palette index 0 is not magenta"
    Assert-Condition (($Image.Indices | Where-Object { $_ -ne 0 }).Count -gt 0) "$Name is empty"
}

function Get-BoundingBox($Image)
{
    $minimumX = $Image.Width
    $minimumY = $Image.Height
    $maximumX = -1
    $maximumY = -1
    for ($y = 0; $y -lt $Image.Height; $y++)
    {
        for ($x = 0; $x -lt $Image.Width; $x++)
        {
            if ($Image.Indices[$y * $Image.Width + $x] -ne 0)
            {
                $minimumX = [Math]::Min($minimumX, $x)
                $minimumY = [Math]::Min($minimumY, $y)
                $maximumX = [Math]::Max($maximumX, $x)
                $maximumY = [Math]::Max($maximumY, $y)
            }
        }
    }
    Assert-Condition ($maximumX -ge 0) 'Image contains no visible pixels'
    return @($minimumX, $minimumY, ($maximumX - $minimumX + 1), ($maximumY - $minimumY + 1))
}

function Read-JascPalette([string]$Path, [bool]$RequireMagenta = $true)
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
        $color = @($parts | ForEach-Object { [int]$_ })
        foreach ($channel in $color)
        {
            Assert-Condition ($channel -ge 0 -and $channel -le 255) "$Path contains an invalid RGB value"
        }
        $colors += ,$color
    }
    if ($RequireMagenta)
    {
        Assert-Condition (Test-EqualArrays $colors[0] @(255, 0, 255)) "$Path index 0 is not magenta"
    }
    return $colors
}

function Assert-EmbeddedPalette($Image, $Palette, [string]$Name)
{
    for ($index = 0; $index -lt 16; $index++)
    {
        $actual = @($Image.Palette[$index].R, $Image.Palette[$index].G, $Image.Palette[$index].B)
        Assert-Condition (Test-EqualArrays $actual $Palette[$index]) "$Name palette differs at index $index"
    }
}

function Assert-IconPalette($Image, $Palette, [string]$Name)
{
    for ($index = 1; $index -lt 16; $index++)
    {
        $actual = @($Image.Palette[$index].R, $Image.Palette[$index].G, $Image.Palette[$index].B)
        Assert-Condition (Test-EqualArrays $actual $Palette[$index]) "$Name is incompatible with its global icon palette at index $index"
    }
}

function Get-SpeciesRecord([string]$Text, [string]$Species)
{
    $record = [regex]::Match($Text, "(?s)\[SPECIES_$Species\]\s*=\s*\{.*?\n    \},").Value
    Assert-Condition ($record.Length -gt 0) "SPECIES_$Species record was not found"
    return $record
}

function Remove-AllowedGraphicsFields([string]$Record)
{
    $normalized = $Record.Replace("`r`n", "`n")
    $normalized = [regex]::Replace(
        $normalized,
        '(?m)^\s*\.(frontPic|frontPicSize|frontPicYOffset|backPic|backPicSize|backPicYOffset|palette|shinyPalette|iconSprite|iconPalIndex)\s*=.*\n',
        ''
    )
    $normalized = [regex]::Replace(
        $normalized,
        '(?ms)^\s*\.frontAnimFrames\s*=\s*(?:ANIM_FRAMES\(.*?^\s*\),|[^,\n]+,)\n',
        ''
    )
    return $normalized
}

$entries = @(
    [pscustomobject]@{ Name='FOLIARVA'; Symbol='Foliarva'; Folder='foliarva'; Placeholder='Caterpie'; IconPalette=1; FrontBox=@(8,12,47,48); BackBox=@(17,12,33,48); FrontSize='MON_COORDS_SIZE(48, 48)'; BackSize='MON_COORDS_SIZE(40, 48)'; FrontOffset=8; BackOffset=8 },
    [pscustomobject]@{ Name='CRISALVIA'; Symbol='Crisalvia'; Folder='crisalvia'; Placeholder='Metapod'; IconPalette=1; FrontBox=@(12,3,39,58); BackBox=@(13,3,37,58); FrontSize='MON_COORDS_SIZE(40, 64)'; BackSize='MON_COORDS_SIZE(40, 64)'; FrontOffset=3; BackOffset=3 },
    [pscustomobject]@{ Name='INFIORALA'; Symbol='Infiorala'; Folder='infiorala'; Placeholder='Butterfree'; IconPalette=1; FrontBox=@(1,6,62,55); BackBox=@(1,5,62,56); FrontSize='MON_COORDS_SIZE(64, 56)'; BackSize='MON_COORDS_SIZE(64, 56)'; FrontOffset=4; BackOffset=4 }
)

$graphics = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/graphics/pokemon.h') -Raw -Encoding UTF8
$species = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/pokemon/species_info.h') -Raw -Encoding UTF8
$baseSpecies = (& git -C $RepositoryRoot show develop:src/data/pokemon/species_info.h | Out-String)
Assert-Condition ($LASTEXITCODE -eq 0) 'Could not read develop version of species_info.h'

foreach ($entry in $entries)
{
    $assetRoot = Join-Path $RepositoryRoot "graphics/pokemon/$($entry.Folder)"
    $expectedFiles = @('anim_front.png', 'back.png', 'icon.png', 'normal.pal', 'shiny.pal')
    $actualFiles = @(Get-ChildItem -LiteralPath $assetRoot -File | Sort-Object Name | ForEach-Object { $_.Name })
    Assert-Condition (Test-EqualArrays $actualFiles ($expectedFiles | Sort-Object)) "$($entry.Name) asset directory contains unexpected files"

    $front = Read-IndexedPng (Join-Path $assetRoot 'anim_front.png')
    $back = Read-IndexedPng (Join-Path $assetRoot 'back.png')
    $icon = Read-IndexedPng (Join-Path $assetRoot 'icon.png')
    Assert-Png $front 64 64 "$($entry.Name) anim_front.png"
    Assert-Png $back 64 64 "$($entry.Name) back.png"
    Assert-Png $icon 32 64 "$($entry.Name) icon.png"
    Assert-Condition (Test-EqualArrays (Get-BoundingBox $front) $entry.FrontBox) "$($entry.Name) front bounding box changed"
    Assert-Condition (Test-EqualArrays (Get-BoundingBox $back) $entry.BackBox) "$($entry.Name) back bounding box changed"

    $normal = Read-JascPalette (Join-Path $assetRoot 'normal.pal')
    $shiny = Read-JascPalette (Join-Path $assetRoot 'shiny.pal')
    Assert-Condition (-not (Test-EqualArrays ($normal | ForEach-Object { $_ -join ',' }) ($shiny | ForEach-Object { $_ -join ',' }))) "$($entry.Name) shiny palette is identical to normal"
    Assert-EmbeddedPalette $front $normal "$($entry.Name) anim_front.png"
    Assert-EmbeddedPalette $back $normal "$($entry.Name) back.png"

    $globalIconPalette = Read-JascPalette (Join-Path $RepositoryRoot "graphics/pokemon/icon_palettes/pal$($entry.IconPalette).pal") $false
    Assert-IconPalette $icon $globalIconPalette "$($entry.Name) icon.png"

    foreach ($declaration in @(
        "gMonFrontPic_$($entry.Symbol)[] = INCGFX_U32(`"graphics/pokemon/$($entry.Folder)/anim_front.png`", `".4bpp.smol`")",
        "gMonBackPic_$($entry.Symbol)[] = INCGFX_U32(`"graphics/pokemon/$($entry.Folder)/back.png`", `".4bpp.smol`")",
        "gMonPalette_$($entry.Symbol)[] = INCGFX_U16(`"graphics/pokemon/$($entry.Folder)/normal.pal`", `".gbapal`")",
        "gMonShinyPalette_$($entry.Symbol)[] = INCGFX_U16(`"graphics/pokemon/$($entry.Folder)/shiny.pal`", `".gbapal`")",
        "gMonIcon_$($entry.Symbol)[] = INCGFX_U8(`"graphics/pokemon/$($entry.Folder)/icon.png`", `".4bpp`")"
    ))
    {
        Assert-Condition ($graphics.Contains($declaration)) "Missing graphics declaration: $declaration"
    }

    $record = Get-SpeciesRecord $species $entry.Name
    $baseRecord = Get-SpeciesRecord $baseSpecies $entry.Name
    foreach ($reference in @(
        ".frontPic = gMonFrontPic_$($entry.Symbol)", ".frontPicSize = $($entry.FrontSize)",
        ".frontPicYOffset = $($entry.FrontOffset)", '.frontAnimFrames = sAnims_SingleFramePlaceHolder',
        ".backPic = gMonBackPic_$($entry.Symbol)", ".backPicSize = $($entry.BackSize)",
        ".backPicYOffset = $($entry.BackOffset)", ".palette = gMonPalette_$($entry.Symbol)",
        ".shinyPalette = gMonShinyPalette_$($entry.Symbol)", ".iconSprite = gMonIcon_$($entry.Symbol)",
        ".iconPalIndex = $($entry.IconPalette)"
    ))
    {
        Assert-Condition ($record.Contains($reference)) "$($entry.Name) has an unexpected graphics reference: $reference"
    }
    foreach ($oldReference in @(
        ".frontPic = gMonFrontPic_$($entry.Placeholder)", ".backPic = gMonBackPic_$($entry.Placeholder)",
        ".palette = gMonPalette_$($entry.Placeholder)", ".shinyPalette = gMonShinyPalette_$($entry.Placeholder)",
        ".iconSprite = gMonIcon_$($entry.Placeholder)"
    ))
    {
        Assert-Condition (-not $record.Contains($oldReference)) "$($entry.Name) still uses placeholder graphics: $oldReference"
    }
    Assert-Condition ((Remove-AllowedGraphicsFields $record) -ceq (Remove-AllowedGraphicsFields $baseRecord)) "$($entry.Name) changed outside the allowed graphics fields"
}

$allowedPaths = @(
    'src/data/graphics/pokemon.h',
    'src/data/pokemon/species_info.h',
    'test/species.c',
    'test/validate_early_ausonia_fauna_batch_b.ps1',
    'test/validate_early_ausonia_graphics_batch_b.ps1'
)
foreach ($entry in $entries)
{
    foreach ($file in 'anim_front.png', 'back.png', 'icon.png', 'normal.pal', 'shiny.pal')
    {
        $allowedPaths += "graphics/pokemon/$($entry.Folder)/$file"
    }
}
$changedPaths = @(
    & git -C $RepositoryRoot diff --name-only develop...HEAD
    & git -C $RepositoryRoot diff --name-only
    & git -C $RepositoryRoot ls-files --others --exclude-standard
) | Where-Object { $_ } | Sort-Object -Unique
foreach ($path in $changedPaths)
{
    Assert-Condition ($path -in $allowedPaths) "Unexpected changed file: $path"
}

Write-Output 'Early Ausonia graphics batch B validation passed.'
