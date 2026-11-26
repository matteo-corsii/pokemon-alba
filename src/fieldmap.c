#include "global.h"
#include "battle_pyramid.h"
#include "bg.h"
#include "fieldmap.h"
#include "fldeff.h"
#include "fldeff_misc.h"
#include "frontier_util.h"
#include "main.h"
#include "menu.h"
#include "mirage_tower.h"
#include "overworld.h"
#include "palette.h"
#include "pokenav.h"
#include "script.h"
#include "secret_base.h"
#include "tilesets.h"
#include "trainer_hill.h"
#include "tv.h"
#include "constants/rgb.h"
#include "constants/layouts.h"
#include "constants/metatile_behaviors.h"
#include "constants/metatile_behaviors_frlg.h"
#include "constants/metatile_labels.h"
#include "wild_encounter.h"

struct ConnectionFlags
{
    u8 south:1;
    u8 north:1;
    u8 west:1;
    u8 east:1;
};

EWRAM_DATA u16 ALIGNED(4) sBackupMapData[MAX_MAP_DATA_SIZE] = {0};
EWRAM_DATA struct MapHeader gMapHeader = {0};
EWRAM_DATA struct Camera gCamera = {0};
EWRAM_DATA static struct ConnectionFlags sMapConnectionFlags = {0};

COMMON_DATA struct BackupMapLayout gBackupMapLayout = {0};

static const struct ConnectionFlags sDummyConnectionFlags = {0};

static void InitMapLayoutData(struct MapHeader *mapHeader);
static void InitBackupMapLayoutData(const u16 *map, u16 width, u16 height);
static void FillSouthConnection(struct MapHeader const *mapHeader, struct MapHeader const *connectedMapHeader, s32 offset);
static void FillNorthConnection(struct MapHeader const *mapHeader, struct MapHeader const *connectedMapHeader, s32 offset);
static void FillWestConnection(struct MapHeader const *mapHeader, struct MapHeader const *connectedMapHeader, s32 offset);
static void FillEastConnection(struct MapHeader const *mapHeader, struct MapHeader const *connectedMapHeader, s32 offset);
static void InitBackupMapLayoutConnections(struct MapHeader *mapHeader);
static void LoadSavedMapView(void);
static bool8 SkipCopyingMetatileFromSavedMap(u16 *mapBlock, u16 mapWidth, u8 yMode);
static const struct MapConnection *GetIncomingConnection(enum Connection direction, int x, int y);
static bool8 IsPosInIncomingConnectingMap(enum Connection direction, int x, int y, const struct MapConnection *connection);
static bool8 IsCoordInIncomingConnectingMap(int coord, int srcMax, int destMax, int offset);
#if !IS_FRLG
static void CopyMapViewSpan(const u16 *src, u16 *dest, int count, int rows, const u32 *remap) ARM_FUNC __attribute__((section(".iwram.code"), noinline));
#endif
static void ClearMapSideBorders(int width, int height) ARM_FUNC __attribute__((section(".iwram.code"), noinline));
static void CopyNativeMapForConnection(const u16 *src, u16 *dest, int width, int height) ARM_FUNC __attribute__((section(".iwram.code"), noinline));

static bool8 IsPreparingPonteLariciaConnection(void)
{
#if !IS_FRLG
    const struct Tileset *tileset = gMapHeader.mapLayout->secondaryTileset;

    return IsVBlankGraphicsDeferred()
        && (tileset == &gTileset_PonteValleLaricia || tileset == &gTileset_LariciaPonte);
#else
    return FALSE;
#endif
}

static void ClearMapBordersForConnection(int width, int height)
{
    int bottom = (width * (height - MAP_OFFSET)) & ~1;

    // The native rectangle is overwritten next. Clear only the surrounding
    // strips, rounding BIOS fills to words and 32-byte transfer blocks.
    CpuFastFill16(MAPGRID_UNDEFINED, sBackupMapData, ((MAP_OFFSET * width + MAP_OFFSET) * sizeof(u16) + 31) & ~31);
    CpuFastFill16(MAPGRID_UNDEFINED, sBackupMapData + bottom, ((width * height - bottom) * sizeof(u16) + 31) & ~31);
    ClearMapSideBorders(width, height);
}

static void ClearMapSideBorders(int width, int height)
{
    typedef u32 MapWord __attribute__((may_alias));
    const u32 empty = MAPGRID_UNDEFINED | (MAPGRID_UNDEFINED << 16);
    int row;

    for (row = MAP_OFFSET; row < height - MAP_OFFSET; row++)
    {
        int border = (row * width + width - MAP_OFFSET_W + MAP_OFFSET) & ~1;
        MapWord *dest = (MapWord *)(sBackupMapData + border);

        // One short fill spans this row's right edge and the next left edge.
        // Any rounded overlap with native cells is overwritten by their copy.
#if MAP_OFFSET_W == 15
        dest[0] = empty;
        dest[1] = empty;
        dest[2] = empty;
        dest[3] = empty;
        dest[4] = empty;
        dest[5] = empty;
        dest[6] = empty;
        dest[7] = empty;
#else
        int i;

        for (i = 0; i < (((MAP_OFFSET_W + 1) * sizeof(u16) + 31) & ~31) / sizeof(u32); i++)
            dest[i] = empty;
#endif
    }
}

static inline u16 GetBorderBlockAt(int x, int y)
{
    const struct MapLayout *mapLayout = gMapHeader.mapLayout;

    if (mapLayout->isFrlg)
    {
        s32 xprime;
        s32 yprime;

        xprime = x - MAP_OFFSET;
        xprime += 8 * mapLayout->borderWidth;
        xprime %= mapLayout->borderWidth;

        yprime = y - MAP_OFFSET;
        yprime += 8 * mapLayout->borderHeight;
        yprime %= mapLayout->borderHeight;

        return mapLayout->border[xprime + yprime * mapLayout->borderWidth] | MAPGRID_COLLISION_MASK;
    }

    int i = (x + 1) & 1;
    i += ((y + 1) & 1) * 2;
    return gMapHeader.mapLayout->border[i] | MAPGRID_IMPASSABLE;
}

#define AreCoordsWithinMapGridBounds(x, y) (x >= 0 && x < gBackupMapLayout.width && y >= 0 && y < gBackupMapLayout.height)

#define GetMapGridBlockAt(x, y) (AreCoordsWithinMapGridBounds(x, y) ? gBackupMapLayout.map[x + gBackupMapLayout.width * y] : GetBorderBlockAt(x, y))

// Masks/shifts for metatile attributes
// This is the format of the data stored in each data/tilesets/*/*/metatile_attributes.bin file
static const u32 sMetatileAttrMasks[METATILE_ATTRIBUTE_COUNT] = {
    [METATILE_ATTRIBUTE_BEHAVIOR]       = METATILE_ATTR_BEHAVIOR_MASK_FRLG, // Bits 0-8
    [METATILE_ATTRIBUTE_TERRAIN]        = 0x00003e00, // Bits 9-13
    [METATILE_ATTRIBUTE_2]              = 0x0003c000, // Bits 14-17
    [METATILE_ATTRIBUTE_3]              = 0x00fc0000, // Bits 18-23
    [METATILE_ATTRIBUTE_ENCOUNTER_TYPE] = 0x07000000, // Bits 24-26
    [METATILE_ATTRIBUTE_5]              = 0x18000000, // Bits 27-28
    [METATILE_ATTRIBUTE_LAYER_TYPE]     = METATILE_ATTR_LAYER_MASK_FRLG, // Bits 29-30
    [METATILE_ATTRIBUTE_7]              = 0x80000000  // Bit  31
};

static const u8 sMetatileAttrShifts[METATILE_ATTRIBUTE_COUNT] = {
    [METATILE_ATTRIBUTE_BEHAVIOR]       = METATILE_ATTR_BEHAVIOR_SHIFT_FRLG,
    [METATILE_ATTRIBUTE_TERRAIN]        = 9,
    [METATILE_ATTRIBUTE_2]              = 14,
    [METATILE_ATTRIBUTE_3]              = 18,
    [METATILE_ATTRIBUTE_ENCOUNTER_TYPE] = 24,
    [METATILE_ATTRIBUTE_5]              = 27,
    [METATILE_ATTRIBUTE_LAYER_TYPE]     = METATILE_ATTR_LAYER_SHIFT_FRLG,
    [METATILE_ATTRIBUTE_7]              = 31
};

static const u32 sMetatileAttrMasksEmerald[METATILE_ATTRIBUTE_COUNT] = {

    [METATILE_ATTRIBUTE_BEHAVIOR]       = METATILE_ATTR_BEHAVIOR_MASK,
    [METATILE_ATTRIBUTE_TERRAIN]        = 0xFFFFFFFF,
    [METATILE_ATTRIBUTE_2]              = 0xFFFFFFFF,
    [METATILE_ATTRIBUTE_3]              = 0xFFFFFFFF,
    [METATILE_ATTRIBUTE_ENCOUNTER_TYPE] = 0xFFFFFFFF,
    [METATILE_ATTRIBUTE_5]              = 0xFFFFFFFF,
    [METATILE_ATTRIBUTE_LAYER_TYPE]     = METATILE_ATTR_LAYER_MASK,
    [METATILE_ATTRIBUTE_7]              = 0xFFFFFFFF
};

static const u8 sMetatileAttrShiftsEmerald[METATILE_ATTRIBUTE_COUNT] = {

    [METATILE_ATTRIBUTE_BEHAVIOR]       = METATILE_ATTR_BEHAVIOR_SHIFT,
    [METATILE_ATTRIBUTE_TERRAIN]        = 0,
    [METATILE_ATTRIBUTE_2]              = 0,
    [METATILE_ATTRIBUTE_3]              = 0,
    [METATILE_ATTRIBUTE_ENCOUNTER_TYPE] = 0,
    [METATILE_ATTRIBUTE_5]              = 0,
    [METATILE_ATTRIBUTE_LAYER_TYPE]     = METATILE_ATTR_LAYER_SHIFT,
    [METATILE_ATTRIBUTE_7]              = 0
};

const struct MapHeader *const GetMapHeaderFromConnection(const struct MapConnection *connection)
{
    return Overworld_GetMapHeaderByGroupAndId(connection->mapGroup, connection->mapNum);
}

void InitMap(void)
{
    InitMapLayoutData(&gMapHeader);
    SetOccupiedSecretBaseEntranceMetatiles(gMapHeader.events);
    RunOnLoadMapScript();
}

void InitMapFromSavedGame(void)
{
    InitMapLayoutData(&gMapHeader);
    InitSecretBaseAppearance(FALSE);
    SetOccupiedSecretBaseEntranceMetatiles(gMapHeader.events);
    LoadSavedMapView();
    RunOnLoadMapScript();
    UpdateTVScreensOnMap(gBackupMapLayout.width, gBackupMapLayout.height);
}

void InitBattlePyramidMap(bool8 setPlayerPosition)
{
    CpuFastFill16(MAPGRID_UNDEFINED, sBackupMapData, sizeof(sBackupMapData));
    GenerateBattlePyramidFloorLayout(sBackupMapData, setPlayerPosition);
}

void InitTrainerHillMap(void)
{
    CpuFastFill16(MAPGRID_UNDEFINED, sBackupMapData, sizeof(sBackupMapData));
    GenerateTrainerHillFloorLayout(sBackupMapData);
}

static void InitMapLayoutData(struct MapHeader *mapHeader)
{
    struct MapLayout const *mapLayout;
    int width;
    int height;
    mapLayout = mapHeader->mapLayout;
    gBackupMapLayout.map = sBackupMapData;
    width = mapLayout->width + MAP_OFFSET_W;
    gBackupMapLayout.width = width;
    height = mapLayout->height + MAP_OFFSET_H;
    gBackupMapLayout.height = height;
    // Keep the ordinary initialization for all other loading paths. Extra
    // capacity here permits the small rounding overlap of the border fills.
    if (IsPreparingPonteLariciaConnection()
     && width * height * sizeof(u16) + 31 <= sizeof(sBackupMapData))
    {
        ClearMapBordersForConnection(width, height);
    }
    else
    {
        CpuFastFill16(MAPGRID_UNDEFINED, sBackupMapData, sizeof(sBackupMapData));
    }
    if (width * height <= MAX_MAP_DATA_SIZE)
    {
        InitBackupMapLayoutData(mapLayout->map, mapLayout->width, mapLayout->height);
        InitBackupMapLayoutConnections(mapHeader);
    }
}

static void CopyNativeMapForConnection(const u16 *src, u16 *dest, int width, int height)
{
    // The deferred graphics callback cannot compete for DMA3 here. Keep the
    // short row setup in fast memory along with the native rectangle copy.
    while (height-- > 0)
    {
        DmaCopy16(3, src, dest, width * 2);
        src += width;
        dest += width + MAP_OFFSET_W;
    }
}

static void InitBackupMapLayoutData(const u16 *map, u16 width, u16 height)
{
    u16 *dest;
    int y;
    dest = gBackupMapLayout.map;
    dest += gBackupMapLayout.width * 7 + MAP_OFFSET;

    if (width > 0 && IsPreparingPonteLariciaConnection())
    {
        CopyNativeMapForConnection(map, dest, width, height);
        return;
    }

    // Other map loads retain their existing CPU copy path.
    for (y = 0; y < height; y++)
    {
        CpuCopy16(map, dest, width * 2);
        dest += width + MAP_OFFSET_W;
        map += width;
    }
}

static void InitBackupMapLayoutConnections(struct MapHeader *mapHeader)
{
    int count;
    const struct MapConnection *connection;
    int i;

    if (mapHeader->connections)
    {
        count = mapHeader->connections->count;
        connection = mapHeader->connections->connections;
        sMapConnectionFlags = sDummyConnectionFlags;
        for (i = 0; i < count; i++, connection++)
        {
            struct MapHeader const *cMap = GetMapHeaderFromConnection(connection);
            s32 offset = connection->offset;
            switch (connection->direction)
            {
            case CONNECTION_SOUTH:
                FillSouthConnection(mapHeader, cMap, offset);
                sMapConnectionFlags.south = TRUE;
                break;
            case CONNECTION_NORTH:
                FillNorthConnection(mapHeader, cMap, offset);
                sMapConnectionFlags.north = TRUE;
                break;
            case CONNECTION_WEST:
                FillWestConnection(mapHeader, cMap, offset);
                sMapConnectionFlags.west = TRUE;
                break;
            case CONNECTION_EAST:
                FillEastConnection(mapHeader, cMap, offset);
                sMapConnectionFlags.east = TRUE;
                break;
            }
        }
    }
}

#if !IS_FRLG

static void CopyRemappedConnection(const u16 *src, u16 *dest, int srcWidth, int width, int height, bool8 toPonte) ARM_FUNC __attribute__((section(".iwram.code"), noinline));

static bool8 IsPonteValleLariciaLariciaConnection(struct MapHeader const *mapHeader, struct MapHeader const *connectedMapHeader)
{
    return mapHeader->mapLayout->secondaryTileset == &gTileset_PonteValleLaricia
        && (connectedMapHeader->mapLayout->secondaryTileset == &gTileset_Laricia
         || connectedMapHeader->mapLayout->secondaryTileset == &gTileset_LariciaPonte);
}

static u16 RemapPonteValleLariciaConnectionMetatile(u16 mapBlock)
{
    switch (UNPACK_METATILE(mapBlock))
    {
    case 0x241:
        return (mapBlock & ~MAPGRID_METATILE_ID_MASK) | METATILE_PonteValleLaricia_Laricia_241;
    case 0x2D9:
        return (mapBlock & ~MAPGRID_METATILE_ID_MASK) | METATILE_PonteValleLaricia_Laricia_2D9;
    default:
        return mapBlock;
    }
}

static bool8 IsLariciaPonteLariciaConnection(struct MapHeader const *mapHeader, struct MapHeader const *connectedMapHeader)
{
    return mapHeader->mapLayout->secondaryTileset == &gTileset_LariciaPonte
        && connectedMapHeader->mapLayout->secondaryTileset == &gTileset_PonteValleLaricia;
}

static u16 RemapLariciaPonteConnectionMetatile(u16 mapBlock)
{
    switch (UNPACK_METATILE(mapBlock))
    {
    case 0x309:
        return (mapBlock & ~MAPGRID_METATILE_ID_MASK) | METATILE_LariciaPonte_Ponte_309;
    case 0x293:
        return (mapBlock & ~MAPGRID_METATILE_ID_MASK) | METATILE_LariciaPonte_Ponte_293;
    case 0x294:
        return (mapBlock & ~MAPGRID_METATILE_ID_MASK) | METATILE_LariciaPonte_Ponte_294;
    default:
        return mapBlock;
    }
}

static u16 RestorePonteValleLariciaConnectionMetatile(u16 mapBlock)
{
    switch (UNPACK_METATILE(mapBlock))
    {
    case METATILE_PonteValleLaricia_Laricia_241:
        return (mapBlock & ~MAPGRID_METATILE_ID_MASK) | 0x241;
    case METATILE_PonteValleLaricia_Laricia_2D9:
        return (mapBlock & ~MAPGRID_METATILE_ID_MASK) | 0x2D9;
    default:
        return mapBlock;
    }
}

static u16 RestoreLariciaPonteConnectionMetatile(u16 mapBlock)
{
    switch (UNPACK_METATILE(mapBlock))
    {
    case METATILE_LariciaPonte_Ponte_309:
        return (mapBlock & ~MAPGRID_METATILE_ID_MASK) | 0x309;
    case METATILE_LariciaPonte_Ponte_293:
        return (mapBlock & ~MAPGRID_METATILE_ID_MASK) | 0x293;
    case METATILE_LariciaPonte_Ponte_294:
        return (mapBlock & ~MAPGRID_METATILE_ID_MASK) | 0x294;
    default:
        return mapBlock;
    }
}

#endif

bool8 AreMapLayoutsUsingRemappedConnection(const struct MapLayout *from, const struct MapLayout *to)
{
#if !IS_FRLG
    return (from->secondaryTileset == &gTileset_PonteValleLaricia
         && to->secondaryTileset == &gTileset_LariciaPonte)
        || (from->secondaryTileset == &gTileset_LariciaPonte
         && to->secondaryTileset == &gTileset_PonteValleLaricia);
#else
    (void)from;
    (void)to;
    return FALSE;
#endif
}

// Saved views contain both the current map and its connection previews. A
// preview alias must become a native ID when that map becomes current; native
// IDs from the old map must instead use the new map's preview aliases. Position
// determines ownership because the two tilesets also share native numeric IDs.
// The same normalization repairs in-game saves made after an older transition.
static u16 NormalizePonteLariciaMapViewBlock(u16 mapBlock, int x, int y)
{
#if !IS_FRLG
    const struct MapLayout *layout = gMapHeader.mapLayout;
    const struct MapConnection *connection;
    const struct MapHeader *connectedMapHeader;
    u16 metatile = UNPACK_METATILE(mapBlock);

    // Most blocks need no translation. Avoid resolving a map connection for
    // every unchanged cell in the saved viewport.
    if (layout->secondaryTileset == &gTileset_PonteValleLaricia)
    {
        if (metatile != 0x241 && metatile != 0x2D9
         && metatile != METATILE_LariciaPonte_Ponte_309
         && metatile != METATILE_LariciaPonte_Ponte_293
         && metatile != METATILE_LariciaPonte_Ponte_294)
            return mapBlock;
    }
    else if (layout->secondaryTileset == &gTileset_LariciaPonte)
    {
        if (metatile != 0x309 && metatile != 0x293 && metatile != 0x294
         && metatile != METATILE_PonteValleLaricia_Laricia_241
         && metatile != METATILE_PonteValleLaricia_Laricia_2D9)
            return mapBlock;
    }
    else
        return mapBlock;

    if (x >= MAP_OFFSET && x < layout->width + MAP_OFFSET
     && y >= MAP_OFFSET && y < layout->height + MAP_OFFSET)
    {
        if (layout->secondaryTileset == &gTileset_PonteValleLaricia)
            return RestoreLariciaPonteConnectionMetatile(mapBlock);
        return RestorePonteValleLariciaConnectionMetatile(mapBlock);
    }

    connection = GetMapConnectionAtPos(x, y);
    if (connection != NULL)
    {
        connectedMapHeader = GetMapHeaderFromConnection(connection);
        if (IsPonteValleLariciaLariciaConnection(&gMapHeader, connectedMapHeader))
            return RemapPonteValleLariciaConnectionMetatile(mapBlock);
        if (IsLariciaPonteLariciaConnection(&gMapHeader, connectedMapHeader))
            return RemapLariciaPonteConnectionMetatile(mapBlock);
    }
#else
    (void)x;
    (void)y;
#endif
    return mapBlock;
}

#if !IS_FRLG
static void CopyRemappedConnection(const u16 *src, u16 *dest, int srcWidth, int width, int height, bool8 toPonte)
{
    int row;
    int srcSkip = srcWidth - width;
    int destSkip = gBackupMapLayout.width - width;

    if (width <= 0)
        return;

    // This loop only handles the existing two remapped connection types.
    // Select the translation once, then copy and translate in one pass.
    // XOR changes only the known metatile ID, preserving collision/elevation.
    if (toPonte)
    {
        for (row = 0; row < height; row++)
        {
            const u16 *end = src + width;

            do
            {
                u32 block = *src++;

                switch (UNPACK_METATILE(block))
                {
                case 0x241:
                    block ^= 0x241 ^ METATILE_PonteValleLaricia_Laricia_241;
                    break;
                case 0x2D9:
                    block ^= 0x2D9 ^ METATILE_PonteValleLaricia_Laricia_2D9;
                    break;
                }
                *dest++ = block;
            } while (src != end);
            src += srcSkip;
            dest += destSkip;
        }
    }
    else
    {
        for (row = 0; row < height; row++)
        {
            const u16 *end = src + width;

            do
            {
                u32 block = *src++;

                switch (UNPACK_METATILE(block))
                {
                case 0x309:
                    block ^= 0x309 ^ METATILE_LariciaPonte_Ponte_309;
                    break;
                case 0x293:
                    block ^= 0x293 ^ METATILE_LariciaPonte_Ponte_293;
                    break;
                case 0x294:
                    block ^= 0x294 ^ METATILE_LariciaPonte_Ponte_294;
                    break;
                }
                *dest++ = block;
            } while (src != end);
            src += srcSkip;
            dest += destSkip;
        }
    }
}
#endif

static void FillConnection(struct MapHeader const *mapHeader, int x, int y, struct MapHeader const *connectedMapHeader, int x2, int y2, int width, int height)
{
    int i;
#if !IS_FRLG
    bool8 remapPonteMetatiles = IsPonteValleLariciaLariciaConnection(mapHeader, connectedMapHeader);
    bool8 remapLariciaMetatiles = IsLariciaPonteLariciaConnection(mapHeader, connectedMapHeader);
#endif
    const u16 *src;
    u16 *dest;
    int mapWidth;
    bool8 useDma = IsPreparingPonteLariciaConnection() && width > 0;

#if IS_FRLG
    (void)mapHeader;
#endif

    mapWidth = connectedMapHeader->mapLayout->width;
    src = &connectedMapHeader->mapLayout->map[mapWidth * y2 + x2];
    dest = &gBackupMapLayout.map[gBackupMapLayout.width * y + x];

#if !IS_FRLG
    if (remapPonteMetatiles || remapLariciaMetatiles)
    {
        CopyRemappedConnection(src, dest, mapWidth, width, height, remapPonteMetatiles);
        return;
    }
#endif

    for (i = 0; i < height; i++)
    {
        if (useDma)
            DmaCopy16(3, src, dest, width * 2);
        else
            CpuCopy16(src, dest, width * 2);
        dest += gBackupMapLayout.width;
        src += mapWidth;
    }
}

static void FillSouthConnection(struct MapHeader const *mapHeader, struct MapHeader const *connectedMapHeader, s32 offset)
{
    int x, y;
    int x2;
    int width;
    int cWidth;

    if (connectedMapHeader)
    {
        cWidth = connectedMapHeader->mapLayout->width;
        x = offset + MAP_OFFSET;
        y = mapHeader->mapLayout->height + MAP_OFFSET;
        if (x < 0)
        {
            x2 = -x;
            x += cWidth;
            if (x < gBackupMapLayout.width)
                width = x;
            else
                width = gBackupMapLayout.width;
            x = 0;
        }
        else
        {
            x2 = 0;
            if (x + cWidth < gBackupMapLayout.width)
                width = cWidth;
            else
                width = gBackupMapLayout.width - x;
        }

        FillConnection(
            mapHeader, x, y,
            connectedMapHeader,
            x2, /*y2*/ 0,
            width, /*height*/ MAP_OFFSET);
    }
}

static void FillNorthConnection(struct MapHeader const *mapHeader, struct MapHeader const *connectedMapHeader, s32 offset)
{
    int x;
    int x2, y2;
    int width;
    int cWidth, cHeight;

    if (connectedMapHeader)
    {
        cWidth = connectedMapHeader->mapLayout->width;
        cHeight = connectedMapHeader->mapLayout->height;
        x = offset + MAP_OFFSET;
        y2 = cHeight - MAP_OFFSET;
        if (x < 0)
        {
            x2 = -x;
            x += cWidth;
            if (x < gBackupMapLayout.width)
                width = x;
            else
                width = gBackupMapLayout.width;
            x = 0;
        }
        else
        {
            x2 = 0;
            if (x + cWidth < gBackupMapLayout.width)
                width = cWidth;
            else
                width = gBackupMapLayout.width - x;
        }

        FillConnection(
            mapHeader, x, /*y*/ 0,
            connectedMapHeader,
            x2, y2,
            width, /*height*/ MAP_OFFSET);

    }
}

static void FillWestConnection(struct MapHeader const *mapHeader, struct MapHeader const *connectedMapHeader, s32 offset)
{
    int y;
    int x2, y2;
    int height;
    int cWidth, cHeight;
    if (connectedMapHeader)
    {
        cWidth = connectedMapHeader->mapLayout->width;
        cHeight = connectedMapHeader->mapLayout->height;
        y = offset + MAP_OFFSET;
        x2 = cWidth - MAP_OFFSET;
        if (y < 0)
        {
            y2 = -y;
            if (y + cHeight < gBackupMapLayout.height)
                height = y + cHeight;
            else
                height = gBackupMapLayout.height;
            y = 0;
        }
        else
        {
            y2 = 0;
            if (y + cHeight < gBackupMapLayout.height)
                height = cHeight;
            else
                height = gBackupMapLayout.height - y;
        }

        FillConnection(
            mapHeader, /*x*/ 0, y,
            connectedMapHeader,
            x2, y2,
            /*width*/ MAP_OFFSET, height);
    }
}

static void FillEastConnection(struct MapHeader const *mapHeader, struct MapHeader const *connectedMapHeader, s32 offset)
{
    int x, y;
    int y2;
    int height;
    int cHeight;
    if (connectedMapHeader)
    {
        cHeight = connectedMapHeader->mapLayout->height;
        x = mapHeader->mapLayout->width + MAP_OFFSET;
        y = offset + MAP_OFFSET;
        if (y < 0)
        {
            y2 = -y;
            if (y + cHeight < gBackupMapLayout.height)
                height = y + cHeight;
            else
                height = gBackupMapLayout.height;
            y = 0;
        }
        else
        {
            y2 = 0;
            if (y + cHeight < gBackupMapLayout.height)
                height = cHeight;
            else
                height = gBackupMapLayout.height - y;
        }

        FillConnection(
            mapHeader, x, y,
            connectedMapHeader,
            /*x2*/ 0, y2,
            /*width*/ MAP_OFFSET + 1, height);
    }
}

u8 MapGridGetElevationAt(int x, int y)
{
    u16 block = GetMapGridBlockAt(x, y);

    if (block == MAPGRID_UNDEFINED)
        return 0;

    return UNPACK_ELEVATION(block);
}

u8 MapGridGetCollisionAt(int x, int y)
{
    u16 block = GetMapGridBlockAt(x, y);

    if (block == MAPGRID_UNDEFINED)
        return TRUE;

    return UNPACK_COLLISION(block);
}

u32 GetNumTilesInPrimary(struct MapLayout const *mapLayout)
{
    return mapLayout->isFrlg ? NUM_TILES_IN_PRIMARY_FRLG : NUM_TILES_IN_PRIMARY;
}

u32 GetNumMetatilesInPrimary(struct MapLayout const *mapLayout)
{
    return mapLayout->isFrlg ? NUM_METATILES_IN_PRIMARY_FRLG : NUM_METATILES_IN_PRIMARY;
}

u32 GetNumPalsInPrimary(struct MapLayout const *mapLayout)
{
    return mapLayout->isFrlg ? NUM_PALS_IN_PRIMARY_FRLG : NUM_PALS_IN_PRIMARY;
}

u32 MapGridGetMetatileIdAt(int x, int y)
{
    u16 block = GetMapGridBlockAt(x, y);

    if (block == MAPGRID_UNDEFINED)
        return GetBorderBlockAt(x, y) & MAPGRID_METATILE_ID_MASK;

    return UNPACK_METATILE(block);
}

u32 MapGridGetMetatileAttributeAt(s16 x, s16 y, u8 attributeType)
{
    u16 metatileId = MapGridGetMetatileIdAt(x, y);
    return GetAttributeByMetatileIdAndMapLayout(metatileId, attributeType, gMapHeader.mapLayout->isFrlg);
}

u32 MapGridGetMetatileBehaviorAt(int x, int y)
{
    return MapGridGetMetatileAttributeAt(x, y, METATILE_ATTRIBUTE_BEHAVIOR);
}

u8 MapGridGetMetatileLayerTypeAt(int x, int y)
{
    return MapGridGetMetatileAttributeAt(x, y, METATILE_ATTRIBUTE_LAYER_TYPE);
}

void MapGridSetMetatileIdAt(int x, int y, u16 metatile)
{
    int i;
    if (AreCoordsWithinMapGridBounds(x, y))
    {
        i = x + y * gBackupMapLayout.width;

        // Elevation is ignored in the argument, but copy metatile ID and collision
        gBackupMapLayout.map[i] = (gBackupMapLayout.map[i] & MAPGRID_ELEVATION_MASK) | (metatile & ~MAPGRID_ELEVATION_MASK);
    }
}

void MapGridSetMetatileEntryAt(int x, int y, u16 metatile)
{
    int i;
    if (AreCoordsWithinMapGridBounds(x, y))
    {
        i = x + gBackupMapLayout.width * y;
        gBackupMapLayout.map[i] = metatile;
    }
}

u32 ExtractMetatileAttribute(u32 attributes, u8 attributeType, bool32 isFrlg)
{
    if (attributeType >= METATILE_ATTRIBUTE_COUNT) // Check for METATILE_ATTRIBUTES_ALL
        return attributes;

    if (isFrlg)
        return (attributes & sMetatileAttrMasks[attributeType]) >> sMetatileAttrShifts[attributeType];

    return (attributes & sMetatileAttrMasksEmerald[attributeType]) >> sMetatileAttrShiftsEmerald[attributeType];
}

static u32 GetAttributeByMetatileIdAndMapLayoutFrlg(u16 metatile, u8 attributeType)
{
    u32 attribute;
    if (metatile < GetNumMetatilesInPrimary(gMapHeader.mapLayout))
    {
        const u32 *attributes = (const u32*)gMapHeader.mapLayout->primaryTileset->metatileAttributes;
        attribute = attributes[metatile];
    }
    else if (metatile < NUM_METATILES_TOTAL)
    {
        const u32 *attributes = (const u32*) gMapHeader.mapLayout->secondaryTileset->metatileAttributes;
        metatile -= GetNumMetatilesInPrimary(gMapHeader.mapLayout);
        attribute = attributes[metatile];
    }
    else
    {
        return MB_INVALID;
    }

    return ExtractMetatileAttribute(attribute, attributeType, TRUE);
}

u32 GetAttributeByMetatileIdAndMapLayout(u16 metatile, u8 attributeType, bool32 isFrlg)
{
    u32 attribute;

    if (isFrlg)
        return GetAttributeByMetatileIdAndMapLayoutFrlg(metatile, attributeType);

    if (metatile < GetNumMetatilesInPrimary(gMapHeader.mapLayout))
    {
        const u16 *attributes = (const u16*)gMapHeader.mapLayout->primaryTileset->metatileAttributes;
        attribute = attributes[metatile];
    }
    else if (metatile < NUM_METATILES_TOTAL)
    {
        const u16 *attributes = (const u16*)gMapHeader.mapLayout->secondaryTileset->metatileAttributes;
        metatile -= GetNumMetatilesInPrimary(gMapHeader.mapLayout);
        attribute = attributes[metatile];
    }
    else
    {
        return MB_INVALID;
    }

    return ExtractMetatileAttribute(attribute, attributeType, FALSE);
}

void SaveMapView(void)
{
    int i, j;
    int x, y;
    u16 *mapView;
    int width;
    mapView = gSaveBlock1Ptr->mapView;
    width = gBackupMapLayout.width;
    x = gSaveBlock1Ptr->pos.x;
    y = gSaveBlock1Ptr->pos.y;
    for (i = y; i < y + MAP_OFFSET_H; i++)
    {
        for (j = x; j < x + MAP_OFFSET_W; j++)
            *mapView++ = sBackupMapData[width * i + j];
    }
}

static bool32 SavedMapViewIsEmpty(void)
{
    u16 i;
    u32 marker = 0;

#ifndef UBFIX
    // BUG: This loop extends past the bounds of the mapView array. Its size is only 0x100.
    for (i = 0; i < 0x200; i++)
        marker |= gSaveBlock1Ptr->mapView[i];
#else
    // UBFIX: Only iterate over 0x100
    for (i = 0; i < ARRAY_COUNT(gSaveBlock1Ptr->mapView); i++)
        marker |= gSaveBlock1Ptr->mapView[i];
#endif


    if (marker == 0)
        return TRUE;
    else
        return FALSE;
}

static void ClearSavedMapView(void)
{
    CpuFill16(0, gSaveBlock1Ptr->mapView, sizeof(gSaveBlock1Ptr->mapView));
}

static void LoadSavedMapView(void)
{
    u8 yMode;
    int i, j;
    int x, y;
    u16 *mapView;
    int width;
    mapView = gSaveBlock1Ptr->mapView;
    if (!SavedMapViewIsEmpty())
    {
        width = gBackupMapLayout.width;
        x = gSaveBlock1Ptr->pos.x;
        y = gSaveBlock1Ptr->pos.y;
        for (i = y; i < y + MAP_OFFSET_H; i++)
        {
            if (i == y && i != 0)
                yMode = 0;
            else if (i == y + MAP_OFFSET_H - 1 && i != gMapHeader.mapLayout->height - 1)
                yMode = 1;
            else
                yMode = 0xFF;

            for (j = x; j < x + MAP_OFFSET_W; j++)
            {
                if (!SkipCopyingMetatileFromSavedMap(&sBackupMapData[j + width * i], width, yMode))
                    sBackupMapData[j + width * i] = NormalizePonteLariciaMapViewBlock(*mapView, j, i);
                mapView++;
            }
        }
        for (j = x; j < x + MAP_OFFSET_W; j++)
        {
            if (y != 0)
                FixLongGrassMetatilesWindowTop(j, y - 1);
            if (i < gMapHeader.mapLayout->height - 1)
                FixLongGrassMetatilesWindowBottom(j, y + MAP_OFFSET_H - 1);
        }
        ClearSavedMapView();
    }
}

#if !IS_FRLG
// Each packed entry contains a source ID and the XOR needed to reach its
// destination ID. All six collision/elevation bits pass through unchanged.
static void CopyMapViewSpan(const u16 *src, u16 *dest, int count, int rows, const u32 *remap)
{
    u32 from0 = remap[0] & 0xFFFF, change0 = remap[0] >> 16;
    u32 from1 = remap[1] & 0xFFFF, change1 = remap[1] >> 16;
    u32 from2 = remap[2] & 0xFFFF, change2 = remap[2] >> 16;
    int srcSkip = MAP_OFFSET_W - count;
    int destSkip = gBackupMapLayout.width - count;

    while (rows-- > 0)
    {
        const u16 *end = src + count;

        while (src != end)
        {
            u32 block = *src++;
            u32 id = UNPACK_METATILE(block);

            if (id == from0)
                block ^= change0;
            else if (id == from1)
                block ^= change1;
            else if (id == from2)
                block ^= change2;
            *dest++ = block;
        }
        src += srcSkip;
        dest += destSkip;
    }
}

static const u32 *GetPonteLariciaMapViewRemap(int index)
{
    static const u32 mappings[][3] = {
        {METATILE_LariciaPonte_Ponte_309 | ((0x309 ^ METATILE_LariciaPonte_Ponte_309) << 16),
         METATILE_LariciaPonte_Ponte_293 | ((0x293 ^ METATILE_LariciaPonte_Ponte_293) << 16),
         METATILE_LariciaPonte_Ponte_294 | ((0x294 ^ METATILE_LariciaPonte_Ponte_294) << 16)},
        {0x241 | ((0x241 ^ METATILE_PonteValleLaricia_Laricia_241) << 16),
         0x2D9 | ((0x2D9 ^ METATILE_PonteValleLaricia_Laricia_2D9) << 16), 0xFFFF},
        {METATILE_PonteValleLaricia_Laricia_241 | ((0x241 ^ METATILE_PonteValleLaricia_Laricia_241) << 16),
         METATILE_PonteValleLaricia_Laricia_2D9 | ((0x2D9 ^ METATILE_PonteValleLaricia_Laricia_2D9) << 16), 0xFFFF},
        {0x309 | ((0x309 ^ METATILE_LariciaPonte_Ponte_309) << 16),
         0x293 | ((0x293 ^ METATILE_LariciaPonte_Ponte_293) << 16),
         0x294 | ((0x294 ^ METATILE_LariciaPonte_Ponte_294) << 16)},
        {0xFFFF, 0xFFFF, 0xFFFF},
    };

    return mappings[index];
}
#endif

static void CopyRemappedMapViewRow(const u16 *src, u16 *dest, int x, int y, int count)
{
#if !IS_FRLG
    const struct MapLayout *layout = gMapHeader.mapLayout;
    bool8 toPonte = layout->secondaryTileset == &gTileset_PonteValleLaricia;
    int nativeEnd = layout->width + MAP_OFFSET;

    // At corners, retain the complete position-based connection lookup.
    if (y >= MAP_OFFSET && y < layout->height + MAP_OFFSET)
    {
        while (count > 0)
        {
            const u32 *remap = GetPonteLariciaMapViewRemap(4);
            int span = count;
            bool8 native = x >= MAP_OFFSET && x < nativeEnd;
            bool8 preview = FALSE;

            if (x < MAP_OFFSET && span > MAP_OFFSET - x)
                span = MAP_OFFSET - x;
            else if (native && span > nativeEnd - x)
                span = nativeEnd - x;

            if (!native)
            {
                const struct MapConnection *connection = GetMapConnectionAtPos(x, y);

                if (connection != NULL)
                {
                    const struct MapHeader *header = GetMapHeaderFromConnection(connection);
                    preview = IsPonteValleLariciaLariciaConnection(&gMapHeader, header)
                        || IsLariciaPonteLariciaConnection(&gMapHeader, header);
                }
            }

            if (native)
                remap = GetPonteLariciaMapViewRemap(toPonte ? 0 : 2);
            else if (preview)
                remap = GetPonteLariciaMapViewRemap(toPonte ? 1 : 3);

            CopyMapViewSpan(src, dest, span, 1, remap);
            src += span;
            dest += span;
            x += span;
            count -= span;
        }
        return;
    }
#endif
    while (count-- > 0)
        *dest++ = NormalizePonteLariciaMapViewBlock(*src++, x++, y);
}

static bool8 TryCopyRemappedMapViewRectangle(const u16 *src, u16 *dest, int x, int y, int count, int rows, enum Connection direction, const struct MapLayout *oldMapLayout)
{
#if !IS_FRLG
    const struct MapLayout *layout = gMapHeader.mapLayout;
    const struct MapConnections *connections = gMapHeader.connections;
    const struct MapConnection *side = NULL;
    const struct MapHeader *neighbor;
    const u32 *nativeRemap, *previewRemap;
    enum Connection previewDirection;
    int nativeEnd = layout->width + MAP_OFFSET;
    int firstCount;
    int i;
    bool8 toPonte = layout->secondaryTileset == &gTileset_PonteValleLaricia;

    // Resolve ownership once only when every row has the same native/preview
    // split. Corners and unusual connection arrangements keep the row lookup.
    if (connections == NULL || connections->connections == NULL
     || count <= 0 || rows <= 0 || x < 0 || x + count > gBackupMapLayout.width
     || y < MAP_OFFSET || y + rows > layout->height + MAP_OFFSET)
        return FALSE;

    if (direction == CONNECTION_EAST)
    {
        if (x >= MAP_OFFSET || x + count <= MAP_OFFSET || x + count > nativeEnd)
            return FALSE;
        firstCount = MAP_OFFSET - x;
        previewDirection = CONNECTION_WEST;
    }
    else if (direction == CONNECTION_WEST)
    {
        if (x < MAP_OFFSET || x >= nativeEnd || x + count <= nativeEnd)
            return FALSE;
        firstCount = nativeEnd - x;
        previewDirection = CONNECTION_EAST;
    }
    else
        return FALSE;

    for (i = 0; i < connections->count; i++)
    {
        const struct MapConnection *connection = &connections->connections[i];

        if (connection->direction == previewDirection)
        {
            if (side != NULL)
                return FALSE;
            side = connection;
        }
    }
    if (side == NULL)
        return FALSE;

    neighbor = GetMapHeaderFromConnection(side);
    if (neighbor == NULL || neighbor->mapLayout != oldMapLayout
     || y < side->offset + MAP_OFFSET
     || y + rows > side->offset + MAP_OFFSET + oldMapLayout->height)
        return FALSE;

    nativeRemap = GetPonteLariciaMapViewRemap(toPonte ? 0 : 2);
    previewRemap = GetPonteLariciaMapViewRemap(toPonte ? 1 : 3);
    CopyMapViewSpan(src, dest, firstCount, rows,
                    direction == CONNECTION_EAST ? previewRemap : nativeRemap);
    CopyMapViewSpan(src + firstCount, dest + firstCount, count - firstCount, rows,
                    direction == CONNECTION_EAST ? nativeRemap : previewRemap);
    return TRUE;
#else
    return FALSE;
#endif
}

static void MoveMapViewToBackup(enum Connection direction, const struct MapLayout *oldMapLayout)
{
    bool8 remapMapView = AreMapLayoutsUsingRemappedConnection(oldMapLayout, gMapHeader.mapLayout);
    int width;
    u16 *mapView;
    int x0, y0;
    int x2, y2;
    u16 *src, *dest;
    int srci, desti;
    int r9, r8;
    int x, y;
    int i, j;
    mapView = gSaveBlock1Ptr->mapView;
    width = gBackupMapLayout.width;
    r9 = 0;
    r8 = 0;
    x0 = gSaveBlock1Ptr->pos.x;
    y0 = gSaveBlock1Ptr->pos.y;
    x2 = MAP_OFFSET_W;
    y2 = MAP_OFFSET_H;

    switch (direction)
    {
    case CONNECTION_NORTH:
        y0 += 1;
        y2 = MAP_OFFSET_H - 1;
        break;
    case CONNECTION_SOUTH:
        r8 = 1;
        y2 = MAP_OFFSET_H - 1;
        break;
    case CONNECTION_WEST:
        x0 += 1;
        x2 = MAP_OFFSET_W - 1;
        break;
    case CONNECTION_EAST:
        r9 = 1;
        x2 = MAP_OFFSET_W - 1;
        break;
    default:
        break;
    }

    if (remapMapView
     && TryCopyRemappedMapViewRectangle(mapView + r8 * MAP_OFFSET_W + r9,
                                       sBackupMapData + width * y0 + x0,
                                       x0, y0, x2, y2, direction, oldMapLayout))
    {
        ClearSavedMapView();
        return;
    }

    for (y = 0; y < y2; y++)
    {
        if (remapMapView)
        {
            src = &mapView[(y + r8) * MAP_OFFSET_W + r9];
            dest = &sBackupMapData[width * (y + y0) + x0];
            CopyRemappedMapViewRow(src, dest, x0, y0 + y, x2);
            continue;
        }

        i = 0;
        j = 0;
        for (x = 0; x < x2; x++)
        {
            desti = width * (y + y0);
            srci = (y + r8) * MAP_OFFSET_W + r9;
            src = &mapView[srci + i];
            dest = &sBackupMapData[x0 + desti + j];
            *dest = *src;
            i++;
            j++;
        }
    }

    ClearSavedMapView();
}

enum Connection GetMapBorderIdAt(int x, int y)
{
    if (GetMapGridBlockAt(x, y) == MAPGRID_UNDEFINED)
        return CONNECTION_INVALID;

    if (x >= (gBackupMapLayout.width - (MAP_OFFSET + 1)))
    {
        if (!sMapConnectionFlags.east)
            return CONNECTION_INVALID;

        return CONNECTION_EAST;
    }
    else if (x < MAP_OFFSET)
    {
        if (!sMapConnectionFlags.west)
            return CONNECTION_INVALID;

        return CONNECTION_WEST;
    }
    else if (y >= (gBackupMapLayout.height - MAP_OFFSET))
    {
        if (!sMapConnectionFlags.south)
            return CONNECTION_INVALID;

        return CONNECTION_SOUTH;
    }
    else if (y < MAP_OFFSET)
    {
        if (!sMapConnectionFlags.north)
            return CONNECTION_INVALID;

        return CONNECTION_NORTH;
    }
    else
    {
        return CONNECTION_NONE;
    }
}

enum Connection GetPostCameraMoveMapBorderId(int x, int y)
{
    return GetMapBorderIdAt(gSaveBlock1Ptr->pos.x + MAP_OFFSET + x, gSaveBlock1Ptr->pos.y + MAP_OFFSET + y);
}

bool32 CanCameraMoveInDirection(enum Direction direction)
{
    int x, y;
    x = gSaveBlock1Ptr->pos.x + MAP_OFFSET + gDirectionToVectors[direction].x;
    y = gSaveBlock1Ptr->pos.y + MAP_OFFSET + gDirectionToVectors[direction].y;

    if (GetMapBorderIdAt(x, y) == CONNECTION_INVALID)
        return FALSE;

    return TRUE;
}

static void SetPositionFromConnection(const struct MapConnection *connection, enum Connection direction, int x, int y)
{
    struct MapHeader const *mapHeader = GetMapHeaderFromConnection(connection);

    switch (direction)
    {
    case CONNECTION_EAST:
        gSaveBlock1Ptr->pos.x = -x;
        gSaveBlock1Ptr->pos.y -= connection->offset;
        break;
    case CONNECTION_WEST:
        gSaveBlock1Ptr->pos.x = mapHeader->mapLayout->width;
        gSaveBlock1Ptr->pos.y -= connection->offset;
        break;
    case CONNECTION_SOUTH:
        gSaveBlock1Ptr->pos.x -= connection->offset;
        gSaveBlock1Ptr->pos.y = -y;
        break;
    case CONNECTION_NORTH:
        gSaveBlock1Ptr->pos.x -= connection->offset;
        gSaveBlock1Ptr->pos.y = mapHeader->mapLayout->height;
        break;
    default:
        errorf("invalid direction: %d", direction);
        break;
    }
}

bool8 CameraMove(int x, int y)
{
    const struct MapLayout *oldMapLayout = gMapHeader.mapLayout;
    enum Connection direction;
    const struct MapConnection *connection;
    int old_x, old_y;
    gCamera.active = FALSE;
    direction = GetPostCameraMoveMapBorderId(x, y);
    if (direction == CONNECTION_NONE || direction == CONNECTION_INVALID)
    {
        gSaveBlock1Ptr->pos.x += x;
        gSaveBlock1Ptr->pos.y += y;
    }
    else
    {
        SaveMapView();
        ClearMirageTowerPulseBlendEffect();
        old_x = gSaveBlock1Ptr->pos.x;
        old_y = gSaveBlock1Ptr->pos.y;
        connection = GetIncomingConnection(direction, gSaveBlock1Ptr->pos.x, gSaveBlock1Ptr->pos.y);
        assertf(connection)
        {
            return gCamera.active;
        }

        SetPositionFromConnection(connection, direction, x, y);
        LoadMapFromCameraTransition(connection->mapGroup, connection->mapNum);
        gCamera.active = TRUE;
        gCamera.x = old_x - gSaveBlock1Ptr->pos.x;
        gCamera.y = old_y - gSaveBlock1Ptr->pos.y;
        gSaveBlock1Ptr->pos.x += x;
        gSaveBlock1Ptr->pos.y += y;
        MoveMapViewToBackup(direction, oldMapLayout);
    }

    return gCamera.active;
}

static const struct MapConnection *GetIncomingConnection(enum Connection direction, int x, int y)
{
    int count;
    int i;
    const struct MapConnection *connection;
    const struct MapConnections *connections = gMapHeader.connections;

#ifdef UBFIX // UB: Multiple possible null dereferences
    if (connections == NULL || connections->connections == NULL)
        return NULL;
#endif
    count = connections->count;
    connection = connections->connections;
    for (i = 0; i < count; i++, connection++)
    {
        if (connection->direction == direction && IsPosInIncomingConnectingMap(direction, x, y, connection) == TRUE)
            return connection;
    }
    return NULL;
}

static bool8 IsPosInIncomingConnectingMap(enum Connection direction, int x, int y, const struct MapConnection *connection)
{
    struct MapHeader const *mapHeader;
    mapHeader = GetMapHeaderFromConnection(connection);
    switch (direction)
    {
    case CONNECTION_SOUTH:
    case CONNECTION_NORTH:
        return IsCoordInIncomingConnectingMap(x, gMapHeader.mapLayout->width, mapHeader->mapLayout->width, connection->offset);
    case CONNECTION_WEST:
    case CONNECTION_EAST:
        return IsCoordInIncomingConnectingMap(y, gMapHeader.mapLayout->height, mapHeader->mapLayout->height, connection->offset);
    default:
        return FALSE;
    }
}

static bool8 IsCoordInIncomingConnectingMap(int coord, int srcMax, int destMax, int offset)
{
    int offset2;
    offset2 = offset;

    if (offset2 < 0)
        offset2 = 0;

    if (destMax + offset < srcMax)
        srcMax = destMax + offset;

    if (offset2 <= coord && coord <= srcMax)
        return TRUE;

    return FALSE;
}

static int IsCoordInConnectingMap(int coord, int max)
{
    if (coord >= 0 && coord < max)
        return TRUE;

    return FALSE;
}

static int IsPosInConnectingMap(const struct MapConnection *connection, int x, int y)
{
    struct MapHeader const *mapHeader;
    mapHeader = GetMapHeaderFromConnection(connection);
    switch (connection->direction)
    {
    case CONNECTION_SOUTH:
    case CONNECTION_NORTH:
        return IsCoordInConnectingMap(x - connection->offset, mapHeader->mapLayout->width);
    case CONNECTION_WEST:
    case CONNECTION_EAST:
        return IsCoordInConnectingMap(y - connection->offset, mapHeader->mapLayout->height);
    }
    return FALSE;
}

const struct MapConnection *GetMapConnectionAtPos(s16 x, s16 y)
{
    int count;
    const struct MapConnection *connection;
    int i;
    enum Connection direction;
    if (!gMapHeader.connections)
    {
        return NULL;
    }
    else
    {
        count = gMapHeader.connections->count;
        connection = gMapHeader.connections->connections;
        for (i = 0; i < count; i++, connection++)
        {
            direction = connection->direction;
            if ((direction == CONNECTION_DIVE || direction == CONNECTION_EMERGE)
             || (direction == CONNECTION_NORTH && y > MAP_OFFSET - 1)
             || (direction == CONNECTION_SOUTH && y < gMapHeader.mapLayout->height + MAP_OFFSET)
             || (direction == CONNECTION_WEST && x > MAP_OFFSET - 1)
             || (direction == CONNECTION_EAST && x < gMapHeader.mapLayout->width + MAP_OFFSET))
            {
                continue;
            }
            if (IsPosInConnectingMap(connection, x - MAP_OFFSET, y - MAP_OFFSET) == TRUE)
            {
                return connection;
            }
        }
    }
    return NULL;
}

void SetCameraFocusCoords(u16 x, u16 y)
{
    gSaveBlock1Ptr->pos.x = x - MAP_OFFSET;
    gSaveBlock1Ptr->pos.y = y - MAP_OFFSET;
}

void GetCameraFocusCoords(u16 *x, u16 *y)
{
    *x = gSaveBlock1Ptr->pos.x + MAP_OFFSET;
    *y = gSaveBlock1Ptr->pos.y + MAP_OFFSET;
}

static void UNUSED SetCameraCoords(u16 x, u16 y)
{
    gSaveBlock1Ptr->pos.x = x;
    gSaveBlock1Ptr->pos.y = y;
}

void GetCameraCoords(u16 *x, u16 *y)
{
    *x = gSaveBlock1Ptr->pos.x;
    *y = gSaveBlock1Ptr->pos.y;
}

void MapGridSetMetatileImpassabilityAt(int x, int y, bool32 impassable)
{
    if (AreCoordsWithinMapGridBounds(x, y))
    {
        if (impassable)
            gBackupMapLayout.map[x + gBackupMapLayout.width * y] |= MAPGRID_COLLISION_MASK;
        else
            gBackupMapLayout.map[x + gBackupMapLayout.width * y] &= ~MAPGRID_COLLISION_MASK;
    }
}

static bool8 SkipCopyingMetatileFromSavedMap(u16 *mapBlock, u16 mapWidth, u8 yMode)
{
    if (yMode == 0xFF)
        return FALSE;

    if (yMode == 0)
        mapBlock -= mapWidth;
    else
        mapBlock += mapWidth;

    if (IsLargeBreakableDecoration(UNPACK_METATILE(*mapBlock), yMode) == TRUE)
        return TRUE;
    return FALSE;
}

static void CopyTilesetToVram(struct Tileset const *tileset, u16 numTiles, u16 offset)
{
    if (tileset)
    {
        if (!tileset->isCompressed)
            LoadBgTiles(2, tileset->tiles, numTiles * 32, offset);
        else
            DecompressAndCopyTileDataToVram(2, tileset->tiles, numTiles * 32, offset, 0);
    }
}

static void CopyTilesetToVramUsingHeap(struct Tileset const *tileset, u16 numTiles, u16 offset)
{
    if (tileset)
    {
        if (!tileset->isCompressed)
            LoadBgTiles(2, tileset->tiles, numTiles * 32, offset);
        else
            DecompressAndLoadBgGfxUsingHeap(2, tileset->tiles, numTiles * 32, offset, 0);
    }
}

// Below two are dummied functions from FRLG, used to tint the overworld palettes for the Quest Log
static void ApplyGlobalTintToPaletteEntries(u16 offset, u16 size)
{

}

static void UNUSED ApplyGlobalTintToPaletteSlot(u8 slot, u8 count)
{

}

static void LoadTilesetPalette(struct Tileset const *tileset, u16 destOffset, u16 size, bool8 skipFaded, u32 numPalsInPrimary)
{
    if (tileset)
    {
        if (tileset->isSecondary == FALSE)
        {
            if (skipFaded)
                CpuFastCopy(tileset->palettes, &gPlttBufferUnfaded[destOffset], size); // always word-aligned
            else
                LoadPaletteFast(tileset->palettes, destOffset, size);
            gPlttBufferFaded[destOffset] = gPlttBufferUnfaded[destOffset] = RGB_BLACK;
            ApplyGlobalTintToPaletteEntries(destOffset + 1, (size - 2) >> 1);
        }
        else if (tileset->isSecondary == TRUE)
        {
            // All 'gTilesetPalettes_' arrays should have ALIGNED(4) in them,
            // but we use SmartCopy here just in case they don't
            if (skipFaded)
                CpuCopy16(tileset->palettes[numPalsInPrimary], &gPlttBufferUnfaded[destOffset], size);
            else
                LoadPaletteFast(tileset->palettes[numPalsInPrimary], destOffset, size);
        }
        else
        {
            LoadPalette((const u16 *)tileset->palettes, destOffset, size);
            ApplyGlobalTintToPaletteEntries(destOffset, size >> 1);
        }
    }
}

void CopyPrimaryTilesetToVram(struct MapLayout const *mapLayout)
{
    CopyTilesetToVram(mapLayout->primaryTileset, GetNumTilesInPrimary(mapLayout), 0);
}

void CopySecondaryTilesetToVram(struct MapLayout const *mapLayout)
{
    CopyTilesetToVram(mapLayout->secondaryTileset, NUM_TILES_TOTAL - GetNumTilesInPrimary(mapLayout), GetNumTilesInPrimary(mapLayout));
}

void CopySecondaryTilesetToVramUsingHeap(struct MapLayout const *mapLayout)
{
    CopyTilesetToVramUsingHeap(mapLayout->secondaryTileset, NUM_TILES_TOTAL - GetNumTilesInPrimary(mapLayout), GetNumTilesInPrimary(mapLayout));
}

static void LoadPrimaryTilesetPalette(struct MapLayout const *mapLayout)
{
    LoadTilesetPalette(mapLayout->primaryTileset, 0, GetNumPalsInPrimary(mapLayout) * PLTT_SIZE_4BPP, FALSE, GetNumPalsInPrimary(mapLayout));
}

void LoadSecondaryTilesetPalette(struct MapLayout const *mapLayout, bool8 skipFaded)
{
    LoadTilesetPalette(mapLayout->secondaryTileset, GetNumPalsInPrimary(mapLayout) * 16, (NUM_PALS_TOTAL - GetNumPalsInPrimary(mapLayout)) * PLTT_SIZE_4BPP, skipFaded, GetNumPalsInPrimary(mapLayout));
}

void CopyMapTilesetsToVram(struct MapLayout const *mapLayout)
{
    if (mapLayout)
    {
        CopyTilesetToVramUsingHeap(mapLayout->primaryTileset, GetNumTilesInPrimary(mapLayout), 0);
        CopyTilesetToVramUsingHeap(mapLayout->secondaryTileset, NUM_TILES_TOTAL - GetNumTilesInPrimary(mapLayout), GetNumTilesInPrimary(mapLayout));
    }
}

void LoadMapTilesetPalettes(struct MapLayout const *mapLayout)
{
    if (mapLayout)
    {
        LoadPrimaryTilesetPalette(mapLayout);
        LoadSecondaryTilesetPalette(mapLayout, FALSE);
    }
}

bool32 AreCoordsInsideMap(u8 mapGroup, u8 mapNum, s16 x, s16 y)
{
    const struct MapLayout *layout = Overworld_GetMapHeaderByGroupAndId(mapGroup, mapNum)->mapLayout;
    s32 width = layout->width;
    s32 height = layout->height;
    x -= MAP_OFFSET;
    y -= MAP_OFFSET;

    if (gMapHeader.mapLayoutId == LAYOUT_BATTLE_FRONTIER_BATTLE_PYRAMID_FLOOR)
    {
        width *= PYRAMID_FLOOR_SQUARES_WIDE;
        height *= PYRAMID_FLOOR_SQUARES_HIGH;
    }

    return (x >= 0 && x < width && y >= 0 && y < height);
}

bool32 AreCoordsInsidePlayerMap(s16 x, s16 y)
{
    return AreCoordsInsideMap(gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum, x, y);
}
