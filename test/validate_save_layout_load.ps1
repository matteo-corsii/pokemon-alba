$ErrorActionPreference = 'Stop'

$root = (Resolve-Path "$PSScriptRoot\\..").Path
$overworld = Get-Content -Raw (Join-Path $root 'src/overworld.c')

function Assert-True($condition, $message) {
    if (-not $condition) { throw $message }
}

$loadSaveblockMapHeader = [regex]::Match(
    $overworld,
    '(?s)static void LoadSaveblockMapHeader\(void\)\s*\{(?<body>.*?)\n\}'
)

Assert-True $loadSaveblockMapHeader.Success 'LoadSaveblockMapHeader is missing.'
$body = $loadSaveblockMapHeader.Groups['body'].Value

Assert-True ($body -match 'gMapHeader\s*=\s*\*Overworld_GetMapHeaderByGroupAndId\(gSaveBlock1Ptr->location\.mapGroup,\s*gSaveBlock1Ptr->location\.mapNum\);') 'Save loading must resolve the current map header from the saved map group and number.'
Assert-True ($body -match 'gSaveBlock1Ptr->mapLayoutId\s*=\s*gMapHeader\.mapLayoutId;') 'Save loading must normalize the persisted layout ID from the resolved map header.'
Assert-True ($body -match 'gMapHeader\.mapLayout\s*=\s*GetMapLayout\(gMapHeader\.mapLayoutId\);') 'Save loading must initialize the layout from the resolved map header.'
Assert-True ($body -notmatch 'GetMapLayout\(gSaveBlock1Ptr->mapLayoutId\)') 'Save loading must not initialize a layout from a stale persisted layout ID.'

$normalizeOffset = $body.IndexOf('gSaveBlock1Ptr->mapLayoutId = gMapHeader.mapLayoutId;')
$layoutOffset = $body.IndexOf('gMapHeader.mapLayout = GetMapLayout(gMapHeader.mapLayoutId);')
Assert-True ($normalizeOffset -ge 0 -and $normalizeOffset -lt $layoutOffset) 'The persisted layout ID must be normalized before loading the map layout.'

Write-Output 'Save layout load validation passed.'
