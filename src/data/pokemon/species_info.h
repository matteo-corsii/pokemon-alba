#include "constants/abilities.h"
#include "constants/teaching_types.h"
#include "species_info/shared_dex_text.h"
#include "species_info/shared_front_pic_anims.h"

// Macros for ease of use.

#define EVOLUTION(...) (const struct Evolution[]) { __VA_ARGS__, { EVOLUTIONS_END }, }
#define CONDITIONS(...) ((const struct EvolutionParam[]) { __VA_ARGS__, {CONDITIONS_END} })

#define ANIM_FRAMES(...) (const union AnimCmd *const[]) { sAnim_GeneralFrame0, (const union AnimCmd[]) { __VA_ARGS__ ANIMCMD_END, }, }

#if P_FOOTPRINTS
#define FOOTPRINT(sprite) .footprint = gMonFootprint_## sprite,
#else
#define FOOTPRINT(sprite)
#endif

#if B_ENEMY_MON_SHADOW_STYLE >= GEN_4 && P_GBA_STYLE_SPECIES_GFX == FALSE
#define SHADOW(x, y, size)  .enemyShadowXOffset = x, .enemyShadowYOffset = y, .enemyShadowSize = size,
#define NO_SHADOW           .suppressEnemyShadow = TRUE,
#else
#define SHADOW(x, y, size)  .enemyShadowXOffset = 0, .enemyShadowYOffset = 0, .enemyShadowSize = 0,
#define NO_SHADOW           .suppressEnemyShadow = FALSE,
#endif

#define SIZE_32x32 1
#define SIZE_64x64 0

// Set .compressed = OW_GFX_COMPRESS
#define COMP OW_GFX_COMPRESS

#if OW_POKEMON_OBJECT_EVENTS
#if OW_PKMN_OBJECTS_SHARE_PALETTES == FALSE
#define OVERWORLD_PAL(...)                                  \
    .overworldPalette = DEFAULT(NULL, __VA_ARGS__),         \
    .overworldShinyPalette = DEFAULT_2(NULL, __VA_ARGS__),
#if P_GENDER_DIFFERENCES
#define OVERWORLD_PAL_FEMALE(...)                                 \
    .overworldPaletteFemale = DEFAULT(NULL, __VA_ARGS__),         \
    .overworldShinyPaletteFemale = DEFAULT_2(NULL, __VA_ARGS__),
#else
#define OVERWORLD_PAL_FEMALE(...)
#endif //P_GENDER_DIFFERENCES
#else
#define OVERWORLD_PAL(...)
#define OVERWORLD_PAL_FEMALE(...)
#endif //OW_PKMN_OBJECTS_SHARE_PALETTES == FALSE

#define OVERWORLD_DATA(picTable, _size, shadow, _tracks, _anims)                                                                     \
{                                                                                                                                       \
    .tileTag = TAG_NONE,                                                                                                                \
    .paletteTag = OBJ_EVENT_PAL_TAG_DYNAMIC,                                                                                            \
    .reflectionPaletteTag = OBJ_EVENT_PAL_TAG_NONE,                                                                                     \
    .size = (_size == SIZE_32x32 ? 512 : 2048),                                                                                         \
    .width = (_size == SIZE_32x32 ? 32 : 64),                                                                                           \
    .height = (_size == SIZE_32x32 ? 32 : 64),                                                                                          \
    .paletteSlot = PALSLOT_NPC_1,                                                                                                       \
    .shadowSize = shadow,                                                                                                               \
    .inanimate = FALSE,                                                                                                                 \
    .compressed = COMP,                                                                                                                 \
    .tracks = _tracks,                                                                                                                  \
    .oam = (_size == SIZE_32x32 ? &gObjectEventBaseOam_32x32 : &gObjectEventBaseOam_64x64),                                             \
    .subspriteTables = (_size == SIZE_32x32 ? sOamTables_32x32 : sOamTables_64x64),                                                     \
    .anims = _anims,                                                                                                                    \
    .images = picTable,                                                                                                                 \
}

#define OVERWORLD(objEventPic, _size, shadow, _tracks, _anims, ...)                                 \
    .overworldData = OVERWORLD_DATA(objEventPic, _size, shadow, _tracks, _anims),                   \
    OVERWORLD_PAL(__VA_ARGS__)

#if P_GENDER_DIFFERENCES
#define OVERWORLD_FEMALE(objEventPic, _size, shadow, _tracks, _anims, ...)                          \
    .overworldDataFemale = OVERWORLD_DATA(objEventPic, _size, shadow, _tracks, _anims),             \
    OVERWORLD_PAL_FEMALE(__VA_ARGS__)
#else
#define OVERWORLD_FEMALE(...)
#endif //P_GENDER_DIFFERENCES

#else
#define OVERWORLD(...)
#define OVERWORLD_FEMALE(...)
#define OVERWORLD_PAL(...)
#define OVERWORLD_PAL_FEMALE(...)
#endif //OW_POKEMON_OBJECT_EVENTS

// Maximum value for a female Pokémon is 254 (MON_FEMALE) which is 100% female.
// 255 (MON_GENDERLESS) is reserved for genderless Pokémon.
#define PERCENT_FEMALE(percent) min(254, ((percent * 255) / 100))

#define MON_TYPES(type1, ...) { type1, DEFAULT(type1, __VA_ARGS__) }
#define MON_EGG_GROUPS(group1, ...) { group1, DEFAULT(group1, __VA_ARGS__) }

#define FLIP    0
#define NO_FLIP 1

// The first Ausonia starter line uses separate tables so each evolution can diverge later.
static const struct LevelUpMove sCingermLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_TACKLE),
    LEVEL_UP_MOVE( 1, MOVE_LEER),
    LEVEL_UP_MOVE( 4, MOVE_LEAFAGE),
    LEVEL_UP_MOVE( 7, MOVE_MUD_SLAP),
    LEVEL_UP_MOVE( 9, MOVE_BITE),
    LEVEL_UP_MOVE(12, MOVE_DEFENSE_CURL),
    LEVEL_UP_MOVE(15, MOVE_ROLLOUT),
    LEVEL_UP_MOVE(18, MOVE_RAZOR_LEAF),
    LEVEL_UP_MOVE(22, MOVE_TAKE_DOWN),
    LEVEL_UP_MOVE(26, MOVE_TRAILBLAZE),
    LEVEL_UP_MOVE(30, MOVE_ASSURANCE),
    LEVEL_UP_MOVE(34, MOVE_SEED_BOMB),
    LEVEL_UP_MOVE(38, MOVE_CRUNCH),
    LEVEL_UP_MOVE(43, MOVE_HIGH_HORSEPOWER),
    LEVEL_UP_MOVE(48, MOVE_WOOD_HAMMER),
    LEVEL_UP_MOVE(54, MOVE_SUCKER_PUNCH),
    LEVEL_UP_END
};

static const struct LevelUpMove sRovascoLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_TACKLE),
    LEVEL_UP_MOVE( 1, MOVE_LEER),
    LEVEL_UP_MOVE( 4, MOVE_LEAFAGE),
    LEVEL_UP_MOVE( 7, MOVE_MUD_SLAP),
    LEVEL_UP_MOVE( 9, MOVE_BITE),
    LEVEL_UP_MOVE(12, MOVE_DEFENSE_CURL),
    LEVEL_UP_MOVE(15, MOVE_ROLLOUT),
    LEVEL_UP_MOVE(18, MOVE_RAZOR_LEAF),
    LEVEL_UP_MOVE(22, MOVE_TAKE_DOWN),
    LEVEL_UP_MOVE(26, MOVE_TRAILBLAZE),
    LEVEL_UP_MOVE(30, MOVE_ASSURANCE),
    LEVEL_UP_MOVE(34, MOVE_SEED_BOMB),
    LEVEL_UP_MOVE(38, MOVE_CRUNCH),
    LEVEL_UP_MOVE(43, MOVE_HIGH_HORSEPOWER),
    LEVEL_UP_MOVE(48, MOVE_WOOD_HAMMER),
    LEVEL_UP_MOVE(54, MOVE_SUCKER_PUNCH),
    LEVEL_UP_END
};

static const struct LevelUpMove sSelvazannaLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_TACKLE),
    LEVEL_UP_MOVE( 1, MOVE_LEER),
    LEVEL_UP_MOVE( 4, MOVE_LEAFAGE),
    LEVEL_UP_MOVE( 7, MOVE_MUD_SLAP),
    LEVEL_UP_MOVE( 9, MOVE_BITE),
    LEVEL_UP_MOVE(12, MOVE_DEFENSE_CURL),
    LEVEL_UP_MOVE(15, MOVE_ROLLOUT),
    LEVEL_UP_MOVE(18, MOVE_RAZOR_LEAF),
    LEVEL_UP_MOVE(22, MOVE_TAKE_DOWN),
    LEVEL_UP_MOVE(26, MOVE_TRAILBLAZE),
    LEVEL_UP_MOVE(30, MOVE_ASSURANCE),
    LEVEL_UP_MOVE(34, MOVE_SEED_BOMB),
    LEVEL_UP_MOVE(38, MOVE_CRUNCH),
    LEVEL_UP_MOVE(43, MOVE_HIGH_HORSEPOWER),
    LEVEL_UP_MOVE(48, MOVE_WOOD_HAMMER),
    LEVEL_UP_MOVE(54, MOVE_SUCKER_PUNCH),
    LEVEL_UP_END
};

static const struct LevelUpMove sSerbraceLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_SCRATCH),
    LEVEL_UP_MOVE( 1, MOVE_LEER),
    LEVEL_UP_MOVE( 4, MOVE_EMBER),
    LEVEL_UP_MOVE( 7, MOVE_SMOKESCREEN),
    LEVEL_UP_MOVE( 9, MOVE_FLAME_CHARGE),
    LEVEL_UP_MOVE(12, MOVE_POISON_STING),
    LEVEL_UP_MOVE(15, MOVE_BITE),
    LEVEL_UP_MOVE(18, MOVE_INCINERATE),
    LEVEL_UP_MOVE(22, MOVE_COIL),
    LEVEL_UP_MOVE(26, MOVE_VENOSHOCK),
    LEVEL_UP_MOVE(30, MOVE_FIRE_SPIN),
    LEVEL_UP_MOVE(34, MOVE_NASTY_PLOT),
    LEVEL_UP_MOVE(38, MOVE_FLAMETHROWER),
    LEVEL_UP_MOVE(43, MOVE_TOXIC),
    LEVEL_UP_MOVE(48, MOVE_SLUDGE_BOMB),
    LEVEL_UP_MOVE(54, MOVE_HEAT_WAVE),
    LEVEL_UP_END
};

static const struct LevelUpMove sVipercenLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_SCRATCH),
    LEVEL_UP_MOVE( 1, MOVE_LEER),
    LEVEL_UP_MOVE( 4, MOVE_EMBER),
    LEVEL_UP_MOVE( 7, MOVE_SMOKESCREEN),
    LEVEL_UP_MOVE( 9, MOVE_FLAME_CHARGE),
    LEVEL_UP_MOVE(12, MOVE_POISON_STING),
    LEVEL_UP_MOVE(15, MOVE_BITE),
    LEVEL_UP_MOVE(18, MOVE_INCINERATE),
    LEVEL_UP_MOVE(22, MOVE_COIL),
    LEVEL_UP_MOVE(26, MOVE_VENOSHOCK),
    LEVEL_UP_MOVE(30, MOVE_FIRE_SPIN),
    LEVEL_UP_MOVE(34, MOVE_NASTY_PLOT),
    LEVEL_UP_MOVE(38, MOVE_FLAMETHROWER),
    LEVEL_UP_MOVE(43, MOVE_TOXIC),
    LEVEL_UP_MOVE(48, MOVE_SLUDGE_BOMB),
    LEVEL_UP_MOVE(54, MOVE_HEAT_WAVE),
    LEVEL_UP_END
};

static const struct LevelUpMove sTossivampaLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_SCRATCH),
    LEVEL_UP_MOVE( 1, MOVE_LEER),
    LEVEL_UP_MOVE( 4, MOVE_EMBER),
    LEVEL_UP_MOVE( 7, MOVE_SMOKESCREEN),
    LEVEL_UP_MOVE( 9, MOVE_FLAME_CHARGE),
    LEVEL_UP_MOVE(12, MOVE_POISON_STING),
    LEVEL_UP_MOVE(15, MOVE_BITE),
    LEVEL_UP_MOVE(18, MOVE_INCINERATE),
    LEVEL_UP_MOVE(22, MOVE_COIL),
    LEVEL_UP_MOVE(26, MOVE_VENOSHOCK),
    LEVEL_UP_MOVE(30, MOVE_FIRE_SPIN),
    LEVEL_UP_MOVE(34, MOVE_NASTY_PLOT),
    LEVEL_UP_MOVE(38, MOVE_FLAMETHROWER),
    LEVEL_UP_MOVE(43, MOVE_TOXIC),
    LEVEL_UP_MOVE(48, MOVE_SLUDGE_BOMB),
    LEVEL_UP_MOVE(54, MOVE_HEAT_WAVE),
    LEVEL_UP_END
};

static const struct LevelUpMove sArdeinoLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_POUND),
    LEVEL_UP_MOVE( 1, MOVE_GROWL),
    LEVEL_UP_MOVE( 4, MOVE_WATER_GUN),
    LEVEL_UP_MOVE( 7, MOVE_PECK),
    LEVEL_UP_MOVE( 9, MOVE_QUICK_ATTACK),
    LEVEL_UP_MOVE(12, MOVE_MIST),
    LEVEL_UP_MOVE(15, MOVE_SUPERSONIC),
    LEVEL_UP_MOVE(18, MOVE_WING_ATTACK),
    LEVEL_UP_MOVE(22, MOVE_AQUA_RING),
    LEVEL_UP_MOVE(26, MOVE_AIR_SLASH),
    LEVEL_UP_MOVE(30, MOVE_BRINE),
    LEVEL_UP_MOVE(34, MOVE_AGILITY),
    LEVEL_UP_MOVE(38, MOVE_TAILWIND),
    LEVEL_UP_MOVE(43, MOVE_ROOST),
    LEVEL_UP_MOVE(48, MOVE_HYDRO_PUMP),
    LEVEL_UP_MOVE(54, MOVE_HURRICANE),
    LEVEL_UP_END
};

static const struct LevelUpMove sVelaironeLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_POUND),
    LEVEL_UP_MOVE( 1, MOVE_GROWL),
    LEVEL_UP_MOVE( 4, MOVE_WATER_GUN),
    LEVEL_UP_MOVE( 7, MOVE_PECK),
    LEVEL_UP_MOVE( 9, MOVE_QUICK_ATTACK),
    LEVEL_UP_MOVE(12, MOVE_MIST),
    LEVEL_UP_MOVE(15, MOVE_SUPERSONIC),
    LEVEL_UP_MOVE(18, MOVE_WING_ATTACK),
    LEVEL_UP_MOVE(22, MOVE_AQUA_RING),
    LEVEL_UP_MOVE(26, MOVE_AIR_SLASH),
    LEVEL_UP_MOVE(30, MOVE_BRINE),
    LEVEL_UP_MOVE(34, MOVE_AGILITY),
    LEVEL_UP_MOVE(38, MOVE_TAILWIND),
    LEVEL_UP_MOVE(43, MOVE_ROOST),
    LEVEL_UP_MOVE(48, MOVE_HYDRO_PUMP),
    LEVEL_UP_MOVE(54, MOVE_HURRICANE),
    LEVEL_UP_END
};

static const struct LevelUpMove sCodaironeLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_POUND),
    LEVEL_UP_MOVE( 1, MOVE_GROWL),
    LEVEL_UP_MOVE( 4, MOVE_WATER_GUN),
    LEVEL_UP_MOVE( 7, MOVE_PECK),
    LEVEL_UP_MOVE( 9, MOVE_QUICK_ATTACK),
    LEVEL_UP_MOVE(12, MOVE_MIST),
    LEVEL_UP_MOVE(15, MOVE_SUPERSONIC),
    LEVEL_UP_MOVE(18, MOVE_WING_ATTACK),
    LEVEL_UP_MOVE(22, MOVE_AQUA_RING),
    LEVEL_UP_MOVE(26, MOVE_AIR_SLASH),
    LEVEL_UP_MOVE(30, MOVE_BRINE),
    LEVEL_UP_MOVE(34, MOVE_AGILITY),
    LEVEL_UP_MOVE(38, MOVE_TAILWIND),
    LEVEL_UP_MOVE(43, MOVE_ROOST),
    LEVEL_UP_MOVE(48, MOVE_HYDRO_PUMP),
    LEVEL_UP_MOVE(54, MOVE_HURRICANE),
    LEVEL_UP_END
};

static const struct LevelUpMove sBorgottoLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_TACKLE),
    LEVEL_UP_MOVE( 1, MOVE_TAIL_WHIP),
    LEVEL_UP_MOVE( 4, MOVE_SAND_ATTACK),
    LEVEL_UP_MOVE( 7, MOVE_BITE),
    LEVEL_UP_MOVE(10, MOVE_ODOR_SLEUTH),
    LEVEL_UP_MOVE(13, MOVE_COVET),
    LEVEL_UP_MOVE(16, MOVE_HELPING_HAND),
    LEVEL_UP_MOVE(20, MOVE_WORK_UP),
    LEVEL_UP_MOVE(24, MOVE_TAKE_DOWN),
    LEVEL_UP_MOVE(28, MOVE_CRUNCH),
    LEVEL_UP_END
};

static const struct LevelUpMove sPastufoLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 0, MOVE_MUD_SLAP),
    LEVEL_UP_MOVE( 1, MOVE_TACKLE),
    LEVEL_UP_MOVE( 1, MOVE_TAIL_WHIP),
    LEVEL_UP_MOVE( 4, MOVE_SAND_ATTACK),
    LEVEL_UP_MOVE( 7, MOVE_BITE),
    LEVEL_UP_MOVE(10, MOVE_ODOR_SLEUTH),
    LEVEL_UP_MOVE(13, MOVE_COVET),
    LEVEL_UP_MOVE(16, MOVE_HELPING_HAND),
    LEVEL_UP_MOVE(20, MOVE_WORK_UP),
    LEVEL_UP_MOVE(22, MOVE_BULLDOZE),
    LEVEL_UP_MOVE(27, MOVE_STOCKPILE),
    LEVEL_UP_MOVE(27, MOVE_SWALLOW),
    LEVEL_UP_MOVE(27, MOVE_SPIT_UP),
    LEVEL_UP_MOVE(33, MOVE_DIG),
    LEVEL_UP_MOVE(39, MOVE_BODY_SLAM),
    LEVEL_UP_MOVE(45, MOVE_CRUNCH),
    LEVEL_UP_MOVE(52, MOVE_EARTHQUAKE),
    LEVEL_UP_END
};

static const struct LevelUpMove sMicioloLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_SCRATCH),
    LEVEL_UP_MOVE( 1, MOVE_GROWL),
    LEVEL_UP_MOVE( 4, MOVE_TAIL_WHIP),
    LEVEL_UP_MOVE( 7, MOVE_FAKE_OUT),
    LEVEL_UP_MOVE(10, MOVE_COVET),
    LEVEL_UP_MOVE(13, MOVE_SWIFT),
    LEVEL_UP_MOVE(16, MOVE_CHARM),
    LEVEL_UP_MOVE(19, MOVE_CONFUSION),
    LEVEL_UP_MOVE(23, MOVE_SING),
    LEVEL_UP_MOVE(27, MOVE_PSYBEAM),
    LEVEL_UP_MOVE(31, MOVE_AGILITY),
    LEVEL_UP_END
};

static const struct LevelUpMove sFelivatesLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 0, MOVE_CONFUSION),
    LEVEL_UP_MOVE( 1, MOVE_SCRATCH),
    LEVEL_UP_MOVE( 1, MOVE_GROWL),
    LEVEL_UP_MOVE( 4, MOVE_TAIL_WHIP),
    LEVEL_UP_MOVE( 7, MOVE_FAKE_OUT),
    LEVEL_UP_MOVE(10, MOVE_COVET),
    LEVEL_UP_MOVE(13, MOVE_SWIFT),
    LEVEL_UP_MOVE(16, MOVE_CHARM),
    LEVEL_UP_MOVE(19, MOVE_CONFUSION),
    LEVEL_UP_MOVE(22, MOVE_PSYBEAM),
    LEVEL_UP_MOVE(26, MOVE_CALM_MIND),
    LEVEL_UP_MOVE(30, MOVE_SAFEGUARD),
    LEVEL_UP_MOVE(34, MOVE_HEAL_BELL),
    LEVEL_UP_MOVE(39, MOVE_PSYCHIC),
    LEVEL_UP_MOVE(44, MOVE_BATON_PASS),
    LEVEL_UP_MOVE(50, MOVE_MOONLIGHT),
    LEVEL_UP_MOVE(56, MOVE_FUTURE_SIGHT),
    LEVEL_UP_END
};

static const struct LevelUpMove sFoliarvaLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_TACKLE),
    LEVEL_UP_MOVE( 1, MOVE_STRING_SHOT),
    LEVEL_UP_MOVE( 4, MOVE_ABSORB),
    LEVEL_UP_MOVE( 6, MOVE_BUG_BITE),
    LEVEL_UP_MOVE( 8, MOVE_STUN_SPORE),
    LEVEL_UP_MOVE(10, MOVE_RAZOR_LEAF),
    LEVEL_UP_MOVE(13, MOVE_STRUGGLE_BUG),
    LEVEL_UP_MOVE(15, MOVE_MEGA_DRAIN),
    LEVEL_UP_END
};

static const struct LevelUpMove sCrisalviaLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_HARDEN),
    LEVEL_UP_MOVE( 1, MOVE_STRING_SHOT),
    LEVEL_UP_MOVE( 1, MOVE_ABSORB),
    LEVEL_UP_MOVE( 6, MOVE_BUG_BITE),
    LEVEL_UP_MOVE( 8, MOVE_STUN_SPORE),
    LEVEL_UP_MOVE(10, MOVE_HARDEN),
    LEVEL_UP_MOVE(12, MOVE_PROTECT),
    LEVEL_UP_MOVE(15, MOVE_MEGA_DRAIN),
    LEVEL_UP_MOVE(18, MOVE_STRUGGLE_BUG),
    LEVEL_UP_END
};

static const struct LevelUpMove sInfioralaLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_TACKLE),
    LEVEL_UP_MOVE( 1, MOVE_STRING_SHOT),
    LEVEL_UP_MOVE( 1, MOVE_ABSORB),
    LEVEL_UP_MOVE( 6, MOVE_BUG_BITE),
    LEVEL_UP_MOVE( 8, MOVE_STUN_SPORE),
    LEVEL_UP_MOVE(10, MOVE_RAZOR_LEAF),
    LEVEL_UP_MOVE(13, MOVE_STRUGGLE_BUG),
    LEVEL_UP_MOVE(15, MOVE_MEGA_DRAIN),
    LEVEL_UP_MOVE(18, MOVE_GUST),
    LEVEL_UP_MOVE(20, MOVE_SLEEP_POWDER),
    LEVEL_UP_MOVE(23, MOVE_AIR_CUTTER),
    LEVEL_UP_MOVE(26, MOVE_POLLEN_PUFF),
    LEVEL_UP_MOVE(30, MOVE_GIGA_DRAIN),
    LEVEL_UP_MOVE(34, MOVE_BUG_BUZZ),
    LEVEL_UP_MOVE(38, MOVE_QUIVER_DANCE),
    LEVEL_UP_MOVE(42, MOVE_ENERGY_BALL),
    LEVEL_UP_MOVE(46, MOVE_AROMATHERAPY),
    LEVEL_UP_END
};

const struct SpeciesInfo gSpeciesInfo[] =
{
    [SPECIES_NONE] =
    {
        .speciesName = _("??????????"),
        .cryId = CRY_PORYGON,
        .natDexNum = NATIONAL_DEX_NONE,
        .categoryName = _("Unknown"),
        .height = 0,
        .weight = 0,
        .description = gFallbackPokedexText,
        .pokemonScale = 256,
        .pokemonOffset = 0,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_CircledQuestionMark,
        .frontPicSize = MON_COORDS_SIZE(40, 40),
        .frontPicYOffset = 12,
        .frontAnimFrames = sAnims_TwoFramePlaceHolder,
        .frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
        .backPic = gMonBackPic_CircledQuestionMark,
        .backPicSize = MON_COORDS_SIZE(40, 40),
        .backPicYOffset = 12,
        .backAnimId = BACK_ANIM_NONE,
        .palette = gMonPalette_CircledQuestionMark,
        .shinyPalette = gMonShinyPalette_CircledQuestionMark,
        .iconSprite = gMonIcon_QuestionMark,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        FOOTPRINT(QuestionMark)
        SHADOW(-1, 0, SHADOW_SIZE_M)
    #if OW_POKEMON_OBJECT_EVENTS
        .overworldData = {
            .tileTag = TAG_NONE,
            .paletteTag = OBJ_EVENT_PAL_TAG_SUBSTITUTE,
            .reflectionPaletteTag = OBJ_EVENT_PAL_TAG_NONE,
            .size = 512,
            .width = 32,
            .height = 32,
            .paletteSlot = PALSLOT_NPC_1,
            .shadowSize = SHADOW_SIZE_M,
            .inanimate = FALSE,
            .compressed = COMP,
            .tracks = TRACKS_FOOT,
            .oam = &gObjectEventBaseOam_32x32,
            .subspriteTables = sOamTables_32x32,
            .anims = sAnimTable_Following,
            .images = sPicTable_Substitute,
        },
    #endif
        .levelUpLearnset = sNoneLevelUpLearnset,
        .teachableLearnset = sNoneTeachableLearnset,
        .eggMoveLearnset = sNoneEggMoveLearnset,
    },

    #include "species_info/gen_1_families.h"
    #include "species_info/gen_2_families.h"
    #include "species_info/gen_3_families.h"
    #include "species_info/gen_4_families.h"
    #include "species_info/gen_5_families.h"
    #include "species_info/gen_6_families.h"
    #include "species_info/gen_7_families.h"
    #include "species_info/gen_8_families.h"
    #include "species_info/gen_9_families.h"

    [SPECIES_EGG] =
    {
        .frontPic = gMonFrontPic_Egg,
        .frontPicSize = MON_COORDS_SIZE(24, 24),
        .frontPicYOffset = 20,
        .backPic = gMonFrontPic_Egg,
        .backPicSize = MON_COORDS_SIZE(24, 24),
        .backPicYOffset = 20,
        .palette = gMonPalette_Egg,
        .shinyPalette = gMonPalette_Egg,
        .iconSprite = gMonIcon_Egg,
        .iconPalIndex = 1,
    },

    // Pokémon Alba: provisional data and reused assets for the Ausonia Grass starter line.
    [SPECIES_CINGERM] =
    {
        .baseHP        = 60,
        .baseAttack    = 65,
        .baseDefense   = 60,
        .baseSpeed     = 45,
        .baseSpAttack  = 35,
        .baseSpDefense = 45,
        .types = MON_TYPES(TYPE_GRASS),
        .catchRate = 45,
        .expYield = 64,
        .evYield_Attack = 1,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD),
        .abilities = { ABILITY_OVERGROW, ABILITY_NONE, ABILITY_DEFIANT },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Cingerm"),
        .cryId = CRY_LECHONK,
        .natDexNum = NATIONAL_DEX_CINGERM,
        .categoryName = _("GERMOGLIO"),
        .height = 5,
        .weight = 125,
        .description = COMPOUND_STRING(
            "Scava il terreno con il muso\n"
            "per cercare radici e semi.\n"
            "Il germoglio sul capo cresce\n"
            "quando il suolo è fertile."),
        .pokemonScale = 356,
        .pokemonOffset = 17,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Cingerm,
        .frontPicSize = MON_COORDS_SIZE(64, 48),
        .frontPicYOffset = 4,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(1, 12),
            ANIMCMD_FRAME(0, 8),
        ),
        .backPic = gMonBackPic_Cingerm,
        .backPicSize = MON_COORDS_SIZE(56, 56),
        .backPicYOffset = 4,
        .palette = gMonPalette_Cingerm,
        .shinyPalette = gMonShinyPalette_Cingerm,
        .iconSprite = gMonIcon_Cingerm,
        .iconPalIndex = 5,
        .pokemonJumpType = PKMN_JUMP_TYPE_SLOW,
        SHADOW(0, 1, SHADOW_SIZE_S)
        FOOTPRINT(Lechonk)
        OVERWORLD(
            sPicTable_Lechonk,
            SIZE_32x32,
            SHADOW_SIZE_S,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Lechonk,
            gShinyOverworldPalette_Lechonk
        )
        .levelUpLearnset = sCingermLevelUpLearnset,
        .teachableLearnset = sNoneTeachableLearnset,
        .eggMoveLearnset = sNoneEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 16, SPECIES_ROVASCO}),
    },

    [SPECIES_ROVASCO] =
    {
        .baseHP        = 80,
        .baseAttack    = 85,
        .baseDefense   = 80,
        .baseSpeed     = 60,
        .baseSpAttack  = 45,
        .baseSpDefense = 55,
        .types = MON_TYPES(TYPE_GRASS),
        .catchRate = 45,
        .expYield = 142,
        .evYield_Attack = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD),
        .abilities = { ABILITY_OVERGROW, ABILITY_NONE, ABILITY_DEFIANT },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Rovasco"),
        .cryId = CRY_OINKOLOGNE_M,
        .natDexNum = NATIONAL_DEX_ROVASCO,
        .categoryName = _("ROVETO"),
        .height = 9,
        .weight = 420,
        .description = COMPOUND_STRING(
            "I rovi sul dorso diventano\n"
            "più fitti quando difende\n"
            "il proprio territorio.\n"
            "Carica senza esitazione."),
        .pokemonScale = 356,
        .pokemonOffset = 17,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Rovasco,
        .frontPicSize = MON_COORDS_SIZE(64, 56),
        .frontPicYOffset = 4,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(1, 12),
            ANIMCMD_FRAME(0, 8),
        ),
        .backPic = gMonBackPic_Rovasco,
        .backPicSize = MON_COORDS_SIZE(48, 56),
        .backPicYOffset = 4,
        .palette = gMonPalette_Rovasco,
        .shinyPalette = gMonShinyPalette_Rovasco,
        .iconSprite = gMonIcon_Rovasco,
        .iconPalIndex = 5,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(5, 6, SHADOW_SIZE_M)
        FOOTPRINT(Oinkologne)
        OVERWORLD(
            sPicTable_OinkologneM,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_OinkologneM,
            gShinyOverworldPalette_OinkologneM
        )
        .levelUpLearnset = sRovascoLevelUpLearnset,
        .teachableLearnset = sNoneTeachableLearnset,
        .eggMoveLearnset = sNoneEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 36, SPECIES_SELVAZANNA}),
    },

    [SPECIES_SELVAZANNA] =
    {
        .baseHP        = 100,
        .baseAttack    = 120,
        .baseDefense   = 105,
        .baseSpeed     = 70,
        .baseSpAttack  = 55,
        .baseSpDefense = 80,
        .types = MON_TYPES(TYPE_GRASS, TYPE_DARK),
        .catchRate = 45,
        .expYield = 265,
        .evYield_Attack = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD),
        .abilities = { ABILITY_OVERGROW, ABILITY_NONE, ABILITY_DEFIANT },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Selvazanna"),
        .cryId = CRY_MAMOSWINE,
        .natDexNum = NATIONAL_DEX_SELVAZANNA,
        .categoryName = _("SELVA"),
        .height = 16,
        .weight = 1450,
        .description = COMPOUND_STRING(
            "Protegge i boschi usando\n"
            "le grandi zanne a radice.\n"
            "Si nasconde nel sottobosco\n"
            "prima di caricare gli intrusi."),
        .pokemonScale = 257,
        .pokemonOffset = 6,
        .trainerScale = 423,
        .trainerOffset = 8,
        .frontPic = gMonFrontPic_Selvazanna,
        .frontPicSize = MON_COORDS_SIZE(48, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(1, 12),
            ANIMCMD_FRAME(0, 8),
        ),
        .frontAnimId = ANIM_BACK_AND_LUNGE,
        .backPic = gMonBackPic_Selvazanna,
        .backPicSize = MON_COORDS_SIZE(48, 56),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_V_SHAKE_LOW,
        .palette = gMonPalette_Selvazanna,
        .shinyPalette = gMonShinyPalette_Selvazanna,
        .iconSprite = gMonIcon_Selvazanna,
        .iconPalIndex = 5,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(7, 7, SHADOW_SIZE_L)
        FOOTPRINT(Mamoswine)
        OVERWORLD(
            sPicTable_Mamoswine,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Mamoswine,
            gShinyOverworldPalette_Mamoswine
        )
        .levelUpLearnset = sSelvazannaLevelUpLearnset,
        .teachableLearnset = sNoneTeachableLearnset,
        .eggMoveLearnset = sNoneEggMoveLearnset,
    },

    [SPECIES_SERBRACE] =
    {
        .baseHP        = 45,
        .baseAttack    = 40,
        .baseDefense   = 40,
        .baseSpeed     = 65,
        .baseSpAttack  = 70,
        .baseSpDefense = 50,
        .types = MON_TYPES(TYPE_FIRE),
        .catchRate = 45,
        .expYield = 64,
        .evYield_SpAttack = 1,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD, EGG_GROUP_DRAGON),
        .abilities = { ABILITY_BLAZE, ABILITY_NONE, ABILITY_CORROSION },
        .bodyColor = BODY_COLOR_BLACK,
        .speciesName = _("Serbrace"),
        .cryId = CRY_EKANS,
        .natDexNum = NATIONAL_DEX_SERBRACE,
        .categoryName = _("BRACE"),
        .height = 6,
        .weight = 60,
        .description = COMPOUND_STRING(
            "Trattiene il calore tra le\n"
            "squame scure del suo corpo.\n"
            "Quando si agita, dal dorso\n"
            "si alzano piccoli sbuffi."),
        .pokemonScale = 298,
        .pokemonOffset = 12,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Serbrace,
        .frontPicSize = MON_COORDS_SIZE(64, 56),
        .frontPicYOffset = 4,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(1, 12),
            ANIMCMD_FRAME(0, 8),
        ),
        .frontAnimId = P_GBA_STYLE_SPECIES_GFX ? ANIM_H_STRETCH : ANIM_V_STRETCH,
        .frontAnimDelay = 30,
        .backPic = gMonBackPic_Serbrace,
        .backPicSize = MON_COORDS_SIZE(64, 56),
        .backPicYOffset = 4,
        .backAnimId = BACK_ANIM_TRIANGLE_DOWN,
        .palette = gMonPalette_Serbrace,
        .shinyPalette = gMonShinyPalette_Serbrace,
        .iconSprite = gMonIcon_Serbrace,
        .iconPalIndex = 3,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 2, SHADOW_SIZE_M)
        FOOTPRINT(Ekans)
        OVERWORLD(
            sPicTable_Ekans,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_SLITHER,
            sAnimTable_Following,
            gOverworldPalette_Ekans,
            gShinyOverworldPalette_Ekans
        )
        .levelUpLearnset = sSerbraceLevelUpLearnset,
        .teachableLearnset = sNoneTeachableLearnset,
        .eggMoveLearnset = sNoneEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 16, SPECIES_VIPERCEN}),
    },

    [SPECIES_VIPERCEN] =
    {
        .baseHP        = 60,
        .baseAttack    = 55,
        .baseDefense   = 55,
        .baseSpeed     = 75,
        .baseSpAttack  = 95,
        .baseSpDefense = 65,
        .types = MON_TYPES(TYPE_FIRE),
        .catchRate = 45,
        .expYield = 142,
        .evYield_SpAttack = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD, EGG_GROUP_DRAGON),
        .abilities = { ABILITY_BLAZE, ABILITY_NONE, ABILITY_CORROSION },
        .bodyColor = BODY_COLOR_BLACK,
        .speciesName = _("Vipercen"),
        .cryId = CRY_ARBOK,
        .natDexNum = NATIONAL_DEX_VIPERCEN,
        .categoryName = _("CENERE"),
        .height = 12,
        .weight = 185,
        .description = COMPOUND_STRING(
            "Le scaglie del collo accumulano\n"
            "cenere e calore vulcanico.\n"
            "Studia a lungo l’avversario\n"
            "prima di attaccare."),
        .pokemonScale = 256,
        .pokemonOffset = 0,
        .trainerScale = 296,
        .trainerOffset = 2,
        .frontPic = gMonFrontPic_Vipercen,
        .frontPicSize = MON_COORDS_SIZE(48, 56),
        .frontPicYOffset = 6,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(1, 12),
            ANIMCMD_FRAME(0, 8),
        ),
        .frontAnimId = ANIM_V_STRETCH,
        .backPic = gMonBackPic_Vipercen,
        .backPicSize = MON_COORDS_SIZE(48, 56),
        .backPicYOffset = 7,
        .backAnimId = BACK_ANIM_V_SHAKE,
        .palette = gMonPalette_Vipercen,
        .shinyPalette = gMonShinyPalette_Vipercen,
        .iconSprite = gMonIcon_Vipercen,
        .iconPalIndex = 3,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(1, 11, SHADOW_SIZE_L)
        FOOTPRINT(Arbok)
        OVERWORLD(
            sPicTable_Arbok,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_SLITHER,
            sAnimTable_Following,
            gOverworldPalette_Arbok,
            gShinyOverworldPalette_Arbok
        )
        .levelUpLearnset = sVipercenLevelUpLearnset,
        .teachableLearnset = sNoneTeachableLearnset,
        .eggMoveLearnset = sNoneEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 36, SPECIES_TOSSIVAMPA}),
    },

    [SPECIES_TOSSIVAMPA] =
    {
        .baseHP        = 75,
        .baseAttack    = 70,
        .baseDefense   = 70,
        .baseSpeed     = 105,
        .baseSpAttack  = 125,
        .baseSpDefense = 85,
        .types = MON_TYPES(TYPE_FIRE, TYPE_POISON),
        .catchRate = 45,
        .expYield = 265,
        .evYield_SpAttack = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD, EGG_GROUP_DRAGON),
        .abilities = { ABILITY_BLAZE, ABILITY_NONE, ABILITY_CORROSION },
        .bodyColor = BODY_COLOR_BLACK,
        .noFlip = TRUE,
        .speciesName = _("Tossivampa"),
        .cryId = CRY_SEVIPER,
        .natDexNum = NATIONAL_DEX_TOSSIVAMPA,
        .categoryName = _("FUMAROLA"),
        .height = 22,
        .weight = 520,
        .description = COMPOUND_STRING(
            "Emette vapori minerali dalle\n"
            "aperture del suo cappuccio.\n"
            "Le sue zanne diffondono un\n"
            "veleno caldo e corrosivo."),
        .pokemonScale = 275,
        .pokemonOffset = 7,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Tossivampa,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 2,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(1, 12),
            ANIMCMD_FRAME(0, 8),
        ),
        .frontAnimId = ANIM_V_STRETCH,
        .backPic = gMonBackPic_Tossivampa,
        .backPicSize = MON_COORDS_SIZE(56, 64),
        .backPicYOffset = 2,
        .backAnimId = BACK_ANIM_V_STRETCH,
        .palette = gMonPalette_Tossivampa,
        .shinyPalette = gMonShinyPalette_Tossivampa,
        .iconSprite = gMonIcon_Tossivampa,
        .iconPalIndex = 3,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-3, 7, SHADOW_SIZE_L)
        FOOTPRINT(Seviper)
        OVERWORLD(
            sPicTable_Seviper,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_BIKE_TIRE,
            sAnimTable_Following_Asym,
            gOverworldPalette_Seviper,
            gShinyOverworldPalette_Seviper
        )
        .levelUpLearnset = sTossivampaLevelUpLearnset,
        .teachableLearnset = sNoneTeachableLearnset,
        .eggMoveLearnset = sNoneEggMoveLearnset,
    },

    [SPECIES_ARDEINO] =
    {
        .baseHP        = 50,
        .baseAttack    = 45,
        .baseDefense   = 50,
        .baseSpeed     = 45,
        .baseSpAttack  = 65,
        .baseSpDefense = 55,
        .types = MON_TYPES(TYPE_WATER),
        .catchRate = 45,
        .expYield = 64,
        .evYield_SpAttack = 1,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FLYING, EGG_GROUP_WATER_1),
        .abilities = { ABILITY_TORRENT, ABILITY_NONE, ABILITY_HYDRATION },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Ardeino"),
        .cryId = CRY_DUCKLETT,
        .natDexNum = NATIONAL_DEX_ARDEINO,
        .categoryName = _("PIUMALAGO"),
        .height = 5,
        .weight = 38,
        .description = COMPOUND_STRING(
            "Osserva a lungo il proprio\n"
            "riflesso nelle acque calme.\n"
            "La piuma sulla coda ondeggia\n"
            "anche quando non c’è vento."),
        .pokemonScale = 432,
        .pokemonOffset = 14,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Ardeino,
        .frontPicSize = MON_COORDS_SIZE(56, 64),
        .frontPicYOffset = 3,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(1, 12),
            ANIMCMD_FRAME(0, 8),
        ),
        .frontAnimId = ANIM_V_STRETCH,
        .backPic = gMonBackPic_Ardeino,
        .backPicSize = MON_COORDS_SIZE(40, 64),
        .backPicYOffset = 4,
        .backAnimId = BACK_ANIM_CONCAVE_ARC_SMALL,
        .palette = gMonPalette_Ardeino,
        .shinyPalette = gMonShinyPalette_Ardeino,
        .iconSprite = gMonIcon_Ardeino,
        .iconPalIndex = 3,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-1, 2, SHADOW_SIZE_S)
        FOOTPRINT(Ducklett)
        OVERWORLD(
            sPicTable_Ducklett,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Ducklett,
            gShinyOverworldPalette_Ducklett
        )
        .levelUpLearnset = sArdeinoLevelUpLearnset,
        .teachableLearnset = sNoneTeachableLearnset,
        .eggMoveLearnset = sNoneEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 16, SPECIES_VELAIRONE}),
    },

    [SPECIES_VELAIRONE] =
    {
        .baseHP        = 65,
        .baseAttack    = 60,
        .baseDefense   = 65,
        .baseSpeed     = 60,
        .baseSpAttack  = 85,
        .baseSpDefense = 70,
        .types = MON_TYPES(TYPE_WATER),
        .catchRate = 45,
        .expYield = 142,
        .evYield_SpAttack = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FLYING, EGG_GROUP_WATER_1),
        .abilities = { ABILITY_TORRENT, ABILITY_NONE, ABILITY_HYDRATION },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Velairone"),
        .cryId = CRY_SWANNA,
        .natDexNum = NATIONAL_DEX_VELAIRONE,
        .categoryName = _("VELO"),
        .height = 10,
        .weight = 125,
        .description = COMPOUND_STRING(
            "Cammina nelle acque basse\n"
            "senza produrre alcun rumore.\n"
            "Le sue lunghe piume seguono\n"
            "il movimento delle correnti."),
        .pokemonScale = 272,
        .pokemonOffset = 3,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Velairone,
        .frontPicSize = MON_COORDS_SIZE(56, 64),
        .frontPicYOffset = 3,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(1, 12),
            ANIMCMD_FRAME(0, 8),
        ),
        .frontAnimId = ANIM_V_STRETCH,
        .backPic = gMonBackPic_Velairone,
        .backPicSize = MON_COORDS_SIZE(32, 64),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_H_STRETCH,
        .palette = gMonPalette_Velairone,
        .shinyPalette = gMonShinyPalette_Velairone,
        .iconSprite = gMonIcon_Velairone,
        .iconPalIndex = 3,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 12, SHADOW_SIZE_M)
        FOOTPRINT(Swanna)
        OVERWORLD(
            sPicTable_Swanna,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Swanna,
            gShinyOverworldPalette_Swanna
        )
        .levelUpLearnset = sVelaironeLevelUpLearnset,
        .teachableLearnset = sNoneTeachableLearnset,
        .eggMoveLearnset = sNoneEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 36, SPECIES_CODAIRONE}),
    },

    [SPECIES_CODAIRONE] =
    {
        .baseHP        = 85,
        .baseAttack    = 70,
        .baseDefense   = 80,
        .baseSpeed     = 90,
        .baseSpAttack  = 110,
        .baseSpDefense = 95,
        .types = MON_TYPES(TYPE_WATER, TYPE_FLYING),
        .catchRate = 45,
        .expYield = 265,
        .evYield_SpAttack = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FLYING, EGG_GROUP_WATER_1),
        .abilities = { ABILITY_TORRENT, ABILITY_NONE, ABILITY_HYDRATION },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Codairone"),
        .cryId = CRY_BOMBIRDIER,
        .natDexNum = NATIONAL_DEX_CODAIRONE,
        .categoryName = _("RIFLESSO"),
        .height = 17,
        .weight = 360,
        .description = COMPOUND_STRING(
            "Le ali disegnano increspature\n"
            "simili a quelle di un lago.\n"
            "Percepisce subito i cambiamenti\n"
            "nell’acqua e nell’animo umano."),
        .pokemonScale = 356,
        .pokemonOffset = 17,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Codairone,
        .frontPicSize = MON_COORDS_SIZE(56, 64),
        .frontPicYOffset = 3,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(1, 12),
            ANIMCMD_FRAME(0, 8),
        ),
        .backPic = gMonBackPic_Codairone,
        .backPicSize = MON_COORDS_SIZE(48, 64),
        .backPicYOffset = 3,
        .palette = gMonPalette_Codairone,
        .shinyPalette = gMonShinyPalette_Codairone,
        .iconSprite = gMonIcon_Codairone,
        .iconPalIndex = 3,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(8, 12, SHADOW_SIZE_M)
        FOOTPRINT(Bombirdier)
        OVERWORLD(
            sPicTable_Bombirdier,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_NONE,
            sAnimTable_Following,
            gOverworldPalette_Bombirdier,
            gShinyOverworldPalette_Bombirdier
        )
        .levelUpLearnset = sCodaironeLevelUpLearnset,
        .teachableLearnset = sNoneTeachableLearnset,
        .eggMoveLearnset = sNoneEggMoveLearnset,
    },

    // Pokémon Alba: common early fauna. Graphics remain authorized placeholders.
    [SPECIES_BORGOTTO] =
    {
        .baseHP        = 45,
        .baseAttack    = 50,
        .baseDefense   = 40,
        .baseSpeed     = 55,
        .baseSpAttack  = 30,
        .baseSpDefense = 40,
        .types = MON_TYPES(TYPE_NORMAL),
        .catchRate = 255,
        .expYield = 64,
        .evYield_HP = 1,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD),
        .abilities = { ABILITY_KEEN_EYE, ABILITY_NONE, ABILITY_PICKUP },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Borgotto"),
        .cryId = CRY_LILLIPUP,
        .natDexNum = NATIONAL_DEX_BORGOTTO,
        .categoryName = _("BORGO"),
        .height = 3,
        .weight = 32,
        .description = COMPOUND_STRING(
            "È molto curioso e ama esplorare\n"
            "case e giardini. Raccoglie piccoli\n"
            "oggetti e li porta nella propria\n"
            "tana."),
        .pokemonScale = 491,
        .pokemonOffset = 15,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Borgotto,
        .frontPicSize = MON_COORDS_SIZE(48, 56),
        .frontPicYOffset = 4,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_H_JUMPS,
        .backPic = gMonBackPic_Borgotto,
        .backPicSize = MON_COORDS_SIZE(48, 48),
        .backPicYOffset = 4,
        .backAnimId = BACK_ANIM_CONCAVE_ARC_SMALL,
        .palette = gMonPalette_Borgotto,
        .shinyPalette = gMonShinyPalette_Borgotto,
        .iconSprite = gMonIcon_Borgotto,
        .iconPalIndex = 2,
        .pokemonJumpType = PKMN_JUMP_TYPE_FAST,
        SHADOW(2, 1, SHADOW_SIZE_S)
        FOOTPRINT(Lillipup)
        OVERWORLD(
            sPicTable_Lillipup,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Lillipup,
            gShinyOverworldPalette_Lillipup
        )
        .levelUpLearnset = sBorgottoLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sBorgottoTeachableLearnset,
        .eggMoveLearnset = sBorgottoEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 18, SPECIES_PASTUFO}),
    },

    [SPECIES_PASTUFO] =
    {
        .baseHP        = 80,
        .baseAttack    = 88,
        .baseDefense   = 78,
        .baseSpeed     = 65,
        .baseSpAttack  = 42,
        .baseSpDefense = 67,
        .types = MON_TYPES(TYPE_NORMAL, TYPE_GROUND),
        .catchRate = 120,
        .expYield = 158,
        .evYield_HP = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD),
        .abilities = { ABILITY_KEEN_EYE, ABILITY_NONE, ABILITY_PICKUP },
        .bodyColor = BODY_COLOR_GRAY,
        .speciesName = _("Pastufo"),
        .cryId = CRY_HERDIER,
        .natDexNum = NATIONAL_DEX_PASTUFO,
        .categoryName = _("PROVVISTA"),
        .height = 6,
        .weight = 98,
        .description = COMPOUND_STRING(
            "Accumula cibo e materiali per\n"
            "prepararsi ai mesi freddi.\n"
            "È robusto, laborioso e molto fedele\n"
            "al proprio gruppo."),
        .pokemonScale = 338,
        .pokemonOffset = 9,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Pastufo,
        .frontPicSize = MON_COORDS_SIZE(56, 64),
        .frontPicYOffset = 3,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_H_STRETCH,
        .backPic = gMonBackPic_Pastufo,
        .backPicSize = MON_COORDS_SIZE(56, 64),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_H_SHAKE,
        .palette = gMonPalette_Pastufo,
        .shinyPalette = gMonShinyPalette_Pastufo,
        .iconSprite = gMonIcon_Pastufo,
        .iconPalIndex = 1,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(3, 5, SHADOW_SIZE_M)
        FOOTPRINT(Herdier)
        OVERWORLD(
            sPicTable_Herdier,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Herdier,
            gShinyOverworldPalette_Herdier
        )
        .levelUpLearnset = sPastufoLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sPastufoTeachableLearnset,
    },

    [SPECIES_MICIOLO] =
    {
        .baseHP        = 42,
        .baseAttack    = 40,
        .baseDefense   = 38,
        .baseSpeed     = 65,
        .baseSpAttack  = 50,
        .baseSpDefense = 45,
        .types = MON_TYPES(TYPE_NORMAL),
        .catchRate = 190,
        .expYield = 70,
        .evYield_Speed = 1,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD),
        .abilities = { ABILITY_LIMBER, ABILITY_NONE, ABILITY_SYNCHRONIZE },
        .bodyColor = BODY_COLOR_PINK,
        .speciesName = _("Miciolo"),
        .cryId = CRY_SKITTY,
        .natDexNum = NATIONAL_DEX_MICIOLO,
        .categoryName = _("GATTO"),
        .height = 4,
        .weight = 41,
        .description = COMPOUND_STRING(
            "Ama le coccole e vive in armonia con\n"
            "le persone. Di notte i suoi grandi\n"
            "occhi riflettono una tenue luce\n"
            "lunare."),
        .pokemonScale = 492,
        .pokemonOffset = 19,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Miciolo,
        .frontPicSize = MON_COORDS_SIZE(48, 64),
        .frontPicYOffset = 3,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
        .backPic = gMonBackPic_Miciolo,
        .backPicSize = MON_COORDS_SIZE(48, 64),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_DIP_RIGHT_SIDE,
        .palette = gMonPalette_Miciolo,
        .shinyPalette = gMonShinyPalette_Miciolo,
        .iconSprite = gMonIcon_Miciolo,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NORMAL,
        SHADOW(-3, 1, SHADOW_SIZE_S)
        FOOTPRINT(Skitty)
        OVERWORLD(
            sPicTable_Skitty,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Skitty,
            gShinyOverworldPalette_Skitty
        )
        .levelUpLearnset = sMicioloLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sMicioloTeachableLearnset,
        .eggMoveLearnset = sMicioloEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 0, SPECIES_FELIVATES, CONDITIONS({IF_MIN_FRIENDSHIP, FRIENDSHIP_EVO_THRESHOLD})}),
    },

    [SPECIES_FELIVATES] =
    {
        .baseHP        = 70,
        .baseAttack    = 52,
        .baseDefense   = 60,
        .baseSpeed     = 100,
        .baseSpAttack  = 98,
        .baseSpDefense = 80,
        .types = MON_TYPES(TYPE_NORMAL, TYPE_PSYCHIC),
        .catchRate = 75,
        .expYield = 168,
        .evYield_Speed = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD),
        .abilities = { ABILITY_LIMBER, ABILITY_NONE, ABILITY_SYNCHRONIZE },
        .bodyColor = BODY_COLOR_PURPLE,
        .speciesName = _("Felivates"),
        .cryId = CRY_ESPEON,
        .natDexNum = NATIONAL_DEX_FELIVATES,
        .categoryName = _("ARMONIA"),
        .height = 9,
        .weight = 136,
        .description = COMPOUND_STRING(
            "La gemma sulla fronte emana una luce\n"
            "ipnotica. La sua presenza calma gli\n"
            "animi inquieti e protegge chi gli è\n"
            "vicino."),
        .pokemonScale = 363,
        .pokemonOffset = 14,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Felivates,
        .frontPicSize = MON_COORDS_SIZE(56, 64),
        .frontPicYOffset = 3,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_GROW_VIBRATE,
        .backPic = gMonBackPic_Felivates,
        .backPicSize = MON_COORDS_SIZE(56, 64),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_SHRINK_GROW_VIBRATE,
        .palette = gMonPalette_Felivates,
        .shinyPalette = gMonShinyPalette_Felivates,
        .iconSprite = gMonIcon_Felivates,
        .iconPalIndex = 1,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(2, 4, SHADOW_SIZE_M)
        FOOTPRINT(Espeon)
        OVERWORLD(
            sPicTable_Espeon,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Espeon,
            gShinyOverworldPalette_Espeon
        )
        .levelUpLearnset = sFelivatesLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sFelivatesTeachableLearnset,
    },

    [SPECIES_FOLIARVA] =
    {
        .baseHP        = 45,
        .baseAttack    = 35,
        .baseDefense   = 40,
        .baseSpeed     = 45,
        .baseSpAttack  = 35,
        .baseSpDefense = 40,
        .types = MON_TYPES(TYPE_BUG),
        .catchRate = 255,
        .expYield = 54,
        .evYield_HP = 1,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_BUG),
        .abilities = { ABILITY_SHIELD_DUST, ABILITY_SWARM, ABILITY_CHLOROPHYLL },
        .bodyColor = BODY_COLOR_GREEN,
        .speciesName = _("Foliarva"),
        .cryId = CRY_CATERPIE,
        .natDexNum = NATIONAL_DEX_FOLIARVA,
        .categoryName = _("LARVAFOGLIA"),
        .height = 3,
        .weight = 24,
        .description = COMPOUND_STRING(
            "Si nutre delle foglie più tenere senza\n"
            "danneggiarne le nervature. Le antenne\n"
            "arancioni cambiano tono quando trova\n"
            "una pianta sana."),
        .pokemonScale = 549,
        .pokemonOffset = 22,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Foliarva,
        .frontPicSize = MON_COORDS_SIZE(48, 48),
        .frontPicYOffset = 8,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_SWING_CONCAVE,
        .backPic = gMonBackPic_Foliarva,
        .backPicSize = MON_COORDS_SIZE(40, 48),
        .backPicYOffset = 8,
        .backAnimId = BACK_ANIM_H_SLIDE,
        .palette = gMonPalette_Foliarva,
        .shinyPalette = gMonShinyPalette_Foliarva,
        .iconSprite = gMonIcon_Foliarva,
        .iconPalIndex = 1,
        .pokemonJumpType = PKMN_JUMP_TYPE_FAST,
        SHADOW(4, 1, SHADOW_SIZE_S)
        FOOTPRINT(Caterpie)
        OVERWORLD(
            sPicTable_Caterpie,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_SPOT,
            sAnimTable_Following,
            gOverworldPalette_Caterpie,
            gShinyOverworldPalette_Caterpie
        )
        .levelUpLearnset = sFoliarvaLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sFoliarvaTeachableLearnset,
        .eggMoveLearnset = sFoliarvaEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 10, SPECIES_CRISALVIA}),
    },

    [SPECIES_CRISALVIA] =
    {
        .baseHP        = 55,
        .baseAttack    = 30,
        .baseDefense   = 75,
        .baseSpeed     = 25,
        .baseSpAttack  = 40,
        .baseSpDefense = 65,
        .types = MON_TYPES(TYPE_BUG, TYPE_GRASS),
        .catchRate = 120,
        .expYield = 108,
        .evYield_Defense = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_BUG),
        .abilities = { ABILITY_SHED_SKIN, ABILITY_LEAF_GUARD, ABILITY_OVERCOAT },
        .bodyColor = BODY_COLOR_GREEN,
        .speciesName = _("Crisalvia"),
        .cryId = CRY_METAPOD,
        .natDexNum = NATIONAL_DEX_CRISALVIA,
        .categoryName = _("CRISALIDE"),
        .height = 5,
        .weight = 68,
        .description = COMPOUND_STRING(
            "Avvolge il corpo in strati di fibra\n"
            "vegetale e resta immobile per giorni.\n"
            "La luce filtrata dalle foglie ne\n"
            "accelera la metamorfosi."),
        .pokemonScale = 350,
        .pokemonOffset = 18,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Crisalvia,
        .frontPicSize = MON_COORDS_SIZE(40, 64),
        .frontPicYOffset = 3,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_SWING_CONCAVE,
        .backPic = gMonBackPic_Crisalvia,
        .backPicSize = MON_COORDS_SIZE(40, 64),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_DIP_RIGHT_SIDE,
        .palette = gMonPalette_Crisalvia,
        .shinyPalette = gMonShinyPalette_Crisalvia,
        .iconSprite = gMonIcon_Crisalvia,
        .iconPalIndex = 1,
        .pokemonJumpType = PKMN_JUMP_TYPE_FAST,
        SHADOW(3, 0, SHADOW_SIZE_S)
        FOOTPRINT(Metapod)
        OVERWORLD(
            sPicTable_Metapod,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_SPOT,
            sAnimTable_Following,
            gOverworldPalette_Metapod,
            gShinyOverworldPalette_Metapod
        )
        .levelUpLearnset = sCrisalviaLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sCrisalviaTeachableLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 18, SPECIES_INFIORALA}),
    },

    [SPECIES_INFIORALA] =
    {
        .baseHP        = 70,
        .baseAttack    = 45,
        .baseDefense   = 65,
        .baseSpeed     = 90,
        .baseSpAttack  = 100,
        .baseSpDefense = 80,
        .types = MON_TYPES(TYPE_BUG, TYPE_GRASS),
        .catchRate = 75,
        .expYield = 178,
        .evYield_SpAttack = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_BUG),
        .abilities = { ABILITY_COMPOUND_EYES, ABILITY_CHLOROPHYLL, ABILITY_TINTED_LENS },
        .bodyColor = BODY_COLOR_GREEN,
        .speciesName = _("Infiorala"),
        .cryId = CRY_BUTTERFREE,
        .natDexNum = NATIONAL_DEX_INFIORALA,
        .categoryName = _("FLOREALE"),
        .height = 9,
        .weight = 142,
        .description = COMPOUND_STRING(
            "Trasporta il polline tra i fiori delle\n"
            "radure con il battito delle ali. Dove\n"
            "passa, la vegetazione rifiorisce più\n"
            "rapidamente."),
        .pokemonScale = 312,
        .pokemonOffset = 2,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Infiorala,
        .frontPicSize = MON_COORDS_SIZE(64, 56),
        .frontPicYOffset = 4,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = P_GBA_STYLE_SPECIES_GFX ? ANIM_H_SLIDE_WOBBLE : ANIM_V_SLIDE_WOBBLE,
        .enemyMonElevation = P_GBA_STYLE_SPECIES_GFX ? 8 : 12,
        .backPic = gMonBackPic_Infiorala,
        .backPicSize = MON_COORDS_SIZE(64, 56),
        .backPicYOffset = 4,
        .backAnimId = BACK_ANIM_CONVEX_DOUBLE_ARC,
        .palette = gMonPalette_Infiorala,
        .shinyPalette = gMonShinyPalette_Infiorala,
        .iconSprite = gMonIcon_Infiorala,
        .iconPalIndex = 1,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-3, 13, SHADOW_SIZE_S)
        FOOTPRINT(Butterfree)
        OVERWORLD(
            sPicTable_Butterfree,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Butterfree,
            gShinyOverworldPalette_Butterfree
        )
        .levelUpLearnset = sInfioralaLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sInfioralaTeachableLearnset,
    },

    /* You may add any custom species below this point based on the following structure: */

    /*
    [SPECIES_NONE] =
    {
        .baseHP        = 1,
        .baseAttack    = 1,
        .baseDefense   = 1,
        .baseSpeed     = 1,
        .baseSpAttack  = 1,
        .baseSpDefense = 1,
        .types = MON_TYPES(TYPE_MYSTERY),
        .catchRate = 255,
        .expYield = 67,
        .evYield_HP = 1,
        .evYield_Defense = 1,
        .evYield_SpDefense = 1,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_NO_EGGS_DISCOVERED),
        .abilities = { ABILITY_NONE, ABILITY_CURSED_BODY, ABILITY_DAMP },
        .bodyColor = BODY_COLOR_BLACK,
        .speciesName = _("??????????"),
        .cryId = CRY_NONE,
        .natDexNum = NATIONAL_DEX_NONE,
        .categoryName = _("Unknown"),
        .height = 0,
        .weight = 0,
        .description = COMPOUND_STRING(
            "This is a newly discovered Pokémon.\n"
            "It is currently under investigation.\n"
            "No detailed information is available\n"
            "at this time."),
        .pokemonScale = 256,
        .pokemonOffset = 0,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_CircledQuestionMark,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_None,
        //.frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
        .backPic = gMonBackPic_CircledQuestionMark,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 7,
#if P_GENDER_DIFFERENCES
        .frontPicFemale = gMonFrontPic_CircledQuestionMark,
        .frontPicSizeFemale = MON_COORDS_SIZE(64, 64),
        .backPicFemale = gMonBackPic_CircledQuestionMarkF,
        .backPicSizeFemale = MON_COORDS_SIZE(64, 64),
        .paletteFemale = gMonPalette_CircledQuestionMarkF,
        .shinyPaletteFemale = gMonShinyPalette_CircledQuestionMarkF,
        .iconSpriteFemale = gMonIcon_QuestionMarkF,
        .iconPalIndexFemale = 1,
#endif //P_GENDER_DIFFERENCES
        .backAnimId = BACK_ANIM_NONE,
        .palette = gMonPalette_CircledQuestionMark,
        .shinyPalette = gMonShinyPalette_CircledQuestionMark,
        .iconSprite = gMonIcon_QuestionMark,
        .iconPalIndex = 0,
        FOOTPRINT(QuestionMark)
        .levelUpLearnset = sNoneLevelUpLearnset,
        .teachableLearnset = sNoneTeachableLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 100, SPECIES_NONE},
                                {EVO_ITEM, ITEM_MOOMOO_MILK, SPECIES_NONE}),
        //.formSpeciesIdTable = sNoneFormSpeciesIdTable,
        //.formChangeTable = sNoneFormChangeTable,
        //.perfectIVCount = NUM_STATS,
    },
    */
};

const struct EggData gEggDatas[EGG_ID_COUNT] =
{
#include "egg_data.h"
};
