#include "global.h"
#include "quest_log.h"
#include "quest_log_internal.h"
#include "event_data.h"
#include "list_menu.h"
#include "main.h"
#include "menu.h"
#include "overworld.h"
#include "palette.h"
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
#define QUEST_LOG_WINDOW_WIDTH 28
#define QUEST_LOG_WINDOW_HEIGHT 18
#define QUEST_LOG_WINDOW_BASE_BLOCK 0x200
#define QUEST_LOG_MAX_LIST_ITEMS 8
#define QUEST_LOG_MAX_QUESTS 32
#define QUEST_LOG_TITLE_BUFFER_SIZE 72

enum QuestLogPage
{
    QUEST_LOG_PAGE_CATEGORIES,
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

static EWRAM_DATA u8 sQuestLogWindowId = WINDOW_NONE;
static EWRAM_DATA u8 sQuestLogTaskId = TASK_NONE;
static EWRAM_DATA u8 sQuestLogListTaskId = TASK_NONE;
static EWRAM_DATA u8 sQuestLogPage = QUEST_LOG_PAGE_CATEGORIES;
static EWRAM_DATA u8 sQuestLogCategory = QUEST_LOG_CATEGORY_STORY;
static EWRAM_DATA u8 sQuestLogQuestIndex = 0;
static EWRAM_DATA struct ListMenuItem sQuestLogListItems[QUEST_LOG_MAX_QUESTS + 1];
static EWRAM_DATA u8 sQuestLogListNames[QUEST_LOG_MAX_QUESTS][QUEST_LOG_TITLE_BUFFER_SIZE];

static const struct ListMenuItem sQuestLogCategories[] =
{
    { _("STORIA"), QUEST_LOG_CATEGORY_STORY },
    { _("SECONDARIE"), QUEST_LOG_CATEGORY_SIDE },
    { _("INCARICHI"), QUEST_LOG_CATEGORY_JOBS },
    { NULL, LIST_CANCEL },
};

static const struct WindowTemplate sQuestLogWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = QUEST_LOG_WINDOW_LEFT,
    .tilemapTop = QUEST_LOG_WINDOW_TOP,
    .width = QUEST_LOG_WINDOW_WIDTH,
    .height = QUEST_LOG_WINDOW_HEIGHT,
    .paletteNum = 15,
    .baseBlock = QUEST_LOG_WINDOW_BASE_BLOCK,
};

static void Task_QuestLog(u8 taskId);
static void QuestLog_ShowCategories(void);
static void QuestLog_ShowQuestList(void);
static void QuestLog_ShowQuestDetail(void);
static void QuestLog_DrawList(const u8 *header, const struct ListMenuItem *items, u8 count);
static void QuestLog_RemoveList(void);
static bool8 QuestLog_EvaluateCondition(const struct QuestLogCondition *condition);
static bool8 QuestLog_EvaluateClause(const struct QuestLogConditionClause *clause);
static u8 QuestLog_GetState(const struct QuestLogQuest *quest);
static const u8 *QuestLog_GetCurrentObjective(const struct QuestLogQuest *quest);
static u8 QuestLog_FindQuestIndex(u16 questId);

bool8 QuestLog_StartMenuCallback(void)
{
    if (gPaletteFade.active)
        return FALSE;

    RemoveStartMenuWindow();
    CleanupOverworldWindowsAndTilemaps();

    sQuestLogPage = QUEST_LOG_PAGE_CATEGORIES;
    sQuestLogCategory = QUEST_LOG_CATEGORY_STORY;
    sQuestLogQuestIndex = 0;
    sQuestLogListTaskId = TASK_NONE;
    sQuestLogWindowId = AddWindow(&sQuestLogWindowTemplate);
    sQuestLogTaskId = CreateTask(Task_QuestLog, 0x50);
    QuestLog_ShowCategories();
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    return TRUE;
}

static u8 QuestLog_GetState(const struct QuestLogQuest *quest)
{
    if (!QuestLog_EvaluateCondition(quest->discoverWhen))
        return QUEST_LOG_STATE_HIDDEN;
    if (quest->completeWhen != NULL && QuestLog_EvaluateCondition(quest->completeWhen))
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
    return _("Nessun obiettivo attivo.");
}

static u8 QuestLog_FindQuestIndex(u16 questId)
{
    u8 i;

    for (i = 0; i < gQuestLogQuestCount; i++)
    {
        if (gQuestLogQuests[i].id == questId)
            return i;
    }
    return 0;
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
    QuestLog_DrawList(_("DIARIO"), sQuestLogCategories, QUEST_LOG_CATEGORY_COUNT);
}

static void QuestLog_ShowQuestList(void)
{
    u8 i;
    u8 count = 0;

    for (i = 0; i < gQuestLogQuestCount && count < QUEST_LOG_MAX_QUESTS; i++)
    {
        const struct QuestLogQuest *quest = &gQuestLogQuests[i];
        if (quest->category != sQuestLogCategory || QuestLog_GetState(quest) == QUEST_LOG_STATE_HIDDEN)
            continue;

        if (QuestLog_GetState(quest) == QUEST_LOG_STATE_COMPLETED)
            StringCopy(sQuestLogListNames[count], _("[OK] "));
        else
            StringCopy(sQuestLogListNames[count], _("     "));
        StringAppend(sQuestLogListNames[count], quest->title);
        sQuestLogListItems[count].name = sQuestLogListNames[count];
        sQuestLogListItems[count].id = quest->id;
        count++;
    }

    sQuestLogListItems[count].name = NULL;
    sQuestLogListItems[count].id = LIST_CANCEL;
    if (count == 0)
    {
        sQuestLogPage = QUEST_LOG_PAGE_EMPTY;
        QuestLog_DrawList(gQuestLogCategoryNames[sQuestLogCategory], NULL, 0);
        AddTextPrinterParameterized(sQuestLogWindowId, FONT_NORMAL, _("Nessuna quest scoperta."), 12, 40, TEXT_SKIP_DRAW, NULL);
        CopyWindowToVram(sQuestLogWindowId, COPYWIN_GFX);
        return;
    }

    sQuestLogPage = QUEST_LOG_PAGE_QUESTS;
    QuestLog_DrawList(gQuestLogCategoryNames[sQuestLogCategory], sQuestLogListItems, count);
}

static void QuestLog_ShowQuestDetail(void)
{
    const struct QuestLogQuest *quest = &gQuestLogQuests[sQuestLogQuestIndex];
    u8 state = QuestLog_GetState(quest);

    sQuestLogPage = QUEST_LOG_PAGE_DETAIL;
    QuestLog_RemoveList();
    DrawStdWindowFrame(sQuestLogWindowId, FALSE);
    AddTextPrinterParameterized(sQuestLogWindowId, FONT_NORMAL, quest->title, 8, 5, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(sQuestLogWindowId, FONT_SMALL, _("CATEGORIA:"), 8, 25, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(sQuestLogWindowId, FONT_SMALL, gQuestLogCategoryNames[quest->category], 84, 25, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(sQuestLogWindowId, FONT_SMALL, _("STATO:"), 8, 35, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(sQuestLogWindowId, FONT_SMALL, state == QUEST_LOG_STATE_COMPLETED ? _("COMPLETATA") : _("ATTIVA"), 52, 35, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(sQuestLogWindowId, FONT_SMALL, _("DESCRIZIONE"), 8, 53, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(sQuestLogWindowId, FONT_SMALL, quest->description, 8, 63, TEXT_SKIP_DRAW, NULL);
    if (state == QUEST_LOG_STATE_ACTIVE)
    {
        AddTextPrinterParameterized(sQuestLogWindowId, FONT_SMALL, _("OBIETTIVO ATTUALE"), 8, 99, TEXT_SKIP_DRAW, NULL);
        AddTextPrinterParameterized(sQuestLogWindowId, FONT_SMALL, QuestLog_GetCurrentObjective(quest), 8, 109, TEXT_SKIP_DRAW, NULL);
    }
    else
    {
        AddTextPrinterParameterized(sQuestLogWindowId, FONT_SMALL, _("COMPLETATA"), 8, 109, TEXT_SKIP_DRAW, NULL);
    }
    AddTextPrinterParameterized(sQuestLogWindowId, FONT_SMALL, _("B: INDIETRO"), 8, 129, TEXT_SKIP_DRAW, NULL);
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
            QuestLog_ShowCategories();
        else
            QuestLog_ReturnToStartMenu();
        return;
    }

    PlaySE(SE_SELECT);
    if (sQuestLogPage == QUEST_LOG_PAGE_CATEGORIES)
    {
        sQuestLogCategory = input;
        QuestLog_ShowQuestList();
    }
    else
    {
        sQuestLogQuestIndex = QuestLog_FindQuestIndex(input);
        QuestLog_ShowQuestDetail();
    }
}
