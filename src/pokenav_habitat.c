#include "global.h"
#include "bg.h"
#include "decompress.h"
#include "graphics.h"
#include "list_menu.h"
#include "main.h"
#include "menu.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokenav.h"
#include "region_map.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "window.h"
#include "wild_encounter.h"
#include "constants/songs.h"

#define HABITAT_VISIBLE_ROWS 4

struct Pokenav_Habitat
{
    u16 count;
    u16 top;
    u16 windowId;
    u8 bg1TilemapBuffer[BG_SCREEN_SIZE];
    enum Species species[HABITAT_MAX_SPECIES];
    struct Sprite *icons[HABITAT_VISIBLE_ROWS];
    u8 bg2TilemapBuffer[BG_SCREEN_SIZE];
    u32 loopedTaskId;
    bool32 (*isTaskActiveCB)(void);
    bool32 exitRequested;
    u8 scrollArrowTaskId;
    bool8 scrollArrowsLoaded;
};

static const u16 sHabitatFramePal[] = INCGFX_U16("graphics/pokenav/ribbons/list_bg.png", ".gbapal");
static const u32 sHabitatFrameGfx[] = INCGFX_U32("graphics/pokenav/ribbons/list_bg.png", ".4bpp.smol");
static const u32 sHabitatFrameTilemap[] = INCGFX_U32("graphics/pokenav/ribbons/list_bg.bin", ".smolTM");
static const u16 sHabitatListPal[] = INCGFX_U16("graphics/pokenav/ribbons/list_ui.pal", ".gbapal");

static const struct BgTemplate sHabitatBgTemplates[] =
{
    {
        .bg = 1, .charBaseIndex = 1, .mapBaseIndex = 0x06, .screenSize = 0,
        .paletteMode = 0, .priority = 2, .baseTile = 0
    },
    {
        .bg = 2, .charBaseIndex = 2, .mapBaseIndex = 0x07, .screenSize = 0,
        .paletteMode = 0, .priority = 3, .baseTile = 0
    },
};

static const struct WindowTemplate sHabitatWindowTemplate =
{
    .bg = 2, .tilemapLeft = 2, .tilemapTop = 5, .width = 28, .height = 19,
    .paletteNum = 2, .baseBlock = 0x10
};

static u32 LoopedTask_OpenHabitat(s32 state);
static u32 LoopedTask_HabitatMoveUp(s32 state);
static u32 LoopedTask_HabitatMoveDown(s32 state);
static u32 LoopedTask_HabitatPageUp(s32 state);
static u32 LoopedTask_HabitatPageDown(s32 state);
static u32 LoopedTask_HabitatExit(s32 state);
static void DrawHabitat(void);
static void DestroyHabitatIcons(void);
static void CleanupHabitatScreen(void);
static bool32 HabitatLoopTaskActive(void);

bool32 PokenavCallback_Init_Habitat(void)
{
    struct Pokenav_Habitat *state = AllocSubstruct(POKENAV_SUBSTRUCT_HABITAT, sizeof(*state));
    if (state == NULL)
        return FALSE;

    state->count = GetCurrentMapHabitatSpecies(state->species, ARRAY_COUNT(state->species));
    state->top = 0;
    state->windowId = WINDOW_NONE;
    state->loopedTaskId = 0;
    state->isTaskActiveCB = NULL;
    state->exitRequested = FALSE;
    state->scrollArrowTaskId = TASK_NONE;
    state->scrollArrowsLoaded = FALSE;
    memset(state->icons, 0, sizeof(state->icons));
    return TRUE;
}

u32 GetHabitatCallback(void)
{
    struct Pokenav_Habitat *state = GetSubstructPtr(POKENAV_SUBSTRUCT_HABITAT);

    if (state->exitRequested)
        return POKENAV_MAIN_MENU_CURSOR_ON_HABITAT;

    if (JOY_REPEAT(DPAD_UP))
        return POKENAV_HABITAT_FUNC_UP;
    if (JOY_REPEAT(DPAD_DOWN))
        return POKENAV_HABITAT_FUNC_DOWN;
    if (JOY_REPEAT(DPAD_LEFT))
        return POKENAV_HABITAT_FUNC_PG_UP;
    if (JOY_REPEAT(DPAD_RIGHT))
        return POKENAV_HABITAT_FUNC_PG_DOWN;
    if (JOY_NEW(B_BUTTON))
    {
        state->exitRequested = TRUE;
        return POKENAV_HABITAT_FUNC_EXIT;
    }

    return POKENAV_HABITAT_FUNC_NONE;
}

bool32 OpenHabitat(void)
{
    struct Pokenav_Habitat *state = GetSubstructPtr(POKENAV_SUBSTRUCT_HABITAT);
    state->loopedTaskId = CreateLoopedTask(LoopedTask_OpenHabitat, 1);
    state->isTaskActiveCB = HabitatLoopTaskActive;
    return TRUE;
}

void CreateHabitatLoopedTask(s32 index)
{
    struct Pokenav_Habitat *state = GetSubstructPtr(POKENAV_SUBSTRUCT_HABITAT);
    LoopedTask task;

    switch (index)
    {
    case POKENAV_HABITAT_FUNC_UP: task = LoopedTask_HabitatMoveUp; break;
    case POKENAV_HABITAT_FUNC_DOWN: task = LoopedTask_HabitatMoveDown; break;
    case POKENAV_HABITAT_FUNC_PG_UP: task = LoopedTask_HabitatPageUp; break;
    case POKENAV_HABITAT_FUNC_PG_DOWN: task = LoopedTask_HabitatPageDown; break;
    case POKENAV_HABITAT_FUNC_EXIT: task = LoopedTask_HabitatExit; break;
    default: return;
    }
    state->loopedTaskId = CreateLoopedTask(task, 1);
}

bool32 IsHabitatLoopedTaskActive(void)
{
    struct Pokenav_Habitat *state = GetSubstructPtr(POKENAV_SUBSTRUCT_HABITAT);
    return state->isTaskActiveCB != NULL && state->isTaskActiveCB();
}

void FreeHabitatSubstruct1(void)
{
    FreePokenavSubstruct(POKENAV_SUBSTRUCT_HABITAT);
}

void FreeHabitatSubstruct2(void)
{
    struct Pokenav_Habitat *state = GetSubstructPtr(POKENAV_SUBSTRUCT_HABITAT);
    if (state == NULL)
        return;
    CleanupHabitatScreen();
    FreePokenavSubstruct(POKENAV_SUBSTRUCT_HABITAT);
}

static u32 LoopedTask_OpenHabitat(s32 taskState)
{
    struct Pokenav_Habitat *state = GetSubstructPtr(POKENAV_SUBSTRUCT_HABITAT);
    switch (taskState)
    {
    case 0:
        InitBgTemplates(sHabitatBgTemplates, ARRAY_COUNT(sHabitatBgTemplates));
        DecompressAndCopyTileDataToVram(1, sHabitatFrameGfx, 0, 0, 0);
        SetBgTilemapBuffer(1, state->bg1TilemapBuffer);
        CopyToBgTilemapBuffer(1, sHabitatFrameTilemap, 0, 0);
        CopyBgTilemapBufferToVram(1);
        SetBgTilemapBuffer(2, state->bg2TilemapBuffer);
        FillBgTilemapBufferRect_Palette0(2, 0, 0, 0, 32, 32);
        CopyBgTilemapBufferToVram(2);
        CopyPaletteIntoBufferUnfaded(sHabitatFramePal, BG_PLTT_ID(1), sizeof(sHabitatFramePal));
        state->windowId = AddWindow(&sHabitatWindowTemplate);
        CopyPaletteIntoBufferUnfaded(sHabitatListPal, BG_PLTT_ID(2), sizeof(sHabitatListPal));
        LoadMonIconPalettes();
        return LT_INC_AND_PAUSE;
    case 1:
        if (FreeTempTileDataBuffersIfPossible())
            return LT_PAUSE;
        FillWindowPixelBuffer(state->windowId, PIXEL_FILL(1));
        PutWindowTilemap(state->windowId);
        DrawHabitat();
        if (state->count > HABITAT_VISIBLE_ROWS)
        {
            state->scrollArrowTaskId = AddScrollIndicatorArrowPairParameterized(
                SCROLL_ARROW_UP, 224, 48, 160, state->count - HABITAT_VISIBLE_ROWS,
                TAG_NONE, TAG_NONE, &state->top);
            state->scrollArrowsLoaded = TRUE;
        }
        ShowBg(1);
        ShowBg(2);
        SetLeftHeaderSpritesInvisibility();
        PokenavFadeScreen(POKENAV_FADE_FROM_BLACK);
        return LT_INC_AND_PAUSE;
    case 2:
        if (IsPaletteFadeActive() || AreLeftHeaderSpritesMoving())
            return LT_PAUSE;
        return LT_FINISH;
    }
    return LT_FINISH;
}

static u32 MoveHabitatWindow(s32 delta)
{
    struct Pokenav_Habitat *state = GetSubstructPtr(POKENAV_SUBSTRUCT_HABITAT);
    s32 maxTop = state->count > HABITAT_VISIBLE_ROWS ? state->count - HABITAT_VISIBLE_ROWS : 0;
    state->top = (state->top + delta < 0) ? 0 : state->top + delta;
    if (state->top > maxTop)
        state->top = maxTop;
    DrawHabitat();
    return LT_FINISH;
}

static u32 LoopedTask_HabitatMoveUp(s32 state)
{
    return state == 0 ? MoveHabitatWindow(-1) : LT_FINISH;
}

static u32 LoopedTask_HabitatMoveDown(s32 state)
{
    return state == 0 ? MoveHabitatWindow(1) : LT_FINISH;
}

static u32 LoopedTask_HabitatPageUp(s32 state)
{
    return state == 0 ? MoveHabitatWindow(-HABITAT_VISIBLE_ROWS) : LT_FINISH;
}

static u32 LoopedTask_HabitatPageDown(s32 state)
{
    return state == 0 ? MoveHabitatWindow(HABITAT_VISIBLE_ROWS) : LT_FINISH;
}

static u32 LoopedTask_HabitatExit(s32 state)
{
    if (state == 0)
    {
        PlaySE(SE_SELECT);
        PokenavFadeScreen(POKENAV_FADE_TO_BLACK);
        SlideMenuHeaderDown();
        return LT_INC_AND_PAUSE;
    }
    if (IsPaletteFadeActive() || MainMenuLoopedTaskIsBusy())
        return LT_PAUSE;
    SetLeftHeaderSpritesInvisibility();
    HideBg(1);
    HideBg(2);
    HideBg(3);
    CleanupHabitatScreen();
    RestorePokenavMainMenuBg0();
    return LT_FINISH;
}

static bool32 HabitatLoopTaskActive(void)
{
    struct Pokenav_Habitat *state = GetSubstructPtr(POKENAV_SUBSTRUCT_HABITAT);
    return IsLoopedTaskActive(state->loopedTaskId);
}

static void DrawHabitat(void)
{
    struct Pokenav_Habitat *state = GetSubstructPtr(POKENAV_SUBSTRUCT_HABITAT);
    u8 areaName[32];
    const struct MapHeader *mapHeader;
    u16 i;

    DestroyHabitatIcons();
    FillWindowPixelBuffer(state->windowId, PIXEL_FILL(1));
    AddTextPrinterParameterized(state->windowId, FONT_NORMAL, COMPOUND_STRING("HABITAT"), 8, 1, TEXT_SKIP_DRAW, NULL);
    mapHeader = Overworld_GetMapHeaderByGroupAndId(gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum);
    GetMapName(areaName, mapHeader->regionMapSectionId, 0);
    AddTextPrinterParameterized(state->windowId, FONT_SMALL, areaName, 80, 1, TEXT_SKIP_DRAW, NULL);

    for (i = 0; i < HABITAT_VISIBLE_ROWS; i++)
    {
        u16 index = state->top + i;
        if (index >= state->count)
            break;
        AddTextPrinterParameterized(state->windowId, FONT_NORMAL, GetSpeciesName(state->species[index]), 42, 24 + i * 32, TEXT_SKIP_DRAW, NULL);
        state->icons[i] = &gSprites[CreateMonIconNoPersonality(state->species[index], SpriteCB_MonIcon, 24, 56 + i * 32, 1)];
    }
    if (state->count == 0)
        AddTextPrinterParameterized(state->windowId, FONT_SMALL, COMPOUND_STRING("NESSUN POKéMON SELVATICO\nDISPONIBILE"), 40, 72, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(state->windowId, COPYWIN_FULL);
}

static void DestroyHabitatIcons(void)
{
    struct Pokenav_Habitat *state = GetSubstructPtr(POKENAV_SUBSTRUCT_HABITAT);
    u16 i;
    if (state == NULL)
        return;
    for (i = 0; i < HABITAT_VISIBLE_ROWS; i++)
    {
        if (state->icons[i] != NULL)
        {
            FreeAndDestroyMonIconSprite(state->icons[i]);
            state->icons[i] = NULL;
        }
    }
}

static void CleanupHabitatScreen(void)
{
    struct Pokenav_Habitat *state = GetSubstructPtr(POKENAV_SUBSTRUCT_HABITAT);

    if (state == NULL)
        return;
    DestroyHabitatIcons();
    if (state->scrollArrowsLoaded)
    {
        RemoveScrollIndicatorArrowPair(state->scrollArrowTaskId);
        state->scrollArrowsLoaded = FALSE;
    }
    if (state->windowId != WINDOW_NONE)
    {
        ClearWindowTilemap(state->windowId);
        RemoveWindow(state->windowId);
        state->windowId = WINDOW_NONE;
    }
    UnsetBgTilemapBuffer(1);
    UnsetBgTilemapBuffer(2);
    FreeMonIconPalettes();
}
