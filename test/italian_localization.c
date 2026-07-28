#include "global.h"
#include "battle_anim_scripts.h"
#include "move.h"
#include "string_util.h"
#include "strings.h"
#include "text.h"
#include "test/test.h"
#include "constants/battle.h"
#include "constants/battle_move_effects.h"
#include "constants/moves.h"

TEST("Italian pause and Pokemon menu labels use the intended text")
{
    EXPECT_EQ(StringCompare(gText_MenuBag, COMPOUND_STRING("BORSA")), 0);
    EXPECT_EQ(StringCompare(gText_MenuSave, COMPOUND_STRING("SALVA")), 0);
    EXPECT_EQ(StringCompare(gText_MenuOption, COMPOUND_STRING("OPZIONI")), 0);
    EXPECT_EQ(StringCompare(gText_MenuExit, COMPOUND_STRING("ESCI")), 0);
    EXPECT_EQ(StringCompare(gMenuText_Give, COMPOUND_STRING("DAI")), 0);
    EXPECT_EQ(StringCompare(gText_Cancel2, COMPOUND_STRING("ANNULLA")), 0);
    EXPECT_EQ(StringCompare(gText_PkmnInfo, COMPOUND_STRING("INFO POKéMON")), 0);
    EXPECT_EQ(StringCompare(gText_PkmnSkills, COMPOUND_STRING("STATISTICHE")), 0);
    EXPECT_EQ(StringCompare(gText_BattleMoves, COMPOUND_STRING("MOSSE LOTTA")), 0);
    EXPECT_EQ(StringCompare(gText_Switch, COMPOUND_STRING("SPOSTA")), 0);
}

TEST("Italian pause and Pokemon summary labels fit their windows")
{
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_MenuBag, 0), 80);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_MenuSave, 0), 80);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_MenuOption, 0), 80);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_MenuExit, 0), 80);

    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_PkmnInfo, 0), 86);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_PkmnSkills, 0), 86);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_BattleMoves, 0), 86);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_ContestMoves, 0), 86);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_Attack3, 0), 42);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_Defense3, 0), 42);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_SpAtk4, 0), 36);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_SpDef4, 0), 36);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_Speed2, 0), 36);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_Status, 0), 46);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_ExpPoints, 0), 82);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_NextLv, 0), 82);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_Power, 0), 52);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, gText_Accuracy2, 0), 52);
}

TEST("Fangosberla preserves Mud-Slap battle data")
{
    const struct MoveInfo *move = &gMovesInfo[MOVE_MUD_SLAP];

    EXPECT_EQ(StringCompare(move->name, COMPOUND_STRING("Fangosberla")), 0);
    EXPECT_EQ(StringCompare(move->description,
                            COMPOUND_STRING("Scaglia fango sul bersaglio\nriducendone la precisione.")),
              0);
    EXPECT_LE(GetStringWidth(FONT_NARROWER, move->name, 0), 59);
    EXPECT_LE(GetStringWidth(FONT_NORMAL, move->description, 0), 152);

    EXPECT_EQ((u32)move->effect, EFFECT_HIT);
    EXPECT_EQ((u32)move->power, 20);
    EXPECT_EQ((u32)move->type, TYPE_GROUND);
    EXPECT_EQ((u32)move->accuracy, 100);
    EXPECT_EQ(move->pp, 10);
    EXPECT_EQ((u32)move->target, TARGET_SELECTED);
    EXPECT_EQ((s32)move->priority, 0);
    EXPECT_EQ((u32)move->category, DAMAGE_CATEGORY_SPECIAL);
    EXPECT_EQ((bool32)move->ballisticMove, TRUE);
    EXPECT_EQ((u32)move->numAdditionalEffects, 1);
    EXPECT_EQ(move->additionalEffects[0].moveEffect, MOVE_EFFECT_STAT_MINUS);
    EXPECT_EQ((u32)move->additionalEffects[0].accuracy, 1);
    EXPECT_EQ(move->additionalEffects[0].chance, 100);
    EXPECT_EQ(move->battleAnimScript, gBattleAnimMove_MudSlap);
}
