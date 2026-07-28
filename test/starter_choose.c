#include "global.h"
#include "pokemon.h"
#include "script_pokemon_util.h"
#include "starter_choose.h"
#include "test/test.h"
#include "constants/abilities.h"
#include "constants/items.h"

TEST("Starter slots preserve FRLG and expose the full Ausonia trio only in Emerald")
{
#if IS_FRLG
    EXPECT_EQ(GetStarterPokemon(0), SPECIES_BULBASAUR);
    EXPECT_EQ(GetStarterPokemon(1), SPECIES_CHARMANDER);
    EXPECT_EQ(GetStarterPokemon(2), SPECIES_SQUIRTLE);
#else
    EXPECT_EQ(GetStarterPokemon(0), SPECIES_CINGERM);
    EXPECT_EQ(GetStarterPokemon(1), SPECIES_SERBRACE);
    EXPECT_EQ(GetStarterPokemon(2), SPECIES_ARDEINO);
#endif
}

TEST("Selecting each Emerald starter creates the expected level 5 Ausonia species")
{
    static const enum Species expectedSpecies[] = { SPECIES_CINGERM, SPECIES_SERBRACE, SPECIES_ARDEINO };
    static const enum Type expectedTypes[] = { TYPE_GRASS, TYPE_FIRE, TYPE_WATER };
    static const enum Ability expectedAbilities[] = { ABILITY_OVERGROW, ABILITY_BLAZE, ABILITY_TORRENT };
    static const enum Move expectedMoves[] = { MOVE_LEAFAGE, MOVE_EMBER, MOVE_WATER_GUN };

    ASSUME(IS_FRLG == FALSE);
    for (u32 slot = 0; slot < ARRAY_COUNT(expectedSpecies); slot++)
    {
        bool32 hasExpectedMove = FALSE;

        ZeroPlayerPartyMons();
        EXPECT_EQ(ScriptGiveMon(GetStarterPokemon(slot), 5, ITEM_NONE), MON_GIVEN_TO_PARTY);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), expectedSpecies[slot]);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 5);
        EXPECT_EQ(GetMonAbility(&gParties[B_TRAINER_PLAYER][0]), expectedAbilities[slot]);
        EXPECT_EQ(gSpeciesInfo[expectedSpecies[slot]].types[0], expectedTypes[slot]);

        for (u32 move = 0; move < MAX_MON_MOVES; move++)
        {
            if (GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE1 + move) == expectedMoves[slot])
                hasExpectedMove = TRUE;
        }
        EXPECT(hasExpectedMove);
    }
}
