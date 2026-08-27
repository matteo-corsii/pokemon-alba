param(
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName PresentationCore

function Assert-Condition([bool]$Condition, [string]$Message) { if (-not $Condition) { throw $Message } }
function Same($A, $B) { if ($A.Count -ne $B.Count) { return $false }; for ($i = 0; $i -lt $A.Count; $i++) { if ($A[$i] -ne $B[$i]) { return $false } }; return $true }
function Read-Png([string]$Path) {
    $stream = [IO.File]::OpenRead($Path)
    try { $decoder = New-Object Windows.Media.Imaging.PngBitmapDecoder($stream, [Windows.Media.Imaging.BitmapCreateOptions]::PreservePixelFormat, [Windows.Media.Imaging.BitmapCacheOption]::OnLoad); $image = $decoder.Frames[0] } finally { $stream.Dispose() }
    Assert-Condition ($image.Format.ToString().StartsWith('Indexed')) "$Path is not indexed"
    Assert-Condition ($image.Format.BitsPerPixel -in 4, 8) "$Path has unsupported indexed depth"
    $stride = [int](($image.PixelWidth * $image.Format.BitsPerPixel + 7) / 8); $packed = New-Object byte[] ($stride * $image.PixelHeight); $image.CopyPixels($packed, $stride, 0); $indices = New-Object byte[] ($image.PixelWidth * $image.PixelHeight)
    for ($y = 0; $y -lt $image.PixelHeight; $y++) { for ($x = 0; $x -lt $image.PixelWidth; $x++) { $value = if ($image.Format.BitsPerPixel -eq 8) { $packed[$y * $stride + $x] } else { $byte = $packed[$y * $stride + [int]($x / 2)]; if (($x % 2) -eq 0) { $byte -shr 4 } else { $byte -band 15 } }; $indices[$y * $image.PixelWidth + $x] = $value } }
    return [pscustomobject]@{ Width=$image.PixelWidth; Height=$image.PixelHeight; Palette=$image.Palette.Colors; Indices=$indices }
}
function Box($Image) {
    $minimumX = $Image.Width; $minimumY = $Image.Height; $maximumX = -1; $maximumY = -1
    for ($y = 0; $y -lt $Image.Height; $y++) { for ($x = 0; $x -lt $Image.Width; $x++) { if ($Image.Indices[$y * $Image.Width + $x] -ne 0) { $minimumX = [Math]::Min($minimumX, $x); $minimumY = [Math]::Min($minimumY, $y); $maximumX = [Math]::Max($maximumX, $x); $maximumY = [Math]::Max($maximumY, $y) } } }
    Assert-Condition ($maximumX -ge 0) 'Empty image'; return @($minimumX, $minimumY, ($maximumX - $minimumX + 1), ($maximumY - $minimumY + 1))
}
function Read-Pal([string]$Path) {
    $lines=Get-Content -LiteralPath $Path; Assert-Condition ($lines.Count -eq 19 -and $lines[0] -eq 'JASC-PAL' -and $lines[1] -eq '0100' -and $lines[2] -eq '16') "$Path is not a 16-color JASC palette"
    $colors = @()
    foreach($line in $lines[3..18]) { $colors += ,@($line -split ' ' | ForEach-Object { [int]$_ }) }
    return ,$colors
}
function Record([string]$Text, [string]$Name) { $r=[regex]::Match($Text,"(?s)\[SPECIES_$Name\]\s*=\s*\{.*?\n    \},").Value; Assert-Condition ($r.Length -gt 0) "Missing SPECIES_$Name"; return $r }
function WithoutGraphics([string]$Record) { return [regex]::Replace($Record.Replace("`r`n","`n"),'(?m)^\s*\.(frontPic|frontPicSize|frontPicYOffset|backPic|backPicSize|backPicYOffset|palette|shinyPalette|iconSprite|iconPalIndex)\s*=.*\n','') }

$entries=@(
    [pscustomobject]@{Name='GAZZUOLA'; Symbol='Gazzuola'; Folder='gazzuola'; Placeholder='Rookidee'; FrontBox=@(14,16,35,38); BackBox=@(17,15,30,34); FrontSize='MON_COORDS_SIZE(40, 40)'; BackSize='MON_COORDS_SIZE(32, 40)'; Offset=12},
    [pscustomobject]@{Name='BRILLAZZA'; Symbol='Brillazza'; Folder='brillazza'; Placeholder='Corvisquire'; FrontBox=@(20,15,33,37); BackBox=@(17,14,31,38); FrontSize='MON_COORDS_SIZE(40, 40)'; BackSize='MON_COORDS_SIZE(32, 40)'; Offset=12},
    [pscustomobject]@{Name='GAZZOMBRA'; Symbol='Gazzombra'; Folder='gazzombra'; Placeholder='Corviknight'; FrontBox=@(8,8,48,53); BackBox=@(11,8,42,53); FrontSize='MON_COORDS_SIZE(48, 56)'; BackSize='MON_COORDS_SIZE(48, 56)'; Offset=4}
)
$graphics=Get-Content (Join-Path $RepositoryRoot 'src/data/graphics/pokemon.h') -Raw
$species=Get-Content (Join-Path $RepositoryRoot 'src/data/pokemon/species_info.h') -Raw -Encoding UTF8
$baseSpecies=& git -C $RepositoryRoot show develop:src/data/pokemon/species_info.h | Out-String
$globalPal=Read-Pal (Join-Path $RepositoryRoot 'graphics/pokemon/icon_palettes/pal6.pal')
Assert-Condition (Same $globalPal[0] @(255,0,255)) 'pal6 index 0 must be magenta'
foreach($i in 0..5) { & git -C $RepositoryRoot diff --quiet develop -- "graphics/pokemon/icon_palettes/pal$i.pal"; Assert-Condition ($LASTEXITCODE -eq 0) "pal$i changed" }
$iconTable=Get-Content (Join-Path $RepositoryRoot 'src/pokemon_icon.c') -Raw
Assert-Condition ($iconTable.Contains('{ gMonIconPalettes[6], POKE_ICON_BASE_PAL_TAG + 6 },')) 'pal6 is absent from gMonIconPaletteTable'
Assert-Condition (([regex]::Matches($iconTable,'gMonIconPalettes\[\d+\]')).Count -eq 7) 'Icon palette table is not append-only with seven entries'
$storage=Get-Content (Join-Path $RepositoryRoot 'src/pokemon_storage_system.c') -Raw
Assert-Condition ($storage.Contains('PALTAG_MON_ICON_6, // Used implicitly in CreateMonIconSprite')) 'Box palette tag 6 is missing'

foreach($entry in $entries) {
    $root=Join-Path $RepositoryRoot "graphics/pokemon/$($entry.Folder)"; $files=@('anim_front.png','back.png','icon.png','normal.pal','shiny.pal'); Assert-Condition (Same @((Get-ChildItem $root -File | Sort-Object Name | ForEach-Object Name)) @($files | Sort-Object)) "$($entry.Name) has unexpected assets"
    $front=Read-Png (Join-Path $root 'anim_front.png'); $back=Read-Png (Join-Path $root 'back.png'); $icon=Read-Png (Join-Path $root 'icon.png')
    foreach($image in @($front,$back,$icon)) { Assert-Condition (($image.Indices | Measure-Object -Maximum).Maximum -le 15) "$($entry.Name) uses index above 15"; Assert-Condition ($image.Palette[0].A -eq 0 -and $image.Palette[0].R -eq 255 -and $image.Palette[0].G -eq 0 -and $image.Palette[0].B -eq 255) "$($entry.Name) has invalid transparent palette entry" }
    Assert-Condition ($front.Width -eq 64 -and $front.Height -eq 64 -and $back.Width -eq 64 -and $back.Height -eq 64 -and $icon.Width -eq 32 -and $icon.Height -eq 64) "$($entry.Name) dimensions differ"
    Assert-Condition (Same (Box $front) $entry.FrontBox) "$($entry.Name) front bounding box differs"; Assert-Condition (Same (Box $back) $entry.BackBox) "$($entry.Name) back bounding box differs"
    $normal=Read-Pal (Join-Path $root 'normal.pal'); $shiny=Read-Pal (Join-Path $root 'shiny.pal'); Assert-Condition ((Same $normal[0] @(255,0,255)) -and (Same $shiny[0] @(255,0,255))) "$($entry.Name) palette entry 0 differs"; Assert-Condition (-not (Same ($normal | ForEach-Object { $_ -join ',' }) ($shiny | ForEach-Object { $_ -join ',' }))) "$($entry.Name) shiny palette equals normal"
    for($i=0;$i-lt 16;$i++){ Assert-Condition (Same @($front.Palette[$i].R,$front.Palette[$i].G,$front.Palette[$i].B) $normal[$i]) "$($entry.Name) front palette differs"; Assert-Condition (Same @($back.Palette[$i].R,$back.Palette[$i].G,$back.Palette[$i].B) $normal[$i]) "$($entry.Name) back palette differs"; Assert-Condition (Same @($icon.Palette[$i].R,$icon.Palette[$i].G,$icon.Palette[$i].B) $globalPal[$i]) "$($entry.Name) icon palette differs from pal6" }
    foreach($line in @("gMonFrontPic_$($entry.Symbol)[] = INCGFX_U32(`"graphics/pokemon/$($entry.Folder)/anim_front.png`", `".4bpp.smol`")","gMonBackPic_$($entry.Symbol)[] = INCGFX_U32(`"graphics/pokemon/$($entry.Folder)/back.png`", `".4bpp.smol`")","gMonPalette_$($entry.Symbol)[] = INCGFX_U16(`"graphics/pokemon/$($entry.Folder)/normal.pal`", `".gbapal`")","gMonShinyPalette_$($entry.Symbol)[] = INCGFX_U16(`"graphics/pokemon/$($entry.Folder)/shiny.pal`", `".gbapal`")","gMonIcon_$($entry.Symbol)[] = INCGFX_U8(`"graphics/pokemon/$($entry.Folder)/icon.png`", `".4bpp`")")){Assert-Condition ($graphics.Contains($line)) "Missing $line"}
    $record=Record $species $entry.Name; $base=Record $baseSpecies $entry.Name
    foreach($line in @(".frontPic = gMonFrontPic_$($entry.Symbol)",".frontPicSize = $($entry.FrontSize)",".frontPicYOffset = $($entry.Offset)",".backPic = gMonBackPic_$($entry.Symbol)",".backPicSize = $($entry.BackSize)",".backPicYOffset = $($entry.Offset)",".palette = gMonPalette_$($entry.Symbol)",".shinyPalette = gMonShinyPalette_$($entry.Symbol)",".iconSprite = gMonIcon_$($entry.Symbol)",'.iconPalIndex = 6',".cryId = CRY_$($entry.Placeholder.ToUpper())","FOOTPRINT($($entry.Placeholder))","sPicTable_$($entry.Placeholder)")){Assert-Condition ($record.Contains($line)) "$($entry.Name) missing $line"}
    Assert-Condition (-not $record.Contains(".frontPic = gMonFrontPic_$($entry.Placeholder)")) "$($entry.Name) keeps placeholder front sprite"; Assert-Condition ((WithoutGraphics $record) -ceq (WithoutGraphics $base)) "$($entry.Name) changed functional data"
}
$allowed=@('graphics/pokemon/icon_palettes/pal6.pal','src/graphics.c','src/pokemon_icon.c','src/pokemon_storage_system.c','src/data/graphics/pokemon.h','src/data/pokemon/species_info.h','src/data/pokemon/all_learnables.json','src/data/pokemon/egg_moves.h','src/data/pokemon/pokedex_orders.h','include/constants/species.h','include/constants/pokedex.h','test/species.c','test/validate_early_ausonia_fauna_batch_b.ps1','test/validate_early_ausonia_fauna_batch_c.ps1','test/validate_early_ausonia_fauna_batch_d.ps1','test/validate_early_ausonia_graphics_batch_a.ps1','test/validate_early_ausonia_graphics_batch_b.ps1','test/validate_early_ausonia_graphics_batch_c.ps1','test/validate_early_ausonia_graphics_batch_d.ps1','test/validate_molospsy.ps1','test/validate_lenghelis.ps1','test/validate_luscinco_luscerp.ps1','test/validate_lumella_omphalux.ps1','test/validate_paludix_sanguilex.ps1','test/validate_albera_storica_secret_base_mentor.ps1','docs/AUSONIA_REGIONAL_DEX_PLAN.md','graphics/pokemon/molospsy/anim_front.png','graphics/pokemon/molospsy/back.png','graphics/pokemon/molospsy/icon.png','graphics/pokemon/molospsy/normal.pal','graphics/pokemon/molospsy/shiny.pal','graphics/pokemon/lenghelis/anim_front.png','graphics/pokemon/lenghelis/back.png','graphics/pokemon/lenghelis/icon.png','graphics/pokemon/lenghelis/normal.pal','graphics/pokemon/lenghelis/shiny.pal','graphics/pokemon/luscinco/anim_front.png','graphics/pokemon/luscinco/back.png','graphics/pokemon/luscinco/icon.png','graphics/pokemon/luscinco/normal.pal','graphics/pokemon/luscinco/shiny.pal','graphics/pokemon/luscerp/anim_front.png','graphics/pokemon/luscerp/back.png','graphics/pokemon/luscerp/icon.png','graphics/pokemon/luscerp/normal.pal','graphics/pokemon/luscerp/shiny.pal','graphics/pokemon/lumella/anim_front.png','graphics/pokemon/lumella/back.png','graphics/pokemon/lumella/icon.png','graphics/pokemon/lumella/normal.pal','graphics/pokemon/lumella/shiny.pal','graphics/pokemon/omphalux/anim_front.png','graphics/pokemon/omphalux/back.png','graphics/pokemon/omphalux/icon.png','graphics/pokemon/omphalux/normal.pal','graphics/pokemon/omphalux/shiny.pal','graphics/pokemon/paludix/anim_front.png','graphics/pokemon/paludix/back.png','graphics/pokemon/paludix/icon.png','graphics/pokemon/paludix/normal.pal','graphics/pokemon/paludix/shiny.pal','graphics/pokemon/sanguilex/anim_front.png','graphics/pokemon/sanguilex/back.png','graphics/pokemon/sanguilex/icon.png','graphics/pokemon/sanguilex/normal.pal','graphics/pokemon/sanguilex/shiny.pal')
foreach($entry in $entries){foreach($file in @('anim_front.png','back.png','icon.png','normal.pal','shiny.pal')){$allowed += "graphics/pokemon/$($entry.Folder)/$file"}}
$allowed += @('graphics/pokemon/tritino/anim_front.png','graphics/pokemon/tritino/back.png','graphics/pokemon/tritino/icon.png','graphics/pokemon/tritino/normal.pal','graphics/pokemon/tritino/shiny.pal','graphics/pokemon/tricrest/anim_front.png','graphics/pokemon/tricrest/back.png','graphics/pokemon/tricrest/icon.png','graphics/pokemon/tricrest/normal.pal','graphics/pokemon/tricrest/shiny.pal','graphics/pokemon/salampolla/anim_front.png','graphics/pokemon/salampolla/back.png','graphics/pokemon/salampolla/icon.png','graphics/pokemon/salampolla/normal.pal','graphics/pokemon/salampolla/shiny.pal','graphics/pokemon/alchimandra/anim_front.png','graphics/pokemon/alchimandra/back.png','graphics/pokemon/alchimandra/icon.png','graphics/pokemon/alchimandra/normal.pal','graphics/pokemon/alchimandra/shiny.pal')
$allowed += @('test/validate_tritino_tricrest.ps1','test/validate_salampolla_alchimandra.ps1','test/validate_cingerm_graphics.ps1','test/validate_albera_amphitheatre_gym.ps1','test/validate_albera_bassa_condominiums.ps1','test/validate_albera_bassa_residential_blockout.ps1','test/validate_albera_bassa_school.ps1','test/validate_via_verdi_first_investigation.ps1','test/validate_lago_di_albera_tileset.ps1')
$allowed += @('graphics/pokemon/cisternide/anim_front.png','graphics/pokemon/cisternide/back.png','graphics/pokemon/cisternide/icon.png','graphics/pokemon/cisternide/normal.pal','graphics/pokemon/cisternide/shiny.pal','graphics/pokemon/calcistern/anim_front.png','graphics/pokemon/calcistern/back.png','graphics/pokemon/calcistern/icon.png','graphics/pokemon/calcistern/normal.pal','graphics/pokemon/calcistern/shiny.pal','test/validate_cisternide_calcistern.ps1')
$allowed += @('test/validate_albera_first_playable_segment.ps1','test/validate_cingerm_starter.ps1','test/validate_full_ausonia_starter_trio.ps1','test/validate_italian_menu_localization.ps1','test/validate_porta_pretoria_localization.ps1')
$paths=@(& git -C $RepositoryRoot diff --name-only develop...HEAD; & git -C $RepositoryRoot diff --name-only; & git -C $RepositoryRoot ls-files --others --exclude-standard) | Where-Object {$_} | Sort-Object -Unique
foreach($path in $paths){Assert-Condition ($path -in $allowed) "Unexpected changed file: $path"; Assert-Condition ($path -notmatch '\.(gba|elf|map|bin|4bpp|gbapal|smol|zip)$' -and $path -notlike 'build/*') "Generated artifact detected: $path"}
& git -C $RepositoryRoot diff --quiet develop -- include/global.h src/pokedex.c src/new_game.c src/overworld.c test/save.c
Assert-Condition ($LASTEXITCODE -eq 0) 'Save runtime changed'
Write-Host 'Graphics Batch D validation passed.' -ForegroundColor Green
