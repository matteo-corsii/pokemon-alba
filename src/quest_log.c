#include "global.h"
#include "quest_log.h"
#include "quest_log_internal.h"
#include "event_data.h"
#include "bg.h"
#include "gpu_regs.h"
#include "list_menu.h"
#include "main.h"
#include "menu.h"
#include "overworld.h"
#include "palette.h"
#include "scanline_effect.h"
#include "sound.h"
#include "start_menu.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "window.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#define QUEST_LOG_WINDOW_LEFT 1
#define QUEST_LOG_WINDOW_TOP 1
#define QUEST_LOG_WINDOW_BASE_BLOCK 0
#define QUEST_LOG_MAX_LIST_ITEMS 8
#define QUEST_LOG_MAX_QUESTS 64
#define QUEST_LOG_MAX_GROUPS 64
#define QUEST_LOG_TITLE_BUFFER_SIZE 72

enum QuestLogPage
{
    QUEST_LOG_PAGE_CATEGORIES,
    QUEST_LOG_PAGE_GROUPS,
    QUEST_LOG_PAGE_QUESTS,
    QUEST_LOG_PAGE_EMPTY,
    QUEST_LOG_PAGE_DETAIL,
};

enum QuestLogState
{
    QUEST_LOG_STATE_HIDDEN,
    QUEST_LOG_STATE_ACTIVE,
    QUEST_LOG_STATE_COMPLETED,
};

static EWRAM_DATA u8 sQuestLogWindowId;
static EWRAM_DATA u8 sQuestLogTaskId;
static EWRAM_DATA u8 sQuestLogListTaskId;
static EWRAM_DATA u8 sQuestLogPage = QUEST_LOG_PAGE_CATEGORIES;
static EWRAM_DATA u8 sQuestLogCategory = QUEST_LOG_CATEGORY_STORY;
static EWRAM_DATA u16 sQuestLogGroupId = 0;
static EWRAM_DATA u8 sQuestLogEmptyParentPage = QUEST_LOG_PAGE_CATEGORIES;
static EWRAM_DATA u8 sQuestLogQuestIndex = 0;
static EWRAM_DATA struct ListMenuItem sQuestLogListItems[QUEST_LOG_MAX_QUESTS + 1];
static EWRAM_DATA u8 sQuestLogListNames[QUEST_LOG_MAX_QUESTS][QUEST_LOG_TITLE_BUFFER_SIZE];
static EWRAM_DATA struct ListMenuItem sQuestLogGroupItems[QUEST_LOG_MAX_GROUPS + 1];
static EWRAM_DATA u8 sQuestLogGroupNames[QUEST_LOG_MAX_GROUPS][QUEST_LOG_TITLE_BUFFER_SIZE];

static const struct ListMenuItem sQuestLogCategories[] =
{
    { COMPOUND_STRING("STORIA"), QUEST_LOG_CATEGORY_STORY },
    { COMPOUND_STRING("SECONDARIE"), QUEST_LOG_CATEGORY_SIDE },
    { COMPOUND_STRING("INCARICHI"), QUEST_LOG_CATEGORY_JOBS },
    { NULL, LIST_CANCEL },
};

static const struct BgTemplate sQuestLogBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0,
    },
};

static const struct WindowTemplate sQuestLogInitWindowTemplates[] =
{
    DUMMY_WIN_TEMPLATE,
};

static const struct WindowTemplate sQuestLogWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = QUEST_LOG_WINDOW_LEFT,
    .tilemapTop = QUEST_LOG_WINDOW_TOP,
    .width = QUEST_LOG_WINDOW_WIDTH_TILES,
    .height = QUEST_LOG_WINDOW_HEIGHT_TILES,
    .paletteNum = 15,
    .baseBlock = QUEST_LOG_WINDOW_BASE_BLOCK,
};

static void Task_QuestLog(u8 taskId);
static void CB2_InitQuestLog(void);
static void CB2_QuestLog(void);
static void VBlankCB_QuestLog(void);
static void QuestLog_InitDisplay(void);
static void QuestLog_ShowCategories(void);
static void QuestLog_ShowGroups(void);
static void QuestLog_ShowQuestList(void);
static void QuestLog_ShowQuestDetail(void);
static void QuestLog_DrawList(const u8 *header, const struct ListMenuItem *items, u8 count);
static void QuestLog_RemoveList(void);
static bool8 QuestLog_EvaluateCondition(const struct QuestLogCondition *condition);
static bool8 QuestLog_EvaluateClause(const struct QuestLogConditionClause *clause);
static u8 QuestLog_GetState(const struct QuestLogQuest *quest);
static u8 QuestLog_GetGroupState(const struct QuestLogGroup *group);
static bool8 QuestLog_GroupHasVisibleQuest(u16 groupId);
static const u8 *QuestLog_GetCurrentObjective(const struct QuestLogQuest *quest);
static u8 QuestLog_FindQuestIndex(u16 questId);
static const struct QuestLogGroup *QuestLog_FindGroup(u16 groupId);

bool8 QuestLog_StartMenuCallback(void)
{
    if (gPaletteFade.active)
        return FALSE;

    sQuestLogWindowId = WINDOW_NONE;
    sQuestLogTaskId = TASK_NONE;
    sQuestLogListTaskId = TASK_NONE;

    RemoveStartMenuWindow();
    CleanupOverworldWindowsAndTilemaps();

    sQuestLogPage = QUEST_LOG_PAGE_CATEGORIES;
    sQuestLogCategory = QUEST_LOG_CATEGORY_STORY;
    sQuestLogGroupId = 0;
    sQuestLogQuestIndex = 0;
    sQuestLogListTaskId = TASK_NONE;
    SetMainCallback2(CB2_InitQuestLog);
    return TRUE;
}

static void CB2_InitQuestLog(void)
{
    QuestLog_InitDisplay();

    sQuestLogWindowId = AddWindow(&sQuestLogWindowTemplate);
    if (sQuestLogWindowId == WINDOW_NONE)
    {
        SetMainCallback2(CB2_ReturnToFieldWithOpenMenu);
        return;
    }

    sQuestLogTaskId = CreateTask(Task_QuestLog, 0x50);
    QuestLog_ShowCategories();
    ShowBg(0);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    SetVBlankCallback(VBlankCB_QuestLog);
    SetMainCallback2(CB2_QuestLog);
}

static void QuestLog_InitDisplay(void)
{
    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
    SetGpuReg(REG_OFFSET_DISPCNT, 0);
    SetGpuReg(REG_OFFSET_BG0CNT, 0);
    SetGpuReg(REG_OFFSET_BG1CNT, 0);
    SetGpuReg(REG_OFFSET_BG2CNT, 0);
    SetGpuReg(REG_OFFSET_BG3CNT, 0);
    SetGpuReg(REG_OFFSET_BG0HOFS, 0);
    SetGpuReg(REG_OFFSET_BG0VOFS, 0);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
    ResetTasks();
    ScanlineEffect_Stop();
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sQuestLogBgTemplates, ARRAY_COUNT(sQuestLogBgTemplates));
    ChangeBgX(0, 0, BG_COORD_SET);
    ChangeBgY(0, 0, BG_COORD_SET);
    ClearScheduledBgCopiesToVram();
    InitWindows(sQuestLogInitWindowTemplates);
    DeactivateAllTextPrinters();
    LoadMessageBoxAndBorderGfx();
    Menu_LoadStdPalAt(BG_PLTT_ID(15));
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0);
}

static void CB2_QuestLog(void)
{
    RunTasks();
    UpdatePaletteFade();
    DoScheduledBgTilemapCopiesToVram();
}

static void VBlankCB_QuestLog(void)
{
    TransferPlttBuffer();
}

static u8 QuestLog_GetState(const struct QuestLogQuest *quest)
{
    if (!QuestLog_EvaluateCondition(quest->discoverWhen))
        return QUEST_LOG_STATE_HIDDEN;
    if (quest->completeWhen != NULL && QuestLog_EvaluateCondition(quest->completeWhen))
        return QUEST_LOG_STATE_COMPLETED;
    return QUEST_LOG_STATE_ACTIVE;
}

static bool8 QuestLog_GroupHasVisibleQuest(u16 groupId)
{
    u8 i;

    for (i = 0; i < gQuestLogQuestCount; i++)
    {
        if (gQuestLogQuests[i].groupId == groupId && QuestLog_GetState(&gQuestLogQuests[i]) != QUEST_LOG_STATE_HIDDEN)
            return TRUE;
    }

    return FALSE;
}

static u8 QuestLog_GetGroupState(const struct QuestLogGroup *group)
{
    u8 i;
    bool8 hasVisibleQuest = FALSE;

    for (i = 0; i < gQuestLogQuestCount; i++)
    {
        const struct QuestLogQuest *quest = &gQuestLogQuests[i];

        if (quest->groupId != group->id || QuestLog_GetState(quest) == QUEST_LOG_STATE_HIDDEN)
            continue;

        hasVisibleQuest = TRUE;
        if (QuestLog_GetState(quest) != QUEST_LOG_STATE_COMPLETED)
            return QUEST_LOG_STATE_ACTIVE;
    }

    if (!hasVisibleQuest)
        return QUEST_LOG_STATE_HIDDEN;
    if (group->completionPolicy == QUEST_LOG_GROUP_COMPLETION_ALL_VISIBLE_QUESTS)
        return QUEST_LOG_STATE_COMPLETED;
    return QUEST_LOG_STATE_ACTIVE;
}

static bool8 QuestLog_EvaluateCondition(const struct QuestLogCondition *condition)
{
    u8 i;

    if (condition == NULL)
        return FALSE;

    for (i = 0; i < condition->allCount; i++)
    {
        if (!QuestLog_EvaluateClause(&condition->allClauses[i]))
            return FALSE;
    }

    if (condition->anyCount == 0)
        return TRUE;

    for (i = 0; i < condition->anyCount; i++)
    {
        if (QuestLog_EvaluateClause(&condition->anyClauses[i]))
            return TRUE;
    }

    return FALSE;
}

static bool8 QuestLog_EvaluateClause(const struct QuestLogConditionClause *clause)
{
    switch (clause->type)
    {
    case QUEST_LOG_CONDITION_FLAG_SET:
        return FlagGet(clause->subject);
    case QUEST_LOG_CONDITION_FLAG_CLEAR:
        return !FlagGet(clause->subject);
    case QUEST_LOG_CONDITION_VAR_EQUALS:
        return VarGet(clause->subject) == clause->value;
    case QUEST_LOG_CONDITION_VAR_GREATER_EQUAL:
        return VarGet(clause->subject) >= clause->value;
    case QUEST_LOG_CONDITION_PLAYER_SECRET_BASE_ID:
        return gSaveBlock1Ptr->secretBases[0].secretBaseId == clause->value
            || gSaveBlock1Ptr->secretBases[0].secretBaseId == clause->secondValue;
    case QUEST_LOG_CONDITION_UNTRACKED:
    default:
        return FALSE;
    }
}

static const u8 *QuestLog_GetCurrentObjective(const struct QuestLogQuest *quest)
{
    u8 i;

    for (i = 0; i < quest->stepCount; i++)
    {
        if (!QuestLog_EvaluateCondition(quest->steps[i].completeWhen))
            return quest->steps[i].objective;
    }
    return COMPOUND_STRING("Nessun obiettivo attivo.");
}

static u8 QuestLog_FindQuestIndex(u16 questId)
{
    u8 i;

    for (i = 0; i < gQuestLogQuestCount; i++)
    {
        if (gQuestLogQuests[i].id == questId)
            return i;
    }
    return QUEST_LOG_MAX_QUESTS;
}

static const struct QuestLogGroup *QuestLog_FindGroup(u16 groupId)
{
    u8 i;

    for (i = 0; i < gQuestLogGroupCount; i++)
    {
        if (gQuestLogGroups[i].id == groupId)
            return &gQuestLogGroups[i];
    }

    return NULL;
}

static void QuestLog_RemoveList(void)
{
    if (sQuestLogListTaskId != TASK_NONE)
    {
        DestroyListMenuTask(sQuestLogListTaskId, NULL, NULL);
        sQuestLogListTaskId = TASK_NONE;
    }
}

static void QuestLog_DrawList(const u8 *header, const struct ListMenuItem *items, u8 count)
{
    struct ListMenuTemplate template;

    QuestLog_RemoveList();
    DrawStdWindowFrame(sQuestLogWindowId, FALSE);
    AddTextPrinterParameterized(sQuestLogWindowId, FONT_SMALL, header, 8, 2, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(sQuestLogWindowId, COPYWIN_FULL);

    if (count == 0)
        return;

    memset(&template, 0, sizeof(template));
    template.items = items;
    template.itemPrintFunc = NULL;
    template.totalItems = count;
    template.maxShowed = QUEST_LOG_MAX_LIST_ITEMS;
    template.windowId = sQuestLogWindowId;
    template.item_X = 12;
    template.cursor_X = 0;
    template.upText_Y = 12;
    template.cursorPal = 2;
    template.fillValue = 1;
    template.cursorShadowPal = 3;
    template.fontId = FONT_SMALL;
    template.cursorKind = CURSOR_BLACK_ARROW;
    template.scrollMultiple = LIST_MULTIPLE_SCROLL_DPAD;
    sQuestLogListTaskId = ListMenuInit(&template, 0, 0);
}

static void QuestLog_ShowCategories(void)
{
    sQuestLogPage = QUEST_LOG_PAGE_CATEGORIES;
    QuestLog_DrawList(COMPOUND_STRING("DIARIO"), sQuestLogCategories, QUEST_LOG_CATEGORY_COUNT);
}

static void QuestLog_ShowGroups(void)
{
    const struct QuestLogGroup *visibleGroups[QUEST_LOG_MAX_GROUPS];
    u8 count = 0;
    u8 i, j;

    for (i = 0; i < gQuestLogGroupCount && count < QUEST_LOG_MAX_GROUPS; i++)
    {
        const struct QuestLogGroup *group = &gQuestLogGroups[i];
        u8 insertAt;

        if (group->category != sQuestLogCategory || !QuestLog_GroupHasVisibleQuest(group->id))
            continue;

        insertAt = count;
        while (insertAt > 0
            && (visibleGroups[insertAt - 1]->order > group->order
                || (visibleGroups[insertAt - 1]->order == group->order && visibleGroups[insertAt - 1]->id > group->id)))
        {
            visibleGroups[insertAt] = visibleGroups[insertAt - 1];
            insertAt--;
        }
        visibleGroups[insertAt] = group;
        count++;
    }

    for (j = 0; j < count; j++)
    {
        const struct QuestLogGroup *group = visibleGroups[j];
        u8 state = QuestLog_GetGroupState(group);

        if (state == QUEST_LOG_STATE_COMPLETED)
            StringCopy(sQuestLogGroupNames[j], COMPOUND_STRING("OK   "));
        else
            StringCopy(sQuestLogGroupNames[j], COMPOUND_STRING("     "));
        StringAppend(sQuestLogGroupNames[j], group->title);
        sQuestLogGroupItems[j].name = sQuestLogGroupNames[j];
        sQuestLogGroupItems[j].id = group->id;
    }

    sQuestLogGroupItems[count].name = NULL;
    sQuestLogGroupItems[count].id = LIST_CANCEL;
    if (count == 0)
    {
        sQuestLogPage = QUEST_LOG_PAGE_EMPTY;
        sQuestLogEmptyParentPage = QUEST_LOG_PAGE_CATEGORIES;
        QuestLog_DrawList(gQuestLogCategoryNames[sQuestLogCategory], NULL, 0);
        AddTextPrinterParameterized(sQuestLogWindowId, FONT_NORMAL, COMPOUND_STRING("Nessuna quest scoperta."), 12, 40, TEXT_SKIP_DRAW, NULL);
        CopyWindowToVram(sQuestLogWindowId, COPYWIN_GFX);
        return;
    }

    sQuestLogPage = QUEST_LOG_PAGE_GROUPS;
    QuestLog_DrawList(gQuestLogCategoryNames[sQuestLogCategory], sQuestLogGroupItems, count);
}

static void QuestLog_ShowQuestList(void)
{
    const struct QuestLogQuest *visibleQuests[QUEST_LOG_MAX_QUESTS];
    const struct QuestLogGroup *group = QuestLog_FindGroup(sQuestLogGroupId);
    u8 i;
    u8 count = 0;

    for (i = 0; i < gQuestLogQuestCount && count < QUEST_LOG_MAX_QUESTS; i++)
    {
        const struct QuestLogQuest *quest = &gQuestLogQuests[i];
        u8 insertAt;

        if (quest->groupId != sQuestLogGroupId || QuestLog_GetState(quest) == QUEST_LOG_STATE_HIDDEN)
            continue;

        insertAt = count;
        while (insertAt > 0
            && (visibleQuests[insertAt - 1]->order > quest->order
                || (visibleQuests[insertAt - 1]->order == quest->order && visibleQuests[insertAt - 1]->id > quest->id)))
        {
            visibleQuests[insertAt] = visibleQuests[insertAt - 1];
            insertAt--;
        }
        visibleQuests[insertAt] = quest;
        count++;
    }

    for (i = 0; i < count; i++)
    {
        const struct QuestLogQuest *quest = visibleQuests[i];

        if (QuestLog_GetState(quest) == QUEST_LOG_STATE_COMPLETED)
            StringCopy(sQuestLogListNames[i], COMPOUND_STRING("OK   "));
        else
            StringCopy(sQuestLogListNames[i], COMPOUND_STRING("     "));
        StringAppend(sQuestLogListNames[i], quest->title);
        sQuestLogListItems[i].name = sQuestLogListNames[i];
        sQuestLogListItems[i].id = quest->id;
    }

    sQuestLogListItems[count].name = NULL;
    sQuestLogListItems[count].id = LIST_CANCEL;
    if (count == 0)
    {
        sQuestLogPage = QUEST_LOG_PAGE_EMPTY;
        sQuestLogEmptyParentPage = QUEST_LOG_PAGE_GROUPS;
        QuestLog_DrawList(group != NULL ? group->title : gQuestLogCategoryNames[sQuestLogCategory], NULL, 0);
        AddTextPrinterParameterized(sQuestLogWindowId, FONT_NORMAL, COMPOUND_STRING("Nessuna quest scoperta."), 12, 40, TEXT_SKIP_DRAW, NULL);
        CopyWindowToVram(sQuestLogWindowId, COPYWIN_GFX);
        return;
    }

    sQuestLogPage = QUEST_LOG_PAGE_QUESTS;
    QuestLog_DrawList(group != NULL ? group->title : gQuestLogCategoryNames[sQuestLogCategory], sQuestLogListItems, count);
}

static void QuestLog_ShowQuestDetail(void)
{
    const struct QuestLogQuest *quest = &gQuestLogQuests[sQuestLogQuestIndex];

    sQuestLogPage = QUEST_LOG_PAGE_DETAIL;
    QuestLog_RemoveList();
    DrawStdWindowFrame(sQuestLogWindowId, FALSE);
    AddTextPrinterParameterized(sQuestLogWindowId, FONT_NORMAL, quest->title, 8, QUEST_LOG_DETAIL_TITLE_Y, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(sQuestLogWindowId, FONT_SMALL, quest->description, 8, QUEST_LOG_DETAIL_DESCRIPTION_Y, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(sQuestLogWindowId, FONT_SMALL, COMPOUND_STRING("OBIETTIVO"), 8, QUEST_LOG_DETAIL_OBJECTIVE_LABEL_Y, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(sQuestLogWindowId, FONT_SMALL, QuestLog_GetCurrentObjective(quest), 8, QUEST_LOG_DETAIL_OBJECTIVE_Y, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(sQuestLogWindowId, FONT_SMALL, COMPOUND_STRING("B: INDIETRO"), 8, QUEST_LOG_DETAIL_BACK_Y, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(sQuestLogWindowId, COPYWIN_FULL);
}

static void QuestLog_ReturnToStartMenu(void)
{
    QuestLog_RemoveList();
    ClearStdWindowAndFrame(sQuestLogWindowId, TRUE);
    RemoveWindow(sQuestLogWindowId);
    sQuestLogWindowId = WINDOW_NONE;
    DestroyTask(sQuestLogTaskId);
    sQuestLogTaskId = TASK_NONE;
    CleanupOverworldWindowsAndTilemaps();
    SetMainCallback2(CB2_ReturnToFieldWithOpenMenu);
}

static void Task_QuestLog(u8 taskId)
{
    s32 input;

    (void)taskId;

    if (gPaletteFade.active)
        return;

    if (sQuestLogPage == QUEST_LOG_PAGE_DETAIL)
    {
        if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            QuestLog_ShowQuestList();
        }
        return;
    }

    if (sQuestLogPage == QUEST_LOG_PAGE_EMPTY)
    {
        if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            if (sQuestLogEmptyParentPage == QUEST_LOG_PAGE_GROUPS)
                QuestLog_ShowGroups();
            else
                QuestLog_ShowCategories();
        }
        return;
    }

    input = ListMenu_ProcessInput(sQuestLogListTaskId);
    if (input == LIST_NOTHING_CHOSEN)
        return;

    if (input == LIST_CANCEL)
    {
        PlaySE(SE_SELECT);
        if (sQuestLogPage == QUEST_LOG_PAGE_QUESTS)
            QuestLog_ShowGroups();
        else
            QuestLog_ReturnToStartMenu();
        return;
    }

    PlaySE(SE_SELECT);
    if (sQuestLogPage == QUEST_LOG_PAGE_CATEGORIES)
    {
        sQuestLogCategory = input;
        QuestLog_ShowGroups();
    }
    else if (sQuestLogPage == QUEST_LOG_PAGE_GROUPS)
    {
        sQuestLogGroupId = input;
        QuestLog_ShowQuestList();
    }
    else
    {
        sQuestLogQuestIndex = QuestLog_FindQuestIndex(input);
        if (sQuestLogQuestIndex >= gQuestLogQuestCount)
            return;
        QuestLog_ShowQuestDetail();
    }
}
