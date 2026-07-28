#include "global.h"
#include "data.h"
#include "pokemon.h"
#include "script_pokemon_util.h"
#include "starter_choose.h"
#include "test/test.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/trainers.h"

struct RivalStarterExpectation
{
    u16 trainerId;
    enum Species species;
    u8 level;
};

static bool32 TrainerPartyContains(u16 trainerId, enum Species species, u8 level)
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][trainerId];

    for (u32 i = 0; i < trainer->partySize; i++)
    {
        if (trainer->party[i].species == species && trainer->party[i].lvl == level)
            return TRUE;
    }
    return FALSE;
}

static bool32 TrainerPartyContainsSpecies(u16 trainerId, enum Species species)
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][trainerId];

    for (u32 i = 0; i < trainer->partySize; i++)
    {
        if (trainer->party[i].species == species)
            return TRUE;
    }
    return FALSE;
}

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

TEST("Nico and Lia use the Cingerm line only when their grass starter branch is selected")
{
    const struct Evolution *rovascoEvolutions = GetSpeciesEvolutions(SPECIES_ROVASCO);
    static const struct RivalStarterExpectation expected[] = {
        { TRAINER_BRENDAN_ROUTE_103_MUDKIP, SPECIES_CINGERM, 5 },
        { TRAINER_MAY_ROUTE_103_MUDKIP, SPECIES_CINGERM, 5 },
        { TRAINER_BRENDAN_RUSTBORO_MUDKIP, SPECIES_CINGERM, 15 },
        { TRAINER_MAY_RUSTBORO_MUDKIP, SPECIES_CINGERM, 15 },
        { TRAINER_BRENDAN_ROUTE_110_MUDKIP, SPECIES_ROVASCO, 20 },
        { TRAINER_MAY_ROUTE_110_MUDKIP, SPECIES_ROVASCO, 20 },
        { TRAINER_BRENDAN_ROUTE_119_MUDKIP, SPECIES_ROVASCO, 31 },
        { TRAINER_MAY_ROUTE_119_MUDKIP, SPECIES_ROVASCO, 31 },
        { TRAINER_BRENDAN_LILYCOVE_MUDKIP, SPECIES_ROVASCO, 34 },
        { TRAINER_MAY_LILYCOVE_MUDKIP, SPECIES_ROVASCO, 34 },
    };

    ASSUME(IS_FRLG == FALSE);

    for (u32 i = 0; i < ARRAY_COUNT(expected); i++)
    {
        EXPECT(TrainerPartyContains(expected[i].trainerId, expected[i].species, expected[i].level));
        EXPECT(!TrainerPartyContainsSpecies(expected[i].trainerId, SPECIES_TREECKO));
        EXPECT(!TrainerPartyContainsSpecies(expected[i].trainerId, SPECIES_GROVYLE));
        EXPECT(!TrainerPartyContainsSpecies(expected[i].trainerId, SPECIES_SCEPTILE));
    }

    // No current Nico/Lia encounter reaches the level-36 final stage.
    // The existing species tests validate Rovasco -> Selvazanna at level 36.
    EXPECT(rovascoEvolutions != NULL);
    EXPECT_EQ(rovascoEvolutions[0].targetSpecies, SPECIES_SELVAZANNA);
    EXPECT_EQ(rovascoEvolutions[0].param, 36);
}
