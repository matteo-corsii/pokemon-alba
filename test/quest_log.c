#include "global.h"
#include "quest_log_internal.h"
#include "string_util.h"
#include "text.h"
#include "test/test.h"

static u8 QuestLog_TestCountLines(const u8 *text)
{
    u8 lines = 1;

    if (text == NULL || *text == EOS)
        return 0;

    while (*text != EOS)
    {
        if (*text == CHAR_NEWLINE)
            lines++;
        text++;
    }

    return lines;
}

TEST("Quest journal detail layout fits the window vertically")
{
    u32 category;
    u32 normalLineHeight = GetFontAttribute(FONT_NORMAL, FONTATTR_MAX_LETTER_HEIGHT)
        + GetFontAttribute(FONT_NORMAL, FONTATTR_LINE_SPACING);
    u32 smallLineHeight = GetFontAttribute(FONT_SMALL, FONTATTR_MAX_LETTER_HEIGHT)
        + GetFontAttribute(FONT_SMALL, FONTATTR_LINE_SPACING);
    u32 titleBottom = QUEST_LOG_DETAIL_TITLE_Y + normalLineHeight;
    u32 descriptionBottom = QUEST_LOG_DETAIL_DESCRIPTION_Y + 3 * smallLineHeight;
    u32 objectiveLabelBottom = QUEST_LOG_DETAIL_OBJECTIVE_LABEL_Y + smallLineHeight;
    u32 objectiveBottom = QUEST_LOG_DETAIL_OBJECTIVE_Y + 2 * smallLineHeight;
    u32 backBottom = QUEST_LOG_DETAIL_BACK_Y + smallLineHeight;

    EXPECT_LE(titleBottom, QUEST_LOG_DETAIL_DESCRIPTION_Y);
    EXPECT_LE(descriptionBottom, QUEST_LOG_DETAIL_OBJECTIVE_LABEL_Y);
    EXPECT_LE(objectiveLabelBottom, QUEST_LOG_DETAIL_OBJECTIVE_Y);
    EXPECT_LE(objectiveBottom, QUEST_LOG_DETAIL_BACK_Y);
    EXPECT_LE(backBottom + QUEST_LOG_DETAIL_BOTTOM_MARGIN, QUEST_LOG_DETAIL_CONTENT_BOTTOM);
    EXPECT_LE(GetStringWidth(FONT_SMALL, COMPOUND_STRING("OBIETTIVO"), 0), QUEST_LOG_DETAIL_TEXT_WIDTH);
    EXPECT_GT(StringLength(COMPOUND_STRING("OBIETTIVO")), 0);
    EXPECT_GT(StringLength(COMPOUND_STRING("B: INDIETRO")), 0);
    EXPECT_GT(StringLength(COMPOUND_STRING("Nessun obiettivo attivo.")), 0);
    EXPECT_LE(QuestLog_TestCountLines(COMPOUND_STRING("Nessun obiettivo attivo.")), 2);
    for (category = 0; category < QUEST_LOG_CATEGORY_COUNT; category++)
    {
        EXPECT_EQ(gQuestLogCategoryNames[category] != NULL, TRUE);
        EXPECT_GT(StringLength(gQuestLogCategoryNames[category]), 0);
    }
    EXPECT_LE(GetStringWidth(FONT_SMALL, COMPOUND_STRING("B: INDIETRO"), 0), QUEST_LOG_DETAIL_TEXT_WIDTH);
    EXPECT_LE(GetStringWidth(FONT_SMALL, COMPOUND_STRING("Nessun obiettivo attivo."), 0), QUEST_LOG_DETAIL_TEXT_WIDTH);
}

TEST("Quest journal definitions fit detail text budgets and have valid structure")
{
    u32 questIndex;
    u32 groupIndex;

    EXPECT_GT(gQuestLogQuestCount, 0);
    EXPECT_GT(gQuestLogGroupCount, 0);

    for (groupIndex = 0; groupIndex < gQuestLogGroupCount; groupIndex++)
    {
        EXPECT_EQ(gQuestLogGroups[groupIndex].title != NULL, TRUE);
        EXPECT_GT(StringLength(gQuestLogGroups[groupIndex].title), 0);
        EXPECT_LT(gQuestLogGroups[groupIndex].category, QUEST_LOG_CATEGORY_COUNT);
    }

    for (questIndex = 0; questIndex < gQuestLogQuestCount; questIndex++)
    {
        const struct QuestLogQuest *quest = &gQuestLogQuests[questIndex];
        bool32 foundMatchingGroup = FALSE;
        u32 i;
        u32 stepIndex;

        EXPECT_EQ(quest->title != NULL, TRUE);
        EXPECT_EQ(quest->description != NULL, TRUE);
        EXPECT_EQ(quest->discoverWhen != NULL, TRUE);
        EXPECT_GT(StringLength(quest->title), 0);
        EXPECT_GT(StringLength(quest->description), 0);
        EXPECT_EQ(QuestLog_TestCountLines(quest->title), 1);
        EXPECT_LE(GetStringWidth(FONT_NORMAL, quest->title, 0), QUEST_LOG_DETAIL_TEXT_WIDTH);
        EXPECT_LE(QuestLog_TestCountLines(quest->description), 3);
        EXPECT_LE(GetStringWidth(FONT_SMALL, quest->description, 0), QUEST_LOG_DETAIL_TEXT_WIDTH);
        EXPECT_LT(quest->category, QUEST_LOG_CATEGORY_COUNT);
        EXPECT_GT(quest->stepCount, 0);
        EXPECT_EQ(quest->steps != NULL, TRUE);

        for (i = 0; i < gQuestLogQuestCount; i++)
        {
            if (i != questIndex)
                EXPECT_NE(quest->id, gQuestLogQuests[i].id);
        }

        for (groupIndex = 0; groupIndex < gQuestLogGroupCount; groupIndex++)
        {
            const struct QuestLogGroup *group = &gQuestLogGroups[groupIndex];

            if (group->id == quest->groupId && group->category == quest->category)
                foundMatchingGroup = TRUE;
        }
        EXPECT_EQ(foundMatchingGroup, TRUE);

        for (stepIndex = 0; stepIndex < quest->stepCount; stepIndex++)
        {
            const struct QuestLogStep *step = &quest->steps[stepIndex];

            EXPECT_EQ(step->objective != NULL, TRUE);
            EXPECT_EQ(step->completeWhen != NULL, TRUE);
            EXPECT_GT(StringLength(step->objective), 0);
            EXPECT_LE(QuestLog_TestCountLines(step->objective), 2);
            EXPECT_LE(GetStringWidth(FONT_SMALL, step->objective, 0), QUEST_LOG_DETAIL_TEXT_WIDTH);
        }
    }
}
