#include "global.h"
#include "pokemon.h"
#include "script_pokemon_util.h"
#include "starter_choose.h"
#include "test/test.h"
#include "constants/abilities.h"
#include "constants/items.h"

TEST("Starter slots preserve FRLG and expose Cingerm only in Emerald")
{
    static const enum Species otherAusoniaStarters[] = {
        SPECIES_ROVASCO,
        SPECIES_SELVAZANNA,
        SPECIES_SERBRACE,
        SPECIES_VIPERCEN,
        SPECIES_TOSSIVAMPA,
        SPECIES_ARDEINO,
        SPECIES_VELAIRONE,
        SPECIES_CODAIRONE,
    };

#if IS_FRLG
    EXPECT_EQ(GetStarterPokemon(0), SPECIES_BULBASAUR);
    EXPECT_EQ(GetStarterPokemon(1), SPECIES_CHARMANDER);
    EXPECT_EQ(GetStarterPokemon(2), SPECIES_SQUIRTLE);
#else
    EXPECT_EQ(GetStarterPokemon(0), SPECIES_CINGERM);
    EXPECT_EQ(GetStarterPokemon(1), SPECIES_TORCHIC);
    EXPECT_EQ(GetStarterPokemon(2), SPECIES_MUDKIP);
#endif

    for (u32 slot = 0; slot < 3; slot++)
    {
        for (u32 i = 0; i < ARRAY_COUNT(otherAusoniaStarters); i++)
            EXPECT_NE(GetStarterPokemon(slot), otherAusoniaStarters[i]);
    }
}

TEST("Selecting the grass slot creates a level 5 Cingerm with its normal data")
{
    bool32 hasLeafage = FALSE;

    ASSUME(IS_FRLG == FALSE);
    ZeroPlayerPartyMons();
    EXPECT_EQ(ScriptGiveMon(GetStarterPokemon(0), 5, ITEM_NONE), MON_GIVEN_TO_PARTY);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_CINGERM);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 5);
    EXPECT_EQ(GetMonAbility(&gParties[B_TRAINER_PLAYER][0]), ABILITY_OVERGROW);
    EXPECT_EQ(gSpeciesInfo[SPECIES_CINGERM].types[0], TYPE_GRASS);
    EXPECT_EQ(gSpeciesInfo[SPECIES_CINGERM].types[1], TYPE_GRASS);

    for (u32 i = 0; i < MAX_MON_MOVES; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE1 + i) == MOVE_LEAFAGE)
            hasLeafage = TRUE;
    }
    EXPECT(hasLeafage);
}
