#ifndef GUARD_QUEST_LOG_INTERNAL_H
#define GUARD_QUEST_LOG_INTERNAL_H

#include "quest_log.h"

enum QuestLogCategory
{
    QUEST_LOG_CATEGORY_STORY,
    QUEST_LOG_CATEGORY_SIDE,
    QUEST_LOG_CATEGORY_JOBS,
    QUEST_LOG_CATEGORY_COUNT,
};

enum QuestLogGroupId
{
    QUEST_GROUP_STORY_ARC_1 = 1,
    QUEST_GROUP_SIDE_GENERAL,
    QUEST_GROUP_SIDE_SHELTERS,
    QUEST_GROUP_JOBS_GENERAL,
    QUEST_GROUP_JOBS_SHELTERS,
};

enum QuestLogGroupCompletionPolicy
{
    QUEST_LOG_GROUP_COMPLETION_NONE,
    QUEST_LOG_GROUP_COMPLETION_ALL_VISIBLE_QUESTS,
};

enum QuestLogConditionType
{
    QUEST_LOG_CONDITION_FLAG_SET,
    QUEST_LOG_CONDITION_FLAG_CLEAR,
    QUEST_LOG_CONDITION_VAR_EQUALS,
    QUEST_LOG_CONDITION_VAR_GREATER_EQUAL,
    QUEST_LOG_CONDITION_PLAYER_SECRET_BASE_ID,
    QUEST_LOG_CONDITION_UNTRACKED,
};

struct QuestLogConditionClause
{
    u8 type;
    u16 subject;
    u16 value;
    u16 secondValue;
};

struct QuestLogCondition
{
    const struct QuestLogConditionClause *allClauses;
    u8 allCount;
    const struct QuestLogConditionClause *anyClauses;
    u8 anyCount;
};

struct QuestLogStep
{
    const u8 *objective;
    const struct QuestLogCondition *completeWhen;
};

struct QuestLogQuest
{
    u16 id;
    u8 category;
    u16 groupId;
    u16 order;
    const u8 *title;
    const u8 *description;
    const struct QuestLogCondition *discoverWhen;
    const struct QuestLogCondition *completeWhen;
    const struct QuestLogStep *steps;
    u8 stepCount;
};

struct QuestLogGroup
{
    u16 id;
    u8 category;
    u16 order;
    u8 completionPolicy;
    const u8 *title;
};

extern const struct QuestLogQuest gQuestLogQuests[];
extern const u8 gQuestLogQuestCount;
extern const struct QuestLogGroup gQuestLogGroups[];
extern const u8 gQuestLogGroupCount;
extern const u8 *const gQuestLogCategoryNames[QUEST_LOG_CATEGORY_COUNT];

#endif // GUARD_QUEST_LOG_INTERNAL_H
