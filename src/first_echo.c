#include "global.h"
#include "battle.h"
#include "battle_message.h"
#include "battle_scripts.h"
#include "battle_setup.h"
#include "event_data.h"
#include "first_echo.h"
#include "load_save.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "string_util.h"
#include "tv.h"
#include "constants/battle.h"
#include "constants/flags.h"
#include "constants/opponents.h"
#include "constants/species.h"
#include "constants/vars.h"

struct FirstEchoTrigger
{
    u16 trainerId;
    enum Species switchInSpecies;
    enum FirstEchoEffect effect;
};

static const struct FirstEchoTrigger sFirstEchoTriggers[] =
{
    {TRAINER_EMISSARIO_AUREA_RECRUIT, SPECIES_CISTERNIDE, FIRST_ECHO_EFFECT_ATK_SPATK},
};

static EWRAM_DATA bool8 sFirstEchoActiveMonWasOriginalStarter = FALSE;
static EWRAM_DATA u8 sFirstEchoActiveMonNickname[POKEMON_NAME_LENGTH + 1] = {0};

static u32 GetStoredStarterPersonality(void)
{
    return (u32)VarGet(VAR_ORIGINAL_STARTER_PERSONALITY_LO)
         | ((u32)VarGet(VAR_ORIGINAL_STARTER_PERSONALITY_HI) << 16);
}

static const struct FirstEchoTrigger *GetFirstEchoTrigger(enum Species species)
{
    u32 i;

    if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER)
     || gBattleTypeFlags & (BATTLE_TYPE_DOUBLE | BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED_LINK | BATTLE_TYPE_FRONTIER))
        return NULL;

    for (i = 0; i < ARRAY_COUNT(sFirstEchoTriggers); i++)
    {
        if (sFirstEchoTriggers[i].trainerId == TRAINER_BATTLE_PARAM.opponentA
         && sFirstEchoTriggers[i].switchInSpecies == species)
            return &sFirstEchoTriggers[i];
    }

    return NULL;
}

static bool32 IsOriginalStarterLine(enum Species species)
{
    u16 starter = VarGet(VAR_STARTER_MON);
    static const enum Species sStarterLines[][3] =
    {
        {SPECIES_CINGERM, SPECIES_ROVASCO, SPECIES_SELVAZANNA},
        {SPECIES_SERBRACE, SPECIES_VIPERCEN, SPECIES_TOSSIVAMPA},
        {SPECIES_ARDEINO, SPECIES_VELAIRONE, SPECIES_CODAIRONE},
    };

    return starter < ARRAY_COUNT(sStarterLines)
        && (species == sStarterLines[starter][0]
         || species == sStarterLines[starter][1]
         || species == sStarterLines[starter][2]);
}

static bool32 IsStarterCandidate(struct BoxPokemon *boxMon, u32 *personality)
{
    if (!GetBoxMonData(boxMon, MON_DATA_SANITY_HAS_SPECIES)
     || GetBoxMonData(boxMon, MON_DATA_SANITY_IS_EGG)
     || !IsOriginalStarterLine(GetBoxMonData(boxMon, MON_DATA_SPECIES))
     || GetBoxMonData(boxMon, MON_DATA_OT_ID) != GetPlayerIDAsU32()
     || GetBoxMonData(boxMon, MON_DATA_MET_LEVEL) != 5)
        return FALSE;

    *personality = GetBoxMonData(boxMon, MON_DATA_PERSONALITY);
    return TRUE;
}

static void StoreStarterPersonality(u32 personality)
{
    VarSet(VAR_ORIGINAL_STARTER_PERSONALITY_LO, personality & 0xFFFF);
    VarSet(VAR_ORIGINAL_STARTER_PERSONALITY_HI, personality >> 16);
    FlagSet(FLAG_ORIGINAL_STARTER_ID_REGISTERED);
}

static bool32 IsOriginalStarterPartyMon(u8 partyIndex)
{
    if (!FlagGet(FLAG_ORIGINAL_STARTER_ID_REGISTERED) || partyIndex >= PARTY_SIZE)
        return FALSE;

    return GetMonData(&gPlayerParty[partyIndex], MON_DATA_SANITY_HAS_SPECIES, NULL)
        && !GetMonData(&gPlayerParty[partyIndex], MON_DATA_SANITY_IS_EGG, NULL)
        && GetMonData(&gPlayerParty[partyIndex], MON_DATA_PERSONALITY, NULL) == GetStoredStarterPersonality();
}

void FirstEcho_RegisterStarterIdentity(enum Species species)
{
    u32 personality;
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES, NULL) == species
         && GetMonData(&gPlayerParty[i], MON_DATA_LEVEL, NULL) == 5
         && GetMonData(&gPlayerParty[i], MON_DATA_OT_ID, NULL) == GetPlayerIDAsU32())
        {
            personality = GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY, NULL);
            StoreStarterPersonality(personality);
            return;
        }
    }
}

// Conservative migration for saves created before the persistent starter PID existed.
// Only an unambiguous player-owned, level-5 starter-line candidate is registered.
void TryMigrateOriginalStarterIdentity(void)
{
    u32 personality = 0;
    u8 candidates = 0;
    u8 i;
    u8 box;

    if (FlagGet(FLAG_ORIGINAL_STARTER_ID_REGISTERED))
    {
        gSpecialVar_Result = TRUE;
        return;
    }

    for (i = 0; i < PARTY_SIZE; i++)
    {
        struct BoxPokemon *boxMon = &gPlayerParty[i].box;
        if (IsStarterCandidate(boxMon, &personality))
            candidates++;
    }
    for (box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        for (i = 0; i < IN_BOX_COUNT; i++)
        {
            if (IsStarterCandidate(&gPokemonStoragePtr->boxes[box][i], &personality))
                candidates++;
        }
    }
    for (i = 0; i < ARRAY_COUNT(gSaveBlock1Ptr->daycare.mons); i++)
    {
        if (IsStarterCandidate(&gSaveBlock1Ptr->daycare.mons[i].mon, &personality))
            candidates++;
    }

    if (candidates == 1)
    {
        StoreStarterPersonality(personality);
        gSpecialVar_Result = TRUE;
    }
    else
    {
        gSpecialVar_Result = FALSE;
    }
}

bool32 FirstEcho_ShouldOfferStarterSwitch(void)
{
    enum BattlerId playerBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
    struct Pokemon *opponentParty;
    u8 opponentPartyIndex;
    u8 playerPartyIndex;

    if (gBattleStruct->firstEchoTriggered || gBattleStruct->firstEchoSwitchPromptPrepared)
        return FALSE;

    opponentPartyIndex = gBattleResources->bufferB[gBattlerFainted][1];
    if (opponentPartyIndex >= PARTY_SIZE)
        return FALSE;

    opponentParty = GetBattlerParty(gBattlerFainted);
    if (GetFirstEchoTrigger(GetMonData(&opponentParty[opponentPartyIndex], MON_DATA_SPECIES, NULL)) == NULL)
        return FALSE;

    gBattleStruct->firstEchoSwitchPromptPrepared = TRUE;
    for (playerPartyIndex = 0; playerPartyIndex < PARTY_SIZE; playerPartyIndex++)
    {
        if (playerPartyIndex != gBattlerPartyIndexes[playerBattler]
         && IsOriginalStarterPartyMon(playerPartyIndex)
         && GetMonData(&gPlayerParty[playerPartyIndex], MON_DATA_HP, NULL) != 0)
        {
            PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, playerBattler, playerPartyIndex)
            return TRUE;
        }
    }

    return FALSE;
}

bool32 FirstEcho_TryActivateOnSwitchIn(void)
{
    enum BattlerId battler;
    enum BattlerId playerBattler;
    const struct FirstEchoTrigger *trigger;

    if (gBattleStruct->firstEchoTriggered)
        return FALSE;

    for (battler = 0; battler < gBattlersCount; battler++)
    {
        if (IsOnPlayerSide(battler)
         || !gBattleStruct->battlerState[battler].switchIn)
            continue;

        trigger = GetFirstEchoTrigger(gBattleMons[battler].species);
        if (trigger == NULL)
            continue;

        playerBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        gBattleStruct->firstEchoTriggered = TRUE;
        gBattleStruct->firstEchoEffect = trigger->effect;
        gBattleScripting.battler = playerBattler;
        gBattlerAttacker = playerBattler;
        PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, playerBattler, gBattlerPartyIndexes[playerBattler])
        sFirstEchoActiveMonWasOriginalStarter = IsOriginalStarterPartyMon(gBattlerPartyIndexes[playerBattler]);
        GetMonData(&gPlayerParty[gBattlerPartyIndexes[playerBattler]], MON_DATA_NICKNAME, sFirstEchoActiveMonNickname);
        BattleScriptCall(BattleScript_FirstEchoActivates);
        return TRUE;
    }

    return FALSE;
}

void FirstEcho_ApplyBoost(void)
{
    enum BattlerId playerBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);

    switch (gBattleStruct->firstEchoEffect)
    {
    case FIRST_ECHO_EFFECT_ATK_SPATK:
        if (gBattleMons[playerBattler].statStages[STAT_ATK] < MAX_STAT_STAGE)
            gBattleMons[playerBattler].statStages[STAT_ATK]++;
        if (gBattleMons[playerBattler].statStages[STAT_SPATK] < MAX_STAT_STAGE)
            gBattleMons[playerBattler].statStages[STAT_SPATK]++;
        break;
    }
}

void Special_GetFirstEchoActiveMonResult(void)
{
    gSpecialVar_Result = sFirstEchoActiveMonWasOriginalStarter;
    StringCopy(gStringVar1, sFirstEchoActiveMonNickname);
}
