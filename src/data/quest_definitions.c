#include "global.h"
#include "quest_log_internal.h"
#include "constants/flags.h"
#include "constants/vars.h"

enum QuestLogQuestId
{
    QUEST_LOG_ID_LAURO_CALL = 1,
    QUEST_LOG_ID_ROUTE101_SOURCES,
    QUEST_LOG_ID_AMPHITHEATRE,
    QUEST_LOG_ID_CISTERNS,
    QUEST_LOG_ID_FIRST_ECHO,
    QUEST_LOG_ID_LARICIA_RECORDS,
    QUEST_LOG_ID_NICO_TURN,
    QUEST_LOG_ID_FIRST_SHELTER,
};

#define FLAG_IS_SET(flag) { QUEST_LOG_CONDITION_FLAG_SET, flag, 0, 0 }
#define VAR_IS(var, value_) { QUEST_LOG_CONDITION_VAR_EQUALS, var, value_, 0 }
#define VAR_AT_LEAST(var, value_) { QUEST_LOG_CONDITION_VAR_GREATER_EQUAL, var, value_, 0 }
#define UNTRACKED { QUEST_LOG_CONDITION_UNTRACKED, 0, 0, 0 }
#define CONDITION(name, ...) \
    static const struct QuestLogConditionClause name##Clauses[] = { __VA_ARGS__ }; \
    static const struct QuestLogCondition name = { name##Clauses, ARRAY_COUNT(name##Clauses), NULL, 0 }
CONDITION(sLauroDiscovered, FLAG_IS_SET(FLAG_ALBERA_HOME_ANOMALY_SEEN));
CONDITION(sLauroReachedLab, VAR_AT_LEAST(VAR_ALBERA_OPENING_STATE, 2));
CONDITION(sLauroStarterChosen, VAR_AT_LEAST(VAR_ALBERA_OPENING_STATE, 3));
CONDITION(sLauroNicoBattle, FLAG_IS_SET(FLAG_ALBERA_NICO_BATTLE_COMPLETED));
CONDITION(sLauroReachedRoute101, VAR_AT_LEAST(VAR_ALBERA_OPENING_STATE, 6));
CONDITION(sLauroCompleted, FLAG_IS_SET(FLAG_ALBERA_WATER_RESEARCH_STARTED), VAR_AT_LEAST(VAR_ALBERA_OPENING_STATE, 6));

CONDITION(sSourcesDiscovered, FLAG_IS_SET(FLAG_ALBERA_WATER_RESEARCH_STARTED), VAR_AT_LEAST(VAR_ALBERA_VIA_VERDI_STATE, 1));
CONDITION(sSourcesFirstSourceChecked, VAR_AT_LEAST(VAR_ALBERA_VIA_VERDI_STATE, 2));
CONDITION(sSourcesLiaAtCanal, VAR_AT_LEAST(VAR_ALBERA_VIA_VERDI_STATE, 3));
CONDITION(sSourcesReported, VAR_IS(VAR_ALBERA_VIA_VERDI_STATE, 4));

CONDITION(sGymDiscovered, VAR_AT_LEAST(VAR_ALBERA_OPENING_STATE, 6));
CONDITION(sGymStanzaOneComplete, FLAG_IS_SET(FLAG_ALBERA_GYM_STROFA_I_COMPLETE));
CONDITION(sGymStanzaOne, FLAG_IS_SET(FLAG_ALBERA_GYM_STROFA_I_COMPLETE), VAR_AT_LEAST(VAR_ALBERA_GYM_STATE, 1));
CONDITION(sGymStanzaTwo, FLAG_IS_SET(FLAG_ALBERA_GYM_STROFA_II_COMPLETE), VAR_AT_LEAST(VAR_ALBERA_GYM_STATE, 2));
CONDITION(sGymStanzaThree, FLAG_IS_SET(FLAG_ALBERA_GYM_STROFA_III_COMPLETE), VAR_AT_LEAST(VAR_ALBERA_GYM_STATE, 3));
CONDITION(sGymLirio, FLAG_IS_SET(FLAG_BADGE01_GET), VAR_IS(VAR_ALBERA_GYM_STATE, 4));

CONDITION(sCisternsDiscovered, FLAG_IS_SET(FLAG_CISTERNONI_LIA_READY));
CONDITION(sCisternsComplete, FLAG_IS_SET(FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE));

CONDITION(sEchoDiscovered, FLAG_IS_SET(FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE));
CONDITION(sEchoLeadDone, FLAG_IS_SET(FLAG_VIA_CONSOLARE_EMISSARIO_LEAD_COMPLETE));
CONDITION(sEchoEncounterComplete, FLAG_IS_SET(FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE));
CONDITION(sEchoSeen, FLAG_IS_SET(FLAG_FIRST_ECHO_SEEN), FLAG_IS_SET(FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE), FLAG_IS_SET(FLAG_BORGO_NICO_LIA_INTRO_DONE));

CONDITION(sRecordsDiscovered, FLAG_IS_SET(FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE));
CONDITION(sRecordsBorgo, FLAG_IS_SET(FLAG_BORGO_NICO_LIA_INTRO_DONE));
CONDITION(sRecordsVilla, FLAG_IS_SET(FLAG_VILLA_PAPALE_ARCHIVIO_DONE));
CONDITION(sRecordsRoad, FLAG_IS_SET(FLAG_STRADA_BORGO_CISTERNONI_NICO_LIA_DONE));
CONDITION(sRecordsLariciaUntracked, UNTRACKED);

CONDITION(sNicoDiscovered, FLAG_IS_SET(FLAG_BADGE01_GET));
CONDITION(sNicoComplete, FLAG_IS_SET(FLAG_ALBERA_NICO_GYM_ENCOUNTER_COMPLETE));

CONDITION(sShelterDiscovered, FLAG_IS_SET(FLAG_LAGO_REFUGE_MASTER_EXPLAINED_STONE_BASES));
CONDITION(sShelterMissionStarted, FLAG_IS_SET(FLAG_LAGO_REFUGE_FIRST_MISSION_STARTED));
static const struct QuestLogConditionClause sShelterFoundClauses[] =
{
    { QUEST_LOG_CONDITION_PLAYER_SECRET_BASE_ID, 0, 84, 85 },
};
static const struct QuestLogCondition sShelterFound = { sShelterFoundClauses, ARRAY_COUNT(sShelterFoundClauses), NULL, 0 };
CONDITION(sShelterReward, FLAG_IS_SET(FLAG_LAGO_REFUGE_AMULET_COIN_RECEIVED));

static const struct QuestLogStep sLauroSteps[] =
{
    { _("Raggiungi il Laboratorio del\nCratere."), &sLauroReachedLab },
    { _("Scegli il tuo primo Pokemon."), &sLauroStarterChosen },
    { _("Affronta Nico."), &sLauroNicoBattle },
    { _("Esci da Albèra e raggiungi\nRoute 101."), &sLauroReachedRoute101 },
};

static const struct QuestLogStep sSourcesSteps[] =
{
    { _("Esamina i segnali lungo Route 101."), &sSourcesFirstSourceChecked },
    { _("Controlla il comportamento dei\nPokémon."), &sSourcesLiaAtCanal },
    { _("Raggiungi l'antico canale."), &sSourcesReported },
    { _("Riferisci l'esito dell'indagine."), &sSourcesReported },
};

static const struct QuestLogStep sGymSteps[] =
{
    { _("Scopri la sequenza delle strofe."), &sGymStanzaOneComplete },
    { _("Completa la prima strofa e supera\nDario."), &sGymStanzaOne },
    { _("Completa la seconda strofa e supera\nMara."), &sGymStanzaTwo },
    { _("Completa la terza strofa e supera\nElio."), &sGymStanzaThree },
    { _("Sconfiggi Lirio."), &sGymLirio },
};

static const struct QuestLogStep sCisternsSteps[] =
{
    { _("Raggiungi i Cisternoni."), &sCisternsComplete },
    { _("Scopri chi sta seguendo\nl'indagine."), &sCisternsComplete },
    { _("Affronta la recluta del Team\nAurea."), &sCisternsComplete },
};

static const struct QuestLogStep sEchoSteps[] =
{
    { _("Raggiungi Nico e Lia sulla Via\nConsolare."), &sEchoLeadDone },
    { _("Segui la pista fino\nall'Emissario."), &sEchoLeadDone },
    { _("Affronta la recluta Aurea."), &sEchoEncounterComplete },
    { _("Hai assistito al primo Eco;\nraggiungi Borgo di Castello."), &sEchoSeen },
};

static const struct QuestLogStep sRecordsSteps[] =
{
    { _("Raggiungi Nico e Lia a Borgo\ndi Castello."), &sRecordsBorgo },
    { _("Consulta l'archivio della Villa\nPapale."), &sRecordsVilla },
    { _("Segui la strada verso Laricia."), &sRecordsRoad },
    { _("Raggiungi Laricia."), &sRecordsLariciaUntracked },
};

static const struct QuestLogStep sNicoSteps[] =
{
    { _("Parla con Nico ad Albera Storica."), &sNicoComplete },
};

static const struct QuestLogStep sShelterSteps[] =
{
    { _("Parla con il Maestro dei Rifugi."), &sShelterDiscovered },
    { _("Parla con l'aiutante per ricevere\nl'incarico."), &sShelterMissionStarted },
    { _("Crea un rifugio sotto il nido dei\nGhepio o sotto la Casa del Maestro."), &sShelterFound },
    { _("Ritira la ricompensa\ndall'aiutante."), &sShelterReward },
};

const u8 *const gQuestLogCategoryNames[QUEST_LOG_CATEGORY_COUNT] =
{
    [QUEST_LOG_CATEGORY_STORY] = _("STORIA"),
    [QUEST_LOG_CATEGORY_SIDE] = _("SECONDARIE"),
    [QUEST_LOG_CATEGORY_JOBS] = _("INCARICHI"),
};

const struct QuestLogGroup gQuestLogGroups[] =
{
    { QUEST_GROUP_STORY_ARC_1, QUEST_LOG_CATEGORY_STORY, 1, QUEST_LOG_GROUP_COMPLETION_NONE, _("ARCO I") },
    { QUEST_GROUP_SIDE_GENERAL, QUEST_LOG_CATEGORY_SIDE, 1, QUEST_LOG_GROUP_COMPLETION_ALL_VISIBLE_QUESTS, _("GENERALE") },
    { QUEST_GROUP_SIDE_SHELTERS, QUEST_LOG_CATEGORY_SIDE, 2, QUEST_LOG_GROUP_COMPLETION_ALL_VISIBLE_QUESTS, _("RIFUGI") },
    { QUEST_GROUP_JOBS_GENERAL, QUEST_LOG_CATEGORY_JOBS, 1, QUEST_LOG_GROUP_COMPLETION_ALL_VISIBLE_QUESTS, _("GENERALE") },
    { QUEST_GROUP_JOBS_SHELTERS, QUEST_LOG_CATEGORY_JOBS, 2, QUEST_LOG_GROUP_COMPLETION_ALL_VISIBLE_QUESTS, _("RIFUGI") },
};

const u8 gQuestLogGroupCount = ARRAY_COUNT(gQuestLogGroups);

const struct QuestLogQuest gQuestLogQuests[] =
{
    { QUEST_LOG_ID_LAURO_CALL, QUEST_LOG_CATEGORY_STORY, QUEST_GROUP_STORY_ARC_1, 10, _("LA CHIAMATA DI LAURO"), _("Lauro ti attende dopo una\nanomalia nella pressione\ndell'acqua."), &sLauroDiscovered, &sLauroCompleted, sLauroSteps, ARRAY_COUNT(sLauroSteps) },
    { QUEST_LOG_ID_ROUTE101_SOURCES, QUEST_LOG_CATEGORY_STORY, QUEST_GROUP_STORY_ARC_1, 20, _("LE SORGENTI DI ROUTE 101"), _("Indaga sulle variazioni\ndell'acqua segnalate da Lia."), &sSourcesDiscovered, &sSourcesReported, sSourcesSteps, ARRAY_COUNT(sSourcesSteps) },
    { QUEST_LOG_ID_AMPHITHEATRE, QUEST_LOG_CATEGORY_STORY, QUEST_GROUP_STORY_ARC_1, 30, _("LA PROVA DELL'ANFITEATRO"), _("Supera le prove musicali e\nsfida il Capopalestra Lirio."), &sGymDiscovered, &sGymLirio, sGymSteps, ARRAY_COUNT(sGymSteps) },
    { QUEST_LOG_ID_CISTERNS, QUEST_LOG_CATEGORY_STORY, QUEST_GROUP_STORY_ARC_1, 40, _("LE MISURE DEI CISTERNONI"), _("Lia confronta le misure delle\nvasche con quelle di Via Verdi."), &sCisternsDiscovered, &sCisternsComplete, sCisternsSteps, ARRAY_COUNT(sCisternsSteps) },
    { QUEST_LOG_ID_FIRST_ECHO, QUEST_LOG_CATEGORY_STORY, QUEST_GROUP_STORY_ARC_1, 50, _("IL PRIMO ECO"), _("Segui l'indagine di Lia e Nico\nfino all'Emissario."), &sEchoDiscovered, &sEchoSeen, sEchoSteps, ARRAY_COUNT(sEchoSteps) },
    { QUEST_LOG_ID_LARICIA_RECORDS, QUEST_LOG_CATEGORY_STORY, QUEST_GROUP_STORY_ARC_1, 60, _("LE CARTE DI LARICIA"), _("Consulta i documenti antichi\nalla Villa Papale."), &sRecordsDiscovered, NULL, sRecordsSteps, ARRAY_COUNT(sRecordsSteps) },
    { QUEST_LOG_ID_NICO_TURN, QUEST_LOG_CATEGORY_SIDE, QUEST_GROUP_SIDE_GENERAL, 10, _("IL TURNO DI NICO"), _("Dopo la Medaglia, Nico vuole\nmisurarsi con Lirio."), &sNicoDiscovered, &sNicoComplete, sNicoSteps, ARRAY_COUNT(sNicoSteps) },
    { QUEST_LOG_ID_FIRST_SHELTER, QUEST_LOG_CATEGORY_JOBS, QUEST_GROUP_JOBS_SHELTERS, 10, _("UN POSTO TUTTO TUO"), _("Costruisci il tuo primo rifugio\nnel luogo indicato."), &sShelterDiscovered, &sShelterReward, sShelterSteps, ARRAY_COUNT(sShelterSteps) },
};

const u8 gQuestLogQuestCount = ARRAY_COUNT(gQuestLogQuests);
