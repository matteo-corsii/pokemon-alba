#include "global.h"
#include "battle_anim_scripts.h"
#include "move.h"
#include "pokemon.h"
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

TEST("Pokemon summary uses all official Italian nature names")
{
    const u8 *const expectedNames[NUM_NATURES] = {
        COMPOUND_STRING("Ardita"), COMPOUND_STRING("Schiva"), COMPOUND_STRING("Audace"),
        COMPOUND_STRING("Decisa"), COMPOUND_STRING("Birbona"), COMPOUND_STRING("Sicura"),
        COMPOUND_STRING("Docile"), COMPOUND_STRING("Placida"), COMPOUND_STRING("Scaltra"),
        COMPOUND_STRING("Fiacca"), COMPOUND_STRING("Timida"), COMPOUND_STRING("Lesta"),
        COMPOUND_STRING("Seria"), COMPOUND_STRING("Allegra"), COMPOUND_STRING("Ingenua"),
        COMPOUND_STRING("Modesta"), COMPOUND_STRING("Mite"), COMPOUND_STRING("Quieta"),
        COMPOUND_STRING("Ritrosa"), COMPOUND_STRING("Ardente"), COMPOUND_STRING("Calma"),
        COMPOUND_STRING("Gentile"), COMPOUND_STRING("Vivace"), COMPOUND_STRING("Cauta"),
        COMPOUND_STRING("Furba"),
    };

    for (u32 nature = 0; nature < NUM_NATURES; nature++)
    {
        EXPECT_EQ(StringCompare(gNaturesInfo[nature].name, expectedNames[nature]), 0);
        EXPECT_LE(GetStringWidth(FONT_NORMAL, gNaturesInfo[nature].name, 0), 72);
    }
}

TEST("Ausonia starter abilities show Italian names and descriptions")
{
    static const enum Ability abilities[] = {
        ABILITY_OVERGROW, ABILITY_DEFIANT, ABILITY_BLAZE,
        ABILITY_CORROSION, ABILITY_TORRENT, ABILITY_HYDRATION,
    };
    const u8 *const names[] = {
        COMPOUND_STRING("Erbaiuto"), COMPOUND_STRING("Agonismo"), COMPOUND_STRING("Aiutofuoco"),
        COMPOUND_STRING("Corrosione"), COMPOUND_STRING("Acquaiuto"), COMPOUND_STRING("Idratazione"),
    };
    static const s8 aiRatings[] = { 5, 5, 5, 5, 5, 4 };

    for (u32 i = 0; i < ARRAY_COUNT(abilities); i++)
    {
        EXPECT_EQ(StringCompare(gAbilitiesInfo[abilities[i]].name, names[i]), 0);
        EXPECT_LE(GetStringWidth(FONT_NORMAL, gAbilitiesInfo[abilities[i]].name, 0), 144);
        EXPECT_LE(GetStringWidth(FONT_NORMAL, gAbilitiesInfo[abilities[i]].description, 0), 144);
        EXPECT_EQ(gAbilitiesInfo[abilities[i]].aiRating, aiRatings[i]);
    }
}

TEST("Italian Pokemon summary headings fit without changing the screen graphics")
{
    EXPECT_LE(GetStringWidth(FONT_SMALL_NARROW, COMPOUND_STRING("ABILITÀ"), 0), 72);
    EXPECT_LE(GetStringWidth(FONT_SMALL_NARROW, COMPOUND_STRING("MEMO ALLENATORE"), 0), 72);
    EXPECT_EQ(StringCompare(gText_OTSlash, COMPOUND_STRING("AO/")), 0);
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
    EXPECT_EQ((bool32)move->ballisticMove, FALSE);
    EXPECT_EQ((u32)move->numAdditionalEffects, 1);
    EXPECT_EQ(move->additionalEffects[0].moveEffect, MOVE_EFFECT_STAT_MINUS);
    EXPECT_EQ((u32)move->additionalEffects[0].accuracy, 1);
    EXPECT_EQ(move->additionalEffects[0].chance, 100);
    EXPECT_EQ(move->battleAnimScript, gBattleAnimMove_MudSlap);
}
