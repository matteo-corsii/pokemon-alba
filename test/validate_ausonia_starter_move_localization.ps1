param(
    [string]$RepositoryRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'

function Assert-Condition([bool]$Condition, [string]$Message)
{
    if (-not $Condition)
    {
        throw $Message
    }
}

function Get-MoveRecord([string]$Content, [string]$Move)
{
    $pattern = '(?ms)^    \[' + [regex]::Escape($Move) + '\] =\r?\n    \{.*?^    \},'
    return [regex]::Match($Content, $pattern).Value
}

function Get-LocalizedBounds([string[]]$Lines)
{
    $start = -1
    $description = -1
    $end = -1
    $conditional = $false
    $depth = 0
    for ($index = 0; $index -lt $Lines.Count; $index++)
    {
        if ($start -lt 0 -and $Lines[$index] -match '^        \.name =')
        {
            $start = $index
        }
        if ($start -ge 0 -and $description -lt 0 -and $Lines[$index] -match '^        \.description =')
        {
            $description = $index
            continue
        }
        if ($description -ge 0)
        {
            if ($Lines[$index] -match '^\s*#if\b')
            {
                $conditional = $true
                $depth++
            }
            if ($Lines[$index] -match '^\s*#endif\b')
            {
                $depth--
                if ($conditional -and $depth -eq 0)
                {
                    $end = $index
                    break
                }
            }
            elseif (-not $conditional -and $Lines[$index].TrimEnd().EndsWith('),'))
            {
                $end = $index
                break
            }
        }
    }
    Assert-Condition ($start -ge 0 -and $description -ge 0 -and $end -ge 0) 'Could not isolate move name and description'
    return @($start, $description, $end)
}

function Remove-LocalizedFields([string]$Record)
{
    $lines = $Record -split '\r?\n'
    $bounds = Get-LocalizedBounds $lines
    $kept = @()
    if ($bounds[0] -gt 0)
    {
        $kept += $lines[0..($bounds[0] - 1)]
    }
    if ($bounds[2] + 1 -lt $lines.Count)
    {
        $kept += $lines[($bounds[2] + 1)..($lines.Count - 1)]
    }
    return ($kept -join "`n")
}

function Parse-Map([string]$Text)
{
    $result = [ordered]@{}
    foreach ($line in ($Text.Trim() -split "`r?`n"))
    {
        $parts = $line -split '=', 2
        $result[$parts[0]] = $parts[1]
    }
    return $result
}

$expectedNames = Parse-Map @'
MOVE_AGILITY=Agilita
MOVE_AIR_SLASH=Eterelama
MOVE_AQUA_RING=Acquanello
MOVE_ASSURANCE=Garanzia
MOVE_BITE=Morso
MOVE_BRINE=Acquadisale
MOVE_COIL=Arrotola
MOVE_CRUNCH=Sgranocchio
MOVE_DEFENSE_CURL=Ricciolscudo
MOVE_EMBER=BRACIERE
MOVE_FIRE_SPIN=Turbofuoco
MOVE_FLAME_CHARGE=Nitrocarica
MOVE_FLAMETHROWER=Lanciafiamme
MOVE_GROWL=RUGGITO
MOVE_HEAT_WAVE=Ondacalda
MOVE_HIGH_HORSEPOWER=Forza Equina
MOVE_HURRICANE=Tifone
MOVE_HYDRO_PUMP=Idropompa
MOVE_INCINERATE=Bruciatutto
MOVE_LEAFAGE=FOGLIAME
MOVE_LEER=FULMISGUARDO
MOVE_MIST=Nebbia
MOVE_MUD_SLAP=Fangosberla
MOVE_NASTY_PLOT=Congiura
MOVE_PECK=Beccata
MOVE_POISON_STING=Velenospina
MOVE_POUND=BOTTA
MOVE_QUICK_ATTACK=ATTACCO RAPIDO
MOVE_RAZOR_LEAF=Foglielama
MOVE_ROLLOUT=Rotolamento
MOVE_ROOST=Trespolo
MOVE_SCRATCH=GRAFFIO
MOVE_SEED_BOMB=Semebomba
MOVE_SLUDGE_BOMB=Fangobomba
MOVE_SMOKESCREEN=Muro di Fumo
MOVE_SUCKER_PUNCH=Sbigoattacco
MOVE_SUPERSONIC=Supersuono
MOVE_TACKLE=AZIONE
MOVE_TAILWIND=Ventoincoda
MOVE_TAKE_DOWN=Riduttore
MOVE_TOXIC=Tossina
MOVE_TRAILBLAZE=Apripista
MOVE_VENOSHOCK=Velenoshock
MOVE_WATER_GUN=PISTOLACQUA
MOVE_WING_ATTACK=Attacco d'Ala
MOVE_WOOD_HAMMER=Mazzuolegno
'@
$expectedNames['MOVE_AGILITY'] = 'Agilit' + [char]0x00E0

$grassLearnset = @(
    '1:MOVE_TACKLE', '1:MOVE_LEER', '4:MOVE_LEAFAGE', '7:MOVE_MUD_SLAP',
    '9:MOVE_BITE', '12:MOVE_DEFENSE_CURL', '15:MOVE_ROLLOUT', '18:MOVE_RAZOR_LEAF',
    '22:MOVE_TAKE_DOWN', '26:MOVE_TRAILBLAZE', '30:MOVE_ASSURANCE', '34:MOVE_SEED_BOMB',
    '38:MOVE_CRUNCH', '43:MOVE_HIGH_HORSEPOWER', '48:MOVE_WOOD_HAMMER', '54:MOVE_SUCKER_PUNCH'
)
$fireLearnset = @(
    '1:MOVE_SCRATCH', '1:MOVE_LEER', '4:MOVE_EMBER', '7:MOVE_SMOKESCREEN',
    '9:MOVE_FLAME_CHARGE', '12:MOVE_POISON_STING', '15:MOVE_BITE', '18:MOVE_INCINERATE',
    '22:MOVE_COIL', '26:MOVE_VENOSHOCK', '30:MOVE_FIRE_SPIN', '34:MOVE_NASTY_PLOT',
    '38:MOVE_FLAMETHROWER', '43:MOVE_TOXIC', '48:MOVE_SLUDGE_BOMB', '54:MOVE_HEAT_WAVE'
)
$waterLearnset = @(
    '1:MOVE_POUND', '1:MOVE_GROWL', '4:MOVE_WATER_GUN', '7:MOVE_PECK',
    '9:MOVE_QUICK_ATTACK', '12:MOVE_MIST', '15:MOVE_SUPERSONIC', '18:MOVE_WING_ATTACK',
    '22:MOVE_AQUA_RING', '26:MOVE_AIR_SLASH', '30:MOVE_BRINE', '34:MOVE_AGILITY',
    '38:MOVE_TAILWIND', '43:MOVE_ROOST', '48:MOVE_HYDRO_PUMP', '54:MOVE_HURRICANE'
)
$expectedLearnsets = [ordered]@{
    Cingerm = $grassLearnset; Rovasco = $grassLearnset; Selvazanna = $grassLearnset
    Serbrace = $fireLearnset; Vipercen = $fireLearnset; Tossivampa = $fireLearnset
    Ardeino = $waterLearnset; Velairone = $waterLearnset; Codairone = $waterLearnset
}

$speciesPath = Join-Path $RepositoryRoot 'src/data/pokemon/species_info.h'
$movesPath = Join-Path $RepositoryRoot 'src/data/moves_info.h'
$species = Get-Content -LiteralPath $speciesPath -Raw -Encoding UTF8
$moves = Get-Content -LiteralPath $movesPath -Raw -Encoding UTF8
$actualMoves = New-Object 'Collections.Generic.HashSet[string]'

foreach ($speciesName in $expectedLearnsets.Keys)
{
    $match = [regex]::Match($species, "(?s)static const struct LevelUpMove s${speciesName}LevelUpLearnset\[\] = \{(.*?)LEVEL_UP_END")
    Assert-Condition $match.Success "Missing level-up learnset for $speciesName"
    $actual = @([regex]::Matches($match.Groups[1].Value, 'LEVEL_UP_MOVE\(\s*(\d+),\s*(MOVE_[A-Z0-9_]+)\)') | ForEach-Object {
        [void]$actualMoves.Add($_.Groups[2].Value)
        "$([int]$_.Groups[1].Value):$($_.Groups[2].Value)"
    })
    Assert-Condition (($actual -join '|') -ceq ($expectedLearnsets[$speciesName] -join '|')) "$speciesName level-up learnset changed"
}

Assert-Condition ($actualMoves.Count -eq 46) "Expected 46 unique level-up moves, found $($actualMoves.Count)"
foreach ($move in $expectedNames.Keys)
{
    Assert-Condition ($actualMoves.Contains($move)) "$move is not present in the nine real learnsets"
    $record = Get-MoveRecord $moves $move
    Assert-Condition ($record.Length -gt 0) "Missing move record $move"
    $name = [regex]::Match($record, '\.name = COMPOUND_STRING\("([^"]*)"\)').Groups[1].Value
    Assert-Condition ($name -ceq $expectedNames[$move]) "$move has unexpected localized name '$name'"
    Assert-Condition ($name.Length -le 16) "$move exceeds MOVE_NAME_LENGTH"

    $lines = $record -split '\r?\n'
    $bounds = Get-LocalizedBounds $lines
    $localizedBlock = $lines[$bounds[1]..$bounds[2]] -join "`n"
    Assert-Condition ($localizedBlock -notmatch '(?i)\b(the|foe|user|attack|raises|lowers|may|with|using|turns|damage|strikes|boost|forms|restores|pounds|bites)\b') "$move retains known English description text"
}

$mudSlap = Get-MoveRecord $moves 'MOVE_MUD_SLAP'
Assert-Condition ($mudSlap.Contains('.name = COMPOUND_STRING("Fangosberla")')) 'Fangosberla name changed'
Assert-Condition ($mudSlap.Contains('Scaglia fango sul bersaglio\n')) 'Fangosberla description changed'
Assert-Condition ($grassLearnset -contains '7:MOVE_MUD_SLAP') 'Cingerm no longer learns Fangosberla at level 7'

$smokescreen = Get-MoveRecord $moves 'MOVE_SMOKESCREEN'
Assert-Condition ($smokescreen.Contains('.name = COMPOUND_STRING("Muro di Fumo")')) 'Muro di Fumo name is missing'
Assert-Condition ($smokescreen.Contains('Crea fumo e riduce la\n')) 'Muro di Fumo description is not Italian'
Assert-Condition ($smokescreen.Contains('Precisione del bersaglio.')) 'Muro di Fumo no longer describes its real effect'

$baseMoves = (& git -C $RepositoryRoot show develop:src/data/moves_info.h | Out-String)
Assert-Condition ($LASTEXITCODE -eq 0) 'Could not read develop version of moves_info.h'
$allCurrent = [regex]::Matches($moves, '(?ms)^    \[(MOVE_[A-Z0-9_]+)\] =\r?\n    \{.*?^    \},')
foreach ($match in $allCurrent)
{
    $move = $match.Groups[1].Value
    $baseRecord = Get-MoveRecord $baseMoves $move
    Assert-Condition ($baseRecord.Length -gt 0) "Missing develop record for $move"
    if ($actualMoves.Contains($move))
    {
        Assert-Condition ((Remove-LocalizedFields $match.Value) -ceq (Remove-LocalizedFields $baseRecord)) "$move technical data changed"
    }
    else
    {
        Assert-Condition ($match.Value.Replace("`r`n", "`n") -ceq $baseRecord.Replace("`r`n", "`n")) "$move was modified outside the audited learnsets"
    }
}

foreach ($path in @('src/starter_choose.c', 'src/data/trainers.party'))
{
    & git -C $RepositoryRoot diff --quiet develop -- $path
    Assert-Condition ($LASTEXITCODE -eq 0) "$path changed during move localization"
}

Write-Output 'Ausonia starter move localization validation passed.'
