#include "global.h"
#include "pokedex.h"
#include "pokemon_storage_system.h"
#include "test/test.h"

// If you would like to ensure save compatibility, update the values below with those for your hack. You can find these through the debug menu.
// Please note that this simple check is not 100% foolproof, but should be able to catch most unintended shifts.
#if IS_FRLG
#define T_SAVEBLOCK1_SIZE 15768
#else
#define T_SAVEBLOCK1_SIZE 15568
#endif
#define T_SAVEBLOCK2_SIZE 3884
#define T_SAVEBLOCK3_SIZE 4
#define T_POKEMONSTORAGE_SIZE 34144

#define T_DEX_SEEN_OFFSET           0x3598
#define T_DEX_CAUGHT_OFFSET         0x361A
#define T_TRAINER_HILL_TIMES_OFFSET 0x369C
#define T_RAM_SCRIPT_OFFSET         0x36AC

TEST("SaveBlock1 is backwards compatible")
{
    EXPECT_EQ(sizeof(struct SaveBlock1), T_SAVEBLOCK1_SIZE);
}

TEST("SaveBlock1 Pokédex fields preserve their historical layout")
{
    EXPECT_EQ(offsetof(struct SaveBlock1, dexSeen), T_DEX_SEEN_OFFSET);
    EXPECT_EQ(sizeof(gSaveBlock1Ptr->dexSeen), DEX_SAVE_LEGACY_FLAG_BYTES);
    EXPECT_EQ(offsetof(struct SaveBlock1, dexCaught), T_DEX_CAUGHT_OFFSET);
    EXPECT_EQ(sizeof(gSaveBlock1Ptr->dexCaught), DEX_SAVE_LEGACY_FLAG_BYTES);
    EXPECT_EQ(offsetof(struct SaveBlock1, trainerHillTimes), T_TRAINER_HILL_TIMES_OFFSET);
    EXPECT_EQ(offsetof(struct SaveBlock1, ramScript), T_RAM_SCRIPT_OFFSET);
    EXPECT_EQ(offsetof(struct SaveBlock1, berryBlenderRecords), 0x9BC);
    EXPECT_EQ(offsetof(struct SaveBlock1, extendedDexSeen), 0x98C);
    EXPECT_EQ(offsetof(struct SaveBlock1, extendedDexCaught), 0x9A4);
    EXPECT_EQ(offsetof(struct SaveBlock1, berryBlenderRecords) - offsetof(struct SaveBlock1, ausoniaDexSaveSignature), 0x34);
    EXPECT_EQ(sizeof(gSaveBlock1Ptr->ausoniaDexSaveSignature), AUSONIA_DEX_SAVE_SIGNATURE_SIZE);
    EXPECT_EQ(sizeof(gSaveBlock1Ptr->extendedDexSeen), DEX_SAVE_EXTENSION_BYTES);
    EXPECT_EQ(sizeof(gSaveBlock1Ptr->extendedDexCaught), DEX_SAVE_EXTENSION_BYTES);
}

TEST("Ausonia Dex initialization migrates empty legacy filler")
{
    EnsureAusoniaDexSaveInitialized();

    EXPECT_EQ(gSaveBlock1Ptr->ausoniaDexSaveSignature[0], 'A');
    EXPECT_EQ(gSaveBlock1Ptr->ausoniaDexSaveSignature[1], 'L');
    EXPECT_EQ(gSaveBlock1Ptr->ausoniaDexSaveSignature[2], 'B');
    EXPECT_EQ(gSaveBlock1Ptr->ausoniaDexSaveSignature[3], AUSONIA_DEX_SAVE_VERSION);
    for (u32 i = 0; i < DEX_SAVE_EXTENSION_BYTES; i++)
    {
        EXPECT_EQ(gSaveBlock1Ptr->extendedDexSeen[i], 0);
        EXPECT_EQ(gSaveBlock1Ptr->extendedDexCaught[i], 0);
    }
}

TEST("Ausonia Dex initialization clears historical nonzero filler")
{
    memset(gSaveBlock1Ptr->ausoniaDexSaveSignature, 0xA5, sizeof(gSaveBlock1Ptr->ausoniaDexSaveSignature));
    memset(gSaveBlock1Ptr->extendedDexSeen, 0xA5, sizeof(gSaveBlock1Ptr->extendedDexSeen));
    memset(gSaveBlock1Ptr->extendedDexCaught, 0xA5, sizeof(gSaveBlock1Ptr->extendedDexCaught));

    EnsureAusoniaDexSaveInitialized();

    EXPECT_EQ(gSaveBlock1Ptr->ausoniaDexSaveSignature[0], 'A');
    EXPECT_EQ(gSaveBlock1Ptr->ausoniaDexSaveSignature[1], 'L');
    EXPECT_EQ(gSaveBlock1Ptr->ausoniaDexSaveSignature[2], 'B');
    EXPECT_EQ(gSaveBlock1Ptr->ausoniaDexSaveSignature[3], AUSONIA_DEX_SAVE_VERSION);
    for (u32 i = 0; i < DEX_SAVE_EXTENSION_BYTES; i++)
    {
        EXPECT_EQ(gSaveBlock1Ptr->extendedDexSeen[i], 0);
        EXPECT_EQ(gSaveBlock1Ptr->extendedDexCaught[i], 0);
    }
}

TEST("Ausonia Dex initialization preserves initialized extension flags")
{
    EnsureAusoniaDexSaveInitialized();
    gSaveBlock1Ptr->extendedDexSeen[0] = 0x81;
    gSaveBlock1Ptr->extendedDexCaught[DEX_SAVE_EXTENSION_BYTES - 1] = 0x80;

    EnsureAusoniaDexSaveInitialized();

    EXPECT_EQ(gSaveBlock1Ptr->extendedDexSeen[0], 0x81);
    EXPECT_EQ(gSaveBlock1Ptr->extendedDexCaught[DEX_SAVE_EXTENSION_BYTES - 1], 0x80);
}

TEST("Pokédex flags route independently across legacy and extension storage")
{
    static const enum NationalDexOrder sCurrentDexNumbers[] =
    {
        1,
        1038,
        1039,
        1040,
        1041,
        1042,
        1043,
        1044,
        1045,
        1046,
        1047,
        1048,
    };

    ClearPokedexSaveFlags();
    for (u32 i = 0; i < ARRAY_COUNT(sCurrentDexNumbers); i++)
    {
        enum NationalDexOrder dexNum = sCurrentDexNumbers[i];

        EXPECT_EQ(GetSetPokedexFlag(dexNum, FLAG_GET_SEEN), FALSE);
        EXPECT_EQ(GetSetPokedexFlag(dexNum, FLAG_GET_CAUGHT), FALSE);
        GetSetPokedexFlag(dexNum, FLAG_SET_SEEN);
        EXPECT_EQ(GetSetPokedexFlag(dexNum, FLAG_GET_SEEN), TRUE);
        EXPECT_EQ(GetSetPokedexFlag(dexNum, FLAG_GET_CAUGHT), FALSE);
        GetSetPokedexFlag(dexNum, FLAG_SET_CAUGHT);
        EXPECT_EQ(GetSetPokedexFlag(dexNum, FLAG_GET_CAUGHT), TRUE);
    }

    EXPECT_EQ(gSaveBlock1Ptr->dexSeen[DEX_SAVE_LEGACY_FLAG_BYTES - 1], 0xE0);
    EXPECT_EQ(gSaveBlock1Ptr->extendedDexSeen[0], 0x7F);
    EXPECT_EQ(gSaveBlock1Ptr->extendedDexCaught[0], 0x7F);
}

TEST("Pokédex flag access rejects invalid and future logical numbers")
{
    ClearPokedexSaveFlags();
    gSaveBlock1Ptr->extendedDexSeen[DEX_SAVE_EXTENSION_BYTES - 1] = 0x80;

    EXPECT_EQ(GetSetPokedexFlag(0, FLAG_SET_SEEN), FALSE);
    EXPECT_EQ(GetSetPokedexFlag(NATIONAL_DEX_COUNT + 1, FLAG_SET_SEEN), FALSE);
    EXPECT_EQ(GetSetPokedexFlag(DEX_SAVE_MAX_NATIONAL, FLAG_GET_SEEN), FALSE);
    EXPECT_EQ(GetSetPokedexFlag(DEX_SAVE_MAX_NATIONAL + 1, FLAG_SET_SEEN), FALSE);
    EXPECT_EQ(gSaveBlock1Ptr->extendedDexSeen[DEX_SAVE_EXTENSION_BYTES - 1], 0x80);
}

TEST("Pokédex reset clears all flags but preserves initialized storage")
{
    ClearPokedexSaveFlags();
    gSaveBlock1Ptr->dexSeen[0] = 0xFF;
    gSaveBlock1Ptr->dexCaught[DEX_SAVE_LEGACY_FLAG_BYTES - 1] = 0xFF;
    gSaveBlock1Ptr->extendedDexSeen[0] = 0xFF;
    gSaveBlock1Ptr->extendedDexCaught[DEX_SAVE_EXTENSION_BYTES - 1] = 0xFF;

    ResetPokedex();

    EXPECT_EQ(gSaveBlock1Ptr->ausoniaDexSaveSignature[0], 'A');
    EXPECT_EQ(gSaveBlock1Ptr->ausoniaDexSaveSignature[1], 'L');
    EXPECT_EQ(gSaveBlock1Ptr->ausoniaDexSaveSignature[2], 'B');
    EXPECT_EQ(gSaveBlock1Ptr->ausoniaDexSaveSignature[3], AUSONIA_DEX_SAVE_VERSION);
    for (u32 i = 0; i < DEX_SAVE_LEGACY_FLAG_BYTES; i++)
    {
        EXPECT_EQ(gSaveBlock1Ptr->dexSeen[i], 0);
        EXPECT_EQ(gSaveBlock1Ptr->dexCaught[i], 0);
    }
    for (u32 i = 0; i < DEX_SAVE_EXTENSION_BYTES; i++)
    {
        EXPECT_EQ(gSaveBlock1Ptr->extendedDexSeen[i], 0);
        EXPECT_EQ(gSaveBlock1Ptr->extendedDexCaught[i], 0);
    }
}

TEST("SaveBlock2 is backwards compatible")
{
    EXPECT_EQ(sizeof(struct SaveBlock2), T_SAVEBLOCK2_SIZE);
}

TEST("SaveBlock3 is backwards compatible")
{
    EXPECT_EQ(sizeof(struct SaveBlock3), T_SAVEBLOCK3_SIZE);
}

TEST("PokemonStorage is backwards compatible")
{
    EXPECT_EQ(sizeof(struct PokemonStorage), T_POKEMONSTORAGE_SIZE);
}

#undef T_SAVEBLOCK1_SIZE
#undef T_SAVEBLOCK2_SIZE
#undef T_SAVEBLOCK3_SIZE
#undef T_POKEMONSTORAGE_SIZE
#undef T_DEX_SEEN_OFFSET
#undef T_DEX_CAUGHT_OFFSET
#undef T_TRAINER_HILL_TIMES_OFFSET
#undef T_RAM_SCRIPT_OFFSET
