#include "global.h"
#include "quest_log_internal.h"
#include "constants/flags.h"
#include "constants/opponents.h"
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
    QUEST_LOG_ID_MACINA_BADGE = 9,
    QUEST_LOG_ID_FOCOLARE,
    QUEST_LOG_ID_LARICIA_SAGRA,
    QUEST_LOG_ID_CHEF_CHALLENGE,
    QUEST_LOG_ID_VALLE_BERRIES,
    QUEST_LOG_ID_LA_MEMORIA,
};

#define FLAG_IS_SET(flag) { QUEST_LOG_CONDITION_FLAG_SET, flag, 0, 0 }
#define VAR_IS(var, value_) { QUEST_LOG_CONDITION_VAR_EQUALS, var, value_, 0 }
#define VAR_AT_LEAST(var, value_) { QUEST_LOG_CONDITION_VAR_GREATER_EQUAL, var, value_, 0 }
#define TRAINER_IS_DEFEATED(trainerId) { QUEST_LOG_CONDITION_TRAINER_DEFEATED, trainerId, 0, 0 }
#define UNTRACKED { QUEST_LOG_CONDITION_UNTRACKED, 0, 0, 0 }
#define CONDITION(name, ...) \
    static const struct QuestLogConditionClause name##Clauses[] = { __VA_ARGS__ }; \
    static const struct QuestLogCondition name = { name##Clauses, ARRAY_COUNT(name##Clauses), NULL, 0 }
#define CONDITION_ANY(name, ...) \
    static const struct QuestLogConditionClause name##Clauses[] = { __VA_ARGS__ }; \
    static const struct QuestLogCondition name = { NULL, 0, name##Clauses, ARRAY_COUNT(name##Clauses) }
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

CONDITION(sMacinaDiscovered, FLAG_IS_SET(FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE));
CONDITION(sMacinaCompleted, TRAINER_IS_DEFEATED(TRAINER_LAGO_WATER_GYM_MARINA), FLAG_IS_SET(FLAG_BADGE02_GET));

CONDITION(sEchoDiscovered, FLAG_IS_SET(FLAG_CISTERNONI_AUREA_ENCOUNTER_COMPLETE));
CONDITION(sEchoLeadDone, FLAG_IS_SET(FLAG_VIA_CONSOLARE_EMISSARIO_LEAD_COMPLETE));
CONDITION(sEchoEncounterComplete, FLAG_IS_SET(FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE));
CONDITION(sEchoSeen, FLAG_IS_SET(FLAG_FIRST_ECHO_SEEN), FLAG_IS_SET(FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE), FLAG_IS_SET(FLAG_BORGO_NICO_LIA_INTRO_DONE));

CONDITION(sRecordsDiscovered, FLAG_IS_SET(FLAG_EMISSARIO_AUREA_ENCOUNTER_COMPLETE));
CONDITION(sRecordsBorgo, FLAG_IS_SET(FLAG_BORGO_NICO_LIA_INTRO_DONE));
CONDITION(sRecordsVilla, FLAG_IS_SET(FLAG_VILLA_PAPALE_ARCHIVIO_DONE));
CONDITION(sRecordsRoad, FLAG_IS_SET(FLAG_STRADA_BORGO_CISTERNONI_NICO_LIA_DONE));
CONDITION_ANY(sRecordsLariciaReached,
    FLAG_IS_SET(FLAG_LARICIA_RECORDS_REACHED),
    FLAG_IS_SET(FLAG_LARICIA_SAGRA_ACTIVE),
    FLAG_IS_SET(FLAG_BADGE03_GET));

CONDITION(sNicoDiscovered, FLAG_IS_SET(FLAG_BADGE01_GET));
CONDITION(sNicoComplete, FLAG_IS_SET(FLAG_ALBERA_NICO_GYM_ENCOUNTER_COMPLETE));

CONDITION(sFocolareDiscovered, FLAG_IS_SET(FLAG_STRADA_BORGO_CISTERNONI_NICO_LIA_DONE));
CONDITION(sFocolareComplete, FLAG_IS_SET(FLAG_BADGE03_GET));
CONDITION(sSagraDiscovered, FLAG_IS_SET(FLAG_LARICIA_SAGRA_ACTIVE));
CONDITION(sSagraComplete, FLAG_IS_SET(FLAG_LARICIA_SAGRA_COMPLETE));
CONDITION_ANY(sChefDiscovered,
    FLAG_IS_SET(FLAG_UCCELLIERA_CHEF_SEEN),
    FLAG_IS_SET(FLAG_UCCELLIERA_CHEF_BERRY_GIVEN),
    FLAG_IS_SET(FLAG_UCCELLIERA_CHEF_DEFEATED));
CONDITION(sChefBerryGiven, FLAG_IS_SET(FLAG_UCCELLIERA_CHEF_BERRY_GIVEN));
CONDITION(sChefDefeated, FLAG_IS_SET(FLAG_UCCELLIERA_CHEF_DEFEATED));
CONDITION_ANY(sValleDiscovered,
    FLAG_IS_SET(FLAG_VALLE_LARICIA_BERRIES_SEEN),
    FLAG_IS_SET(FLAG_RECEIVED_VALLE_LARICIA_ORAN_BERRIES),
    FLAG_IS_SET(FLAG_RECEIVED_VALLE_LARICIA_PECHA_BERRIES),
    FLAG_IS_SET(FLAG_RECEIVED_VALLE_LARICIA_CHERI_BERRIES));
CONDITION(sValleComplete,
    FLAG_IS_SET(FLAG_RECEIVED_VALLE_LARICIA_ORAN_BERRIES),
    FLAG_IS_SET(FLAG_RECEIVED_VALLE_LARICIA_PECHA_BERRIES),
    FLAG_IS_SET(FLAG_RECEIVED_VALLE_LARICIA_CHERI_BERRIES));

CONDITION(sLaMemoriaDiscovered, FLAG_IS_SET(FLAG_GALLORO_LA_MEMORIA_SEEN));
CONDITION(sLaMemoriaComplete, FLAG_IS_SET(FLAG_GALLORO_LA_MEMORIA_COMPLETED));

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
    { COMPOUND_STRING("Raggiungi il Laboratorio del\nCratere."), &sLauroReachedLab },
    { COMPOUND_STRING("Scegli il tuo primo Pokémon."), &sLauroStarterChosen },
    { COMPOUND_STRING("Affronta Nico."), &sLauroNicoBattle },
    { COMPOUND_STRING("Esci da Albèra e raggiungi\nVia Verdi."), &sLauroReachedRoute101 },
};

static const struct QuestLogStep sSourcesSteps[] =
{
    { COMPOUND_STRING("Esamina i segnali lungo Via Verdi."), &sSourcesFirstSourceChecked },
    { COMPOUND_STRING("Controlla il comportamento dei\nPokémon."), &sSourcesLiaAtCanal },
    { COMPOUND_STRING("Raggiungi l'antico canale e riferisci\nl'esito dell'indagine."), &sSourcesReported },
};

static const struct QuestLogStep sGymSteps[] =
{
    { COMPOUND_STRING("Scopri la sequenza delle strofe."), &sGymStanzaOneComplete },
    { COMPOUND_STRING("Completa la prima strofa e supera\nDario."), &sGymStanzaOne },
    { COMPOUND_STRING("Completa la seconda strofa e supera\nMara."), &sGymStanzaTwo },
    { COMPOUND_STRING("Completa la terza strofa e supera\nElio."), &sGymStanzaThree },
    { COMPOUND_STRING("Sconfiggi Lirio."), &sGymLirio },
};

static const struct QuestLogStep sCisternsSteps[] =
{
    { COMPOUND_STRING("Indaga sui Cisternoni e affronta\nla recluta del Team Aurea."), &sCisternsComplete },
};

static const struct QuestLogStep sMacinaSteps[] =
{
    { COMPOUND_STRING("Sconfiggi Marina e ottieni la\nMedaglia Macina."), &sMacinaCompleted },
};

static const struct QuestLogStep sEchoSteps[] =
{
    { COMPOUND_STRING("Raggiungi Nico e Lia sulla Via\nConsolare."), &sEchoLeadDone },
    { COMPOUND_STRING("Segui la pista fino\nall'Emissario."), &sEchoLeadDone },
    { COMPOUND_STRING("Affronta la recluta Aurea."), &sEchoEncounterComplete },
    { COMPOUND_STRING("Hai assistito al primo Eco;\nraggiungi Borgo di Castello."), &sEchoSeen },
};

static const struct QuestLogStep sRecordsSteps[] =
{
    { COMPOUND_STRING("Raggiungi Nico e Lia a Borgo\ndi Castello."), &sRecordsBorgo },
    { COMPOUND_STRING("Consulta l'archivio della Villa\nPapale."), &sRecordsVilla },
    { COMPOUND_STRING("Segui la strada verso Laricia."), &sRecordsRoad },
    { COMPOUND_STRING("Raggiungi Laricia."), &sRecordsLariciaReached },
};

static const struct QuestLogStep sNicoSteps[] =
{
    { COMPOUND_STRING("Parla con Nico ad Albera Storica."), &sNicoComplete },
};

static const struct QuestLogStep sShelterSteps[] =
{
    { COMPOUND_STRING("Parla con il Maestro dei Rifugi."), &sShelterDiscovered },
    { COMPOUND_STRING("Parla con l'aiutante per ricevere\nl'incarico."), &sShelterMissionStarted },
    { COMPOUND_STRING("Crea un rifugio sotto il nido dei\nGhepio o sotto la Casa del Maestro."), &sShelterFound },
    { COMPOUND_STRING("Ritira la ricompensa\ndall'aiutante."), &sShelterReward },
};

static const struct QuestLogStep sFocolareSteps[] =
{
    { COMPOUND_STRING("Sconfiggi Augusto e ottieni la\nMedaglia Focolare."), &sFocolareComplete },
};

static const struct QuestLogStep sSagraSteps[] =
{
    { COMPOUND_STRING("Partecipa alla Sagra della\nPorchetta."), &sSagraComplete },
};

static const struct QuestLogStep sChefSteps[] =
{
    { COMPOUND_STRING("Consegna una Bacca allo Chef."), &sChefBerryGiven },
    { COMPOUND_STRING("Vinci la sfida contro Frascotto e\nFraschietto."), &sChefDefeated },
};

static const struct QuestLogStep sValleSteps[] =
{
    { COMPOUND_STRING("Ricevi le tre ricompense dai casali\ndella Valle."), &sValleComplete },
};

static const struct QuestLogStep sLaMemoriaSteps[] =
{
    { COMPOUND_STRING("Ascolta il Custode e lo Spirito."), &sLaMemoriaDiscovered },
    { COMPOUND_STRING("Aiuta il Sudowoodo sofferente."), &sLaMemoriaComplete },
};

const u8 *const gQuestLogCategoryNames[QUEST_LOG_CATEGORY_COUNT] =
{
    [QUEST_LOG_CATEGORY_STORY] = COMPOUND_STRING("STORIA"),
    [QUEST_LOG_CATEGORY_SIDE] = COMPOUND_STRING("SECONDARIE"),
    [QUEST_LOG_CATEGORY_JOBS] = COMPOUND_STRING("INCARICHI"),
};

const struct QuestLogGroup gQuestLogGroups[] =
{
    { QUEST_GROUP_STORY_ARC_1, QUEST_LOG_CATEGORY_STORY, 1, QUEST_LOG_GROUP_COMPLETION_NONE, COMPOUND_STRING("ARCO I") },
    { QUEST_GROUP_SIDE_GENERAL, QUEST_LOG_CATEGORY_SIDE, 1, QUEST_LOG_GROUP_COMPLETION_ALL_VISIBLE_QUESTS, COMPOUND_STRING("GENERALE") },
    { QUEST_GROUP_SIDE_SHELTERS, QUEST_LOG_CATEGORY_SIDE, 2, QUEST_LOG_GROUP_COMPLETION_ALL_VISIBLE_QUESTS, COMPOUND_STRING("RIFUGI") },
    { QUEST_GROUP_JOBS_GENERAL, QUEST_LOG_CATEGORY_JOBS, 1, QUEST_LOG_GROUP_COMPLETION_ALL_VISIBLE_QUESTS, COMPOUND_STRING("GENERALE") },
    { QUEST_GROUP_JOBS_SHELTERS, QUEST_LOG_CATEGORY_JOBS, 2, QUEST_LOG_GROUP_COMPLETION_ALL_VISIBLE_QUESTS, COMPOUND_STRING("RIFUGI") },
};

const u8 gQuestLogGroupCount = ARRAY_COUNT(gQuestLogGroups);

const struct QuestLogQuest gQuestLogQuests[] =
{
    { QUEST_LOG_ID_LAURO_CALL, QUEST_LOG_CATEGORY_STORY, QUEST_GROUP_STORY_ARC_1, 10, COMPOUND_STRING("LA CHIAMATA DI LAURO"), COMPOUND_STRING("Lauro ti attende dopo una\nanomalia nella pressione\ndell'acqua."), &sLauroDiscovered, &sLauroCompleted, sLauroSteps, ARRAY_COUNT(sLauroSteps) },
    { QUEST_LOG_ID_ROUTE101_SOURCES, QUEST_LOG_CATEGORY_STORY, QUEST_GROUP_STORY_ARC_1, 20, COMPOUND_STRING("LE SORGENTI DI VIA VERDI"), COMPOUND_STRING("Indaga sulle variazioni\ndell'acqua segnalate da Lia."), &sSourcesDiscovered, &sSourcesReported, sSourcesSteps, ARRAY_COUNT(sSourcesSteps) },
    { QUEST_LOG_ID_AMPHITHEATRE, QUEST_LOG_CATEGORY_STORY, QUEST_GROUP_STORY_ARC_1, 30, COMPOUND_STRING("LA PROVA DELL'ANFITEATRO"), COMPOUND_STRING("Supera le prove musicali e\nsfida il Capopalestra Lirio."), &sGymDiscovered, &sGymLirio, sGymSteps, ARRAY_COUNT(sGymSteps) },
    { QUEST_LOG_ID_CISTERNS, QUEST_LOG_CATEGORY_STORY, QUEST_GROUP_STORY_ARC_1, 40, COMPOUND_STRING("LE MISURE DEI CISTERNONI"), COMPOUND_STRING("Lia confronta le misure delle\nvasche con quelle di Via Verdi."), &sCisternsDiscovered, &sCisternsComplete, sCisternsSteps, ARRAY_COUNT(sCisternsSteps) },
    { QUEST_LOG_ID_MACINA_BADGE, QUEST_LOG_CATEGORY_STORY, QUEST_GROUP_STORY_ARC_1, 45, COMPOUND_STRING("LA MEDAGLIA MACINA"), COMPOUND_STRING("Affronta Marina alla Palestra\ndelle Macine."), &sMacinaDiscovered, &sMacinaCompleted, sMacinaSteps, ARRAY_COUNT(sMacinaSteps) },
    { QUEST_LOG_ID_FIRST_ECHO, QUEST_LOG_CATEGORY_STORY, QUEST_GROUP_STORY_ARC_1, 50, COMPOUND_STRING("IL PRIMO ECO"), COMPOUND_STRING("Segui l'indagine di Lia e Nico\nfino all'Emissario."), &sEchoDiscovered, &sEchoSeen, sEchoSteps, ARRAY_COUNT(sEchoSteps) },
    { QUEST_LOG_ID_LARICIA_RECORDS, QUEST_LOG_CATEGORY_STORY, QUEST_GROUP_STORY_ARC_1, 60, COMPOUND_STRING("LE CARTE DI LARICIA"), COMPOUND_STRING("Consulta i documenti antichi\nalla Villa Papale."), &sRecordsDiscovered, &sRecordsLariciaReached, sRecordsSteps, ARRAY_COUNT(sRecordsSteps) },
    { QUEST_LOG_ID_FOCOLARE, QUEST_LOG_CATEGORY_STORY, QUEST_GROUP_STORY_ARC_1, 70, COMPOUND_STRING("LA MEDAGLIA FOCOLARE"), COMPOUND_STRING("Affronta la Palestra Fuoco di Laricia\ne ottieni la Medaglia Focolare."), &sFocolareDiscovered, &sFocolareComplete, sFocolareSteps, ARRAY_COUNT(sFocolareSteps) },
    { QUEST_LOG_ID_LARICIA_SAGRA, QUEST_LOG_CATEGORY_STORY, QUEST_GROUP_STORY_ARC_1, 80, COMPOUND_STRING("LA SAGRA DI LARICIA"), COMPOUND_STRING("Celebra la vittoria con la\nSagra della Porchetta."), &sSagraDiscovered, &sSagraComplete, sSagraSteps, ARRAY_COUNT(sSagraSteps) },
    { QUEST_LOG_ID_NICO_TURN, QUEST_LOG_CATEGORY_SIDE, QUEST_GROUP_SIDE_GENERAL, 10, COMPOUND_STRING("IL TURNO DI NICO"), COMPOUND_STRING("Dopo la Medaglia, Nico vuole\nmisurarsi con Lirio."), &sNicoDiscovered, &sNicoComplete, sNicoSteps, ARRAY_COUNT(sNicoSteps) },
    { QUEST_LOG_ID_CHEF_CHALLENGE, QUEST_LOG_CATEGORY_SIDE, QUEST_GROUP_SIDE_GENERAL, 20, COMPOUND_STRING("LA SFIDA DELLO CHEF"), COMPOUND_STRING("Aiuta Chef Frascotto e affronta\nla sua sfida."), &sChefDiscovered, &sChefDefeated, sChefSteps, ARRAY_COUNT(sChefSteps) },
    { QUEST_LOG_ID_FIRST_SHELTER, QUEST_LOG_CATEGORY_JOBS, QUEST_GROUP_JOBS_SHELTERS, 10, COMPOUND_STRING("UN POSTO TUTTO TUO"), COMPOUND_STRING("Costruisci il tuo primo rifugio\nnel luogo indicato."), &sShelterDiscovered, &sShelterReward, sShelterSteps, ARRAY_COUNT(sShelterSteps) },
    { QUEST_LOG_ID_VALLE_BERRIES, QUEST_LOG_CATEGORY_JOBS, QUEST_GROUP_JOBS_GENERAL, 10, COMPOUND_STRING("I FRUTTI DELLA VALLE"), COMPOUND_STRING("Scopri le Bacche e la coltivazione\nnella Valle di Laricia."), &sValleDiscovered, &sValleComplete, sValleSteps, ARRAY_COUNT(sValleSteps) },
    { QUEST_LOG_ID_LA_MEMORIA, QUEST_LOG_CATEGORY_SIDE, QUEST_GROUP_SIDE_GENERAL, 30, COMPOUND_STRING("LA MEMORIA"), COMPOUND_STRING("Aiuta un antico amico del bosco\na ritrovare la forza."), &sLaMemoriaDiscovered, &sLaMemoriaComplete, sLaMemoriaSteps, ARRAY_COUNT(sLaMemoriaSteps) },
};

const u8 gQuestLogQuestCount = ARRAY_COUNT(gQuestLogQuests);
