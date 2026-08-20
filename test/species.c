#include "global.h"
#include "random_mon_generation.h"
#include "string_util.h"
#include "test/test.h"
#include "constants/form_change_types.h"
#include "constants/teaching_types.h"

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

static bool32 MoveListContains(const u16 *moves, u16 move)
{
    for (u32 i = 0; moves[i] != MOVE_UNAVAILABLE; i++)
    {
        if (moves[i] == move)
            return TRUE;
    }
    return FALSE;
}

static u32 MoveListCount(const u16 *moves)
{
    u32 count = 0;

    while (moves[count] != MOVE_UNAVAILABLE)
        count++;
    return count;
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
    EXPECT_EQ(SPECIES_BORGOTTO, SPECIES_CODAIRONE + 1);
    EXPECT_EQ(SPECIES_PASTUFO, SPECIES_BORGOTTO + 1);
    EXPECT_EQ(SPECIES_MICIOLO, SPECIES_PASTUFO + 1);
    EXPECT_EQ(SPECIES_FELIVATES, SPECIES_MICIOLO + 1);
    EXPECT_EQ(SPECIES_FOLIARVA, SPECIES_FELIVATES + 1);
    EXPECT_EQ(SPECIES_CRISALVIA, SPECIES_FOLIARVA + 1);
    EXPECT_EQ(SPECIES_INFIORALA, SPECIES_CRISALVIA + 1);
    EXPECT_EQ(SPECIES_GHEPIO, SPECIES_INFIORALA + 1);
    EXPECT_EQ(SPECIES_TINUNCOL, SPECIES_GHEPIO + 1);
    EXPECT_EQ(SPECIES_PEREGRINUS, SPECIES_TINUNCOL + 1);
    EXPECT_EQ(SPECIES_GAZZUOLA, SPECIES_PEREGRINUS + 1);
    EXPECT_EQ(SPECIES_BRILLAZZA, SPECIES_GAZZUOLA + 1);
    EXPECT_EQ(SPECIES_GAZZOMBRA, SPECIES_BRILLAZZA + 1);
    EXPECT_EQ(SPECIES_MOLOSPSY, SPECIES_GAZZOMBRA + 1);
    EXPECT_EQ(SPECIES_EGG, SPECIES_MOLOSPSY + 1);
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
    EXPECT_EQ(NATIONAL_DEX_BORGOTTO, NATIONAL_DEX_CODAIRONE + 1);
    EXPECT_EQ(NATIONAL_DEX_PASTUFO, NATIONAL_DEX_BORGOTTO + 1);
    EXPECT_EQ(NATIONAL_DEX_MICIOLO, NATIONAL_DEX_PASTUFO + 1);
    EXPECT_EQ(NATIONAL_DEX_FELIVATES, NATIONAL_DEX_MICIOLO + 1);
    EXPECT_EQ(NATIONAL_DEX_FOLIARVA, NATIONAL_DEX_FELIVATES + 1);
    EXPECT_EQ(NATIONAL_DEX_CRISALVIA, NATIONAL_DEX_FOLIARVA + 1);
    EXPECT_EQ(NATIONAL_DEX_INFIORALA, NATIONAL_DEX_CRISALVIA + 1);
    EXPECT_EQ(NATIONAL_DEX_GHEPIO, NATIONAL_DEX_INFIORALA + 1);
    EXPECT_EQ(NATIONAL_DEX_TINUNCOL, NATIONAL_DEX_GHEPIO + 1);
    EXPECT_EQ(NATIONAL_DEX_PEREGRINUS, NATIONAL_DEX_TINUNCOL + 1);
    EXPECT_EQ(NATIONAL_DEX_GAZZUOLA, NATIONAL_DEX_PEREGRINUS + 1);
    EXPECT_EQ(NATIONAL_DEX_BRILLAZZA, NATIONAL_DEX_GAZZUOLA + 1);
    EXPECT_EQ(NATIONAL_DEX_GAZZOMBRA, NATIONAL_DEX_BRILLAZZA + 1);
    EXPECT_EQ(NATIONAL_DEX_MOLOSPSY, NATIONAL_DEX_GAZZOMBRA + 1);
    EXPECT_EQ(NATIONAL_DEX_COUNT, NATIONAL_DEX_MOLOSPSY);

    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_CINGERM), COMPOUND_STRING("Cingerm")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_ROVASCO), COMPOUND_STRING("Rovasco")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_SELVAZANNA), COMPOUND_STRING("Selvazanna")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_SERBRACE), COMPOUND_STRING("Serbrace")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_VIPERCEN), COMPOUND_STRING("Vipercen")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_TOSSIVAMPA), COMPOUND_STRING("Tossivampa")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_ARDEINO), COMPOUND_STRING("Ardeino")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_VELAIRONE), COMPOUND_STRING("Velairone")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_CODAIRONE), COMPOUND_STRING("Codairone")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_BORGOTTO), COMPOUND_STRING("Borgotto")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_PASTUFO), COMPOUND_STRING("Pastufo")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_MICIOLO), COMPOUND_STRING("Miciolo")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_FELIVATES), COMPOUND_STRING("Felivates")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_FOLIARVA), COMPOUND_STRING("Foliarva")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_CRISALVIA), COMPOUND_STRING("Crisalvia")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_INFIORALA), COMPOUND_STRING("Infiorala")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_GHEPIO), COMPOUND_STRING("Ghepio")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_TINUNCOL), COMPOUND_STRING("Tinuncol")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_PEREGRINUS), COMPOUND_STRING("Peregrinus")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_GAZZUOLA), COMPOUND_STRING("Gazzuola")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_BRILLAZZA), COMPOUND_STRING("Brillazza")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_GAZZOMBRA), COMPOUND_STRING("Gazzombra")), 0);
    EXPECT_EQ(StringCompare(GetSpeciesName(SPECIES_MOLOSPSY), COMPOUND_STRING("Molospsy")), 0);
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

TEST("Ausonia common fauna base data matches the canonical batch")
{
    static const enum Species species[] = { SPECIES_BORGOTTO, SPECIES_PASTUFO, SPECIES_MICIOLO, SPECIES_FELIVATES };
    static const u8 stats[][NUM_STATS] = {
        { 45, 50, 40, 55, 30, 40 },
        { 80, 88, 78, 65, 42, 67 },
        { 42, 40, 38, 65, 50, 45 },
        { 70, 52, 60, 100, 98, 80 },
    };
    static const u16 statTotals[] = { 260, 420, 280, 460 };
    static const u8 catchRates[] = { 255, 120, 190, 75 };
    static const u16 expYields[] = { 64, 158, 70, 168 };
    static const u16 heights[] = { 3, 6, 4, 9 };
    static const u16 weights[] = { 32, 98, 41, 136 };
    const u8 borgottoHpYield = gSpeciesInfo[SPECIES_BORGOTTO].evYield_HP;
    const u8 pastufoHpYield = gSpeciesInfo[SPECIES_PASTUFO].evYield_HP;
    const u8 micioloSpeedYield = gSpeciesInfo[SPECIES_MICIOLO].evYield_Speed;
    const u8 felivatesSpeedYield = gSpeciesInfo[SPECIES_FELIVATES].evYield_Speed;

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species[i]];

        EXPECT_EQ(info->baseHP, stats[i][STAT_HP]);
        EXPECT_EQ(info->baseAttack, stats[i][STAT_ATK]);
        EXPECT_EQ(info->baseDefense, stats[i][STAT_DEF]);
        EXPECT_EQ(info->baseSpeed, stats[i][STAT_SPEED]);
        EXPECT_EQ(info->baseSpAttack, stats[i][STAT_SPATK]);
        EXPECT_EQ(info->baseSpDefense, stats[i][STAT_SPDEF]);
        EXPECT_EQ(GetBaseStatTotalForTest(species[i]), statTotals[i]);
        EXPECT_EQ(info->catchRate, catchRates[i]);
        EXPECT_EQ(info->expYield, expYields[i]);
        EXPECT_EQ(info->height, heights[i]);
        EXPECT_EQ(info->weight, weights[i]);
        EXPECT_EQ(info->eggCycles, 15);
        EXPECT_EQ(info->friendship, 70);
        EXPECT_EQ(info->growthRate, GROWTH_MEDIUM_FAST);
        EXPECT_EQ(info->eggGroups[0], EGG_GROUP_FIELD);
        EXPECT_EQ(info->eggGroups[1], EGG_GROUP_FIELD);
        EXPECT_NE(StringCompare(info->description, gFallbackPokedexText), 0);
    }

    EXPECT_EQ(gSpeciesInfo[SPECIES_BORGOTTO].types[0], TYPE_NORMAL);
    EXPECT_EQ(gSpeciesInfo[SPECIES_BORGOTTO].types[1], TYPE_NORMAL);
    EXPECT_EQ(gSpeciesInfo[SPECIES_PASTUFO].types[0], TYPE_NORMAL);
    EXPECT_EQ(gSpeciesInfo[SPECIES_PASTUFO].types[1], TYPE_GROUND);
    EXPECT_EQ(gSpeciesInfo[SPECIES_MICIOLO].types[0], TYPE_NORMAL);
    EXPECT_EQ(gSpeciesInfo[SPECIES_MICIOLO].types[1], TYPE_NORMAL);
    EXPECT_EQ(gSpeciesInfo[SPECIES_FELIVATES].types[0], TYPE_NORMAL);
    EXPECT_EQ(gSpeciesInfo[SPECIES_FELIVATES].types[1], TYPE_PSYCHIC);
    EXPECT_EQ(gSpeciesInfo[SPECIES_BORGOTTO].abilities[0], ABILITY_KEEN_EYE);
    EXPECT_EQ(gSpeciesInfo[SPECIES_BORGOTTO].abilities[2], ABILITY_PICKUP);
    EXPECT_EQ(gSpeciesInfo[SPECIES_PASTUFO].abilities[0], ABILITY_KEEN_EYE);
    EXPECT_EQ(gSpeciesInfo[SPECIES_PASTUFO].abilities[2], ABILITY_PICKUP);
    EXPECT_EQ(gSpeciesInfo[SPECIES_MICIOLO].abilities[0], ABILITY_LIMBER);
    EXPECT_EQ(gSpeciesInfo[SPECIES_MICIOLO].abilities[2], ABILITY_SYNCHRONIZE);
    EXPECT_EQ(gSpeciesInfo[SPECIES_FELIVATES].abilities[0], ABILITY_LIMBER);
    EXPECT_EQ(gSpeciesInfo[SPECIES_FELIVATES].abilities[2], ABILITY_SYNCHRONIZE);
    EXPECT_EQ(borgottoHpYield, 1);
    EXPECT_EQ(pastufoHpYield, 2);
    EXPECT_EQ(micioloSpeedYield, 1);
    EXPECT_EQ(felivatesSpeedYield, 2);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_BORGOTTO].categoryName, COMPOUND_STRING("BORGO")), 0);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_PASTUFO].categoryName, COMPOUND_STRING("PROVVISTA")), 0);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_MICIOLO].categoryName, COMPOUND_STRING("GATTO")), 0);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_FELIVATES].categoryName, COMPOUND_STRING("ARMONIA")), 0);
}

TEST("Ausonia common fauna evolutions use the canonical methods")
{
    const struct Evolution *borgottoEvolutions = GetSpeciesEvolutions(SPECIES_BORGOTTO);
    const struct Evolution *micioloEvolutions = GetSpeciesEvolutions(SPECIES_MICIOLO);

    EXPECT_EQ(borgottoEvolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(borgottoEvolutions[0].param, 18);
    EXPECT_EQ(borgottoEvolutions[0].targetSpecies, SPECIES_PASTUFO);
    EXPECT_EQ(borgottoEvolutions[1].method, EVOLUTIONS_END);
    EXPECT(GetSpeciesEvolutions(SPECIES_PASTUFO) == NULL);

    EXPECT_EQ(micioloEvolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(micioloEvolutions[0].param, 0);
    EXPECT_EQ(micioloEvolutions[0].targetSpecies, SPECIES_FELIVATES);
    EXPECT(micioloEvolutions[0].params != NULL);
    EXPECT_EQ(micioloEvolutions[0].params[0].condition, IF_MIN_FRIENDSHIP);
    EXPECT_EQ(micioloEvolutions[0].params[0].arg1, 160);
    EXPECT_EQ(micioloEvolutions[0].params[1].condition, CONDITIONS_END);
    EXPECT_EQ(micioloEvolutions[1].method, EVOLUTIONS_END);
    EXPECT(GetSpeciesEvolutions(SPECIES_FELIVATES) == NULL);
}

TEST("Ausonia common fauna level-up learnsets are canonical and ordered")
{
    static const u8 borgottoLevels[] = { 1, 1, 4, 7, 10, 13, 16, 20, 24, 28 };
    static const u16 borgottoMoves[] = {
        MOVE_TACKLE, MOVE_TAIL_WHIP, MOVE_SAND_ATTACK, MOVE_BITE, MOVE_ODOR_SLEUTH,
        MOVE_COVET, MOVE_HELPING_HAND, MOVE_WORK_UP, MOVE_TAKE_DOWN, MOVE_CRUNCH,
    };
    static const u8 pastufoLevels[] = { 0, 1, 1, 4, 7, 10, 13, 16, 20, 22, 27, 27, 27, 33, 39, 45, 52 };
    static const u16 pastufoMoves[] = {
        MOVE_MUD_SLAP, MOVE_TACKLE, MOVE_TAIL_WHIP, MOVE_SAND_ATTACK, MOVE_BITE,
        MOVE_ODOR_SLEUTH, MOVE_COVET, MOVE_HELPING_HAND, MOVE_WORK_UP, MOVE_BULLDOZE,
        MOVE_STOCKPILE, MOVE_SWALLOW, MOVE_SPIT_UP, MOVE_DIG, MOVE_BODY_SLAM,
        MOVE_CRUNCH, MOVE_EARTHQUAKE,
    };
    static const u8 micioloLevels[] = { 1, 1, 4, 7, 10, 13, 16, 19, 23, 27, 31 };
    static const u16 micioloMoves[] = {
        MOVE_SCRATCH, MOVE_GROWL, MOVE_TAIL_WHIP, MOVE_FAKE_OUT, MOVE_COVET,
        MOVE_SWIFT, MOVE_CHARM, MOVE_CONFUSION, MOVE_SING, MOVE_PSYBEAM, MOVE_AGILITY,
    };
    static const u8 felivatesLevels[] = { 0, 1, 1, 4, 7, 10, 13, 16, 19, 22, 26, 30, 34, 39, 44, 50, 56 };
    static const u16 felivatesMoves[] = {
        MOVE_CONFUSION, MOVE_SCRATCH, MOVE_GROWL, MOVE_TAIL_WHIP, MOVE_FAKE_OUT,
        MOVE_COVET, MOVE_SWIFT, MOVE_CHARM, MOVE_CONFUSION, MOVE_PSYBEAM,
        MOVE_CALM_MIND, MOVE_SAFEGUARD, MOVE_HEAL_BELL, MOVE_PSYCHIC,
        MOVE_BATON_PASS, MOVE_MOONLIGHT, MOVE_FUTURE_SIGHT,
    };
    const struct LevelUpMove *learnsets[] = {
        GetSpeciesLevelUpLearnset(SPECIES_BORGOTTO),
        GetSpeciesLevelUpLearnset(SPECIES_PASTUFO),
        GetSpeciesLevelUpLearnset(SPECIES_MICIOLO),
        GetSpeciesLevelUpLearnset(SPECIES_FELIVATES),
    };
    const u8 *levels[] = { borgottoLevels, pastufoLevels, micioloLevels, felivatesLevels };
    const u16 *moves[] = { borgottoMoves, pastufoMoves, micioloMoves, felivatesMoves };
    const u32 counts[] = {
        ARRAY_COUNT(borgottoMoves), ARRAY_COUNT(pastufoMoves),
        ARRAY_COUNT(micioloMoves), ARRAY_COUNT(felivatesMoves),
    };

    for (u32 i = 0; i < ARRAY_COUNT(learnsets); i++)
    {
        for (u32 j = 0; j < counts[i]; j++)
        {
            EXPECT_EQ(learnsets[i][j].level, levels[i][j]);
            EXPECT_EQ(learnsets[i][j].move, moves[i][j]);
            if (j > 0)
                EXPECT_GE(learnsets[i][j].level, learnsets[i][j - 1].level);
        }
        EXPECT_EQ(learnsets[i][counts[i]].move, LEVEL_UP_MOVE_END);
    }
}

TEST("Ausonia common fauna teachable and Egg Move compatibility is complete")
{
    static const u16 borgottoTeachables[] = {
        MOVE_PROTECT, MOVE_REST, MOVE_SLEEP_TALK, MOVE_SUBSTITUTE, MOVE_FACADE,
        MOVE_ENDURE, MOVE_THIEF, MOVE_SNARL, MOVE_SWIFT, MOVE_CHARM, MOVE_DIG,
        MOVE_ROCK_SMASH, MOVE_STRENGTH, MOVE_HELPING_HAND, MOVE_HYPER_VOICE,
        MOVE_KNOCK_OFF, MOVE_SUPER_FANG, MOVE_ENDEAVOR, MOVE_UPROAR,
    };
    static const u16 pastufoExtraTeachables[] = {
        MOVE_BULLDOZE, MOVE_EARTHQUAKE, MOVE_SANDSTORM, MOVE_ROCK_TOMB,
        MOVE_ROCK_SLIDE, MOVE_BRICK_BREAK, MOVE_LOW_SWEEP, MOVE_STOMPING_TANTRUM,
        MOVE_HIGH_HORSEPOWER, MOVE_IRON_HEAD,
    };
    static const u16 micioloTeachables[] = {
        MOVE_PROTECT, MOVE_REST, MOVE_SLEEP_TALK, MOVE_SUBSTITUTE, MOVE_FACADE,
        MOVE_ENDURE, MOVE_THIEF, MOVE_SWIFT, MOVE_CHARM, MOVE_DIG, MOVE_CALM_MIND,
        MOVE_PSYCH_UP, MOVE_THUNDER_WAVE, MOVE_SHADOW_BALL, MOVE_LIGHT_SCREEN,
        MOVE_REFLECT, MOVE_SAFEGUARD, MOVE_HELPING_HAND, MOVE_HEAL_BELL,
        MOVE_HYPER_VOICE, MOVE_KNOCK_OFF, MOVE_COVET, MOVE_ZEN_HEADBUTT,
    };
    static const u16 felivatesExtraTeachables[] = {
        MOVE_PSYCHIC, MOVE_PSYSHOCK, MOVE_DAZZLING_GLEAM, MOVE_ENERGY_BALL,
        MOVE_TRICK_ROOM, MOVE_STORED_POWER, MOVE_TRICK, MOVE_MAGIC_COAT,
        MOVE_ALLY_SWITCH, MOVE_EXPANDING_FORCE,
    };
    static const u16 borgottoEggMoves[] = {
        MOVE_BABY_DOLL_EYES, MOVE_HOWL, MOVE_YAWN, MOVE_DOUBLE_EDGE,
        MOVE_LAST_RESORT, MOVE_PLAY_ROUGH,
    };
    static const u16 micioloEggMoves[] = {
        MOVE_COPYCAT, MOVE_FAKE_TEARS, MOVE_WISH, MOVE_YAWN,
        MOVE_BATON_PASS, MOVE_HEAL_BELL, MOVE_TICKLE,
    };
    const u16 *borgotto = gSpeciesInfo[SPECIES_BORGOTTO].teachableLearnset;
    const u16 *pastufo = gSpeciesInfo[SPECIES_PASTUFO].teachableLearnset;
    const u16 *miciolo = gSpeciesInfo[SPECIES_MICIOLO].teachableLearnset;
    const u16 *felivates = gSpeciesInfo[SPECIES_FELIVATES].teachableLearnset;

    for (u32 i = 0; i < ARRAY_COUNT(borgottoTeachables); i++)
    {
        EXPECT(MoveListContains(borgotto, borgottoTeachables[i]));
        EXPECT(MoveListContains(pastufo, borgottoTeachables[i]));
    }
    for (u32 i = 0; i < ARRAY_COUNT(pastufoExtraTeachables); i++)
        EXPECT(MoveListContains(pastufo, pastufoExtraTeachables[i]));
    for (u32 i = 0; i < ARRAY_COUNT(micioloTeachables); i++)
    {
        EXPECT(MoveListContains(miciolo, micioloTeachables[i]));
        EXPECT(MoveListContains(felivates, micioloTeachables[i]));
    }
    for (u32 i = 0; i < ARRAY_COUNT(felivatesExtraTeachables); i++)
        EXPECT(MoveListContains(felivates, felivatesExtraTeachables[i]));
    for (u32 i = 0; i < ARRAY_COUNT(borgottoEggMoves); i++)
        EXPECT(MoveListContains(gSpeciesInfo[SPECIES_BORGOTTO].eggMoveLearnset, borgottoEggMoves[i]));
    for (u32 i = 0; i < ARRAY_COUNT(micioloEggMoves); i++)
        EXPECT(MoveListContains(gSpeciesInfo[SPECIES_MICIOLO].eggMoveLearnset, micioloEggMoves[i]));

    EXPECT_EQ(MoveListCount(borgotto), ARRAY_COUNT(borgottoTeachables));
    EXPECT_EQ(MoveListCount(pastufo), 29);
    EXPECT_EQ(MoveListCount(miciolo), ARRAY_COUNT(micioloTeachables));
    EXPECT_EQ(MoveListCount(felivates), 33);
    EXPECT_EQ(MoveListCount(gSpeciesInfo[SPECIES_BORGOTTO].eggMoveLearnset), ARRAY_COUNT(borgottoEggMoves));
    EXPECT_EQ(MoveListCount(gSpeciesInfo[SPECIES_MICIOLO].eggMoveLearnset), ARRAY_COUNT(micioloEggMoves));
}

TEST("Ausonia common fauna uses original battle graphics and provisional auxiliary assets")
{
    static const enum Species species[] = { SPECIES_BORGOTTO, SPECIES_PASTUFO, SPECIES_MICIOLO, SPECIES_FELIVATES };
    static const enum Species placeholders[] = { SPECIES_LILLIPUP, SPECIES_HERDIER, SPECIES_SKITTY, SPECIES_ESPEON };
    static const u8 frontPicSizes[] = {
        (6 << 4) | 7,
        (7 << 4) | 8,
        (6 << 4) | 8,
        (7 << 4) | 8,
    };
    static const u8 backPicSizes[] = {
        (6 << 4) | 6,
        (7 << 4) | 8,
        (6 << 4) | 8,
        (7 << 4) | 8,
    };
    static const u8 picOffsets[] = { 4, 3, 3, 3 };
    static const u8 iconPalIndices[] = { 2, 1, 0, 1 };

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species[i]];
        const struct SpeciesInfo *placeholder = &gSpeciesInfo[placeholders[i]];

        EXPECT(info->frontPic != placeholder->frontPic);
        EXPECT(info->backPic != placeholder->backPic);
        EXPECT(info->palette != placeholder->palette);
        EXPECT(info->shinyPalette != placeholder->shinyPalette);
        EXPECT(info->iconSprite != placeholder->iconSprite);
        EXPECT_EQ((u32)info->cryId, (u32)placeholder->cryId);
        EXPECT_EQ((u32)info->frontPicSize, (u32)frontPicSizes[i]);
        EXPECT_EQ((u32)info->backPicSize, (u32)backPicSizes[i]);
        EXPECT_EQ((u32)info->frontPicYOffset, (u32)picOffsets[i]);
        EXPECT_EQ((u32)info->backPicYOffset, (u32)picOffsets[i]);
        EXPECT_EQ((u32)info->iconPalIndex, (u32)iconPalIndices[i]);
#if P_FOOTPRINTS
        EXPECT(info->footprint == placeholder->footprint);
#endif
#if OW_POKEMON_OBJECT_EVENTS
        EXPECT(info->overworldData.images == placeholder->overworldData.images);
#endif
    }
}

TEST("Ausonia early Bug fauna uses original battle graphics and provisional auxiliary assets")
{
    static const enum Species species[] = { SPECIES_FOLIARVA, SPECIES_CRISALVIA, SPECIES_INFIORALA };
    static const enum Species placeholders[] = { SPECIES_CATERPIE, SPECIES_METAPOD, SPECIES_BUTTERFREE };
    static const u8 frontPicSizes[] = {
        (6 << 4) | 6,
        (5 << 4) | 8,
        (8 << 4) | 7,
    };
    static const u8 backPicSizes[] = {
        (5 << 4) | 6,
        (5 << 4) | 8,
        (8 << 4) | 7,
    };
    static const u8 frontOffsets[] = { 8, 3, 4 };
    static const u8 backOffsets[] = { 8, 3, 4 };

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species[i]];
        const struct SpeciesInfo *placeholder = &gSpeciesInfo[placeholders[i]];

        EXPECT(info->frontPic != placeholder->frontPic);
        EXPECT(info->backPic != placeholder->backPic);
        EXPECT(info->palette != placeholder->palette);
        EXPECT(info->shinyPalette != placeholder->shinyPalette);
        EXPECT(info->iconSprite != placeholder->iconSprite);
        EXPECT_EQ((u32)info->frontPicSize, (u32)frontPicSizes[i]);
        EXPECT_EQ((u32)info->backPicSize, (u32)backPicSizes[i]);
        EXPECT_EQ((u32)info->frontPicYOffset, (u32)frontOffsets[i]);
        EXPECT_EQ((u32)info->backPicYOffset, (u32)backOffsets[i]);
        EXPECT_EQ((u32)info->iconPalIndex, 1);

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
}

TEST("Ausonia early Bug fauna base data matches the canonical batch")
{
    static const enum Species species[] = { SPECIES_FOLIARVA, SPECIES_CRISALVIA, SPECIES_INFIORALA };
    static const u8 stats[][NUM_STATS] = {
        { 45, 35, 40, 45, 35, 40 },
        { 55, 30, 75, 25, 40, 65 },
        { 70, 45, 65, 90, 100, 80 },
    };
    static const u16 statTotals[] = { 240, 290, 450 };
    static const u8 catchRates[] = { 255, 120, 75 };
    static const u16 expYields[] = { 54, 108, 178 };
    static const u16 heights[] = { 3, 5, 9 };
    static const u16 weights[] = { 24, 68, 142 };
    const u8 foliarvaHpYield = gSpeciesInfo[SPECIES_FOLIARVA].evYield_HP;
    const u8 crisalviaDefenseYield = gSpeciesInfo[SPECIES_CRISALVIA].evYield_Defense;
    const u8 infioralaSpAttackYield = gSpeciesInfo[SPECIES_INFIORALA].evYield_SpAttack;

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species[i]];

        EXPECT_EQ(info->baseHP, stats[i][STAT_HP]);
        EXPECT_EQ(info->baseAttack, stats[i][STAT_ATK]);
        EXPECT_EQ(info->baseDefense, stats[i][STAT_DEF]);
        EXPECT_EQ(info->baseSpeed, stats[i][STAT_SPEED]);
        EXPECT_EQ(info->baseSpAttack, stats[i][STAT_SPATK]);
        EXPECT_EQ(info->baseSpDefense, stats[i][STAT_SPDEF]);
        EXPECT_EQ(GetBaseStatTotalForTest(species[i]), statTotals[i]);
        EXPECT_EQ(info->catchRate, catchRates[i]);
        EXPECT_EQ(info->expYield, expYields[i]);
        EXPECT_EQ(info->height, heights[i]);
        EXPECT_EQ(info->weight, weights[i]);
        EXPECT_EQ(info->genderRatio, (50 * 255) / 100);
        EXPECT_EQ(info->eggCycles, 15);
        EXPECT_EQ(info->friendship, 70);
        EXPECT_EQ(info->growthRate, GROWTH_MEDIUM_FAST);
        EXPECT_EQ(info->eggGroups[0], EGG_GROUP_BUG);
        EXPECT_EQ(info->eggGroups[1], EGG_GROUP_BUG);
        EXPECT_EQ((u32)info->bodyColor, BODY_COLOR_GREEN);
        EXPECT_NE(StringCompare(info->description, gFallbackPokedexText), 0);
        EXPECT_EQ((u32)info->teachingType, EXPLICIT_TEACHABLES);
    }

    EXPECT_EQ(gSpeciesInfo[SPECIES_FOLIARVA].types[0], TYPE_BUG);
    EXPECT_EQ(gSpeciesInfo[SPECIES_FOLIARVA].types[1], TYPE_BUG);
    EXPECT_EQ(gSpeciesInfo[SPECIES_CRISALVIA].types[0], TYPE_BUG);
    EXPECT_EQ(gSpeciesInfo[SPECIES_CRISALVIA].types[1], TYPE_GRASS);
    EXPECT_EQ(gSpeciesInfo[SPECIES_INFIORALA].types[0], TYPE_BUG);
    EXPECT_EQ(gSpeciesInfo[SPECIES_INFIORALA].types[1], TYPE_GRASS);
    EXPECT_EQ(gSpeciesInfo[SPECIES_FOLIARVA].abilities[0], ABILITY_SHIELD_DUST);
    EXPECT_EQ(gSpeciesInfo[SPECIES_FOLIARVA].abilities[1], ABILITY_SWARM);
    EXPECT_EQ(gSpeciesInfo[SPECIES_FOLIARVA].abilities[2], ABILITY_CHLOROPHYLL);
    EXPECT_EQ(gSpeciesInfo[SPECIES_CRISALVIA].abilities[0], ABILITY_SHED_SKIN);
    EXPECT_EQ(gSpeciesInfo[SPECIES_CRISALVIA].abilities[1], ABILITY_LEAF_GUARD);
    EXPECT_EQ(gSpeciesInfo[SPECIES_CRISALVIA].abilities[2], ABILITY_OVERCOAT);
    EXPECT_EQ(gSpeciesInfo[SPECIES_INFIORALA].abilities[0], ABILITY_COMPOUND_EYES);
    EXPECT_EQ(gSpeciesInfo[SPECIES_INFIORALA].abilities[1], ABILITY_CHLOROPHYLL);
    EXPECT_EQ(gSpeciesInfo[SPECIES_INFIORALA].abilities[2], ABILITY_TINTED_LENS);
    EXPECT_EQ(foliarvaHpYield, 1);
    EXPECT_EQ(crisalviaDefenseYield, 2);
    EXPECT_EQ(infioralaSpAttackYield, 2);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_FOLIARVA].categoryName, COMPOUND_STRING("LARVAFOGLIA")), 0);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_CRISALVIA].categoryName, COMPOUND_STRING("CRISALIDE")), 0);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_INFIORALA].categoryName, COMPOUND_STRING("FLOREALE")), 0);
}

TEST("Ausonia early Bug fauna evolutions use the canonical levels")
{
    const struct Evolution *foliarvaEvolutions = GetSpeciesEvolutions(SPECIES_FOLIARVA);
    const struct Evolution *crisalviaEvolutions = GetSpeciesEvolutions(SPECIES_CRISALVIA);

    EXPECT_EQ(foliarvaEvolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(foliarvaEvolutions[0].param, 10);
    EXPECT_EQ(foliarvaEvolutions[0].targetSpecies, SPECIES_CRISALVIA);
    EXPECT_EQ(foliarvaEvolutions[1].method, EVOLUTIONS_END);
    EXPECT_EQ(crisalviaEvolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(crisalviaEvolutions[0].param, 18);
    EXPECT_EQ(crisalviaEvolutions[0].targetSpecies, SPECIES_INFIORALA);
    EXPECT_EQ(crisalviaEvolutions[1].method, EVOLUTIONS_END);
    EXPECT(GetSpeciesEvolutions(SPECIES_INFIORALA) == NULL);
}

TEST("Ausonia early Bug fauna level-up learnsets are canonical and ordered")
{
    static const u8 foliarvaLevels[] = { 1, 1, 4, 6, 8, 10, 13, 15 };
    static const u16 foliarvaMoves[] = {
        MOVE_TACKLE, MOVE_STRING_SHOT, MOVE_ABSORB, MOVE_BUG_BITE,
        MOVE_STUN_SPORE, MOVE_RAZOR_LEAF, MOVE_STRUGGLE_BUG, MOVE_MEGA_DRAIN,
    };
    static const u8 crisalviaLevels[] = { 1, 1, 1, 6, 8, 10, 12, 15, 18 };
    static const u16 crisalviaMoves[] = {
        MOVE_HARDEN, MOVE_STRING_SHOT, MOVE_ABSORB, MOVE_BUG_BITE,
        MOVE_STUN_SPORE, MOVE_HARDEN, MOVE_PROTECT, MOVE_MEGA_DRAIN,
        MOVE_STRUGGLE_BUG,
    };
    static const u8 infioralaLevels[] = { 1, 1, 1, 6, 8, 10, 13, 15, 18, 20, 23, 26, 30, 34, 38, 42, 46 };
    static const u16 infioralaMoves[] = {
        MOVE_TACKLE, MOVE_STRING_SHOT, MOVE_ABSORB, MOVE_BUG_BITE,
        MOVE_STUN_SPORE, MOVE_RAZOR_LEAF, MOVE_STRUGGLE_BUG, MOVE_MEGA_DRAIN,
        MOVE_GUST, MOVE_SLEEP_POWDER, MOVE_AIR_CUTTER, MOVE_POLLEN_PUFF,
        MOVE_GIGA_DRAIN, MOVE_BUG_BUZZ, MOVE_QUIVER_DANCE, MOVE_ENERGY_BALL,
        MOVE_AROMATHERAPY,
    };
    const struct LevelUpMove *learnsets[] = {
        GetSpeciesLevelUpLearnset(SPECIES_FOLIARVA),
        GetSpeciesLevelUpLearnset(SPECIES_CRISALVIA),
        GetSpeciesLevelUpLearnset(SPECIES_INFIORALA),
    };
    const u8 *levels[] = { foliarvaLevels, crisalviaLevels, infioralaLevels };
    const u16 *moves[] = { foliarvaMoves, crisalviaMoves, infioralaMoves };
    const u32 counts[] = {
        ARRAY_COUNT(foliarvaMoves), ARRAY_COUNT(crisalviaMoves), ARRAY_COUNT(infioralaMoves),
    };

    for (u32 i = 0; i < ARRAY_COUNT(learnsets); i++)
    {
        for (u32 j = 0; j < counts[i]; j++)
        {
            EXPECT_EQ(learnsets[i][j].level, levels[i][j]);
            EXPECT_EQ(learnsets[i][j].move, moves[i][j]);
            if (j > 0)
                EXPECT_GE(learnsets[i][j].level, learnsets[i][j - 1].level);
        }
        EXPECT_EQ(learnsets[i][counts[i]].move, LEVEL_UP_MOVE_END);
    }
}

TEST("Ausonia early Bug fauna teachables Egg Moves and placeholders are complete")
{
    static const enum Species species[] = { SPECIES_FOLIARVA, SPECIES_CRISALVIA, SPECIES_INFIORALA };
    static const enum Species placeholders[] = { SPECIES_CATERPIE, SPECIES_METAPOD, SPECIES_BUTTERFREE };
    static const u16 foliarvaTeachables[] = {
        MOVE_PROTECT, MOVE_REST, MOVE_SLEEP_TALK, MOVE_SUBSTITUTE,
        MOVE_SUNNY_DAY, MOVE_STRUGGLE_BUG, MOVE_GIGA_DRAIN, MOVE_ENERGY_BALL,
    };
    static const u16 crisalviaExtraTeachables[] = { MOVE_SOLAR_BEAM, MOVE_GRASS_KNOT };
    static const u16 infioralaExtraTeachables[] = {
        MOVE_U_TURN, MOVE_ACROBATICS, MOVE_AIR_SLASH, MOVE_POLLEN_PUFF,
    };
    static const u16 eggMoves[] = {
        MOVE_RAGE_POWDER, MOVE_WORRY_SEED, MOVE_BATON_PASS, MOVE_GRASSY_TERRAIN,
    };
    const u16 *foliarva = gSpeciesInfo[SPECIES_FOLIARVA].teachableLearnset;
    const u16 *crisalvia = gSpeciesInfo[SPECIES_CRISALVIA].teachableLearnset;
    const u16 *infiorala = gSpeciesInfo[SPECIES_INFIORALA].teachableLearnset;

    for (u32 i = 0; i < ARRAY_COUNT(foliarvaTeachables); i++)
    {
        EXPECT(MoveListContains(foliarva, foliarvaTeachables[i]));
        EXPECT(MoveListContains(crisalvia, foliarvaTeachables[i]));
        EXPECT(MoveListContains(infiorala, foliarvaTeachables[i]));
    }
    for (u32 i = 0; i < ARRAY_COUNT(crisalviaExtraTeachables); i++)
    {
        EXPECT(MoveListContains(crisalvia, crisalviaExtraTeachables[i]));
        EXPECT(MoveListContains(infiorala, crisalviaExtraTeachables[i]));
    }
    for (u32 i = 0; i < ARRAY_COUNT(infioralaExtraTeachables); i++)
        EXPECT(MoveListContains(infiorala, infioralaExtraTeachables[i]));
    for (u32 i = 0; i < ARRAY_COUNT(eggMoves); i++)
        EXPECT(MoveListContains(gSpeciesInfo[SPECIES_FOLIARVA].eggMoveLearnset, eggMoves[i]));

    EXPECT_EQ(MoveListCount(foliarva), ARRAY_COUNT(foliarvaTeachables));
    EXPECT_EQ(MoveListCount(crisalvia), 10);
    EXPECT_EQ(MoveListCount(infiorala), 14);
    EXPECT_EQ(MoveListCount(gSpeciesInfo[SPECIES_FOLIARVA].eggMoveLearnset), ARRAY_COUNT(eggMoves));

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species[i]];
        const struct SpeciesInfo *placeholder = &gSpeciesInfo[placeholders[i]];

        EXPECT(info->frontPic != placeholder->frontPic);
        EXPECT(info->backPic != placeholder->backPic);
        EXPECT(info->palette != placeholder->palette);
        EXPECT(info->shinyPalette != placeholder->shinyPalette);
        EXPECT(info->iconSprite != placeholder->iconSprite);
        EXPECT(info->frontAnimFrames != NULL);
        EXPECT_EQ((u32)info->cryId, (u32)placeholder->cryId);
        EXPECT_EQ((u32)info->iconPalIndex, 1);
#if P_FOOTPRINTS
        EXPECT(info->footprint == placeholder->footprint);
#endif
#if OW_POKEMON_OBJECT_EVENTS
        EXPECT(info->overworldData.images == placeholder->overworldData.images);
#endif
    }
}

TEST("Ausonia early falcon fauna base data matches the canonical batch")
{
    static const enum Species species[] = { SPECIES_GHEPIO, SPECIES_TINUNCOL, SPECIES_PEREGRINUS };
    static const u8 stats[][NUM_STATS] = {
        { 40, 45, 35, 70, 30, 35 },
        { 55, 65, 50, 90, 40, 50 },
        { 75, 110, 70, 120, 55, 70 },
    };
    static const u16 statTotals[] = { 255, 350, 500 };
    static const u8 catchRates[] = { 255, 120, 45 };
    static const u16 expYields[] = { 56, 113, 177 };
    static const u16 heights[] = { 3, 6, 11 };
    static const u16 weights[] = { 21, 72, 234 };
    const u8 ghepioSpeedYield = gSpeciesInfo[SPECIES_GHEPIO].evYield_Speed;
    const u8 tinuncolSpeedYield = gSpeciesInfo[SPECIES_TINUNCOL].evYield_Speed;
    const u8 peregrinusSpeedYield = gSpeciesInfo[SPECIES_PEREGRINUS].evYield_Speed;

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species[i]];

        EXPECT_EQ(info->baseHP, stats[i][STAT_HP]);
        EXPECT_EQ(info->baseAttack, stats[i][STAT_ATK]);
        EXPECT_EQ(info->baseDefense, stats[i][STAT_DEF]);
        EXPECT_EQ(info->baseSpeed, stats[i][STAT_SPEED]);
        EXPECT_EQ(info->baseSpAttack, stats[i][STAT_SPATK]);
        EXPECT_EQ(info->baseSpDefense, stats[i][STAT_SPDEF]);
        EXPECT_EQ(GetBaseStatTotalForTest(species[i]), statTotals[i]);
        EXPECT_EQ(info->catchRate, catchRates[i]);
        EXPECT_EQ(info->expYield, expYields[i]);
        EXPECT_EQ(info->height, heights[i]);
        EXPECT_EQ(info->weight, weights[i]);
        EXPECT_EQ(info->genderRatio, (50 * 255) / 100);
        EXPECT_EQ(info->eggCycles, 15);
        EXPECT_EQ(info->friendship, 70);
        EXPECT_EQ(info->growthRate, GROWTH_MEDIUM_FAST);
        EXPECT_EQ(info->eggGroups[0], EGG_GROUP_FLYING);
        EXPECT_EQ(info->eggGroups[1], EGG_GROUP_FLYING);
        EXPECT_NE(StringCompare(info->description, gFallbackPokedexText), 0);
        EXPECT_EQ((u32)info->teachingType, EXPLICIT_TEACHABLES);
    }

    EXPECT_EQ(gSpeciesInfo[SPECIES_GHEPIO].types[0], TYPE_FLYING);
    EXPECT_EQ(gSpeciesInfo[SPECIES_GHEPIO].types[1], TYPE_FLYING);
    EXPECT_EQ(gSpeciesInfo[SPECIES_TINUNCOL].types[0], TYPE_FLYING);
    EXPECT_EQ(gSpeciesInfo[SPECIES_TINUNCOL].types[1], TYPE_FLYING);
    EXPECT_EQ(gSpeciesInfo[SPECIES_PEREGRINUS].types[0], TYPE_FLYING);
    EXPECT_EQ(gSpeciesInfo[SPECIES_PEREGRINUS].types[1], TYPE_FIGHTING);
    EXPECT_EQ(gSpeciesInfo[SPECIES_GHEPIO].abilities[0], ABILITY_KEEN_EYE);
    EXPECT_EQ(gSpeciesInfo[SPECIES_GHEPIO].abilities[1], ABILITY_BIG_PECKS);
    EXPECT_EQ(gSpeciesInfo[SPECIES_GHEPIO].abilities[2], ABILITY_RECKLESS);
    EXPECT_EQ(gSpeciesInfo[SPECIES_TINUNCOL].abilities[0], ABILITY_KEEN_EYE);
    EXPECT_EQ(gSpeciesInfo[SPECIES_TINUNCOL].abilities[1], ABILITY_BIG_PECKS);
    EXPECT_EQ(gSpeciesInfo[SPECIES_TINUNCOL].abilities[2], ABILITY_RECKLESS);
    EXPECT_EQ(gSpeciesInfo[SPECIES_PEREGRINUS].abilities[0], ABILITY_KEEN_EYE);
    EXPECT_EQ(gSpeciesInfo[SPECIES_PEREGRINUS].abilities[1], ABILITY_DEFIANT);
    EXPECT_EQ(gSpeciesInfo[SPECIES_PEREGRINUS].abilities[2], ABILITY_RECKLESS);
    EXPECT_EQ(ghepioSpeedYield, 1);
    EXPECT_EQ(tinuncolSpeedYield, 2);
    EXPECT_EQ(peregrinusSpeedYield, 2);
    EXPECT_EQ((u32)gSpeciesInfo[SPECIES_GHEPIO].bodyColor, BODY_COLOR_BROWN);
    EXPECT_EQ((u32)gSpeciesInfo[SPECIES_TINUNCOL].bodyColor, BODY_COLOR_BROWN);
    EXPECT_EQ((u32)gSpeciesInfo[SPECIES_PEREGRINUS].bodyColor, BODY_COLOR_GRAY);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_GHEPIO].categoryName, COMPOUND_STRING("FALCHETTO")), 0);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_TINUNCOL].categoryName, COMPOUND_STRING("GHEPPIO")), 0);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_PEREGRINUS].categoryName, COMPOUND_STRING("PICCHIATA")), 0);
}

TEST("Ausonia early falcon fauna evolutions use the canonical levels")
{
    const struct Evolution *ghepioEvolutions = GetSpeciesEvolutions(SPECIES_GHEPIO);
    const struct Evolution *tinuncolEvolutions = GetSpeciesEvolutions(SPECIES_TINUNCOL);

    EXPECT_EQ(ghepioEvolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(ghepioEvolutions[0].param, 16);
    EXPECT_EQ(ghepioEvolutions[0].targetSpecies, SPECIES_TINUNCOL);
    EXPECT_EQ(ghepioEvolutions[1].method, EVOLUTIONS_END);
    EXPECT_EQ(tinuncolEvolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(tinuncolEvolutions[0].param, 34);
    EXPECT_EQ(tinuncolEvolutions[0].targetSpecies, SPECIES_PEREGRINUS);
    EXPECT_EQ(tinuncolEvolutions[1].method, EVOLUTIONS_END);
    EXPECT(GetSpeciesEvolutions(SPECIES_PEREGRINUS) == NULL);
}

TEST("Ausonia early falcon fauna level-up learnsets are canonical and ordered")
{
    static const u8 ghepioLevels[] = { 1, 1, 4, 7, 10, 13, 16, 20, 24 };
    static const u16 ghepioMoves[] = {
        MOVE_PECK, MOVE_GROWL, MOVE_QUICK_ATTACK, MOVE_LEER, MOVE_WING_ATTACK,
        MOVE_FOCUS_ENERGY, MOVE_AERIAL_ACE, MOVE_AGILITY, MOVE_TAILWIND,
    };
    static const u8 tinuncolLevels[] = { 1, 1, 1, 1, 10, 13, 16, 20, 24, 28, 32 };
    static const u16 tinuncolMoves[] = {
        MOVE_PECK, MOVE_GROWL, MOVE_QUICK_ATTACK, MOVE_LEER, MOVE_WING_ATTACK,
        MOVE_FOCUS_ENERGY, MOVE_AERIAL_ACE, MOVE_AGILITY, MOVE_TAILWIND,
        MOVE_DETECT, MOVE_ACROBATICS,
    };
    static const u8 peregrinusLevels[] = { 1, 1, 1, 1, 10, 13, 16, 20, 24, 28, 32, 34, 38, 42, 46, 50 };
    static const u16 peregrinusMoves[] = {
        MOVE_PECK, MOVE_GROWL, MOVE_QUICK_ATTACK, MOVE_LEER, MOVE_WING_ATTACK,
        MOVE_FOCUS_ENERGY, MOVE_AERIAL_ACE, MOVE_AGILITY, MOVE_TAILWIND,
        MOVE_DETECT, MOVE_ACROBATICS, MOVE_CLOSE_COMBAT, MOVE_ROOST,
        MOVE_DUAL_WINGBEAT, MOVE_BRAVE_BIRD, MOVE_QUICK_GUARD,
    };
    const struct LevelUpMove *learnsets[] = {
        GetSpeciesLevelUpLearnset(SPECIES_GHEPIO),
        GetSpeciesLevelUpLearnset(SPECIES_TINUNCOL),
        GetSpeciesLevelUpLearnset(SPECIES_PEREGRINUS),
    };
    const u8 *levels[] = { ghepioLevels, tinuncolLevels, peregrinusLevels };
    const u16 *moves[] = { ghepioMoves, tinuncolMoves, peregrinusMoves };
    const u32 counts[] = {
        ARRAY_COUNT(ghepioMoves), ARRAY_COUNT(tinuncolMoves), ARRAY_COUNT(peregrinusMoves),
    };

    for (u32 i = 0; i < ARRAY_COUNT(learnsets); i++)
    {
        for (u32 j = 0; j < counts[i]; j++)
        {
            EXPECT_EQ(learnsets[i][j].level, levels[i][j]);
            EXPECT_EQ(learnsets[i][j].move, moves[i][j]);
            if (j > 0)
                EXPECT_GE(learnsets[i][j].level, learnsets[i][j - 1].level);
        }
        EXPECT_EQ(learnsets[i][counts[i]].move, LEVEL_UP_MOVE_END);
    }
}

TEST("Ausonia early falcon fauna teachables Egg Moves graphics and auxiliary placeholders are complete")
{
    static const enum Species species[] = { SPECIES_GHEPIO, SPECIES_TINUNCOL, SPECIES_PEREGRINUS };
    static const enum Species placeholders[] = { SPECIES_FLETCHLING, SPECIES_FLETCHINDER, SPECIES_TALONFLAME };
    static const u8 frontPicSizes[] = {
        (8 << 4) | 7,
        (8 << 4) | 8,
        (8 << 4) | 5,
    };
    static const u8 backPicSizes[] = {
        (6 << 4) | 7,
        (7 << 4) | 8,
        (7 << 4) | 7,
    };
    static const u8 frontOffsets[] = { 4, 2, 12 };
    static const u8 backOffsets[] = { 4, 2, 4 };
    static const u16 ghepioTeachables[] = {
        MOVE_PROTECT, MOVE_REST, MOVE_SLEEP_TALK, MOVE_SUBSTITUTE, MOVE_SUNNY_DAY,
        MOVE_AERIAL_ACE, MOVE_ACROBATICS, MOVE_U_TURN, MOVE_FLY,
    };
    static const u16 tinuncolExtraTeachables[] = { MOVE_ROOST, MOVE_TAILWIND, MOVE_STEEL_WING };
    static const u16 peregrinusExtraTeachables[] = { MOVE_CLOSE_COMBAT, MOVE_BRICK_BREAK, MOVE_LOW_SWEEP, MOVE_BULK_UP };
    static const u16 eggMoves[] = { MOVE_FEINT, MOVE_QUICK_GUARD, MOVE_DEFOG, MOVE_SKY_ATTACK };
    const u16 *ghepio = gSpeciesInfo[SPECIES_GHEPIO].teachableLearnset;
    const u16 *tinuncol = gSpeciesInfo[SPECIES_TINUNCOL].teachableLearnset;
    const u16 *peregrinus = gSpeciesInfo[SPECIES_PEREGRINUS].teachableLearnset;

    for (u32 i = 0; i < ARRAY_COUNT(ghepioTeachables); i++)
    {
        EXPECT(MoveListContains(ghepio, ghepioTeachables[i]));
        EXPECT(MoveListContains(tinuncol, ghepioTeachables[i]));
        EXPECT(MoveListContains(peregrinus, ghepioTeachables[i]));
    }
    for (u32 i = 0; i < ARRAY_COUNT(tinuncolExtraTeachables); i++)
    {
        EXPECT(MoveListContains(tinuncol, tinuncolExtraTeachables[i]));
        EXPECT(MoveListContains(peregrinus, tinuncolExtraTeachables[i]));
    }
    for (u32 i = 0; i < ARRAY_COUNT(peregrinusExtraTeachables); i++)
        EXPECT(MoveListContains(peregrinus, peregrinusExtraTeachables[i]));
    for (u32 i = 0; i < ARRAY_COUNT(eggMoves); i++)
        EXPECT(MoveListContains(gSpeciesInfo[SPECIES_GHEPIO].eggMoveLearnset, eggMoves[i]));

    EXPECT_EQ(MoveListCount(ghepio), ARRAY_COUNT(ghepioTeachables));
    EXPECT_EQ(MoveListCount(tinuncol), 12);
    EXPECT_EQ(MoveListCount(peregrinus), 16);
    EXPECT_EQ(MoveListCount(gSpeciesInfo[SPECIES_GHEPIO].eggMoveLearnset), ARRAY_COUNT(eggMoves));

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species[i]];
        const struct SpeciesInfo *placeholder = &gSpeciesInfo[placeholders[i]];

        EXPECT(info->frontPic != placeholder->frontPic);
        EXPECT(info->backPic != placeholder->backPic);
        EXPECT(info->palette != placeholder->palette);
        EXPECT(info->shinyPalette != placeholder->shinyPalette);
        EXPECT(info->iconSprite != placeholder->iconSprite);
        EXPECT(info->frontAnimFrames != placeholder->frontAnimFrames);
        EXPECT_EQ((u32)info->frontPicSize, (u32)frontPicSizes[i]);
        EXPECT_EQ((u32)info->backPicSize, (u32)backPicSizes[i]);
        EXPECT_EQ((u32)info->frontPicYOffset, (u32)frontOffsets[i]);
        EXPECT_EQ((u32)info->backPicYOffset, (u32)backOffsets[i]);
        EXPECT_EQ((u32)info->iconPalIndex, 3);

        // Audio, footprint, overworld, shadow and auxiliary animations stay provisional.
        EXPECT_EQ((u32)info->cryId, (u32)placeholder->cryId);
        EXPECT_EQ(info->frontAnimId, placeholder->frontAnimId);
        EXPECT_EQ(info->backAnimId, placeholder->backAnimId);
        EXPECT_EQ(info->enemyMonElevation, placeholder->enemyMonElevation);
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
}

TEST("Ausonia early magpie fauna graphics retain only approved auxiliary placeholders")
{
    static const enum Species species[] = { SPECIES_GAZZUOLA, SPECIES_BRILLAZZA, SPECIES_GAZZOMBRA };
    static const enum Species placeholders[] = { SPECIES_ROOKIDEE, SPECIES_CORVISQUIRE, SPECIES_CORVIKNIGHT };
    static const u8 frontPicSizes[] = { (5 << 4) | 5, (5 << 4) | 5, (6 << 4) | 7 };
    static const u8 backPicSizes[] = { (4 << 4) | 5, (4 << 4) | 5, (6 << 4) | 7 };
    static const u8 picOffsets[] = { 12, 12, 4 };

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species[i]];
        const struct SpeciesInfo *placeholder = &gSpeciesInfo[placeholders[i]];

        EXPECT(info->frontPic != placeholder->frontPic);
        EXPECT(info->backPic != placeholder->backPic);
        EXPECT(info->palette != placeholder->palette);
        EXPECT(info->shinyPalette != placeholder->shinyPalette);
        EXPECT(info->iconSprite != placeholder->iconSprite);
        EXPECT_EQ((u32)info->frontPicSize, (u32)frontPicSizes[i]);
        EXPECT_EQ((u32)info->backPicSize, (u32)backPicSizes[i]);
        EXPECT_EQ((u32)info->frontPicYOffset, (u32)picOffsets[i]);
        EXPECT_EQ((u32)info->backPicYOffset, (u32)picOffsets[i]);
        EXPECT_EQ((u32)info->iconPalIndex, 6);

        EXPECT_EQ((u32)info->cryId, (u32)placeholder->cryId);
        EXPECT(info->frontAnimFrames != placeholder->frontAnimFrames);
        EXPECT_EQ(info->frontAnimId, placeholder->frontAnimId);
        EXPECT_EQ(info->backAnimId, placeholder->backAnimId);
        EXPECT_EQ(info->enemyMonElevation, placeholder->enemyMonElevation);
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
}

TEST("Ausonia early magpie fauna base data matches the canonical batch")
{
    static const enum Species species[] = { SPECIES_GAZZUOLA, SPECIES_BRILLAZZA, SPECIES_GAZZOMBRA };
    static const u8 stats[][NUM_STATS] = {
        { 45, 40, 40, 60, 35, 40 },
        { 60, 65, 55, 80, 45, 55 },
        { 75, 95, 70, 100, 55, 75 },
    };
    static const u16 statTotals[] = { 260, 360, 470 };
    static const u8 catchRates[] = { 255, 120, 60 };
    static const u16 expYields[] = { 56, 116, 170 };
    static const u16 heights[] = { 3, 6, 9 };
    static const u16 weights[] = { 19, 55, 98 };
    const u8 gazzuolaSpeedYield = gSpeciesInfo[SPECIES_GAZZUOLA].evYield_Speed;
    const u8 brillazzaAttackYield = gSpeciesInfo[SPECIES_BRILLAZZA].evYield_Attack;
    const u8 brillazzaSpeedYield = gSpeciesInfo[SPECIES_BRILLAZZA].evYield_Speed;
    const u8 gazzombraAttackYield = gSpeciesInfo[SPECIES_GAZZOMBRA].evYield_Attack;

    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        const struct SpeciesInfo *info = &gSpeciesInfo[species[i]];

        EXPECT_EQ(info->baseHP, stats[i][STAT_HP]);
        EXPECT_EQ(info->baseAttack, stats[i][STAT_ATK]);
        EXPECT_EQ(info->baseDefense, stats[i][STAT_DEF]);
        EXPECT_EQ(info->baseSpeed, stats[i][STAT_SPEED]);
        EXPECT_EQ(info->baseSpAttack, stats[i][STAT_SPATK]);
        EXPECT_EQ(info->baseSpDefense, stats[i][STAT_SPDEF]);
        EXPECT_EQ(GetBaseStatTotalForTest(species[i]), statTotals[i]);
        EXPECT_EQ(info->catchRate, catchRates[i]);
        EXPECT_EQ(info->expYield, expYields[i]);
        EXPECT_EQ(info->height, heights[i]);
        EXPECT_EQ(info->weight, weights[i]);
        EXPECT_EQ(info->genderRatio, (50 * 255) / 100);
        EXPECT_EQ(info->eggCycles, 15);
        EXPECT_EQ(info->friendship, 70);
        EXPECT_EQ(info->growthRate, GROWTH_MEDIUM_FAST);
        EXPECT_EQ(info->eggGroups[0], EGG_GROUP_FLYING);
        EXPECT_EQ(info->eggGroups[1], EGG_GROUP_FLYING);
        EXPECT_EQ((u32)info->bodyColor, BODY_COLOR_BLACK);
        EXPECT_NE(StringCompare(info->description, gFallbackPokedexText), 0);
        EXPECT_EQ((u32)info->teachingType, EXPLICIT_TEACHABLES);
    }

    EXPECT_EQ(gSpeciesInfo[SPECIES_GAZZUOLA].types[0], TYPE_NORMAL);
    EXPECT_EQ(gSpeciesInfo[SPECIES_GAZZUOLA].types[1], TYPE_FLYING);
    EXPECT_EQ(gSpeciesInfo[SPECIES_BRILLAZZA].types[0], TYPE_DARK);
    EXPECT_EQ(gSpeciesInfo[SPECIES_BRILLAZZA].types[1], TYPE_FLYING);
    EXPECT_EQ(gSpeciesInfo[SPECIES_GAZZOMBRA].types[0], TYPE_DARK);
    EXPECT_EQ(gSpeciesInfo[SPECIES_GAZZOMBRA].types[1], TYPE_FLYING);
    EXPECT_EQ(gSpeciesInfo[SPECIES_GAZZUOLA].abilities[0], ABILITY_PICKUP);
    EXPECT_EQ(gSpeciesInfo[SPECIES_GAZZUOLA].abilities[1], ABILITY_KEEN_EYE);
    EXPECT_EQ(gSpeciesInfo[SPECIES_GAZZUOLA].abilities[2], ABILITY_SUPER_LUCK);
    EXPECT_EQ(gSpeciesInfo[SPECIES_BRILLAZZA].abilities[0], ABILITY_PICKUP);
    EXPECT_EQ(gSpeciesInfo[SPECIES_BRILLAZZA].abilities[1], ABILITY_FRISK);
    EXPECT_EQ(gSpeciesInfo[SPECIES_BRILLAZZA].abilities[2], ABILITY_SUPER_LUCK);
    EXPECT_EQ(gSpeciesInfo[SPECIES_GAZZOMBRA].abilities[0], ABILITY_FRISK);
    EXPECT_EQ(gSpeciesInfo[SPECIES_GAZZOMBRA].abilities[1], ABILITY_PICKPOCKET);
    EXPECT_EQ(gSpeciesInfo[SPECIES_GAZZOMBRA].abilities[2], ABILITY_SUPER_LUCK);
    EXPECT_EQ(gazzuolaSpeedYield, 1);
    EXPECT_EQ(brillazzaAttackYield, 1);
    EXPECT_EQ(brillazzaSpeedYield, 1);
    EXPECT_EQ(gazzombraAttackYield, 2);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_GAZZUOLA].categoryName, COMPOUND_STRING("CURIOSA")), 0);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_BRILLAZZA].categoryName, COMPOUND_STRING("MONILE")), 0);
    EXPECT_EQ(StringCompare(gSpeciesInfo[SPECIES_GAZZOMBRA].categoryName, COMPOUND_STRING("BOTTINO")), 0);
}

TEST("Ausonia early magpie fauna evolutions use the canonical levels")
{
    const struct Evolution *gazzuolaEvolutions = GetSpeciesEvolutions(SPECIES_GAZZUOLA);
    const struct Evolution *brillazzaEvolutions = GetSpeciesEvolutions(SPECIES_BRILLAZZA);

    EXPECT_EQ(gazzuolaEvolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(gazzuolaEvolutions[0].param, 18);
    EXPECT_EQ(gazzuolaEvolutions[0].targetSpecies, SPECIES_BRILLAZZA);
    EXPECT_EQ(gazzuolaEvolutions[1].method, EVOLUTIONS_END);
    EXPECT_EQ(brillazzaEvolutions[0].method, EVO_LEVEL);
    EXPECT_EQ(brillazzaEvolutions[0].param, 34);
    EXPECT_EQ(brillazzaEvolutions[0].targetSpecies, SPECIES_GAZZOMBRA);
    EXPECT_EQ(brillazzaEvolutions[1].method, EVOLUTIONS_END);
    EXPECT(GetSpeciesEvolutions(SPECIES_GAZZOMBRA) == NULL);
}

TEST("Ausonia early magpie fauna level-up learnsets are canonical and ordered")
{
    static const u8 gazzuolaLevels[] = { 1, 1, 4, 7, 10, 13, 16, 18, 22, 26 };
    static const u16 gazzuolaMoves[] = { MOVE_PECK, MOVE_GROWL, MOVE_COVET, MOVE_QUICK_ATTACK, MOVE_PLUCK, MOVE_THIEF, MOVE_ASSURANCE, MOVE_AERIAL_ACE, MOVE_TAUNT, MOVE_AGILITY };
    static const u8 brillazzaLevels[] = { 1, 1, 1, 1, 10, 13, 16, 18, 22, 26, 30, 34 };
    static const u16 brillazzaMoves[] = { MOVE_PECK, MOVE_GROWL, MOVE_COVET, MOVE_QUICK_ATTACK, MOVE_PLUCK, MOVE_THIEF, MOVE_ASSURANCE, MOVE_AERIAL_ACE, MOVE_TAUNT, MOVE_AGILITY, MOVE_KNOCK_OFF, MOVE_TAILWIND };
    static const u8 gazzombraLevels[] = { 1, 1, 1, 1, 10, 13, 16, 18, 22, 26, 30, 34, 38, 42, 46, 50 };
    static const u16 gazzombraMoves[] = { MOVE_PECK, MOVE_GROWL, MOVE_COVET, MOVE_QUICK_ATTACK, MOVE_PLUCK, MOVE_THIEF, MOVE_ASSURANCE, MOVE_AERIAL_ACE, MOVE_TAUNT, MOVE_AGILITY, MOVE_KNOCK_OFF, MOVE_FOUL_PLAY, MOVE_U_TURN, MOVE_ROOST, MOVE_BRAVE_BIRD, MOVE_SWITCHEROO };
    const struct LevelUpMove *learnsets[] = { GetSpeciesLevelUpLearnset(SPECIES_GAZZUOLA), GetSpeciesLevelUpLearnset(SPECIES_BRILLAZZA), GetSpeciesLevelUpLearnset(SPECIES_GAZZOMBRA) };
    const u8 *levels[] = { gazzuolaLevels, brillazzaLevels, gazzombraLevels };
    const u16 *moves[] = { gazzuolaMoves, brillazzaMoves, gazzombraMoves };
    const u32 counts[] = { ARRAY_COUNT(gazzuolaMoves), ARRAY_COUNT(brillazzaMoves), ARRAY_COUNT(gazzombraMoves) };

    for (u32 i = 0; i < ARRAY_COUNT(learnsets); i++)
    {
        for (u32 j = 0; j < counts[i]; j++)
        {
            EXPECT_EQ(learnsets[i][j].level, levels[i][j]);
            EXPECT_EQ(learnsets[i][j].move, moves[i][j]);
            if (j > 0)
                EXPECT_GE(learnsets[i][j].level, learnsets[i][j - 1].level);
        }
        EXPECT_EQ(learnsets[i][counts[i]].move, LEVEL_UP_MOVE_END);
    }
}

TEST("Ausonia early magpie fauna teachables Egg Moves and placeholders are complete")
{
    static const u16 gazzuolaTeachables[] = { MOVE_PROTECT, MOVE_REST, MOVE_SLEEP_TALK, MOVE_SUBSTITUTE, MOVE_AERIAL_ACE, MOVE_ACROBATICS, MOVE_U_TURN, MOVE_THIEF, MOVE_FLY };
    static const u16 brillazzaExtras[] = { MOVE_TAUNT, MOVE_SNARL, MOVE_DARK_PULSE, MOVE_FOUL_PLAY };
    static const u16 gazzombraExtras[] = { MOVE_KNOCK_OFF, MOVE_ROOST, MOVE_TAILWIND, MOVE_STEEL_WING };
    static const u16 eggMoves[] = { MOVE_FEATHER_DANCE, MOVE_DEFOG, MOVE_ROOST, MOVE_SWITCHEROO };
    const u16 *gazzuola = gSpeciesInfo[SPECIES_GAZZUOLA].teachableLearnset;
    const u16 *brillazza = gSpeciesInfo[SPECIES_BRILLAZZA].teachableLearnset;
    const u16 *gazzombra = gSpeciesInfo[SPECIES_GAZZOMBRA].teachableLearnset;

    for (u32 i = 0; i < ARRAY_COUNT(gazzuolaTeachables); i++)
    {
        EXPECT(MoveListContains(gazzuola, gazzuolaTeachables[i]));
        EXPECT(MoveListContains(brillazza, gazzuolaTeachables[i]));
        EXPECT(MoveListContains(gazzombra, gazzuolaTeachables[i]));
    }
    for (u32 i = 0; i < ARRAY_COUNT(brillazzaExtras); i++)
    {
        EXPECT(MoveListContains(brillazza, brillazzaExtras[i]));
        EXPECT(MoveListContains(gazzombra, brillazzaExtras[i]));
    }
    for (u32 i = 0; i < ARRAY_COUNT(gazzombraExtras); i++)
        EXPECT(MoveListContains(gazzombra, gazzombraExtras[i]));
    for (u32 i = 0; i < ARRAY_COUNT(eggMoves); i++)
        EXPECT(MoveListContains(gSpeciesInfo[SPECIES_GAZZUOLA].eggMoveLearnset, eggMoves[i]));

    EXPECT_EQ(MoveListCount(gazzuola), ARRAY_COUNT(gazzuolaTeachables));
    EXPECT_EQ(MoveListCount(brillazza), 13);
    EXPECT_EQ(MoveListCount(gazzombra), 17);
    EXPECT_EQ(MoveListCount(gSpeciesInfo[SPECIES_GAZZUOLA].eggMoveLearnset), ARRAY_COUNT(eggMoves));
    EXPECT(gSpeciesInfo[SPECIES_GAZZUOLA].frontPic != gSpeciesInfo[SPECIES_ROOKIDEE].frontPic);
    EXPECT(gSpeciesInfo[SPECIES_BRILLAZZA].frontPic != gSpeciesInfo[SPECIES_CORVISQUIRE].frontPic);
    EXPECT(gSpeciesInfo[SPECIES_GAZZOMBRA].frontPic != gSpeciesInfo[SPECIES_CORVIKNIGHT].frontPic);
}

TEST("Molospsy canonical data and graphics are valid")
{
    static const u8 expectedLevels[] = { 1, 1, 4, 7, 10, 13, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52 };
    static const u16 expectedMoves[] = {
        MOVE_TACKLE,
        MOVE_LEER,
        MOVE_MEDITATE,
        MOVE_LOW_KICK,
        MOVE_PROTECT,
        MOVE_CONFUSION,
        MOVE_DETECT,
        MOVE_HELPING_HAND,
        MOVE_PSYBEAM,
        MOVE_BULK_UP,
        MOVE_SAFEGUARD,
        MOVE_FORCE_PALM,
        MOVE_ZEN_HEADBUTT,
        MOVE_IRON_DEFENSE,
        MOVE_CALM_MIND,
        MOVE_WIDE_GUARD,
    };
    static const u16 expectedTeachables[] = {
        MOVE_PROTECT,
        MOVE_REST,
        MOVE_SLEEP_TALK,
        MOVE_SUBSTITUTE,
        MOVE_ENDURE,
        MOVE_MEDITATE,
        MOVE_SWIFT,
        MOVE_LOW_KICK,
        MOVE_ROCK_SMASH,
        MOVE_HELPING_HAND,
        MOVE_DETECT,
        MOVE_FORCE_PALM,
        MOVE_BULK_UP,
        MOVE_CALM_MIND,
        MOVE_SAFEGUARD,
        MOVE_IRON_DEFENSE,
        MOVE_ZEN_HEADBUTT,
        MOVE_POWER_UP_PUNCH,
        MOVE_QUICK_GUARD,
        MOVE_WIDE_GUARD,
        MOVE_PSYCH_UP,
        MOVE_MIRROR_COAT,
    };
    static const u16 expectedEggMoves[] = {
        MOVE_COUNTER,
        MOVE_DETECT,
        MOVE_ENDURE,
        MOVE_HELPING_HAND,
        MOVE_MIRROR_COAT,
        MOVE_POWER_UP_PUNCH,
        MOVE_QUICK_GUARD,
        MOVE_WIDE_GUARD,
    };
    const struct SpeciesInfo *info = &gSpeciesInfo[SPECIES_MOLOSPSY];
    const struct SpeciesInfo *placeholder = &gSpeciesInfo[SPECIES_MABOSSTIFF];
    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(SPECIES_MOLOSPSY);
    const u16 *teachable = info->teachableLearnset;
    const u16 *eggMoves = info->eggMoveLearnset;

    EXPECT_EQ(info->baseHP, 65);
    EXPECT_EQ(info->baseAttack, 85);
    EXPECT_EQ(info->baseDefense, 80);
    EXPECT_EQ(info->baseSpeed, 45);
    EXPECT_EQ(info->baseSpAttack, 60);
    EXPECT_EQ(info->baseSpDefense, 70);
    EXPECT_EQ(info->types[0], TYPE_FIGHTING);
    EXPECT_EQ(info->types[1], TYPE_PSYCHIC);
    EXPECT_EQ(info->catchRate, 90);
    EXPECT_EQ(info->expYield, 145);
    EXPECT_EQ((u32)info->evYield_Attack, 1);
    EXPECT_EQ((u32)info->genderRatio, 127);
    EXPECT_EQ(info->eggCycles, 20);
    EXPECT_EQ(info->friendship, 50);
    EXPECT_EQ(info->growthRate, GROWTH_MEDIUM_FAST);
    EXPECT_EQ(info->eggGroups[0], EGG_GROUP_FIELD);
    EXPECT_EQ(info->eggGroups[1], EGG_GROUP_FIELD);
    EXPECT_EQ((u32)info->abilities[0], ABILITY_INNER_FOCUS);
    EXPECT_EQ((u32)info->abilities[1], ABILITY_STEADFAST);
    EXPECT_EQ((u32)info->abilities[2], ABILITY_GUARD_DOG);
    EXPECT_EQ((u32)info->bodyColor, BODY_COLOR_GRAY);
    EXPECT_EQ((u32)info->natDexNum, NATIONAL_DEX_MOLOSPSY);
    EXPECT_EQ(StringCompare(info->speciesName, COMPOUND_STRING("Molospsy")), 0);
    EXPECT_EQ(StringCompare(info->categoryName, COMPOUND_STRING("GUARDIANO")), 0);
    EXPECT_EQ(info->height, 12);
    EXPECT_EQ(info->weight, 610);
    EXPECT_NE(StringCompare(info->description, gFallbackPokedexText), 0);
    EXPECT_EQ(info->frontPic, gMonFrontPic_Molospsy);
    EXPECT_EQ(info->backPic, gMonBackPic_Molospsy);
    EXPECT_EQ(info->palette, gMonPalette_Molospsy);
    EXPECT_EQ(info->shinyPalette, gMonShinyPalette_Molospsy);
    EXPECT_EQ(info->iconSprite, gMonIcon_Molospsy);
    EXPECT_EQ(info->frontPicSize, placeholder->frontPicSize);
    EXPECT_EQ(info->backPicSize, placeholder->backPicSize);
    EXPECT_EQ(info->frontPicYOffset, placeholder->frontPicYOffset);
    EXPECT_EQ(info->backPicYOffset, placeholder->backPicYOffset);
    EXPECT_EQ(info->frontAnimFrames, placeholder->frontAnimFrames);
    EXPECT_EQ(info->frontAnimId, placeholder->frontAnimId);
    EXPECT_EQ(info->backAnimId, placeholder->backAnimId);
    EXPECT_EQ((u32)info->iconPalIndex, (u32)placeholder->iconPalIndex);
    EXPECT_EQ((u32)info->pokemonJumpType, (u32)placeholder->pokemonJumpType);
    EXPECT_EQ(info->pokemonScale, placeholder->pokemonScale);
    EXPECT_EQ(info->pokemonOffset, placeholder->pokemonOffset);
    EXPECT_EQ(info->trainerScale, placeholder->trainerScale);
    EXPECT_EQ(info->trainerOffset, placeholder->trainerOffset);
    EXPECT_EQ((u32)info->cryId, (u32)placeholder->cryId);
#if P_FOOTPRINTS
    EXPECT(info->footprint == placeholder->footprint);
#endif
    EXPECT_EQ(info->enemyShadowXOffset, placeholder->enemyShadowXOffset);
    EXPECT_EQ(info->enemyShadowYOffset, placeholder->enemyShadowYOffset);
    EXPECT_EQ((u32)info->enemyShadowSize, (u32)placeholder->enemyShadowSize);
#if OW_POKEMON_OBJECT_EVENTS
    EXPECT(info->overworldData.images == placeholder->overworldData.images);
#if OW_PKMN_OBJECTS_SHARE_PALETTES == FALSE
    EXPECT(info->overworldPalette == placeholder->overworldPalette);
    EXPECT(info->overworldShinyPalette == placeholder->overworldShinyPalette);
#endif
#endif
    EXPECT(GetSpeciesEvolutions(SPECIES_MOLOSPSY) == NULL);

    for (u32 i = 0; i < ARRAY_COUNT(expectedMoves); i++)
    {
        EXPECT_EQ(learnset[i].level, expectedLevels[i]);
        EXPECT_EQ(learnset[i].move, expectedMoves[i]);
        if (i != 0)
            EXPECT_GE(learnset[i].level, learnset[i - 1].level);
    }
    EXPECT_EQ(learnset[ARRAY_COUNT(expectedMoves)].move, LEVEL_UP_MOVE_END);

    for (u32 i = 0; i < ARRAY_COUNT(expectedTeachables); i++)
        EXPECT_EQ(teachable[i], expectedTeachables[i]);
    EXPECT_EQ(teachable[ARRAY_COUNT(expectedTeachables)], MOVE_UNAVAILABLE);

    for (u32 i = 0; i < ARRAY_COUNT(expectedEggMoves); i++)
        EXPECT_EQ(eggMoves[i], expectedEggMoves[i]);
    EXPECT_EQ(eggMoves[ARRAY_COUNT(expectedEggMoves)], MOVE_UNAVAILABLE);
}
