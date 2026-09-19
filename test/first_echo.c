#include "global.h"
#include "battle.h"
#include "first_echo.h"
#include "malloc.h"
#include "test/test.h"
#include "constants/battle.h"

struct FirstEchoTestState
{
    struct BattleStruct *battleStruct;
    u8 battlersCount;
    u8 battlerPosition;
    u8 attackStage;
    u8 spAttackStage;
};

static struct FirstEchoTestState SetUpFirstEchoTest(enum FirstEchoEffect effect, u8 attackStage, u8 spAttackStage)
{
    struct FirstEchoTestState state =
    {
        .battleStruct = gBattleStruct,
        .battlersCount = gBattlersCount,
        .battlerPosition = gBattlerPositions[0],
        .attackStage = gBattleMons[0].statStages[STAT_ATK],
        .spAttackStage = gBattleMons[0].statStages[STAT_SPATK],
    };

    gBattleStruct = AllocZeroed(sizeof(*gBattleStruct));
    gBattleStruct->firstEchoEffect = effect;
    gBattlersCount = 2;
    gBattlerPositions[0] = B_POSITION_PLAYER_LEFT;
    gBattleMons[0].statStages[STAT_ATK] = attackStage;
    gBattleMons[0].statStages[STAT_SPATK] = spAttackStage;

    return state;
}

static void TearDownFirstEchoTest(struct FirstEchoTestState state)
{
    Free(gBattleStruct);
    gBattleStruct = state.battleStruct;
    gBattlersCount = state.battlersCount;
    gBattlerPositions[0] = state.battlerPosition;
    gBattleMons[0].statStages[STAT_ATK] = state.attackStage;
    gBattleMons[0].statStages[STAT_SPATK] = state.spAttackStage;
}

TEST("First Echo directly raises Attack and Special Attack by one stage")
{
    struct FirstEchoTestState state = SetUpFirstEchoTest(FIRST_ECHO_EFFECT_ATK_SPATK, DEFAULT_STAT_STAGE, DEFAULT_STAT_STAGE);

    FirstEcho_ApplyBoost();

    EXPECT_EQ(gBattleMons[0].statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    EXPECT_EQ(gBattleMons[0].statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    TearDownFirstEchoTest(state);
}

TEST("First Echo respects the normal maximum stat stage")
{
    struct FirstEchoTestState state = SetUpFirstEchoTest(FIRST_ECHO_EFFECT_ATK_SPATK, MAX_STAT_STAGE, MAX_STAT_STAGE);

    FirstEcho_ApplyBoost();

    EXPECT_EQ(gBattleMons[0].statStages[STAT_ATK], MAX_STAT_STAGE);
    EXPECT_EQ(gBattleMons[0].statStages[STAT_SPATK], MAX_STAT_STAGE);
    TearDownFirstEchoTest(state);
}
