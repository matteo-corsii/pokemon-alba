#include "global.h"
#include "bg.h"
#include "decompress.h"
#include "graphics.h"
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
    enum Species species[HABITAT_MAX_SPECIES];
    struct Sprite *icons[HABITAT_VISIBLE_ROWS];
    u8 bgTilemapBuffer[BG_SCREEN_SIZE];
    u32 loopedTaskId;
    bool32 (*isTaskActiveCB)(void);
    bool32 exitRequested;
};

static const u16 sHabitatUiPal[] = INCGFX_U16("graphics/pokenav/match_call/ui.png", ".gbapal");
static const u32 sHabitatUiGfx[] = INCGFX_U32("graphics/pokenav/match_call/ui.png", ".4bpp.smol", "-num_tiles 13 -Wnum_tiles");
static const u32 sHabitatUiTilemap[] = INCGFX_U32("graphics/pokenav/match_call/ui.bin", ".smolTM");
static const u16 sHabitatListPal[] = INCGFX_U16("graphics/pokenav/match_call/list_window.pal", ".gbapal");

static const struct BgTemplate sHabitatBgTemplates[] =
{
    {
        .bg = 1, .charBaseIndex = 3, .mapBaseIndex = 0x1F, .screenSize = 0,
        .paletteMode = 0, .priority = 1, .baseTile = 0
    },
    {
        .bg = 2, .charBaseIndex = 2, .mapBaseIndex = 0x06, .screenSize = 0,
        .paletteMode = 0, .priority = 2, .baseTile = 0x80
    },
};

static const struct WindowTemplate sHabitatWindowTemplate =
{
    .bg = 1, .tilemapLeft = 1, .tilemapTop = 1, .width = 28, .height = 19,
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
    DestroyHabitatIcons();
    if (state->windowId != WINDOW_NONE)
        RemoveWindow(state->windowId);
    FreeMonIconPalettes();
    FreePokenavSubstruct(POKENAV_SUBSTRUCT_HABITAT);
}

static u32 LoopedTask_OpenHabitat(s32 taskState)
{
    struct Pokenav_Habitat *state = GetSubstructPtr(POKENAV_SUBSTRUCT_HABITAT);
    switch (taskState)
    {
    case 0:
        InitBgTemplates(sHabitatBgTemplates, ARRAY_COUNT(sHabitatBgTemplates));
        DecompressAndCopyTileDataToVram(1, sHabitatUiGfx, 0, 0, 0);
        DecompressAndCopyTileDataToVram(2, sHabitatUiGfx, 0, 0, 0);
        SetBgTilemapBuffer(2, state->bgTilemapBuffer);
        CopyToBgTilemapBuffer(2, sHabitatUiTilemap, 0, 0);
        CopyPaletteIntoBufferUnfaded(sHabitatUiPal, BG_PLTT_ID(2), sizeof(sHabitatUiPal));
        state->windowId = AddWindow(&sHabitatWindowTemplate);
        CopyPaletteIntoBufferUnfaded(sHabitatListPal, BG_PLTT_ID(1), sizeof(sHabitatListPal));
        LoadMonIconPalettes();
        return LT_INC_AND_PAUSE;
    case 1:
        if (FreeTempTileDataBuffersIfPossible())
            return LT_PAUSE;
        FillWindowPixelBuffer(state->windowId, PIXEL_FILL(1));
        PutWindowTilemap(state->windowId);
        DrawHabitat();
        ShowBg(1);
        ShowBg(2);
        LoadLeftHeaderGfxForIndex(POKENAV_GFX_MAIN_MENU);
        ShowLeftHeaderGfx(POKENAV_GFX_MAIN_MENU, TRUE, FALSE);
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
    struct PokenavHabitatListItem item;
    u8 areaName[32];
    u16 i;

    DestroyHabitatIcons();
    FillWindowPixelBuffer(state->windowId, PIXEL_FILL(1));
    AddTextPrinterParameterized(state->windowId, FONT_NORMAL, COMPOUND_STRING("HABITAT"), 8, 1, TEXT_SKIP_DRAW, NULL);
    GetMapName(areaName, GetCurrentRegionMapSectionId(), 0);
    AddTextPrinterParameterized(state->windowId, FONT_SMALL, areaName, 80, 1, TEXT_SKIP_DRAW, NULL);

    for (i = 0; i < HABITAT_VISIBLE_ROWS; i++)
    {
        u16 index = state->top + i;
        if (index >= state->count)
            break;
        item.species = state->species[index];
        AddTextPrinterParameterized(state->windowId, FONT_NORMAL, GetSpeciesName(item.species), 42, 32 + i * 32, TEXT_SKIP_DRAW, NULL);
        state->icons[i] = &gSprites[CreateMonIconNoPersonality(item.species, SpriteCB_MonIcon, 24, 48 + i * 32, 1)];
    }
    if (state->count == 0)
        AddTextPrinterParameterized(state->windowId, FONT_NORMAL, COMPOUND_STRING("NO WILD POKEMON"), 24, 72, TEXT_SKIP_DRAW, NULL);
    if (state->top != 0)
        AddTextPrinterParameterized(state->windowId, FONT_SMALL, COMPOUND_STRING("UP"), 216, 136, TEXT_SKIP_DRAW, NULL);
    if (state->top + HABITAT_VISIBLE_ROWS < state->count)
        AddTextPrinterParameterized(state->windowId, FONT_SMALL, COMPOUND_STRING("DOWN"), 216, 144, TEXT_SKIP_DRAW, NULL);
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
