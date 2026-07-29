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
        BitsPerPixel = $bitsPerPixel
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

function Assert-Png($Image, [int]$Width, [int]$Height, [int]$FrameHeight, [string]$Name)
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
        Assert-Condition (-not (Test-EqualArrays $first $second)) "$Name frames are identical"
    }

    foreach ($index in ($Image.Indices | Sort-Object -Unique | Where-Object { $_ -ne 0 }))
    {
        $color = $Image.Palette[$index]
        Assert-Condition (-not ($color.R -eq 255 -and $color.G -eq 0 -and $color.B -eq 255)) "$Name exposes magenta at index $index"
    }
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
        Assert-Condition (Test-EqualArrays $colors[0] @(255, 0, 255)) "$Path index 0 is not the transparent color"
    }
    return $colors
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

$speciesData = @(
    [pscustomobject]@{ Name='ROVASCO'; Folder='rovasco'; Placeholder='OinkologneM'; Palette=5; Front0=@(2,7,60,53); Front1=@(2,6,60,54); Back=@(9,4,47,56); Icon0=@(1,4,29,27); Icon1=@(1,5,29,27); FrontSize='MON_COORDS_SIZE(64, 56)'; BackSize='MON_COORDS_SIZE(48, 56)'; FrontOffset=4; BackOffset=4; Cry='CRY_OINKOLOGNE_M'; Footprint='Oinkologne' },
    [pscustomobject]@{ Name='SELVAZANNA'; Folder='selvazanna'; Placeholder='Mamoswine'; Palette=5; Front0=@(7,7,48,57); Front1=@(7,7,48,57); Back=@(7,8,48,56); Icon0=@(1,5,29,21); Icon1=@(1,4,29,24); FrontSize='MON_COORDS_SIZE(48, 64)'; BackSize='MON_COORDS_SIZE(48, 56)'; FrontOffset=0; BackOffset=0; Cry='CRY_MAMOSWINE'; Footprint='Mamoswine' },
    [pscustomobject]@{ Name='VIPERCEN'; Folder='vipercen'; Placeholder='Arbok'; Palette=3; Front0=@(11,5,42,53); Front1=@(14,5,37,51); Back=@(11,5,42,52); Icon0=@(6,4,20,24); Icon1=@(6,1,19,21); FrontSize='MON_COORDS_SIZE(48, 56)'; BackSize='MON_COORDS_SIZE(48, 56)'; FrontOffset=6; BackOffset=7; Cry='CRY_ARBOK'; Footprint='Arbok' },
    [pscustomobject]@{ Name='TOSSIVAMPA'; Folder='tossivampa'; Placeholder='Seviper'; Palette=3; Front0=@(3,2,57,60); Front1=@(4,2,55,60); Back=@(6,2,51,60); Icon0=@(1,2,30,27); Icon1=@(1,1,30,30); FrontSize='MON_COORDS_SIZE(64, 64)'; BackSize='MON_COORDS_SIZE(56, 64)'; FrontOffset=2; BackOffset=2; Cry='CRY_SEVIPER'; Footprint='Seviper' },
    [pscustomobject]@{ Name='VELAIRONE'; Folder='velairone'; Placeholder='Swanna'; Palette=3; Front0=@(7,3,48,58); Front1=@(7,3,49,58); Back=@(17,3,29,58); Icon0=@(1,6,30,25); Icon1=@(1,7,30,25); FrontSize='MON_COORDS_SIZE(56, 64)'; BackSize='MON_COORDS_SIZE(32, 64)'; FrontOffset=3; BackOffset=3; Cry='CRY_SWANNA'; Footprint='Swanna' },
    [pscustomobject]@{ Name='CODAIRONE'; Folder='codairone'; Placeholder='Bombirdier'; Palette=3; Front0=@(4,3,55,58); Front1=@(5,3,53,58); Back=@(10,4,44,57); Icon0=@(3,2,25,29); Icon1=@(4,2,24,29); FrontSize='MON_COORDS_SIZE(56, 64)'; BackSize='MON_COORDS_SIZE(48, 64)'; FrontOffset=3; BackOffset=3; Cry='CRY_BOMBIRDIER'; Footprint='Bombirdier' }
)

$graphics = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/graphics/pokemon.h') -Raw -Encoding UTF8
$species = Get-Content -LiteralPath (Join-Path $RepositoryRoot 'src/data/pokemon/species_info.h') -Raw -Encoding UTF8
$baseSpecies = (& git -C $RepositoryRoot show develop:src/data/pokemon/species_info.h | Out-String)
Assert-Condition ($LASTEXITCODE -eq 0) 'Could not read develop version of species_info.h'

foreach ($entry in $speciesData)
{
    $assetRoot = Join-Path $RepositoryRoot "graphics/pokemon/$($entry.Folder)"
    $expectedFiles = @('anim_front.png', 'back.png', 'icon.png', 'normal.pal', 'shiny.pal')
    $actualFiles = @(Get-ChildItem -LiteralPath $assetRoot -File | Sort-Object Name | ForEach-Object { $_.Name })
    Assert-Condition (Test-EqualArrays $actualFiles ($expectedFiles | Sort-Object)) "$($entry.Name) asset directory contains unexpected files"

    $front = Read-IndexedPng (Join-Path $assetRoot 'anim_front.png')
    $back = Read-IndexedPng (Join-Path $assetRoot 'back.png')
    $icon = Read-IndexedPng (Join-Path $assetRoot 'icon.png')
    Assert-Png $front 64 128 64 "$($entry.Name) anim_front.png"
    Assert-Png $back 64 64 64 "$($entry.Name) back.png"
    Assert-Png $icon 32 64 32 "$($entry.Name) icon.png"
    Assert-Condition (Test-EqualArrays (Get-BoundingBox $front 0 64) $entry.Front0) "$($entry.Name) front frame 0 bounding box changed"
    Assert-Condition (Test-EqualArrays (Get-BoundingBox $front 1 64) $entry.Front1) "$($entry.Name) front frame 1 bounding box changed"
    Assert-Condition (Test-EqualArrays (Get-BoundingBox $back 0 64) $entry.Back) "$($entry.Name) back bounding box changed"
    Assert-Condition (Test-EqualArrays (Get-BoundingBox $icon 0 32) $entry.Icon0) "$($entry.Name) icon frame 0 bounding box changed"
    Assert-Condition (Test-EqualArrays (Get-BoundingBox $icon 1 32) $entry.Icon1) "$($entry.Name) icon frame 1 bounding box changed"

    Read-JascPalette (Join-Path $assetRoot 'normal.pal') | Out-Null
    Read-JascPalette (Join-Path $assetRoot 'shiny.pal') | Out-Null
    $globalIconPalette = Read-JascPalette (Join-Path $RepositoryRoot "graphics/pokemon/icon_palettes/pal$($entry.Palette).pal") $false
    for ($index = 0; $index -lt 16; $index++)
    {
        $actual = @($icon.Palette[$index].R, $icon.Palette[$index].G, $icon.Palette[$index].B)
        Assert-Condition (Test-EqualArrays $actual $globalIconPalette[$index]) "$($entry.Name) icon palette index $index does not match pal$($entry.Palette)"
    }

    $symbol = $entry.Name.Substring(0, 1) + $entry.Name.Substring(1).ToLowerInvariant()
    foreach ($declaration in @(
        "gMonFrontPic_$symbol[] = INCGFX_U32(`"graphics/pokemon/$($entry.Folder)/anim_front.png`", `".4bpp.smol`")",
        "gMonBackPic_$symbol[] = INCGFX_U32(`"graphics/pokemon/$($entry.Folder)/back.png`", `".4bpp.smol`")",
        "gMonPalette_$symbol[] = INCGFX_U16(`"graphics/pokemon/$($entry.Folder)/normal.pal`", `".gbapal`")",
        "gMonShinyPalette_$symbol[] = INCGFX_U16(`"graphics/pokemon/$($entry.Folder)/shiny.pal`", `".gbapal`")",
        "gMonIcon_$symbol[] = INCGFX_U8(`"graphics/pokemon/$($entry.Folder)/icon.png`", `".4bpp`")"
    ))
    {
        Assert-Condition ($graphics.Contains($declaration)) "Missing graphics declaration: $declaration"
    }

    $record = Get-SpeciesRecord $species $entry.Name
    $baseRecord = Get-SpeciesRecord $baseSpecies $entry.Name
    foreach ($reference in @(
        ".frontPic = gMonFrontPic_$symbol", ".backPic = gMonBackPic_$symbol",
        ".palette = gMonPalette_$symbol", ".shinyPalette = gMonShinyPalette_$symbol",
        ".iconSprite = gMonIcon_$symbol", ".frontPicSize = $($entry.FrontSize)",
        ".backPicSize = $($entry.BackSize)", ".frontPicYOffset = $($entry.FrontOffset)",
        ".backPicYOffset = $($entry.BackOffset)", ".iconPalIndex = $($entry.Palette)",
        'ANIMCMD_FRAME(1, 12)', 'ANIMCMD_FRAME(0, 8)', ".cryId = $($entry.Cry)",
        "FOOTPRINT($($entry.Footprint))", "sPicTable_$($entry.Footprint)",
        "gOverworldPalette_$($entry.Footprint)", "gShinyOverworldPalette_$($entry.Footprint)"
    ))
    {
        Assert-Condition ($record.Contains($reference)) "$($entry.Name) has an unexpected graphics or provisional reference: $reference"
    }
    foreach ($oldReference in @(
        ".frontPic = gMonFrontPic_$($entry.Placeholder)", ".backPic = gMonBackPic_$($entry.Placeholder)",
        ".palette = gMonPalette_$($entry.Placeholder)", ".shinyPalette = gMonShinyPalette_$($entry.Placeholder)",
        ".iconSprite = gMonIcon_$($entry.Placeholder)"
    ))
    {
        Assert-Condition (-not $record.Contains($oldReference)) "$($entry.Name) still uses placeholder graphics: $oldReference"
    }
    Assert-Condition (([regex]::Matches($record, 'ANIMCMD_FRAME\(')).Count -eq 2) "$($entry.Name) front animation must contain exactly two commands"
    Assert-Condition ((Remove-AllowedGraphicsFields $record) -ceq (Remove-AllowedGraphicsFields $baseRecord)) "$($entry.Name) changed outside the allowed graphics fields"
}

foreach ($reference in @(
    '.frontPic = gMonFrontPic_Cingerm', '.frontPic = gMonFrontPic_Serbrace', '.frontPic = gMonFrontPic_Ardeino',
    'EVOLUTION({EVO_LEVEL, 16, SPECIES_ROVASCO})', 'EVOLUTION({EVO_LEVEL, 36, SPECIES_SELVAZANNA})',
    'EVOLUTION({EVO_LEVEL, 16, SPECIES_VIPERCEN})', 'EVOLUTION({EVO_LEVEL, 36, SPECIES_TOSSIVAMPA})',
    'EVOLUTION({EVO_LEVEL, 16, SPECIES_VELAIRONE})', 'EVOLUTION({EVO_LEVEL, 36, SPECIES_CODAIRONE})'
))
{
    Assert-Condition ($species.Contains($reference)) "Approved starter or evolution state changed: $reference"
}

foreach ($path in @(
    'graphics/pokemon/cingerm', 'graphics/pokemon/serbrace', 'graphics/pokemon/ardeino',
    'src/starter_choose.c', 'src/data/trainers.party', 'src/data/pokemon/level_up_learnsets.h',
    'src/data/moves_info.h', 'data/maps', 'data/scripts', '.github/workflows', 'Makefile'
))
{
    & git -C $RepositoryRoot diff --quiet develop -- $path
    Assert-Condition ($LASTEXITCODE -eq 0) "$path changed during the evolution graphics milestone"
}

& (Join-Path $RepositoryRoot 'test/validate_ausonia_starter_move_localization.ps1') -RepositoryRoot $RepositoryRoot | Out-Null
Write-Output 'Ausonia starter evolution graphics validation passed.'
