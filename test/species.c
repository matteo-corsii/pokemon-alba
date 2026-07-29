#include "global.h"
#include "random_mon_generation.h"
#include "string_util.h"
#include "test/test.h"
#include "constants/form_change_types.h"

TEST("Form species ID tables are shared between all forms")
{
    u32 species = SPECIES_NONE;
    const u16 *formSpeciesIdTable;

    for (u32 i = 0; i < NUM_SPECIES; i++)
    {
        if (gSpeciesInfo[i].formSpeciesIdTable)
        {
            PARAMETRIZE_LABEL("ID:%d - %S", i, gSpeciesInfo[i].speciesName) { species = i; }
        }
    }

    formSpeciesIdTable = gSpeciesInfo[species].formSpeciesIdTable;
    for (u32 i = 0; formSpeciesIdTable[i] != FORM_SPECIES_END; i++)
    {
        u32 formSpeciesId = formSpeciesIdTable[i];
        EXPECT_EQ(gSpeciesInfo[formSpeciesId].formSpeciesIdTable, formSpeciesIdTable);
    }
}

TEST("Form species ID tables fit within RANDOM_MON_MAX_FORMS")
{
    u32 formCount;
    u32 species = SPECIES_NONE;
    const u16 *formSpeciesIdTable;

    for (u32 i = 0; i < NUM_SPECIES; i++)
    {
        if (gSpeciesInfo[i].formSpeciesIdTable)
            PARAMETRIZE_LABEL("ID:%d - %S", i, gSpeciesInfo[i].speciesName) { species = i; }
    }

    formSpeciesIdTable = gSpeciesInfo[species].formSpeciesIdTable;
    for (formCount = 0; formSpeciesIdTable[formCount] != FORM_SPECIES_END; formCount++)
        ;

    EXPECT(formCount <= RANDOM_MON_MAX_FORMS);
}

TEST("Form change tables contain only forms in the form species ID table")
{
    u32 species = SPECIES_NONE;
    const struct FormChange *formChangeTable;
    const u16 *formSpeciesIdTable;

    for (u32 i = 0; i < NUM_SPECIES; i++)
    {
        if (gSpeciesInfo[i].formChangeTable)
        {
            PARAMETRIZE_LABEL("ID:%d - %S", i, gSpeciesInfo[i].speciesName) { species = i; }
        }
    }

    formChangeTable = gSpeciesInfo[species].formChangeTable;
    formSpeciesIdTable = gSpeciesInfo[species].formSpeciesIdTable;
    EXPECT(formSpeciesIdTable);

    for (u32 i = 0; formChangeTable[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        u32 j;

        if (formChangeTable[i].targetSpecies == SPECIES_NONE)
            continue;
        for (j = 0; formSpeciesIdTable[j] != FORM_SPECIES_END; j++)
        {
            if (formChangeTable[i].targetSpecies == formSpeciesIdTable[j])
            {
                break;
            }
        }
        EXPECT(formSpeciesIdTable[j] != FORM_SPECIES_END);
    }
}

TEST("Forms have the appropriate species form changes")
{
    u32 species = SPECIES_NONE;

    for (u32 i = 0; i < NUM_SPECIES; i++)
    {
        if (gSpeciesInfo[i].isMegaEvolution
            || gSpeciesInfo[i].isGigantamax
            || gSpeciesInfo[i].isUltraBurst
            || gSpeciesInfo[i].isPrimalReversion)
        {
            PARAMETRIZE_LABEL("ID:%d - %S", i, gSpeciesInfo[i].speciesName) { species = i; }
        }
    }
    bool32 hasBattleEnd = FALSE, hasFaint = FALSE;

    const struct FormChange *formChanges = GetSpeciesFormChanges(species);
    EXPECT(formChanges != NULL);

    for (u32 j = 0; formChanges[j].method != FORM_CHANGE_TERMINATOR; j++)
    {
        if (species != formChanges[j].targetSpecies)
        {
            if (formChanges[j].method == FORM_CHANGE_END_BATTLE)
                hasBattleEnd = TRUE;
            else if (formChanges[j].method == FORM_CHANGE_FAINT)
                hasFaint = TRUE;
        }
    }

    EXPECT(hasBattleEnd);

    // Primal Reversion don't change forms upon fainting
    if (gSpeciesInfo[species].isMegaEvolution
        || gSpeciesInfo[species].isGigantamax
        || gSpeciesInfo[species].isUltraBurst)
    {
        EXPECT(hasFaint);
    }
}

TEST("Form change targets have the appropriate species flags")
{
    u32 species = SPECIES_NONE;
    const struct FormChange *formChangeTable;

    for (u32 i = 0; i < NUM_SPECIES; i++)
    {
        if (gSpeciesInfo[i].formChangeTable)
        {
            PARAMETRIZE_LABEL("ID:%d - %S", i, gSpeciesInfo[i].speciesName) { species = i; }
        }
    }

    formChangeTable = gSpeciesInfo[species].formChangeTable;
    for (u32 i = 0; formChangeTable[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        const struct SpeciesInfo *targetSpeciesInfo = &gSpeciesInfo[formChangeTable[i].targetSpecies];
        switch (formChangeTable[i].method)
        {
        case FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM:
        case FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE:
            EXPECT(targetSpeciesInfo->isMegaEvolution);
            break;
        case FORM_CHANGE_BATTLE_PRIMAL_REVERSION:
            EXPECT(targetSpeciesInfo->isPrimalReversion);
            break;
        case FORM_CHANGE_BATTLE_ULTRA_BURST:
            EXPECT(targetSpeciesInfo->isUltraBurst);
            break;
        case FORM_CHANGE_BATTLE_GIGANTAMAX:
            EXPECT(targetSpeciesInfo->isGigantamax);
            break;
       }
    }
}

TEST("No species has two evolutions that use the evolution tracker")
{
    u32 species = SPECIES_NONE;
    u32 evolutionTrackerEvolutions;
    bool32 hasRecoilEvo;
    const struct Evolution *evolutions;

    for (u32 i = 0; i < NUM_SPECIES; i++)
    {
        if (IsSpeciesEnabled(i) && GetSpeciesEvolutions(i) != NULL)
            PARAMETRIZE_LABEL("ID:%d - %S", i, GetSpeciesName(i)) { species = i; }
    }

    evolutionTrackerEvolutions = 0;
    hasRecoilEvo = FALSE;
    evolutions = GetSpeciesEvolutions(species);

    for (u32 i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
    {
        if (evolutions[i].params == NULL)
            continue;
        for (u32 j = 0; evolutions[i].params[j].condition != CONDITIONS_END; j++)
        {
            if (evolutions[i].params[j].condition == IF_USED_MOVE_X_TIMES
             || evolutions[i].params[j].condition == IF_DEFEAT_X_WITH_ITEMS
            )
                evolutionTrackerEvolutions++;

            if (evolutions[i].params[j].condition == IF_RECOIL_DAMAGE_GE)
            {
                // Special handling for these since they can be combined as the evolution tracker field is used for the same purpose
                if (!hasRecoilEvo)
                {
                    hasRecoilEvo = TRUE;
                    evolutionTrackerEvolutions++;
                }
            }
        }
    }

    EXPECT(evolutionTrackerEvolutions < 2);
}

extern const u8 gFallbackPokedexText[];

TEST("Every species has a description")
{
    u32 species = SPECIES_NONE;
    for (u32 i = 1; i < NUM_SPECIES; i++)
    {
        if (IsSpeciesEnabled(i))
            PARAMETRIZE_LABEL("ID:%d - %S", i, GetSpeciesName(i)) { species = i; }
    }

    EXPECT_NE(StringCompare(GetSpeciesPokedexDescription(species), gFallbackPokedexText), 0);
}

static u16 GetBaseStatTotalForTest(enum Species species)
{
    return gSpeciesInfo[species].baseHP
         + gSpeciesInfo[species].baseAttack
         + gSpeciesInfo[species].baseDefense
         + gSpeciesInfo[species].baseSpeed
         + gSpeciesInfo[species].baseSpAttack
         + gSpeciesInfo[species].baseSpDefense;
}

TEST("Ausonia starter IDs are append-only and distinct")
{
    EXPECT_EQ(SPECIES_GLIMMORA_MEGA, 1572);
    EXPECT_EQ(SPECIES_CINGERM, SPECIES_GLIMMORA_MEGA + 1);
    EXPECT_EQ(SPECIES_ROVASCO, SPECIES_CINGERM + 1);
    EXPECT_EQ(SPECIES_SELVAZANNA, SPECIES_ROVASCO + 1);
    EXPECT_EQ(SPECIES_SERBRACE, SPECIES_SELVAZANNA + 1);
    EXPECT_EQ(SPECIES_VIPERCEN, SPECIES_SERBRACE + 1);
    EXPECT_EQ(SPECIES_TOSSIVAMPA, SPECIES_VIPERCEN + 1);
    EXPECT_EQ(SPECIES_ARDEINO, SPECIES_TOSSIVAMPA + 1);
    EXPECT_EQ(SPECIES_VELAIRONE, SPECIES_ARDEINO + 1);
    EXPECT_EQ(SPECIES_CODAIRONE, SPECIES_VELAIRONE + 1);
    EXPECT_EQ(SPECIES_EGG, SPECIES_CODAIRONE + 1);
    EXPECT_EQ(NUM_SPECIES, SPECIES_EGG);

    EXPECT_EQ(NATIONAL_DEX_PECHARUNT, 1025);
    EXPECT_EQ(NATIONAL_DEX_CINGERM, NATIONAL_DEX_PECHARUNT + 1);
    EXPECT_EQ(NATIONAL_DEX_ROVASCO, NATIONAL_DEX_CINGERM + 1);
    EXPECT_EQ(NATIONAL_DEX_SELVAZANNA, NATIONAL_DEX_ROVASCO + 1);
    EXPECT_EQ(NATIONAL_DEX_SERBRACE, NATIONAL_DEX_SELVAZANNA + 1);
    EXPECT_EQ(NATIONAL_DEX_VIPERCEN, NATIONAL_DEX_SERBRACE + 1);
    EXPECT_EQ(NATIONAL_DEX_TOSSIVAMPA, NATIONAL_DEX_VIPERCEN + 1);
    EXPECT_EQ(NATIONAL_DEX_ARDEINO, NATIONAL_DEX_TOSSIVAMPA + 1);
    EXPECT_EQ(NATIONAL_DEX_VELAIRONE, NATIONAL_DEX_ARDEINO + 1);
    EXPECT_EQ(NATIONAL_DEX_CODAIRONE, NATIONAL_DEX_VELAIRONE + 1);
    EXPECT_EQ(NATIONAL_DEX_COUNT, NATIONAL_DEX_CODAIRONE);

    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_CINGERM), COMPOUND_STRING("Cingerm")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_ROVASCO), COMPOUND_STRING("Rovasco")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_SELVAZANNA), COMPOUND_STRING("Selvazanna")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_SERBRACE), COMPOUND_STRING("Serbrace")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_VIPERCEN), COMPOUND_STRING("Vipercen")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_TOSSIVAMPA), COMPOUND_STRING("Tossivampa")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_ARDEINO), COMPOUND_STRING("Ardeino")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_VELAIRONE), COMPOUND_STRING("Velairone")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_CODAIRONE), COMPOUND_STRING("Codairone")), 0);
}

TEST("Ausonia Grass starter base data matches the approved prototype")
{
    const struct SpeciesInfo *cingerm = &gSpeciesInfo[SPECIES_CINGERM];
    const struct SpeciesInfo *rovasco = &gSpeciesInfo[SPECIES_ROVASCO];
    const struct SpeciesInfo *selvazanna = &gSpeciesInfo[SPECIES_SELVAZANNA];

    EXPECT_EQ(GetBaseStatTotalForTest(SPECIES_CINGERM), 310);
    EXPECT_EQ(GetBaseStatTotalForTest(SPECIES_ROVASCO), 405);
    EXPECT_EQ(GetBaseStatTotalForTest(SPECIES_SELVAZANNA), 530);

    EXPECT_EQ(cingerm->baseHP, 60);
    EXPECT_EQ(cingerm->baseAttack, 65);
    EXPECT_EQ(cingerm->baseDefense, 60);
    EXPECT_EQ(cingerm->baseSpAttack, 35);
    EXPECT_EQ(cingerm->baseSpDefense, 45);
    EXPECT_EQ(cingerm->baseSpeed, 45);
    EXPECT_EQ(rovasco->baseHP, 80);
    EXPECT_EQ(rovasco->baseAttack, 85);
    EXPECT_EQ(rovasco->baseDefense, 80);
    EXPECT_EQ(rovasco->baseSpAttack, 45);
    EXPECT_EQ(rovasco->baseSpDefense, 55);
    EXPECT_EQ(rovasco->baseSpeed, 60);
    EXPECT_EQ(selvazanna->baseHP, 100);
    EXPECT_EQ(selvazanna->baseAttack, 120);
    EXPECT_EQ(selvazanna->baseDefense, 105);
    EXPECT_EQ(selvazanna->baseSpAttack, 55);
    EXPECT_EQ(selvazanna->baseSpDefense, 80);
    EXPECT_EQ(selvazanna->baseSpeed, 70);

    EXPECT_EQ(cingerm->types[0], TYPE_GRASS);
    EXPECT_EQ(cingerm->types[1], TYPE_GRASS);
    EXPECT_EQ(rovasco->types[0], TYPE_GRASS);
    EXPECT_EQ(rovasco->types[1], TYPE_GRASS);
    EXPECT_EQ(selvazanna->types[0], TYPE_GRASS);
    EXPECT_EQ(selvazanna->types[1], TYPE_DARK);

    EXPECT_EQ(cingerm->abilities[0], ABILITY_OVERGROW);
    EXPECT_EQ(cingerm->abilities[1], ABILITY_NONE);
    EXPECT_EQ(cingerm->abilities[2], ABILITY_DEFIANT);
    EXPECT_EQ(rovasco->abilities[0], ABILITY_OVERGROW);
    EXPECT_EQ(rovasco->abilities[1], ABILITY_NONE);
    EXPECT_EQ(rovasco->abilities[2], ABILITY_DEFIANT);
    EXPECT_EQ(selvazanna->abilities[0], ABILITY_OVERGROW);
    EXPECT_EQ(selvazanna->abilities[1], ABILITY_NONE);
    EXPECT_EQ(selvazanna->abilities[2], ABILITY_DEFIANT);

    EXPECT_EQ(cingerm->growthRate, GROWTH_MEDIUM_SLOW);
    EXPECT_EQ(rovasco->growthRate, GROWTH_MEDIUM_SLOW);
    EXPECT_EQ(selvazanna->growthRate, GROWTH_MEDIUM_SLOW);
    EXPECT_EQ(cingerm->genderRatio, (50 * 255) / 100);
    EXPECT_EQ(rovasco->genderRatio, (50 * 255) / 100);
    EXPECT_EQ(selvazanna->genderRatio, (50 * 255) / 100);
    EXPECT_EQ((u32)cingerm->evYield_Attack, 1);
    EXPECT_EQ((u32)rovasco->evYield_Attack, 2);
    EXPECT_EQ((u32)selvazanna->evYield_Attack, 3);
    EXPECT_EQ(cingerm->catchRate, 45);
    EXPECT_EQ(rovasco->catchRate, 45);
    EXPECT_EQ(selvazanna->catchRate, 45);
    EXPECT_EQ(cingerm->friendship, 70);
    EXPECT_EQ(rovasco->friendship, 70);
    EXPECT_EQ(selvazanna->friendship, 70);
    EXPECT_EQ(cingerm->eggGroups[0], EGG_GROUP_FIELD);
    EXPECT_EQ(cingerm->eggGroups[1], EGG_GROUP_FIELD);
    EXPECT_EQ(rovasco->eggGroups[0], EGG_GROUP_FIELD);
    EXPECT_EQ(rovasco->eggGroups[1], EGG_GROUP_FIELD);
    EXPECT_EQ(selvazanna->eggGroups[0], EGG_GROUP_FIELD);
    EXPECT_EQ(selvazanna->eggGroups[1], EGG_GROUP_FIELD);
    EXPECT_EQ(cingerm->itemCommon, ITEM_NONE);
    EXPECT_EQ(cingerm->itemRare, ITEM_NONE);
    EXPECT_EQ(rovasco->itemCommon, ITEM_NONE);
    EXPECT_EQ(rovasco->itemRare, ITEM_NONE);
    EXPECT_EQ(selvazanna->itemCommon, ITEM_NONE);
    EXPECT_EQ(selvazanna->itemRare, ITEM_NONE);
}

TEST("Ausonia Grass starter evolutions use only the approved levels")
{
    const struct Evolution *cingermEvolutions = GetSpeciesEvolutions(SPECIES_CINGERM);
    const struct Evolution *rovascoEvolutions = GetSpeciesEvolutions(SPECIES_ROVASCO);

    EXPECT(cingermEvolutions != NULL);
    EXPECT_EQ(cingermEvolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(cingermEvolutions[0].param, 16);
    EXPECT_GT(cingermEvolutions[0].param, 15);
    EXPECT_EQ(cingermEvolutions[0].targetSpecies, SPECIES_ROVASCO);
    EXPECT_EQ(cingermEvolutions[1].method, EVOLUTIONS_END);

    EXPECT(rovascoEvolutions != NULL);
    EXPECT_EQ(rovascoEvolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(rovascoEvolutions[0].param, 36);
    EXPECT_GT(rovascoEvolutions[0].param, 35);
    EXPECT_EQ(rovascoEvolutions[0].targetSpecies, SPECIES_SELVAZANNA);
    EXPECT_EQ(rovascoEvolutions[1].method, EVOLUTIONS_END);

    EXPECT(GetSpeciesEvolutions(SPECIES_SELVAZANNA) == NULL);
}

TEST("Ausonia Grass starter level-up learnsets are complete and ordered")
{
    static const enum Species species[] = { SPECIES_CINGERM, SPECIES_ROVASCO, SPECIES_SELVAZANNA };
    static const u8 expectedLevels[] = { 1, 1, 4, 7, 9, 12, 15, 18, 22, 26, 30, 34, 38, 43, 48, 54 };
    static const u16 expectedMoves[] = {
        MOVE_TACKLE, MOVE_LEER, MOVE_LEAFAGE, MOVE_MUD_SLAP,
        MOVE_BITE, MOVE_DEFENSE_CURL, MOVE_ROLLOUT, MOVE_RAZOR_LEAF,
        MOVE_TAKE_DOWN, MOVE_TRAILBLAZE, MOVE_ASSURANCE, MOVE_SEED_BOMB,
        MOVE_CRUNCH, MOVE_HIGH_HORSEPOWER, MOVE_WOOD_HAMMER, MOVE_SUCKER_PUNCH,
    };

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(species[i]);

        for (u32 j = 0; j < ARRAY_COUNT(expectedMoves); j++)
        {
            EXPECT_EQ(learnset[j].level, expectedLevels[j]);
            EXPECT_EQ(learnset[j].move, expectedMoves[j]);
            if (j != 0)
                EXPECT_GE(learnset[j].level, learnset[j - 1].level);
        }
        EXPECT_EQ(learnset[ARRAY_COUNT(expectedMoves)].move, LEVEL_UP_MOVE_END);
    }
}

TEST("Ausonia Grass starter placeholder assets and Pokédex data are valid")
{
    static const enum Species species[] = { SPECIES_CINGERM, SPECIES_ROVASCO, SPECIES_SELVAZANNA };
    static const enum PokemonCry cries[] = { CRY_LECHONK, CRY_OINKOLOGNE_M, CRY_MAMOSWINE };
    static const enum NationalDexOrder dexNums[] = { NATIONAL_DEX_CINGERM, NATIONAL_DEX_ROVASCO, NATIONAL_DEX_SELVAZANNA };
    static const u16 heights[] = { 5, 9, 16 };
    static const u16 weights[] = { 125, 420, 1450 };

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species[i]];

        EXPECT(info->frontPic != NULL);
        EXPECT(info->backPic != NULL);
        EXPECT(info->iconSprite != NULL);
        EXPECT(info->palette != NULL);
        EXPECT(info->shinyPalette != NULL);
    #if P_FOOTPRINTS
        EXPECT(info->footprint != NULL);
    #endif
        EXPECT_EQ((u32)info->cryId, cries[i]);
        EXPECT_GT((u32)info->cryId, CRY_NONE);
        EXPECT_LT((u32)info->cryId, CRY_COUNT);
        EXPECT_EQ((u32)info->natDexNum, dexNums[i]);
        EXPECT_EQ(info->height, heights[i]);
        EXPECT_EQ(info->weight, weights[i]);
        EXPECT_NE(StringCompare(info->description, gFallbackPokedexText), 0);
        EXPECT_EQ(info->teachableLearnset[0], MOVE_UNAVAILABLE);
        EXPECT_EQ(info->eggMoveLearnset[0], MOVE_UNAVAILABLE);
    }

    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_CINGERM].categoryName, COMPOUND_STRING("GERMOGLIO")), 0);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_ROVASCO].categoryName, COMPOUND_STRING("ROVETO")), 0);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_SELVAZANNA].categoryName, COMPOUND_STRING("SELVA")), 0);
}

TEST("Cingerm original graphics replace only its Lechonk placeholders")
{
    const struct SpeciesInfo *cingerm = &gSpeciesInfo[SPECIES_CINGERM];
    const struct SpeciesInfo *lechonk = &gSpeciesInfo[SPECIES_LECHONK];

    EXPECT(cingerm->frontPic != NULL);
    EXPECT(cingerm->backPic != NULL);
    EXPECT(cingerm->palette != NULL);
    EXPECT(cingerm->shinyPalette != NULL);
    EXPECT(cingerm->iconSprite != NULL);
    EXPECT(cingerm->frontAnimFrames != NULL);
    EXPECT(cingerm->frontPic != lechonk->frontPic);
    EXPECT(cingerm->backPic != lechonk->backPic);
    EXPECT(cingerm->palette != lechonk->palette);
    EXPECT(cingerm->shinyPalette != lechonk->shinyPalette);
    EXPECT(cingerm->iconSprite != lechonk->iconSprite);
    EXPECT(cingerm->frontAnimFrames != lechonk->frontAnimFrames);
    EXPECT_EQ(cingerm->frontPicSize, (8 << 4) | 6);
    EXPECT_EQ(cingerm->backPicSize, (7 << 4) | 7);
    EXPECT_EQ(cingerm->frontPicYOffset, 4);
    EXPECT_EQ(cingerm->backPicYOffset, 4);
    EXPECT(cingerm->iconPalIndex == 5);

    // Audio, footprint and overworld graphics remain explicitly provisional.
    EXPECT_EQ((u32)cingerm->cryId, CRY_LECHONK);
#if P_FOOTPRINTS
    EXPECT(cingerm->footprint == lechonk->footprint);
#endif
#if OW_POKEMON_OBJECT_EVENTS
    EXPECT(cingerm->overworldData.images == lechonk->overworldData.images);
#if OW_PKMN_OBJECTS_SHARE_PALETTES == FALSE
    EXPECT(cingerm->overworldPalette == lechonk->overworldPalette);
    EXPECT(cingerm->overworldShinyPalette == lechonk->overworldShinyPalette);
#endif
#endif

}

TEST("Serbrace original graphics replace only its Ekans placeholders")
{
    const struct SpeciesInfo *serbrace = &gSpeciesInfo[SPECIES_SERBRACE];
    const struct SpeciesInfo *ekans = &gSpeciesInfo[SPECIES_EKANS];
    const struct SpeciesInfo *cingerm = &gSpeciesInfo[SPECIES_CINGERM];
    const struct SpeciesInfo *lechonk = &gSpeciesInfo[SPECIES_LECHONK];
    const struct SpeciesInfo *ardeino = &gSpeciesInfo[SPECIES_ARDEINO];
    const struct SpeciesInfo *ducklett = &gSpeciesInfo[SPECIES_DUCKLETT];
    const struct Evolution *evolutions = GetSpeciesEvolutions(SPECIES_SERBRACE);

    EXPECT(serbrace->frontPic != NULL);
    EXPECT(serbrace->backPic != NULL);
    EXPECT(serbrace->palette != NULL);
    EXPECT(serbrace->shinyPalette != NULL);
    EXPECT(serbrace->iconSprite != NULL);
    EXPECT(serbrace->frontAnimFrames != NULL);
    EXPECT(serbrace->frontPic != ekans->frontPic);
    EXPECT(serbrace->backPic != ekans->backPic);
    EXPECT(serbrace->palette != ekans->palette);
    EXPECT(serbrace->shinyPalette != ekans->shinyPalette);
    EXPECT(serbrace->iconSprite != ekans->iconSprite);
    EXPECT(serbrace->frontAnimFrames != ekans->frontAnimFrames);
    EXPECT_EQ(serbrace->frontPicSize, (8 << 4) | 7);
    EXPECT_EQ(serbrace->backPicSize, (8 << 4) | 7);
    EXPECT_EQ(serbrace->frontPicYOffset, 4);
    EXPECT_EQ(serbrace->backPicYOffset, 4);
    EXPECT(serbrace->iconPalIndex == 3);

    // Gameplay data and the remaining audiovisual placeholders stay unchanged.
    EXPECT_EQ(serbrace->baseHP, 45);
    EXPECT_EQ(serbrace->baseAttack, 40);
    EXPECT_EQ(serbrace->baseDefense, 40);
    EXPECT_EQ(serbrace->baseSpeed, 65);
    EXPECT_EQ(serbrace->baseSpAttack, 70);
    EXPECT_EQ(serbrace->baseSpDefense, 50);
    EXPECT_EQ(serbrace->types[0], TYPE_FIRE);
    EXPECT_EQ(serbrace->types[1], TYPE_FIRE);
    EXPECT_EQ(serbrace->abilities[0], ABILITY_BLAZE);
    EXPECT_EQ(serbrace->abilities[1], ABILITY_NONE);
    EXPECT_EQ(serbrace->abilities[2], ABILITY_CORROSION);
    EXPECT_EQ((u32)serbrace->cryId, CRY_EKANS);
    EXPECT_EQ(serbrace->backAnimId, ekans->backAnimId);
    EXPECT(evolutions != NULL);
    EXPECT_EQ(evolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(evolutions[0].param, 16);
    EXPECT_EQ(evolutions[0].targetSpecies, SPECIES_VIPERCEN);
#if P_FOOTPRINTS
    EXPECT(serbrace->footprint == ekans->footprint);
#endif
#if OW_POKEMON_OBJECT_EVENTS
    EXPECT(serbrace->overworldData.images == ekans->overworldData.images);
#if OW_PKMN_OBJECTS_SHARE_PALETTES == FALSE
    EXPECT(serbrace->overworldPalette == ekans->overworldPalette);
    EXPECT(serbrace->overworldShinyPalette == ekans->overworldShinyPalette);
#endif
#endif

    // The other two starter graphics retain their approved states.
    EXPECT(cingerm->frontPic != lechonk->frontPic);
    EXPECT(cingerm->backPic != lechonk->backPic);
    EXPECT(ardeino->frontPic != ducklett->frontPic);
    EXPECT(ardeino->backPic != ducklett->backPic);
    EXPECT(ardeino->palette != ducklett->palette);
    EXPECT(ardeino->iconSprite != ducklett->iconSprite);
}

TEST("Ardeino original graphics replace only its Ducklett placeholders")
{
    const struct SpeciesInfo *ardeino = &gSpeciesInfo[SPECIES_ARDEINO];
    const struct SpeciesInfo *ducklett = &gSpeciesInfo[SPECIES_DUCKLETT];
    const struct SpeciesInfo *cingerm = &gSpeciesInfo[SPECIES_CINGERM];
    const struct SpeciesInfo *lechonk = &gSpeciesInfo[SPECIES_LECHONK];
    const struct SpeciesInfo *serbrace = &gSpeciesInfo[SPECIES_SERBRACE];
    const struct SpeciesInfo *ekans = &gSpeciesInfo[SPECIES_EKANS];
    const struct Evolution *evolutions = GetSpeciesEvolutions(SPECIES_ARDEINO);

    EXPECT(ardeino->frontPic != NULL);
    EXPECT(ardeino->backPic != NULL);
    EXPECT(ardeino->palette != NULL);
    EXPECT(ardeino->shinyPalette != NULL);
    EXPECT(ardeino->iconSprite != NULL);
    EXPECT(ardeino->frontAnimFrames != NULL);
    EXPECT(ardeino->frontPic != ducklett->frontPic);
    EXPECT(ardeino->backPic != ducklett->backPic);
    EXPECT(ardeino->palette != ducklett->palette);
    EXPECT(ardeino->shinyPalette != ducklett->shinyPalette);
    EXPECT(ardeino->iconSprite != ducklett->iconSprite);
    EXPECT(ardeino->frontAnimFrames != ducklett->frontAnimFrames);
    EXPECT_EQ(ardeino->frontPicSize, (7 << 4) | 8);
    EXPECT_EQ(ardeino->backPicSize, (5 << 4) | 8);
    EXPECT_EQ(ardeino->frontPicYOffset, 3);
    EXPECT_EQ(ardeino->backPicYOffset, 4);
    EXPECT_EQ((u32)ardeino->iconPalIndex, 3);

    // Gameplay data and the remaining audiovisual placeholders stay unchanged.
    EXPECT_EQ(ardeino->baseHP, 50);
    EXPECT_EQ(ardeino->baseAttack, 45);
    EXPECT_EQ(ardeino->baseDefense, 50);
    EXPECT_EQ(ardeino->baseSpeed, 45);
    EXPECT_EQ(ardeino->baseSpAttack, 65);
    EXPECT_EQ(ardeino->baseSpDefense, 55);
    EXPECT_EQ(ardeino->types[0], TYPE_WATER);
    EXPECT_EQ(ardeino->types[1], TYPE_WATER);
    EXPECT_EQ(ardeino->abilities[0], ABILITY_TORRENT);
    EXPECT_EQ(ardeino->abilities[1], ABILITY_NONE);
    EXPECT_EQ(ardeino->abilities[2], ABILITY_HYDRATION);
    EXPECT_EQ((u32)ardeino->cryId, CRY_DUCKLETT);
    EXPECT_EQ(ardeino->backAnimId, ducklett->backAnimId);
    EXPECT_EQ(ardeino->enemyShadowXOffset, ducklett->enemyShadowXOffset);
    EXPECT_EQ(ardeino->enemyShadowYOffset, ducklett->enemyShadowYOffset);
    EXPECT_EQ((u32)ardeino->enemyShadowSize, (u32)ducklett->enemyShadowSize);
    EXPECT(evolutions != NULL);
    EXPECT_EQ(evolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(evolutions[0].param, 16);
    EXPECT_EQ(evolutions[0].targetSpecies, SPECIES_VELAIRONE);
#if P_FOOTPRINTS
    EXPECT(ardeino->footprint == ducklett->footprint);
#endif
#if OW_POKEMON_OBJECT_EVENTS
    EXPECT(ardeino->overworldData.images == ducklett->overworldData.images);
#if OW_PKMN_OBJECTS_SHARE_PALETTES == FALSE
    EXPECT(ardeino->overworldPalette == ducklett->overworldPalette);
    EXPECT(ardeino->overworldShinyPalette == ducklett->overworldShinyPalette);
#endif
#endif

    EXPECT(cingerm->frontPic != lechonk->frontPic);
    EXPECT(cingerm->backPic != lechonk->backPic);
    EXPECT(serbrace->frontPic != ekans->frontPic);
    EXPECT(serbrace->backPic != ekans->backPic);
}

TEST("Ausonia starter evolutions use original graphics without changing provisional assets")
{
    static const enum Species species[] = {
        SPECIES_ROVASCO,
        SPECIES_SELVAZANNA,
        SPECIES_VIPERCEN,
        SPECIES_TOSSIVAMPA,
        SPECIES_VELAIRONE,
        SPECIES_CODAIRONE,
    };
    static const enum Species placeholders[] = {
        SPECIES_OINKOLOGNE_M,
        SPECIES_MAMOSWINE,
        SPECIES_ARBOK,
        SPECIES_SEVIPER,
        SPECIES_SWANNA,
        SPECIES_BOMBIRDIER,
    };
    static const u8 frontPicSizes[] = {
        (8 << 4) | 7,
        (6 << 4) | 8,
        (6 << 4) | 7,
        (8 << 4) | 8,
        (7 << 4) | 8,
        (7 << 4) | 8,
    };
    static const u8 backPicSizes[] = {
        (6 << 4) | 7,
        (6 << 4) | 7,
        (6 << 4) | 7,
        (7 << 4) | 8,
        (4 << 4) | 8,
        (6 << 4) | 8,
    };
    static const u8 frontOffsets[] = { 4, 0, 6, 2, 3, 3 };
    static const u8 backOffsets[] = { 4, 0, 7, 2, 3, 3 };
    static const u8 iconPalIndices[] = { 5, 5, 3, 3, 3, 3 };

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species[i]];
        const struct SpeciesInfo *placeholder = &gSpeciesInfo[placeholders[i]];

        EXPECT(info->frontPic != NULL);
        EXPECT(info->backPic != NULL);
        EXPECT(info->palette != NULL);
        EXPECT(info->shinyPalette != NULL);
        EXPECT(info->iconSprite != NULL);
        EXPECT(info->frontAnimFrames != NULL);
        EXPECT(info->frontPic != placeholder->frontPic);
        EXPECT(info->backPic != placeholder->backPic);
        EXPECT(info->palette != placeholder->palette);
        EXPECT(info->shinyPalette != placeholder->shinyPalette);
        EXPECT(info->iconSprite != placeholder->iconSprite);
        EXPECT(info->frontAnimFrames != placeholder->frontAnimFrames);
        EXPECT_EQ(info->frontPicSize, frontPicSizes[i]);
        EXPECT_EQ(info->backPicSize, backPicSizes[i]);
        EXPECT_EQ(info->frontPicYOffset, frontOffsets[i]);
        EXPECT_EQ(info->backPicYOffset, backOffsets[i]);
        EXPECT_EQ((u32)info->iconPalIndex, iconPalIndices[i]);

        // Audio, footprint, overworld, shadow and back animation stay provisional.
        EXPECT_EQ((u32)info->cryId, (u32)placeholder->cryId);
        EXPECT_EQ(info->backAnimId, placeholder->backAnimId);
        EXPECT_EQ(info->enemyShadowXOffset, placeholder->enemyShadowXOffset);
        EXPECT_EQ(info->enemyShadowYOffset, placeholder->enemyShadowYOffset);
        EXPECT_EQ((u32)info->enemyShadowSize, (u32)placeholder->enemyShadowSize);
#if P_FOOTPRINTS
        EXPECT(info->footprint == placeholder->footprint);
#endif
#if OW_POKEMON_OBJECT_EVENTS
        EXPECT(info->overworldData.images == placeholder->overworldData.images);
#if OW_PKMN_OBJECTS_SHARE_PALETTES == FALSE
        EXPECT(info->overworldPalette == placeholder->overworldPalette);
        EXPECT(info->overworldShinyPalette == placeholder->overworldShinyPalette);
#endif
#endif
    }

    EXPECT(gSpeciesInfo[SPECIES_CINGERM].frontPic != gSpeciesInfo[SPECIES_LECHONK].frontPic);
    EXPECT(gSpeciesInfo[SPECIES_SERBRACE].frontPic != gSpeciesInfo[SPECIES_EKANS].frontPic);
    EXPECT(gSpeciesInfo[SPECIES_ARDEINO].frontPic != gSpeciesInfo[SPECIES_DUCKLETT].frontPic);
}

TEST("Ausonia Fire starter base data matches the approved prototype")
{
    static const enum Species species[] = { SPECIES_SERBRACE, SPECIES_VIPERCEN, SPECIES_TOSSIVAMPA };
    static const u16 statTotals[] = { 310, 405, 530 };
    static const u8 spAttackEvs[] = { 1, 2, 3 };
    static const u8 stats[][NUM_STATS] = {
        { 45, 40, 40, 65, 70, 50 },
        { 60, 55, 55, 75, 95, 65 },
        { 75, 70, 70, 105, 125, 85 },
    };

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species[i]];

        EXPECT_EQ(info->baseHP, stats[i][STAT_HP]);
        EXPECT_EQ(info->baseAttack, stats[i][STAT_ATK]);
        EXPECT_EQ(info->baseDefense, stats[i][STAT_DEF]);
        EXPECT_EQ(info->baseSpAttack, stats[i][STAT_SPATK]);
        EXPECT_EQ(info->baseSpDefense, stats[i][STAT_SPDEF]);
        EXPECT_EQ(info->baseSpeed, stats[i][STAT_SPEED]);
        EXPECT_EQ(GetBaseStatTotalForTest(species[i]), statTotals[i]);
        EXPECT_EQ(info->types[0], TYPE_FIRE);
        EXPECT_EQ(info->types[1], i == 2 ? TYPE_POISON : TYPE_FIRE);
        EXPECT_EQ(info->abilities[0], ABILITY_BLAZE);
        EXPECT_EQ(info->abilities[1], ABILITY_NONE);
        EXPECT_EQ(info->abilities[2], ABILITY_CORROSION);
        EXPECT_EQ(info->growthRate, GROWTH_MEDIUM_SLOW);
        EXPECT_EQ(info->genderRatio, (50 * 255) / 100);
        EXPECT_EQ(info->eggGroups[0], EGG_GROUP_FIELD);
        EXPECT_EQ(info->eggGroups[1], EGG_GROUP_DRAGON);
        EXPECT_EQ((u32)info->evYield_SpAttack, spAttackEvs[i]);
        EXPECT_EQ(info->catchRate, 45);
        EXPECT_EQ(info->friendship, 70);
        EXPECT_EQ(info->itemCommon, ITEM_NONE);
        EXPECT_EQ(info->itemRare, ITEM_NONE);
    }
}

TEST("Ausonia Fire starter evolutions use only the approved levels")
{
    const struct Evolution *serbraceEvolutions = GetSpeciesEvolutions(SPECIES_SERBRACE);
    const struct Evolution *vipercenEvolutions = GetSpeciesEvolutions(SPECIES_VIPERCEN);

    EXPECT(serbraceEvolutions != NULL);
    EXPECT_EQ(serbraceEvolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(serbraceEvolutions[0].param, 16);
    EXPECT_EQ(serbraceEvolutions[0].targetSpecies, SPECIES_VIPERCEN);
    EXPECT_EQ(serbraceEvolutions[1].method, EVOLUTIONS_END);

    EXPECT(vipercenEvolutions != NULL);
    EXPECT_EQ(vipercenEvolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(vipercenEvolutions[0].param, 36);
    EXPECT_EQ(vipercenEvolutions[0].targetSpecies, SPECIES_TOSSIVAMPA);
    EXPECT_EQ(vipercenEvolutions[1].method, EVOLUTIONS_END);

    EXPECT(GetSpeciesEvolutions(SPECIES_TOSSIVAMPA) == NULL);
}

TEST("Ausonia Fire starter level-up learnsets are complete and ordered")
{
    static const enum Species species[] = { SPECIES_SERBRACE, SPECIES_VIPERCEN, SPECIES_TOSSIVAMPA };
    static const u8 expectedLevels[] = { 1, 1, 4, 7, 9, 12, 15, 18, 22, 26, 30, 34, 38, 43, 48, 54 };
    static const u16 expectedMoves[] = {
        MOVE_SCRATCH, MOVE_LEER, MOVE_EMBER, MOVE_SMOKESCREEN,
        MOVE_FLAME_CHARGE, MOVE_POISON_STING, MOVE_BITE, MOVE_INCINERATE,
        MOVE_COIL, MOVE_VENOSHOCK, MOVE_FIRE_SPIN, MOVE_NASTY_PLOT,
        MOVE_FLAMETHROWER, MOVE_TOXIC, MOVE_SLUDGE_BOMB, MOVE_HEAT_WAVE,
    };

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(species[i]);

        for (u32 j = 0; j < ARRAY_COUNT(expectedMoves); j++)
        {
            EXPECT_EQ(learnset[j].level, expectedLevels[j]);
            EXPECT_EQ(learnset[j].move, expectedMoves[j]);
            if (j != 0)
                EXPECT_GE(learnset[j].level, learnset[j - 1].level);
        }
        EXPECT_EQ(learnset[ARRAY_COUNT(expectedMoves)].move, LEVEL_UP_MOVE_END);
    }
}

TEST("Ausonia Fire starter placeholder assets and Pokédex data are valid")
{
    static const enum Species species[] = { SPECIES_SERBRACE, SPECIES_VIPERCEN, SPECIES_TOSSIVAMPA };
    static const enum PokemonCry cries[] = { CRY_EKANS, CRY_ARBOK, CRY_SEVIPER };
    static const enum NationalDexOrder dexNums[] = { NATIONAL_DEX_SERBRACE, NATIONAL_DEX_VIPERCEN, NATIONAL_DEX_TOSSIVAMPA };
    static const u16 heights[] = { 6, 12, 22 };
    static const u16 weights[] = { 60, 185, 520 };

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species[i]];

        EXPECT(info->frontPic != NULL);
        EXPECT(info->backPic != NULL);
        EXPECT(info->iconSprite != NULL);
        EXPECT(info->palette != NULL);
        EXPECT(info->shinyPalette != NULL);
    #if P_FOOTPRINTS
        EXPECT(info->footprint != NULL);
    #endif
        EXPECT_EQ((u32)info->cryId, cries[i]);
        EXPECT_GT((u32)info->cryId, CRY_NONE);
        EXPECT_LT((u32)info->cryId, CRY_COUNT);
        EXPECT_EQ((u32)info->natDexNum, dexNums[i]);
        EXPECT_EQ(info->height, heights[i]);
        EXPECT_EQ(info->weight, weights[i]);
        EXPECT_NE(StringCompare(info->description, gFallbackPokedexText), 0);
        EXPECT_EQ(info->teachableLearnset[0], MOVE_UNAVAILABLE);
        EXPECT_EQ(info->eggMoveLearnset[0], MOVE_UNAVAILABLE);
    }

    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_SERBRACE].categoryName, COMPOUND_STRING("BRACE")), 0);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_VIPERCEN].categoryName, COMPOUND_STRING("CENERE")), 0);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_TOSSIVAMPA].categoryName, COMPOUND_STRING("FUMAROLA")), 0);
}

TEST("Ausonia Water starter base data matches the approved prototype")
{
    static const enum Species species[] = { SPECIES_ARDEINO, SPECIES_VELAIRONE, SPECIES_CODAIRONE };
    static const u16 statTotals[] = { 310, 405, 530 };
    static const u8 spAttackEvs[] = { 1, 2, 3 };
    static const u8 stats[][NUM_STATS] = {
        { 50, 45, 50, 45, 65, 55 },
        { 65, 60, 65, 60, 85, 70 },
        { 85, 70, 80, 90, 110, 95 },
    };

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species[i]];

        EXPECT_EQ(info->baseHP, stats[i][STAT_HP]);
        EXPECT_EQ(info->baseAttack, stats[i][STAT_ATK]);
        EXPECT_EQ(info->baseDefense, stats[i][STAT_DEF]);
        EXPECT_EQ(info->baseSpAttack, stats[i][STAT_SPATK]);
        EXPECT_EQ(info->baseSpDefense, stats[i][STAT_SPDEF]);
        EXPECT_EQ(info->baseSpeed, stats[i][STAT_SPEED]);
        EXPECT_EQ(GetBaseStatTotalForTest(species[i]), statTotals[i]);
        EXPECT_EQ(info->types[0], TYPE_WATER);
        EXPECT_EQ(info->types[1], i == 2 ? TYPE_FLYING : TYPE_WATER);
        EXPECT_EQ(info->abilities[0], ABILITY_TORRENT);
        EXPECT_EQ(info->abilities[1], ABILITY_NONE);
        EXPECT_EQ(info->abilities[2], ABILITY_HYDRATION);
        EXPECT_EQ(info->growthRate, GROWTH_MEDIUM_SLOW);
        EXPECT_EQ(info->genderRatio, (50 * 255) / 100);
        EXPECT_EQ(info->eggGroups[0], EGG_GROUP_FLYING);
        EXPECT_EQ(info->eggGroups[1], EGG_GROUP_WATER_1);
        EXPECT_EQ((u32)info->evYield_SpAttack, spAttackEvs[i]);
        EXPECT_EQ(info->catchRate, 45);
        EXPECT_EQ(info->friendship, 70);
        EXPECT_EQ(info->itemCommon, ITEM_NONE);
        EXPECT_EQ(info->itemRare, ITEM_NONE);
    }
}

TEST("Ausonia Water starter evolutions use only the approved levels")
{
    const struct Evolution *ardeinoEvolutions = GetSpeciesEvolutions(SPECIES_ARDEINO);
    const struct Evolution *velaironeEvolutions = GetSpeciesEvolutions(SPECIES_VELAIRONE);

    EXPECT(ardeinoEvolutions != NULL);
    EXPECT_EQ(ardeinoEvolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(ardeinoEvolutions[0].param, 16);
    EXPECT_EQ(ardeinoEvolutions[0].targetSpecies, SPECIES_VELAIRONE);
    EXPECT_EQ(ardeinoEvolutions[1].method, EVOLUTIONS_END);

    EXPECT(velaironeEvolutions != NULL);
    EXPECT_EQ(velaironeEvolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(velaironeEvolutions[0].param, 36);
    EXPECT_EQ(velaironeEvolutions[0].targetSpecies, SPECIES_CODAIRONE);
    EXPECT_EQ(velaironeEvolutions[1].method, EVOLUTIONS_END);

    EXPECT(GetSpeciesEvolutions(SPECIES_CODAIRONE) == NULL);
}

TEST("Ausonia Water starter level-up learnsets are complete and ordered")
{
    static const enum Species species[] = { SPECIES_ARDEINO, SPECIES_VELAIRONE, SPECIES_CODAIRONE };
    static const u8 expectedLevels[] = { 1, 1, 4, 7, 9, 12, 15, 18, 22, 26, 30, 34, 38, 43, 48, 54 };
    static const u16 expectedMoves[] = {
        MOVE_POUND, MOVE_GROWL, MOVE_WATER_GUN, MOVE_PECK,
        MOVE_QUICK_ATTACK, MOVE_MIST, MOVE_SUPERSONIC, MOVE_WING_ATTACK,
        MOVE_AQUA_RING, MOVE_AIR_SLASH, MOVE_BRINE, MOVE_AGILITY,
        MOVE_TAILWIND, MOVE_ROOST, MOVE_HYDRO_PUMP, MOVE_HURRICANE,
    };

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(species[i]);

        for (u32 j = 0; j < ARRAY_COUNT(expectedMoves); j++)
        {
            EXPECT_EQ(learnset[j].level, expectedLevels[j]);
            EXPECT_EQ(learnset[j].move, expectedMoves[j]);
            if (j != 0)
                EXPECT_GE(learnset[j].level, learnset[j - 1].level);
        }
        EXPECT_EQ(learnset[ARRAY_COUNT(expectedMoves)].move, LEVEL_UP_MOVE_END);
    }
}

TEST("Ausonia Water starter graphics and Pokédex data are valid")
{
    static const enum Species species[] = { SPECIES_ARDEINO, SPECIES_VELAIRONE, SPECIES_CODAIRONE };
    static const enum PokemonCry cries[] = { CRY_DUCKLETT, CRY_SWANNA, CRY_BOMBIRDIER };
    static const enum NationalDexOrder dexNums[] = { NATIONAL_DEX_ARDEINO, NATIONAL_DEX_VELAIRONE, NATIONAL_DEX_CODAIRONE };
    static const u16 heights[] = { 5, 10, 17 };
    static const u16 weights[] = { 38, 125, 360 };

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species[i]];

        EXPECT(info->frontPic != NULL);
        EXPECT(info->backPic != NULL);
        EXPECT(info->iconSprite != NULL);
        EXPECT(info->palette != NULL);
        EXPECT(info->shinyPalette != NULL);
    #if P_FOOTPRINTS
        EXPECT(info->footprint != NULL);
    #endif
        EXPECT_EQ((u32)info->cryId, cries[i]);
        EXPECT_GT((u32)info->cryId, CRY_NONE);
        EXPECT_LT((u32)info->cryId, CRY_COUNT);
        EXPECT_EQ((u32)info->natDexNum, dexNums[i]);
        EXPECT_EQ(info->height, heights[i]);
        EXPECT_EQ(info->weight, weights[i]);
        EXPECT_NE(StringCompare(info->description, gFallbackPokedexText), 0);
        EXPECT_EQ(info->teachableLearnset[0], MOVE_UNAVAILABLE);
        EXPECT_EQ(info->eggMoveLearnset[0], MOVE_UNAVAILABLE);
    }

    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_ARDEINO].categoryName, COMPOUND_STRING("PIUMALAGO")), 0);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_VELAIRONE].categoryName, COMPOUND_STRING("VELO")), 0);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_CODAIRONE].categoryName, COMPOUND_STRING("RIFLESSO")), 0);
}
