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

static const struct LevelUpMove sGhepioLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_PECK),
    LEVEL_UP_MOVE( 1, MOVE_GROWL),
    LEVEL_UP_MOVE( 4, MOVE_QUICK_ATTACK),
    LEVEL_UP_MOVE( 7, MOVE_LEER),
    LEVEL_UP_MOVE(10, MOVE_WING_ATTACK),
    LEVEL_UP_MOVE(13, MOVE_FOCUS_ENERGY),
    LEVEL_UP_MOVE(16, MOVE_AERIAL_ACE),
    LEVEL_UP_MOVE(20, MOVE_AGILITY),
    LEVEL_UP_MOVE(24, MOVE_TAILWIND),
    LEVEL_UP_END
};

static const struct LevelUpMove sTinuncolLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_PECK),
    LEVEL_UP_MOVE( 1, MOVE_GROWL),
    LEVEL_UP_MOVE( 1, MOVE_QUICK_ATTACK),
    LEVEL_UP_MOVE( 1, MOVE_LEER),
    LEVEL_UP_MOVE(10, MOVE_WING_ATTACK),
    LEVEL_UP_MOVE(13, MOVE_FOCUS_ENERGY),
    LEVEL_UP_MOVE(16, MOVE_AERIAL_ACE),
    LEVEL_UP_MOVE(20, MOVE_AGILITY),
    LEVEL_UP_MOVE(24, MOVE_TAILWIND),
    LEVEL_UP_MOVE(28, MOVE_DETECT),
    LEVEL_UP_MOVE(32, MOVE_ACROBATICS),
    LEVEL_UP_END
};

static const struct LevelUpMove sPeregrinusLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_PECK),
    LEVEL_UP_MOVE( 1, MOVE_GROWL),
    LEVEL_UP_MOVE( 1, MOVE_QUICK_ATTACK),
    LEVEL_UP_MOVE( 1, MOVE_LEER),
    LEVEL_UP_MOVE(10, MOVE_WING_ATTACK),
    LEVEL_UP_MOVE(13, MOVE_FOCUS_ENERGY),
    LEVEL_UP_MOVE(16, MOVE_AERIAL_ACE),
    LEVEL_UP_MOVE(20, MOVE_AGILITY),
    LEVEL_UP_MOVE(24, MOVE_TAILWIND),
    LEVEL_UP_MOVE(28, MOVE_DETECT),
    LEVEL_UP_MOVE(32, MOVE_ACROBATICS),
    LEVEL_UP_MOVE(34, MOVE_CLOSE_COMBAT),
    LEVEL_UP_MOVE(38, MOVE_ROOST),
    LEVEL_UP_MOVE(42, MOVE_DUAL_WINGBEAT),
    LEVEL_UP_MOVE(46, MOVE_BRAVE_BIRD),
    LEVEL_UP_MOVE(50, MOVE_QUICK_GUARD),
    LEVEL_UP_END
};

static const struct LevelUpMove sGazzuolaLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_PECK),
    LEVEL_UP_MOVE( 1, MOVE_GROWL),
    LEVEL_UP_MOVE( 4, MOVE_COVET),
    LEVEL_UP_MOVE( 7, MOVE_QUICK_ATTACK),
    LEVEL_UP_MOVE(10, MOVE_PLUCK),
    LEVEL_UP_MOVE(13, MOVE_THIEF),
    LEVEL_UP_MOVE(16, MOVE_ASSURANCE),
    LEVEL_UP_MOVE(18, MOVE_AERIAL_ACE),
    LEVEL_UP_MOVE(22, MOVE_TAUNT),
    LEVEL_UP_MOVE(26, MOVE_AGILITY),
    LEVEL_UP_END
};

static const struct LevelUpMove sBrillazzaLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_PECK),
    LEVEL_UP_MOVE( 1, MOVE_GROWL),
    LEVEL_UP_MOVE( 1, MOVE_COVET),
    LEVEL_UP_MOVE( 1, MOVE_QUICK_ATTACK),
    LEVEL_UP_MOVE(10, MOVE_PLUCK),
    LEVEL_UP_MOVE(13, MOVE_THIEF),
    LEVEL_UP_MOVE(16, MOVE_ASSURANCE),
    LEVEL_UP_MOVE(18, MOVE_AERIAL_ACE),
    LEVEL_UP_MOVE(22, MOVE_TAUNT),
    LEVEL_UP_MOVE(26, MOVE_AGILITY),
    LEVEL_UP_MOVE(30, MOVE_KNOCK_OFF),
    LEVEL_UP_MOVE(34, MOVE_TAILWIND),
    LEVEL_UP_END
};

static const struct LevelUpMove sGazzombraLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_PECK),
    LEVEL_UP_MOVE( 1, MOVE_GROWL),
    LEVEL_UP_MOVE( 1, MOVE_COVET),
    LEVEL_UP_MOVE( 1, MOVE_QUICK_ATTACK),
    LEVEL_UP_MOVE(10, MOVE_PLUCK),
    LEVEL_UP_MOVE(13, MOVE_THIEF),
    LEVEL_UP_MOVE(16, MOVE_ASSURANCE),
    LEVEL_UP_MOVE(18, MOVE_AERIAL_ACE),
    LEVEL_UP_MOVE(22, MOVE_TAUNT),
    LEVEL_UP_MOVE(26, MOVE_AGILITY),
    LEVEL_UP_MOVE(30, MOVE_KNOCK_OFF),
    LEVEL_UP_MOVE(34, MOVE_FOUL_PLAY),
    LEVEL_UP_MOVE(38, MOVE_U_TURN),
    LEVEL_UP_MOVE(42, MOVE_ROOST),
    LEVEL_UP_MOVE(46, MOVE_BRAVE_BIRD),
    LEVEL_UP_MOVE(50, MOVE_SWITCHEROO),
    LEVEL_UP_END
};

static const struct LevelUpMove sMolospsyLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_TACKLE),
    LEVEL_UP_MOVE( 1, MOVE_LEER),
    LEVEL_UP_MOVE( 4, MOVE_MEDITATE),
    LEVEL_UP_MOVE( 7, MOVE_LOW_KICK),
    LEVEL_UP_MOVE(10, MOVE_PROTECT),
    LEVEL_UP_MOVE(13, MOVE_CONFUSION),
    LEVEL_UP_MOVE(16, MOVE_DETECT),
    LEVEL_UP_MOVE(20, MOVE_HELPING_HAND),
    LEVEL_UP_MOVE(24, MOVE_PSYBEAM),
    LEVEL_UP_MOVE(28, MOVE_BULK_UP),
    LEVEL_UP_MOVE(32, MOVE_SAFEGUARD),
    LEVEL_UP_MOVE(36, MOVE_FORCE_PALM),
    LEVEL_UP_MOVE(40, MOVE_ZEN_HEADBUTT),
    LEVEL_UP_MOVE(44, MOVE_IRON_DEFENSE),
    LEVEL_UP_MOVE(48, MOVE_CALM_MIND),
    LEVEL_UP_MOVE(52, MOVE_WIDE_GUARD),
    LEVEL_UP_END
};

static const struct LevelUpMove sLenghelisLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_ASTONISH),
    LEVEL_UP_MOVE( 1, MOVE_TAIL_WHIP),
    LEVEL_UP_MOVE( 4, MOVE_BABY_DOLL_EYES),
    LEVEL_UP_MOVE( 7, MOVE_FAIRY_WIND),
    LEVEL_UP_MOVE(10, MOVE_CONFUSE_RAY),
    LEVEL_UP_MOVE(13, MOVE_QUICK_ATTACK),
    LEVEL_UP_MOVE(16, MOVE_NIGHT_SHADE),
    LEVEL_UP_MOVE(20, MOVE_DISARMING_VOICE),
    LEVEL_UP_MOVE(24, MOVE_WILL_O_WISP),
    LEVEL_UP_MOVE(28, MOVE_HEX),
    LEVEL_UP_MOVE(32, MOVE_MOONLIGHT),
    LEVEL_UP_MOVE(36, MOVE_SWIFT),
    LEVEL_UP_MOVE(40, MOVE_SHADOW_BALL),
    LEVEL_UP_MOVE(44, MOVE_DAZZLING_GLEAM),
    LEVEL_UP_MOVE(48, MOVE_MYSTICAL_FIRE),
    LEVEL_UP_MOVE(52, MOVE_MOONBLAST),
    LEVEL_UP_END
};

static const struct LevelUpMove sTritinoLevelUpLearnset[] = {
    LEVEL_UP_MOVE(1, MOVE_WATER_GUN), LEVEL_UP_MOVE(1, MOVE_TAIL_WHIP),
    LEVEL_UP_MOVE(4, MOVE_SUPERSONIC), LEVEL_UP_MOVE(7, MOVE_WATER_PULSE),
    LEVEL_UP_MOVE(10, MOVE_CONFUSION), LEVEL_UP_MOVE(13, MOVE_BUBBLE_BEAM),
    LEVEL_UP_MOVE(16, MOVE_AQUA_RING), LEVEL_UP_MOVE(20, MOVE_DRAGON_BREATH),
    LEVEL_UP_MOVE(24, MOVE_PSYBEAM), LEVEL_UP_MOVE(28, MOVE_SURF),
    LEVEL_UP_MOVE(32, MOVE_DRAGON_PULSE), LEVEL_UP_MOVE(36, MOVE_HYDRO_PUMP),
    LEVEL_UP_END
};

static const struct LevelUpMove sTricrestLevelUpLearnset[] = {
    LEVEL_UP_MOVE(1, MOVE_WATER_GUN), LEVEL_UP_MOVE(1, MOVE_TAIL_WHIP),
    LEVEL_UP_MOVE(1, MOVE_SUPERSONIC), LEVEL_UP_MOVE(1, MOVE_WATER_PULSE),
    LEVEL_UP_MOVE(10, MOVE_CONFUSION), LEVEL_UP_MOVE(13, MOVE_BUBBLE_BEAM),
    LEVEL_UP_MOVE(16, MOVE_AQUA_RING), LEVEL_UP_MOVE(20, MOVE_DRAGON_BREATH),
    LEVEL_UP_MOVE(24, MOVE_PSYBEAM), LEVEL_UP_MOVE(28, MOVE_SURF),
    LEVEL_UP_MOVE(32, MOVE_DRAGON_PULSE), LEVEL_UP_MOVE(36, MOVE_ICE_BEAM),
    LEVEL_UP_MOVE(42, MOVE_HYDRO_PUMP), LEVEL_UP_MOVE(48, MOVE_OUTRAGE),
    LEVEL_UP_END
};

static const struct LevelUpMove sSalampollaLevelUpLearnset[] = {
    LEVEL_UP_MOVE(1, MOVE_POISON_STING), LEVEL_UP_MOVE(1, MOVE_GROWL),
    LEVEL_UP_MOVE(4, MOVE_ABSORB), LEVEL_UP_MOVE(7, MOVE_ACID),
    LEVEL_UP_MOVE(10, MOVE_STUN_SPORE), LEVEL_UP_MOVE(13, MOVE_VENOSHOCK),
    LEVEL_UP_MOVE(16, MOVE_CONFUSE_RAY), LEVEL_UP_MOVE(20, MOVE_GIGA_DRAIN),
    LEVEL_UP_MOVE(24, MOVE_TOXIC), LEVEL_UP_MOVE(28, MOVE_MOONLIGHT),
    LEVEL_UP_MOVE(32, MOVE_SLUDGE_BOMB), LEVEL_UP_MOVE(36, MOVE_HEAL_BELL),
    LEVEL_UP_END
};

static const struct LevelUpMove sAlchimandraLevelUpLearnset[] = {
    LEVEL_UP_MOVE(1, MOVE_POISON_STING), LEVEL_UP_MOVE(1, MOVE_GROWL),
    LEVEL_UP_MOVE(1, MOVE_ABSORB), LEVEL_UP_MOVE(1, MOVE_ACID),
    LEVEL_UP_MOVE(10, MOVE_STUN_SPORE), LEVEL_UP_MOVE(13, MOVE_VENOSHOCK),
    LEVEL_UP_MOVE(16, MOVE_CONFUSE_RAY), LEVEL_UP_MOVE(20, MOVE_GIGA_DRAIN),
    LEVEL_UP_MOVE(24, MOVE_TOXIC), LEVEL_UP_MOVE(28, MOVE_MOONLIGHT),
    LEVEL_UP_MOVE(32, MOVE_DAZZLING_GLEAM), LEVEL_UP_MOVE(36, MOVE_SLUDGE_WAVE),
    LEVEL_UP_MOVE(42, MOVE_HEAL_BELL), LEVEL_UP_MOVE(48, MOVE_TOXIC_SPIKES),
    LEVEL_UP_END
};

static const struct LevelUpMove sCisternideLevelUpLearnset[] = {
    LEVEL_UP_MOVE(1, MOVE_WATER_GUN), LEVEL_UP_MOVE(1, MOVE_WITHDRAW),
    LEVEL_UP_MOVE(5, MOVE_HARDEN), LEVEL_UP_MOVE(9, MOVE_WATER_PULSE),
    LEVEL_UP_MOVE(13, MOVE_AQUA_RING), LEVEL_UP_MOVE(17, MOVE_ANCIENT_POWER),
    LEVEL_UP_MOVE(21, MOVE_BRINE), LEVEL_UP_MOVE(25, MOVE_PROTECT),
    LEVEL_UP_MOVE(29, MOVE_SURF), LEVEL_UP_MOVE(33, MOVE_POWER_GEM), LEVEL_UP_END
};
static const struct LevelUpMove sCalcisternLevelUpLearnset[] = {
    LEVEL_UP_MOVE(1, MOVE_WATER_GUN), LEVEL_UP_MOVE(1, MOVE_WITHDRAW),
    LEVEL_UP_MOVE(1, MOVE_HARDEN), LEVEL_UP_MOVE(1, MOVE_WATER_PULSE),
    LEVEL_UP_MOVE(30, MOVE_ANCIENT_POWER), LEVEL_UP_MOVE(34, MOVE_AQUA_RING),
    LEVEL_UP_MOVE(38, MOVE_BRINE), LEVEL_UP_MOVE(42, MOVE_POWER_GEM),
    LEVEL_UP_MOVE(46, MOVE_PROTECT), LEVEL_UP_MOVE(50, MOVE_SURF), LEVEL_UP_END
};

static const struct LevelUpMove sLuscincoLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_TACKLE),
    LEVEL_UP_MOVE( 1, MOVE_TAIL_WHIP),
    LEVEL_UP_MOVE( 4, MOVE_ABSORB),
    LEVEL_UP_MOVE( 7, MOVE_QUICK_ATTACK),
    LEVEL_UP_MOVE(10, MOVE_GROWTH),
    LEVEL_UP_MOVE(13, MOVE_BITE),
    LEVEL_UP_MOVE(16, MOVE_MEGA_DRAIN),
    LEVEL_UP_MOVE(20, MOVE_GLARE),
    LEVEL_UP_MOVE(28, MOVE_LEAF_BLADE),
    LEVEL_UP_MOVE(32, MOVE_SLAM),
    LEVEL_UP_MOVE(36, MOVE_GIGA_DRAIN),
    LEVEL_UP_MOVE(40, MOVE_SYNTHESIS),
    LEVEL_UP_MOVE(44, MOVE_CRUNCH),
    LEVEL_UP_END
};

static const struct LevelUpMove sLuscerpLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_TACKLE),
    LEVEL_UP_MOVE( 1, MOVE_TAIL_WHIP),
    LEVEL_UP_MOVE( 4, MOVE_ABSORB),
    LEVEL_UP_MOVE( 7, MOVE_QUICK_ATTACK),
    LEVEL_UP_MOVE(10, MOVE_GROWTH),
    LEVEL_UP_MOVE(13, MOVE_BITE),
    LEVEL_UP_MOVE(16, MOVE_MEGA_DRAIN),
    LEVEL_UP_MOVE(20, MOVE_GLARE),
    LEVEL_UP_MOVE(24, MOVE_DRAGON_TAIL),
    LEVEL_UP_MOVE(28, MOVE_LEAF_BLADE),
    LEVEL_UP_MOVE(32, MOVE_COIL),
    LEVEL_UP_MOVE(36, MOVE_CRUNCH),
    LEVEL_UP_MOVE(40, MOVE_SYNTHESIS),
    LEVEL_UP_MOVE(44, MOVE_DRAGON_CLAW),
    LEVEL_UP_MOVE(48, MOVE_GIGA_DRAIN),
    LEVEL_UP_MOVE(52, MOVE_OUTRAGE),
    LEVEL_UP_END
};

static const struct LevelUpMove sLumellaLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_ABSORB), LEVEL_UP_MOVE( 1, MOVE_STUN_SPORE),
    LEVEL_UP_MOVE( 4, MOVE_ASTONISH), LEVEL_UP_MOVE( 7, MOVE_GROWTH),
    LEVEL_UP_MOVE(10, MOVE_MEGA_DRAIN), LEVEL_UP_MOVE(13, MOVE_CONFUSE_RAY),
    LEVEL_UP_MOVE(16, MOVE_SHOCK_WAVE), LEVEL_UP_MOVE(19, MOVE_SLEEP_POWDER),
    LEVEL_UP_MOVE(26, MOVE_GIGA_DRAIN), LEVEL_UP_MOVE(30, MOVE_MOONLIGHT),
    LEVEL_UP_MOVE(34, MOVE_ENERGY_BALL), LEVEL_UP_MOVE(38, MOVE_SYNTHESIS),
    LEVEL_UP_MOVE(42, MOVE_SPORE), LEVEL_UP_END
};

static const struct LevelUpMove sOmphaluxLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_ABSORB), LEVEL_UP_MOVE( 1, MOVE_STUN_SPORE),
    LEVEL_UP_MOVE( 4, MOVE_ASTONISH), LEVEL_UP_MOVE( 7, MOVE_GROWTH),
    LEVEL_UP_MOVE(10, MOVE_MEGA_DRAIN), LEVEL_UP_MOVE(13, MOVE_CONFUSE_RAY),
    LEVEL_UP_MOVE(16, MOVE_SHOCK_WAVE), LEVEL_UP_MOVE(19, MOVE_SLEEP_POWDER),
    LEVEL_UP_MOVE(22, MOVE_CHARGE_BEAM), LEVEL_UP_MOVE(26, MOVE_GIGA_DRAIN),
    LEVEL_UP_MOVE(30, MOVE_THUNDER_WAVE), LEVEL_UP_MOVE(34, MOVE_DISCHARGE),
    LEVEL_UP_MOVE(38, MOVE_SYNTHESIS), LEVEL_UP_MOVE(42, MOVE_ENERGY_BALL),
    LEVEL_UP_MOVE(46, MOVE_THUNDERBOLT), LEVEL_UP_MOVE(50, MOVE_SPORE),
    LEVEL_UP_END
};

static const struct LevelUpMove sPaludixLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_WATER_GUN), LEVEL_UP_MOVE( 1, MOVE_STRING_SHOT),
    LEVEL_UP_MOVE( 4, MOVE_INFESTATION), LEVEL_UP_MOVE( 7, MOVE_SUPERSONIC),
    LEVEL_UP_MOVE(10, MOVE_AQUA_JET), LEVEL_UP_MOVE(13, MOVE_STRUGGLE_BUG),
    LEVEL_UP_MOVE(16, MOVE_ABSORB), LEVEL_UP_MOVE(22, MOVE_BUBBLE_BEAM),
    LEVEL_UP_MOVE(26, MOVE_AQUA_RING), LEVEL_UP_MOVE(30, MOVE_BUG_BUZZ),
    LEVEL_UP_MOVE(34, MOVE_WATER_PULSE), LEVEL_UP_MOVE(38, MOVE_GASTRO_ACID),
    LEVEL_UP_MOVE(42, MOVE_HYDRO_PUMP), LEVEL_UP_END
};

static const struct LevelUpMove sSanguilexLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_WATER_GUN), LEVEL_UP_MOVE( 1, MOVE_STRING_SHOT),
    LEVEL_UP_MOVE( 4, MOVE_INFESTATION), LEVEL_UP_MOVE( 7, MOVE_SUPERSONIC),
    LEVEL_UP_MOVE(10, MOVE_AQUA_JET), LEVEL_UP_MOVE(13, MOVE_STRUGGLE_BUG),
    LEVEL_UP_MOVE(16, MOVE_ABSORB), LEVEL_UP_MOVE(18, MOVE_POISON_FANG),
    LEVEL_UP_MOVE(22, MOVE_BUG_BITE), LEVEL_UP_MOVE(26, MOVE_LEECH_LIFE),
    LEVEL_UP_MOVE(30, MOVE_AGILITY), LEVEL_UP_MOVE(34, MOVE_CROSS_POISON),
    LEVEL_UP_MOVE(38, MOVE_U_TURN), LEVEL_UP_MOVE(42, MOVE_TOXIC),
    LEVEL_UP_MOVE(46, MOVE_LUNGE), LEVEL_UP_MOVE(50, MOVE_GUNK_SHOT),
    LEVEL_UP_END
};

static const struct LevelUpMove sCarpulusLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_TACKLE), LEVEL_UP_MOVE( 1, MOVE_TAIL_WHIP),
    LEVEL_UP_MOVE( 4, MOVE_WATER_GUN), LEVEL_UP_MOVE( 7, MOVE_FLAIL),
    LEVEL_UP_MOVE(10, MOVE_AQUA_JET), LEVEL_UP_MOVE(13, MOVE_HARDEN),
    LEVEL_UP_MOVE(16, MOVE_BITE), LEVEL_UP_MOVE(20, MOVE_AQUA_RING),
    LEVEL_UP_MOVE(24, MOVE_TAKE_DOWN), LEVEL_UP_MOVE(28, MOVE_WATER_PULSE),
    LEVEL_UP_MOVE(32, MOVE_AGILITY), LEVEL_UP_MOVE(36, MOVE_AQUA_TAIL),
    LEVEL_UP_MOVE(40, MOVE_DOUBLE_EDGE), LEVEL_UP_MOVE(44, MOVE_HYDRO_PUMP),
    LEVEL_UP_END
};

static const struct LevelUpMove sLucinusLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_TACKLE), LEVEL_UP_MOVE( 1, MOVE_LEER),
    LEVEL_UP_MOVE( 4, MOVE_BITE), LEVEL_UP_MOVE( 8, MOVE_AQUA_JET),
    LEVEL_UP_MOVE(12, MOVE_FOCUS_ENERGY), LEVEL_UP_MOVE(16, MOVE_ICE_FANG),
    LEVEL_UP_MOVE(20, MOVE_ASSURANCE), LEVEL_UP_MOVE(24, MOVE_WATER_PULSE),
    LEVEL_UP_MOVE(28, MOVE_CRUNCH), LEVEL_UP_MOVE(32, MOVE_AGILITY),
    LEVEL_UP_MOVE(36, MOVE_LIQUIDATION), LEVEL_UP_MOVE(40, MOVE_NIGHT_SLASH),
    LEVEL_UP_MOVE(44, MOVE_PSYCHIC_FANGS), LEVEL_UP_MOVE(48, MOVE_HYDRO_PUMP),
    LEVEL_UP_END
};

static const struct LevelUpMove sNaufragusLevelUpLearnset[] = {
    LEVEL_UP_MOVE( 1, MOVE_TACKLE), LEVEL_UP_MOVE( 1, MOVE_HARDEN),
    LEVEL_UP_MOVE( 5, MOVE_WATER_GUN), LEVEL_UP_MOVE( 9, MOVE_METAL_CLAW),
    LEVEL_UP_MOVE(13, MOVE_PROTECT), LEVEL_UP_MOVE(17, MOVE_AQUA_JET),
    LEVEL_UP_MOVE(21, MOVE_IRON_DEFENSE), LEVEL_UP_MOVE(25, MOVE_BRINE),
    LEVEL_UP_MOVE(29, MOVE_ANCIENT_POWER), LEVEL_UP_MOVE(33, MOVE_AQUA_TAIL),
    LEVEL_UP_MOVE(37, MOVE_IRON_HEAD), LEVEL_UP_MOVE(41, MOVE_HEAVY_SLAM),
    LEVEL_UP_MOVE(45, MOVE_RAIN_DANCE), LEVEL_UP_MOVE(49, MOVE_HYDRO_PUMP),
    LEVEL_UP_MOVE(53, MOVE_GYRO_BALL),
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

    [SPECIES_GHEPIO] =
    {
        .baseHP        = 40,
        .baseAttack    = 45,
        .baseDefense   = 35,
        .baseSpeed     = 70,
        .baseSpAttack  = 30,
        .baseSpDefense = 35,
        .types = MON_TYPES(TYPE_FLYING),
        .catchRate = 255,
        .expYield = 56,
        .evYield_Speed = 1,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FLYING),
        .abilities = { ABILITY_KEEN_EYE, ABILITY_BIG_PECKS, ABILITY_RECKLESS },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Ghepio"),
        .cryId = CRY_FLETCHLING,
        .natDexNum = NATIONAL_DEX_GHEPIO,
        .categoryName = _("FALCHETTO"),
        .height = 3,
        .weight = 21,
        .description = COMPOUND_STRING(
            "Rimane sospeso controvento scrutando\n"
            "i prati dall’alto. Quando individua una\n"
            "preda, si lascia cadere all’improvviso\n"
            "con sorprendente precisione."),
        .pokemonScale = 530,
        .pokemonOffset = 13,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Ghepio,
        .frontPicSize = MON_COORDS_SIZE(64, 56),
        .frontPicYOffset = 4,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_V_JUMPS_SMALL,
        .backPic = gMonBackPic_Ghepio,
        .backPicSize = MON_COORDS_SIZE(48, 56),
        .backPicYOffset = 4,
        .backAnimId = BACK_ANIM_TRIANGLE_DOWN,
        .palette = gMonPalette_Ghepio,
        .shinyPalette = gMonShinyPalette_Ghepio,
        .iconSprite = gMonIcon_Ghepio,
        .iconPalIndex = 3,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-2, 0, SHADOW_SIZE_S)
        FOOTPRINT(Fletchling)
        OVERWORLD(
            sPicTable_Fletchling,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Fletchling,
            gShinyOverworldPalette_Fletchling
        )
        .levelUpLearnset = sGhepioLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sGhepioTeachableLearnset,
        .eggMoveLearnset = sGhepioEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 16, SPECIES_TINUNCOL}),
    },

    [SPECIES_TINUNCOL] =
    {
        .baseHP        = 55,
        .baseAttack    = 65,
        .baseDefense   = 50,
        .baseSpeed     = 90,
        .baseSpAttack  = 40,
        .baseSpDefense = 50,
        .types = MON_TYPES(TYPE_FLYING),
        .catchRate = 120,
        .expYield = 113,
        .evYield_Speed = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FLYING),
        .abilities = { ABILITY_KEEN_EYE, ABILITY_BIG_PECKS, ABILITY_RECKLESS },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Tinuncol"),
        .cryId = CRY_FLETCHINDER,
        .natDexNum = NATIONAL_DEX_TINUNCOL,
        .categoryName = _("GHEPPIO"),
        .height = 6,
        .weight = 72,
        .description = COMPOUND_STRING(
            "Studia le correnti ascensionali per ore\n"
            "prima di attaccare. Sa correggere la\n"
            "traiettoria in volo con movimenti quasi\n"
            "impercettibili delle ali."),
        .pokemonScale = 365,
        .pokemonOffset = 12,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Tinuncol,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 2,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_V_SLIDE_SLOW,
        .enemyMonElevation = 9,
        .backPic = gMonBackPic_Tinuncol,
        .backPicSize = MON_COORDS_SIZE(56, 64),
        .backPicYOffset = 2,
        .backAnimId = BACK_ANIM_CONCAVE_ARC_SMALL,
        .palette = gMonPalette_Tinuncol,
        .shinyPalette = gMonShinyPalette_Tinuncol,
        .iconSprite = gMonIcon_Tinuncol,
        .iconPalIndex = 3,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 12, SHADOW_SIZE_S)
        FOOTPRINT(Fletchinder)
        OVERWORLD(
            sPicTable_Fletchinder,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Fletchinder,
            gShinyOverworldPalette_Fletchinder
        )
        .levelUpLearnset = sTinuncolLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sTinuncolTeachableLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 34, SPECIES_PEREGRINUS}),
    },

    [SPECIES_PEREGRINUS] =
    {
        .baseHP        = 75,
        .baseAttack    = 110,
        .baseDefense   = 70,
        .baseSpeed     = 120,
        .baseSpAttack  = 55,
        .baseSpDefense = 70,
        .types = MON_TYPES(TYPE_FLYING, TYPE_FIGHTING),
        .catchRate = 45,
        .expYield = 177,
        .evYield_Speed = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FLYING),
        .abilities = { ABILITY_KEEN_EYE, ABILITY_DEFIANT, ABILITY_RECKLESS },
        .bodyColor = BODY_COLOR_GRAY,
        .speciesName = _("Peregrinus"),
        .cryId = CRY_TALONFLAME,
        .natDexNum = NATIONAL_DEX_PEREGRINUS,
        .categoryName = _("PICCHIATA"),
        .height = 11,
        .weight = 234,
        .description = COMPOUND_STRING(
            "In picchiata concentra tutto il peso del\n"
            "corpo in un solo impatto. Può cambiare\n"
            "direzione pochi istanti prima di colpire\n"
            "senza perdere velocità."),
        .pokemonScale = 282,
        .pokemonOffset = 4,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Peregrinus,
        .frontPicSize = MON_COORDS_SIZE(64, 40),
        .frontPicYOffset = 12,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_V_SLIDE_WOBBLE,
        .enemyMonElevation = 7,
        .backPic = gMonBackPic_Peregrinus,
        .backPicSize = MON_COORDS_SIZE(56, 56),
        .backPicYOffset = 4,
        .backAnimId = BACK_ANIM_SHRINK_GROW_VIBRATE,
        .palette = gMonPalette_Peregrinus,
        .shinyPalette = gMonShinyPalette_Peregrinus,
        .iconSprite = gMonIcon_Peregrinus,
        .iconPalIndex = 3,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-2, 17, SHADOW_SIZE_M)
        FOOTPRINT(Talonflame)
        OVERWORLD(
            sPicTable_Talonflame,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Talonflame,
            gShinyOverworldPalette_Talonflame
        )
        .levelUpLearnset = sPeregrinusLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sPeregrinusTeachableLearnset,
    },

    [SPECIES_GAZZUOLA] =
    {
        .baseHP        = 45,
        .baseAttack    = 40,
        .baseDefense   = 40,
        .baseSpeed     = 60,
        .baseSpAttack  = 35,
        .baseSpDefense = 40,
        .types = MON_TYPES(TYPE_NORMAL, TYPE_FLYING),
        .catchRate = 255,
        .expYield = 56,
        .evYield_Speed = 1,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FLYING),
        .abilities = { ABILITY_PICKUP, ABILITY_KEEN_EYE, ABILITY_SUPER_LUCK },
        .bodyColor = BODY_COLOR_BLACK,
        .speciesName = _("Gazzuola"),
        .cryId = CRY_ROOKIDEE,
        .natDexNum = NATIONAL_DEX_GAZZUOLA,
        .categoryName = _("CURIOSA"),
        .height = 3,
        .weight = 19,
        .description = COMPOUND_STRING(
            "Raccoglie frammenti di vetro e metallo\n"
            "che brillano al sole e li nasconde tra\n"
            "i rami. Ricorda a lungo il luogo di\n"
            "ogni piccolo tesoro."),
        .pokemonScale = 682,
        .pokemonOffset = 24,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Gazzuola,
        .frontPicSize = MON_COORDS_SIZE(40, 40),
        .frontPicYOffset = 12,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_H_JUMPS,
        .backPic = gMonBackPic_Gazzuola,
        .backPicSize = MON_COORDS_SIZE(32, 40),
        .backPicYOffset = 12,
        .palette = gMonPalette_Gazzuola,
        .shinyPalette = gMonShinyPalette_Gazzuola,
        .iconSprite = gMonIcon_Gazzuola,
        .iconPalIndex = 6,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-1, -3, SHADOW_SIZE_S)
        FOOTPRINT(Rookidee)
        OVERWORLD(
            sPicTable_Rookidee,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Rookidee,
            gShinyOverworldPalette_Rookidee
        )
        .levelUpLearnset = sGazzuolaLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sGazzuolaTeachableLearnset,
        .eggMoveLearnset = sGazzuolaEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 18, SPECIES_BRILLAZZA}),
    },

    [SPECIES_BRILLAZZA] =
    {
        .baseHP        = 60,
        .baseAttack    = 65,
        .baseDefense   = 55,
        .baseSpeed     = 80,
        .baseSpAttack  = 45,
        .baseSpDefense = 55,
        .types = MON_TYPES(TYPE_DARK, TYPE_FLYING),
        .catchRate = 120,
        .expYield = 116,
        .evYield_Attack = 1,
        .evYield_Speed = 1,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FLYING),
        .abilities = { ABILITY_PICKUP, ABILITY_FRISK, ABILITY_SUPER_LUCK },
        .bodyColor = BODY_COLOR_BLACK,
        .speciesName = _("Brillazza"),
        .cryId = CRY_CORVISQUIRE,
        .natDexNum = NATIONAL_DEX_BRILLAZZA,
        .categoryName = _("MONILE"),
        .height = 6,
        .weight = 55,
        .description = COMPOUND_STRING(
            "Sa distinguere a colpo d'occhio ciò\n"
            "che ha davvero valore. Se un oggetto\n"
            "attira la sua attenzione, studia per\n"
            "giorni il momento migliore per prenderlo."),
        .pokemonScale = 366,
        .pokemonOffset = 7,
        .trainerScale = 257,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Brillazza,
        .frontPicSize = MON_COORDS_SIZE(40, 40),
        .frontPicYOffset = 12,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_CIRCLE_INTO_BG,
        .enemyMonElevation = 10,
        .backPic = gMonBackPic_Brillazza,
        .backPicSize = MON_COORDS_SIZE(32, 40),
        .backPicYOffset = 12,
        .palette = gMonPalette_Brillazza,
        .shinyPalette = gMonShinyPalette_Brillazza,
        .iconSprite = gMonIcon_Brillazza,
        .iconPalIndex = 6,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(2, 16, SHADOW_SIZE_S)
        FOOTPRINT(Corvisquire)
        OVERWORLD(
            sPicTable_Corvisquire,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Corvisquire,
            gShinyOverworldPalette_Corvisquire
        )
        .levelUpLearnset = sBrillazzaLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sBrillazzaTeachableLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 34, SPECIES_GAZZOMBRA}),
    },

    [SPECIES_GAZZOMBRA] =
    {
        .baseHP        = 75,
        .baseAttack    = 95,
        .baseDefense   = 70,
        .baseSpeed     = 100,
        .baseSpAttack  = 55,
        .baseSpDefense = 75,
        .types = MON_TYPES(TYPE_DARK, TYPE_FLYING),
        .catchRate = 60,
        .expYield = 170,
        .evYield_Attack = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FLYING),
        .abilities = { ABILITY_FRISK, ABILITY_PICKPOCKET, ABILITY_SUPER_LUCK },
        .bodyColor = BODY_COLOR_BLACK,
        .speciesName = _("Gazzombra"),
        .cryId = CRY_CORVIKNIGHT,
        .natDexNum = NATIONAL_DEX_GAZZOMBRA,
        .categoryName = _("BOTTINO"),
        .height = 9,
        .weight = 98,
        .description = COMPOUND_STRING(
            "Osserva silenziosa i passanti dai tetti\n"
            "e sceglie solo oggetti che considera\n"
            "degni della sua collezione. Spesso lascia\n"
            "un gingillo senza valore al loro posto."),
        .pokemonScale = 256,
        .pokemonOffset = 0,
        .trainerScale = 348,
        .trainerOffset = 6,
        .frontPic = gMonFrontPic_Gazzombra,
        .frontPicSize = MON_COORDS_SIZE(48, 56),
        .frontPicYOffset = 4,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_H_STRETCH_FAR_SLOW,
        .backPic = gMonBackPic_Gazzombra,
        .backPicSize = MON_COORDS_SIZE(48, 56),
        .backPicYOffset = 4,
        .palette = gMonPalette_Gazzombra,
        .shinyPalette = gMonShinyPalette_Gazzombra,
        .iconSprite = gMonIcon_Gazzombra,
        .iconPalIndex = 6,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-1, 9, SHADOW_SIZE_L)
        FOOTPRINT(Corviknight)
        OVERWORLD(
            sPicTable_Corviknight,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Corviknight,
            gShinyOverworldPalette_Corviknight
        )
        .levelUpLearnset = sGazzombraLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sGazzombraTeachableLearnset,
    },

    [SPECIES_MOLOSPSY] =
    {
        .baseHP        = 65,
        .baseAttack    = 85,
        .baseDefense   = 80,
        .baseSpeed     = 45,
        .baseSpAttack  = 60,
        .baseSpDefense = 70,
        .types = MON_TYPES(TYPE_FIGHTING, TYPE_PSYCHIC),
        .catchRate = 90,
        .expYield = 145,
        .evYield_Attack = 1,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = 50,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD),
        .abilities = { ABILITY_INNER_FOCUS, ABILITY_STEADFAST, ABILITY_GUARD_DOG },
        .bodyColor = BODY_COLOR_GRAY,
        .speciesName = _("Molospsy"),
        .cryId = CRY_MABOSSTIFF,
        .natDexNum = NATIONAL_DEX_MOLOSPSY,
        .categoryName = _("GUARDIANO"),
        .height = 12,
        .weight = 610,
        .description = COMPOUND_STRING(
            "Molospsy sorveglia rovine e\n"
            "varchi antichi. Si dice che\n"
            "percepisca l'intenzione di chi\n"
            "si avvicina prima di muoversi."),
        .pokemonScale = 356,
        .pokemonOffset = 17,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Molospsy,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 5,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .backPic = gMonBackPic_Molospsy,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 13,
        .palette = gMonPalette_Molospsy,
        .shinyPalette = gMonShinyPalette_Molospsy,
        .iconSprite = gMonIcon_Molospsy,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(1, 5, SHADOW_SIZE_XL_BATTLE_ONLY)
        FOOTPRINT(Mabosstiff)
        OVERWORLD(
            sPicTable_Mabosstiff,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Mabosstiff,
            gShinyOverworldPalette_Mabosstiff
        )
        .levelUpLearnset = sMolospsyLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sMolospsyTeachableLearnset,
        .eggMoveLearnset = sMolospsyEggMoveLearnset,
    },

    [SPECIES_LENGHELIS] =
    {
        .baseHP        = 65,
        .baseAttack    = 50,
        .baseDefense   = 65,
        .baseSpeed     = 85,
        .baseSpAttack  = 90,
        .baseSpDefense = 85,
        .types = MON_TYPES(TYPE_GHOST, TYPE_FAIRY),
        .catchRate = 90,
        .expYield = 145,
        .evYield_SpAttack = 1,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = 70,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD, EGG_GROUP_AMORPHOUS),
        .abilities = { ABILITY_ILLUMINATE, ABILITY_FRISK, ABILITY_INFILTRATOR },
        .bodyColor = BODY_COLOR_PURPLE,
        .speciesName = _("Lenghelis"),
        .cryId = CRY_MISDREAVUS,
        .natDexNum = NATIONAL_DEX_LENGHELIS,
        .categoryName = _("SPIRITELLO"),
        .height = 6,
        .weight = 72,
        .description = COMPOUND_STRING(
            "Di notte percorre antiche strade\n"
            "con piccoli fuochi azzurri.\n"
            "Confonde chi profana i boschi,\n"
            "ma guida a casa chi si è perso."),
        .pokemonScale = 320,
        .pokemonOffset = 10,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Lenghelis,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 4,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_H_JUMPS,
        .backPic = gMonBackPic_Lenghelis,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 4,
        .backAnimId = BACK_ANIM_CONCAVE_ARC_SMALL,
        .palette = gMonPalette_Lenghelis,
        .shinyPalette = gMonShinyPalette_Lenghelis,
        .iconSprite = gMonIcon_Lenghelis,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 4, SHADOW_SIZE_M)
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
        .levelUpLearnset = sLenghelisLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sLenghelisTeachableLearnset,
        .eggMoveLearnset = sLenghelisEggMoveLearnset,
    },

    [SPECIES_LUSCINCO] =
    {
        .baseHP = 45, .baseAttack = 60, .baseDefense = 45, .baseSpeed = 70,
        .baseSpAttack = 35, .baseSpDefense = 45,
        .types = MON_TYPES(TYPE_GRASS), .catchRate = 190, .expYield = 64,
        .evYield_Speed = 1, .genderRatio = PERCENT_FEMALE(50), .eggCycles = 20,
        .friendship = 50, .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD, EGG_GROUP_DRAGON),
        .abilities = { ABILITY_CHLOROPHYLL, ABILITY_SHED_SKIN, ABILITY_REGENERATOR },
        .bodyColor = BODY_COLOR_GREEN, .speciesName = _("Luscinco"),
        .cryId = CRY_TREECKO, .natDexNum = NATIONAL_DEX_LUSCINCO,
        .categoryName = _("LUSCENGOLA"), .height = 4, .weight = 48,
        .description = COMPOUND_STRING(
            "Si scalda sui muretti delle antiche\n"
            "strade assorbendo la luce solare.\n"
            "Se viene afferrato, abbandona la\n"
            "coda e fugge rapido tra l'erba."),
        .pokemonScale = 300, .pokemonOffset = 10, .trainerScale = 256, .trainerOffset = 0,
        .frontPic = gMonFrontPic_Luscinco, .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 4, .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_H_SLIDE, .backPic = gMonBackPic_Luscinco,
        .backPicSize = MON_COORDS_SIZE(64, 64), .backPicYOffset = 4,
        .backAnimId = BACK_ANIM_H_SLIDE, .palette = gMonPalette_Luscinco,
        .shinyPalette = gMonShinyPalette_Luscinco, .iconSprite = gMonIcon_Luscinco,
        .iconPalIndex = 0, .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 4, SHADOW_SIZE_M)
        FOOTPRINT(Treecko)
        OVERWORLD(sPicTable_Treecko, SIZE_32x32, SHADOW_SIZE_M, TRACKS_FOOT,
            sAnimTable_Following, gOverworldPalette_Treecko, gShinyOverworldPalette_Treecko)
        .levelUpLearnset = sLuscincoLevelUpLearnset, .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sLuscincoTeachableLearnset, .eggMoveLearnset = sLuscincoEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 24, SPECIES_LUSCERP}),
    },

    [SPECIES_LUSCERP] =
    {
        .baseHP = 70, .baseAttack = 105, .baseDefense = 75, .baseSpeed = 100,
        .baseSpAttack = 55, .baseSpDefense = 75,
        .types = MON_TYPES(TYPE_GRASS, TYPE_DRAGON), .catchRate = 75, .expYield = 175,
        .evYield_Attack = 2, .genderRatio = PERCENT_FEMALE(50), .eggCycles = 20,
        .friendship = 50, .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD, EGG_GROUP_DRAGON),
        .abilities = { ABILITY_CHLOROPHYLL, ABILITY_SHED_SKIN, ABILITY_REGENERATOR },
        .bodyColor = BODY_COLOR_GREEN, .speciesName = _("Luscerp"),
        .cryId = CRY_SCEPTILE, .natDexNum = NATIONAL_DEX_LUSCERP,
        .categoryName = _("SAURO"), .height = 15, .weight = 280,
        .description = COMPOUND_STRING(
            "Pattuglia oliveti e sentieri con\n"
            "movimenti fulminei. Le dure scaglie\n"
            "sulla schiena conservano il calore\n"
            "accumulato durante il giorno."),
        .pokemonScale = 300, .pokemonOffset = 10, .trainerScale = 256, .trainerOffset = 0,
        .frontPic = gMonFrontPic_Luscerp, .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 4, .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_H_SLIDE, .backPic = gMonBackPic_Luscerp,
        .backPicSize = MON_COORDS_SIZE(64, 64), .backPicYOffset = 4,
        .backAnimId = BACK_ANIM_H_SLIDE, .palette = gMonPalette_Luscerp,
        .shinyPalette = gMonShinyPalette_Luscerp, .iconSprite = gMonIcon_Luscerp,
        .iconPalIndex = 0, .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 4, SHADOW_SIZE_M)
        FOOTPRINT(Sceptile)
        OVERWORLD(sPicTable_Sceptile, SIZE_32x32, SHADOW_SIZE_M, TRACKS_FOOT,
            sAnimTable_Following, gOverworldPalette_Sceptile, gShinyOverworldPalette_Sceptile)
        .levelUpLearnset = sLuscerpLevelUpLearnset, .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sLuscerpTeachableLearnset, .eggMoveLearnset = sLuscerpEggMoveLearnset,
    },

    [SPECIES_LUMELLA] =
    {
        .baseHP = 50, .baseAttack = 35, .baseDefense = 55, .baseSpeed = 35,
        .baseSpAttack = 65, .baseSpDefense = 65,
        .types = MON_TYPES(TYPE_GRASS), .catchRate = 190, .expYield = 62,
        .evYield_SpAttack = 1, .genderRatio = PERCENT_FEMALE(50), .eggCycles = 20,
        .friendship = 50, .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_GRASS, EGG_GROUP_AMORPHOUS),
        .abilities = { ABILITY_ILLUMINATE, ABILITY_EFFECT_SPORE, ABILITY_RAIN_DISH },
        .bodyColor = BODY_COLOR_BLUE, .speciesName = _("Lumella"),
        .cryId = CRY_MORELULL, .natDexNum = NATIONAL_DEX_LUMELLA,
        .categoryName = _("LUMEFUNGO"), .height = 3, .weight = 21,
        .description = COMPOUND_STRING(
            "Al calare del sole emette una luce\n"
            "tenue tra le radici degli olivi.\n"
            "Le sue spore luminose indicano dove\n"
            "il terreno conserva più umidità."),
        .pokemonScale = 256, .pokemonOffset = 0, .trainerScale = 256, .trainerOffset = 0,
        .frontPic = gMonFrontPic_Lumella, .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 4, .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_V_SQUISH_AND_BOUNCE, .backPic = gMonBackPic_Lumella,
        .backPicSize = MON_COORDS_SIZE(64, 64), .backPicYOffset = 4,
        .backAnimId = BACK_ANIM_SHRINK_GROW, .palette = gMonPalette_Lumella,
        .shinyPalette = gMonShinyPalette_Lumella, .iconSprite = gMonIcon_Lumella,
        .iconPalIndex = 0, .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 4, SHADOW_SIZE_M)
        FOOTPRINT(Morelull)
        OVERWORLD(sPicTable_Morelull, SIZE_32x32, SHADOW_SIZE_M, TRACKS_FOOT,
            sAnimTable_Following, gOverworldPalette_Morelull, gShinyOverworldPalette_Morelull)
        .levelUpLearnset = sLumellaLevelUpLearnset, .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sLumellaTeachableLearnset, .eggMoveLearnset = sLumellaEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 22, SPECIES_OMPHALUX, CONDITIONS({IF_TIME, TIME_NIGHT})}),
    },

    [SPECIES_OMPHALUX] =
    {
        .baseHP = 80, .baseAttack = 45, .baseDefense = 85, .baseSpeed = 60,
        .baseSpAttack = 115, .baseSpDefense = 100,
        .types = MON_TYPES(TYPE_GRASS, TYPE_ELECTRIC), .catchRate = 75, .expYield = 170,
        .evYield_SpAttack = 2, .genderRatio = PERCENT_FEMALE(50), .eggCycles = 20,
        .friendship = 50, .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_GRASS, EGG_GROUP_AMORPHOUS),
        .abilities = { ABILITY_ILLUMINATE, ABILITY_EFFECT_SPORE, ABILITY_LIGHTNING_ROD },
        .bodyColor = BODY_COLOR_BLUE, .speciesName = _("Omphalux"),
        .cryId = CRY_SHIINOTIC, .natDexNum = NATIONAL_DEX_OMPHALUX,
        .categoryName = _("BIOLUMINE"), .height = 9, .weight = 185,
        .description = COMPOUND_STRING(
            "Accumula energia nei disegni del\n"
            "cappello e la libera in silenziosi\n"
            "lampi. Interi oliveti risplendono\n"
            "quando molti esemplari si radunano."),
        .pokemonScale = 256, .pokemonOffset = 0, .trainerScale = 256, .trainerOffset = 0,
        .frontPic = gMonFrontPic_Omphalux, .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 4, .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_GLOW_BLACK, .backPic = gMonBackPic_Omphalux,
        .backPicSize = MON_COORDS_SIZE(64, 64), .backPicYOffset = 4,
        .backAnimId = BACK_ANIM_SHRINK_GROW, .palette = gMonPalette_Omphalux,
        .shinyPalette = gMonShinyPalette_Omphalux, .iconSprite = gMonIcon_Omphalux,
        .iconPalIndex = 0, .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 4, SHADOW_SIZE_M)
        FOOTPRINT(Shiinotic)
        OVERWORLD(sPicTable_Shiinotic, SIZE_32x32, SHADOW_SIZE_M, TRACKS_FOOT,
            sAnimTable_Following, gOverworldPalette_Shiinotic, gShinyOverworldPalette_Shiinotic)
        .levelUpLearnset = sOmphaluxLevelUpLearnset, .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sOmphaluxTeachableLearnset, .eggMoveLearnset = sOmphaluxEggMoveLearnset,
    },

    [SPECIES_PALUDIX] =
    {
        .baseHP = 45, .baseAttack = 35, .baseDefense = 45, .baseSpeed = 65,
        .baseSpAttack = 55, .baseSpDefense = 50,
        .types = MON_TYPES(TYPE_BUG, TYPE_WATER), .catchRate = 200, .expYield = 58,
        .evYield_Speed = 1, .genderRatio = PERCENT_FEMALE(50), .eggCycles = 15,
        .friendship = 50, .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_BUG, EGG_GROUP_WATER_3),
        .abilities = { ABILITY_SWIFT_SWIM, ABILITY_HYDRATION, ABILITY_RAIN_DISH },
        .bodyColor = BODY_COLOR_BLUE, .speciesName = _("Paludix"),
        .cryId = CRY_SURSKIT, .natDexNum = NATIONAL_DEX_PALUDIX,
        .categoryName = _("LARVA"), .height = 4, .weight = 32,
        .description = COMPOUND_STRING(
            "Vive nelle canalette dove l'acqua\n"
            "scorre lentamente. Filtra impurità\n"
            "con la bocca e avverte le vibrazioni\n"
            "prodotte da chi si avvicina."),
        .pokemonScale = 256, .pokemonOffset = 0, .trainerScale = 256, .trainerOffset = 0,
        .frontPic = gMonFrontPic_Paludix, .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 4, .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_V_SQUISH_AND_BOUNCE, .backPic = gMonBackPic_Paludix,
        .backPicSize = MON_COORDS_SIZE(64, 64), .backPicYOffset = 4,
        .backAnimId = BACK_ANIM_H_SLIDE, .palette = gMonPalette_Paludix,
        .shinyPalette = gMonShinyPalette_Paludix, .iconSprite = gMonIcon_Paludix,
        .iconPalIndex = 0, .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 4, SHADOW_SIZE_M)
        FOOTPRINT(Surskit)
        OVERWORLD(sPicTable_Surskit, SIZE_32x32, SHADOW_SIZE_M, TRACKS_FOOT,
            sAnimTable_Following, gOverworldPalette_Surskit, gShinyOverworldPalette_Surskit)
        .levelUpLearnset = sPaludixLevelUpLearnset, .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sPaludixTeachableLearnset, .eggMoveLearnset = sPaludixEggMoveLearnset,
        .evolutions = EVOLUTION(
            {EVO_LEVEL, 18, SPECIES_SANGUILEX, CONDITIONS({IF_TIME, TIME_EVENING})},
            {EVO_LEVEL, 18, SPECIES_SANGUILEX, CONDITIONS({IF_TIME, TIME_NIGHT})}),
    },

    [SPECIES_SANGUILEX] =
    {
        .baseHP = 65, .baseAttack = 90, .baseDefense = 60, .baseSpeed = 120,
        .baseSpAttack = 70, .baseSpDefense = 65,
        .types = MON_TYPES(TYPE_BUG, TYPE_POISON), .catchRate = 90, .expYield = 168,
        .evYield_Speed = 2, .genderRatio = PERCENT_FEMALE(50), .eggCycles = 15,
        .friendship = 50, .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_BUG, EGG_GROUP_WATER_3),
        .abilities = { ABILITY_SWARM, ABILITY_INFILTRATOR, ABILITY_POISON_TOUCH },
        .bodyColor = BODY_COLOR_PURPLE, .speciesName = _("Sanguilex"),
        .cryId = CRY_VENOMOTH, .natDexNum = NATIONAL_DEX_SANGUILEX,
        .categoryName = _("ZANZARA"), .height = 11, .weight = 190,
        .description = COMPOUND_STRING(
            "Vola senza produrre alcun rumore.\n"
            "Con il lungo rostro assorbe energia\n"
            "dalle prede e inocula un veleno che\n"
            "rallenta perfino i Pokémon più agili."),
        .pokemonScale = 256, .pokemonOffset = 0, .trainerScale = 256, .trainerOffset = 0,
        .frontPic = gMonFrontPic_Sanguilex, .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 4, .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_CIRCLE_C_CLOCKWISE_LONG, .backPic = gMonBackPic_Sanguilex,
        .backPicSize = MON_COORDS_SIZE(64, 64), .backPicYOffset = 4,
        .backAnimId = BACK_ANIM_H_SLIDE, .palette = gMonPalette_Sanguilex,
        .shinyPalette = gMonShinyPalette_Sanguilex, .iconSprite = gMonIcon_Sanguilex,
        .iconPalIndex = 0, .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 4, SHADOW_SIZE_M)
        FOOTPRINT(Masquerain)
        OVERWORLD(sPicTable_Masquerain, SIZE_32x32, SHADOW_SIZE_M, TRACKS_FOOT,
            sAnimTable_Following, gOverworldPalette_Masquerain, gShinyOverworldPalette_Masquerain)
        .levelUpLearnset = sSanguilexLevelUpLearnset, .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sSanguilexTeachableLearnset, .eggMoveLearnset = sSanguilexEggMoveLearnset,
    },

    [SPECIES_TRITINO] =
    {
        .baseHP = 45, .baseAttack = 40, .baseDefense = 45, .baseSpeed = 55,
        .baseSpAttack = 70, .baseSpDefense = 55,
        .types = MON_TYPES(TYPE_WATER), .catchRate = 190, .expYield = 64,
        .evYield_SpAttack = 1, .genderRatio = PERCENT_FEMALE(50), .eggCycles = 20,
        .friendship = 50, .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_WATER_1, EGG_GROUP_DRAGON),
        .abilities = { ABILITY_SWIFT_SWIM, ABILITY_WATER_ABSORB, ABILITY_STORM_DRAIN },
        .bodyColor = BODY_COLOR_BLUE, .speciesName = _("Tritino"),
        .cryId = CRY_TENTACOOL, .natDexNum = NATIONAL_DEX_TRITINO,
        .categoryName = _("TRITONE"), .height = 4, .weight = 38,
        .description = COMPOUND_STRING(
            "Vive sulle rive del Lago di Albèra e\n"
            "percepisce ogni variazione di pressione\n"
            "nell'acqua prima che il flusso cambi."),
        .pokemonScale = 256, .pokemonOffset = 0, .trainerScale = 256, .trainerOffset = 0,
        .frontPic = gMonFrontPic_Tritino, .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 4, .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .backPic = gMonBackPic_Tritino, .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 4, .palette = gMonPalette_Tritino,
        .shinyPalette = gMonShinyPalette_Tritino, .iconSprite = gMonIcon_Tritino,
        .iconPalIndex = 0, .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 4, SHADOW_SIZE_M)
        FOOTPRINT(Tentacool)
        OVERWORLD(sPicTable_Tentacool, SIZE_32x32, SHADOW_SIZE_M, TRACKS_FOOT,
            sAnimTable_Following, gOverworldPalette_Tentacool, gShinyOverworldPalette_Tentacool)
        .levelUpLearnset = sTritinoLevelUpLearnset, .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sTritinoTeachableLearnset, .eggMoveLearnset = sTritinoEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 27, SPECIES_TRICREST}),
    },

    [SPECIES_TRICREST] =
    {
        .baseHP = 75, .baseAttack = 60, .baseDefense = 75, .baseSpeed = 85,
        .baseSpAttack = 110, .baseSpDefense = 95,
        .types = MON_TYPES(TYPE_WATER, TYPE_DRAGON), .catchRate = 45, .expYield = 170,
        .evYield_SpAttack = 2, .genderRatio = PERCENT_FEMALE(50), .eggCycles = 20,
        .friendship = 50, .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_WATER_1, EGG_GROUP_DRAGON),
        .abilities = { ABILITY_SWIFT_SWIM, ABILITY_WATER_ABSORB, ABILITY_STORM_DRAIN },
        .bodyColor = BODY_COLOR_BLUE, .speciesName = _("Tricrest"),
        .cryId = CRY_DRATINI, .natDexNum = NATIONAL_DEX_TRICREST,
        .categoryName = _("CRESTA"), .height = 12, .weight = 185,
        .description = COMPOUND_STRING(
            "Usa la cresta per avvertire correnti e\n"
            "vibrazioni profonde che attraversano\n"
            "il lago e i suoi condotti."),
        .pokemonScale = 256, .pokemonOffset = 0, .trainerScale = 256, .trainerOffset = 0,
        .frontPic = gMonFrontPic_Tricrest, .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 4, .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .backPic = gMonBackPic_Tricrest, .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 4, .palette = gMonPalette_Tricrest,
        .shinyPalette = gMonShinyPalette_Tricrest, .iconSprite = gMonIcon_Tricrest,
        .iconPalIndex = 0, .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 4, SHADOW_SIZE_M)
        FOOTPRINT(Tentacool)
        OVERWORLD(sPicTable_Tentacool, SIZE_32x32, SHADOW_SIZE_M, TRACKS_FOOT,
            sAnimTable_Following, gOverworldPalette_Tentacool, gShinyOverworldPalette_Tentacool)
        .levelUpLearnset = sTricrestLevelUpLearnset, .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sTricrestTeachableLearnset, .eggMoveLearnset = sTricrestEggMoveLearnset,
    },

    [SPECIES_SALAMPOLLA] =
    {
        .baseHP = 40, .baseAttack = 35, .baseDefense = 40, .baseSpeed = 75,
        .baseSpAttack = 70, .baseSpDefense = 50,
        .types = MON_TYPES(TYPE_POISON), .catchRate = 190, .expYield = 64,
        .evYield_Speed = 1, .genderRatio = PERCENT_FEMALE(50), .eggCycles = 20,
        .friendship = 50, .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD, EGG_GROUP_AMORPHOUS),
        .abilities = { ABILITY_POISON_POINT, ABILITY_COMPOUND_EYES, ABILITY_CORROSION },
        .bodyColor = BODY_COLOR_PURPLE, .speciesName = _("Salampolla"),
        .cryId = CRY_SALANDIT, .natDexNum = NATIONAL_DEX_SALAMPOLLA,
        .categoryName = _("ALCHEMICA"), .height = 3, .weight = 18,
        .description = COMPOUND_STRING(
            "Raccoglie sostanze da funghi, erbe e\n"
            "acqua stagnante per preparare secrezioni\n"
            "dal profumo pungente."),
        .pokemonScale = 256, .pokemonOffset = 0, .trainerScale = 256, .trainerOffset = 0,
        .frontPic = gMonFrontPic_Salampolla, .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 4, .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .backPic = gMonBackPic_Salampolla, .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 4, .palette = gMonPalette_Salampolla,
        .shinyPalette = gMonShinyPalette_Salampolla, .iconSprite = gMonIcon_Salampolla,
        .iconPalIndex = 0, .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 4, SHADOW_SIZE_M)
        FOOTPRINT(Salandit)
        OVERWORLD(sPicTable_Salandit, SIZE_32x32, SHADOW_SIZE_M, TRACKS_FOOT,
            sAnimTable_Following, gOverworldPalette_Salandit, gShinyOverworldPalette_Salandit)
        .levelUpLearnset = sSalampollaLevelUpLearnset, .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sSalampollaTeachableLearnset, .eggMoveLearnset = sSalampollaEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 28, SPECIES_ALCHIMANDRA, CONDITIONS({IF_TIME, TIME_NIGHT})}),
    },

    [SPECIES_ALCHIMANDRA] =
    {
        .baseHP = 65, .baseAttack = 50, .baseDefense = 60, .baseSpeed = 110,
        .baseSpAttack = 105, .baseSpDefense = 80,
        .types = MON_TYPES(TYPE_POISON, TYPE_FAIRY), .catchRate = 45, .expYield = 160,
        .evYield_Speed = 2, .genderRatio = PERCENT_FEMALE(50), .eggCycles = 20,
        .friendship = 50, .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD, EGG_GROUP_AMORPHOUS),
        .abilities = { ABILITY_POISON_POINT, ABILITY_COMPOUND_EYES, ABILITY_CORROSION },
        .bodyColor = BODY_COLOR_PURPLE, .speciesName = _("Alchimandra"),
        .cryId = CRY_SALANDIT, .natDexNum = NATIONAL_DEX_ALCHIMANDRA,
        .categoryName = _("ALCHIMICA"), .height = 8, .weight = 82,
        .description = COMPOUND_STRING(
            "Combina secrezioni e sostanze vegetali\n"
            "per produrre veleni e rimedi di grande\n"
            "efficacia."),
        .pokemonScale = 256, .pokemonOffset = 0, .trainerScale = 256, .trainerOffset = 0,
        .frontPic = gMonFrontPic_Alchimandra, .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 4, .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .backPic = gMonBackPic_Alchimandra, .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 4, .palette = gMonPalette_Alchimandra,
        .shinyPalette = gMonShinyPalette_Alchimandra, .iconSprite = gMonIcon_Alchimandra,
        .iconPalIndex = 0, .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 4, SHADOW_SIZE_M)
        FOOTPRINT(Salandit)
        OVERWORLD(sPicTable_Salandit, SIZE_32x32, SHADOW_SIZE_M, TRACKS_FOOT,
            sAnimTable_Following, gOverworldPalette_Salandit, gShinyOverworldPalette_Salandit)
        .levelUpLearnset = sAlchimandraLevelUpLearnset, .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sAlchimandraTeachableLearnset, .eggMoveLearnset = sAlchimandraEggMoveLearnset,
    },

    [SPECIES_CISTERNIDE] =
    {
        .baseHP = 55, .baseAttack = 35, .baseDefense = 65, .baseSpeed = 20,
        .baseSpAttack = 50, .baseSpDefense = 75,
        .types = MON_TYPES(TYPE_WATER), .catchRate = 190, .expYield = 64,
        .evYield_SpDefense = 1, .genderRatio = PERCENT_FEMALE(50), .eggCycles = 20,
        .friendship = 50, .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_WATER_3, EGG_GROUP_MINERAL),
        .abilities = { ABILITY_WATER_ABSORB, ABILITY_PRESSURE, ABILITY_STORM_DRAIN },
        .bodyColor = BODY_COLOR_BLUE, .speciesName = _("Cisternide"),
        .cryId = CRY_TENTACOOL, .natDexNum = NATIONAL_DEX_CISTERNIDE,
        .categoryName = _("CONDOTTO"), .height = 5, .weight = 80,
        .description = COMPOUND_STRING("Percepisce pressione e vibrazioni nei\ncondotti e attraversa grate strette."),
        .pokemonScale = 256, .pokemonOffset = 0, .trainerScale = 256, .trainerOffset = 0,
        .frontPic = gMonFrontPic_Cisternide, .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 4, .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .backPic = gMonBackPic_Cisternide, .backPicSize = MON_COORDS_SIZE(64, 64), .backPicYOffset = 4,
        .palette = gMonPalette_Cisternide, .shinyPalette = gMonShinyPalette_Cisternide, .iconSprite = gMonIcon_Cisternide,
        .iconPalIndex = 0, .pokemonJumpType = PKMN_JUMP_TYPE_NONE, SHADOW(0, 4, SHADOW_SIZE_M)
        FOOTPRINT(Anorith)
        OVERWORLD(sPicTable_Anorith, SIZE_32x32, SHADOW_SIZE_M, TRACKS_FOOT, sAnimTable_Following, gOverworldPalette_Anorith, gShinyOverworldPalette_Anorith)
        .levelUpLearnset = sCisternideLevelUpLearnset, .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sCisternideTeachableLearnset, .eggMoveLearnset = sCisternideEggMoveLearnset,
        .evolutions = EVOLUTION({EVO_LEVEL, 30, SPECIES_CALCISTERN}),
    },

    [SPECIES_CALCISTERN] =
    {
        .baseHP = 90, .baseAttack = 55, .baseDefense = 105, .baseSpeed = 35,
        .baseSpAttack = 80, .baseSpDefense = 125,
        .types = MON_TYPES(TYPE_WATER, TYPE_ROCK), .catchRate = 45, .expYield = 160,
        .evYield_SpDefense = 2, .genderRatio = PERCENT_FEMALE(50), .eggCycles = 20,
        .friendship = 50, .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_WATER_3, EGG_GROUP_MINERAL),
        .abilities = { ABILITY_WATER_ABSORB, ABILITY_PRESSURE, ABILITY_SOLID_ROCK },
        .bodyColor = BODY_COLOR_GRAY, .speciesName = _("Calcistern"),
        .cryId = CRY_REGIROCK, .natDexNum = NATIONAL_DEX_CALCISTERN,
        .categoryName = _("CALCARE"), .height = 14, .weight = 420,
        .description = COMPOUND_STRING("Accumula calcare nel carapace e regola\nla pressione delle camere piu antiche."),
        .pokemonScale = 256, .pokemonOffset = 0, .trainerScale = 256, .trainerOffset = 0,
        .frontPic = gMonFrontPic_Calcistern, .frontPicSize = MON_COORDS_SIZE(64, 64), .frontPicYOffset = 4,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder, .backPic = gMonBackPic_Calcistern,
        .backPicSize = MON_COORDS_SIZE(64, 64), .backPicYOffset = 4, .palette = gMonPalette_Calcistern,
        .shinyPalette = gMonShinyPalette_Calcistern, .iconSprite = gMonIcon_Calcistern, .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE, SHADOW(0, 4, SHADOW_SIZE_L)
        FOOTPRINT(Crustle)
        OVERWORLD(sPicTable_Crustle, SIZE_32x32, SHADOW_SIZE_L, TRACKS_FOOT, sAnimTable_Following, gOverworldPalette_Crustle, gShinyOverworldPalette_Crustle)
        .levelUpLearnset = sCalcisternLevelUpLearnset, .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sCalcisternTeachableLearnset, .eggMoveLearnset = sCalcisternEggMoveLearnset,
    },

    [SPECIES_CARPULUS] =
    {
        .baseHP = 55, .baseAttack = 55, .baseDefense = 55, .baseSpeed = 55, .baseSpAttack = 50, .baseSpDefense = 60,
        .types = MON_TYPES(TYPE_WATER), .catchRate = 190, .expYield = 64, .evYield_Speed = 1, .genderRatio = PERCENT_FEMALE(50), .eggCycles = 20,
        .friendship = 70, .growthRate = GROWTH_MEDIUM_FAST, .eggGroups = MON_EGG_GROUPS(EGG_GROUP_WATER_2),
        .abilities = { ABILITY_SWIFT_SWIM, ABILITY_WATER_VEIL, ABILITY_HYDRATION }, .bodyColor = BODY_COLOR_GREEN,
        .speciesName = _("Carpulus"), .cryId = CRY_MAGIKARP, .natDexNum = NATIONAL_DEX_CARPULUS, .categoryName = _("SCAGLIA"), .height = 6, .weight = 120,
        .description = COMPOUND_STRING(
            "Vive in branchi presso rive e pontili.\n"
            "Le scaglie circolari riflettono la luce.\n"
            "Così si orienta nell'acqua torbida."),
        .pokemonScale = 256, .frontPic = gMonFrontPic_Carpulus, .frontPicSize = MON_COORDS_SIZE(64,64), .frontPicYOffset = 4,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder, .backPic = gMonBackPic_Carpulus, .backPicSize = MON_COORDS_SIZE(64,64), .backPicYOffset = 4,
        .palette = gMonPalette_Carpulus, .shinyPalette = gMonShinyPalette_Carpulus, .iconSprite = gMonIcon_Carpulus, .iconPalIndex = 0,
        FOOTPRINT(Magikarp) OVERWORLD(sPicTable_Magikarp, SIZE_32x32, SHADOW_SIZE_M, TRACKS_FOOT, sAnimTable_Following, gOverworldPalette_Magikarp, gShinyOverworldPalette_Magikarp)
        .levelUpLearnset = sCarpulusLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sCarpulusTeachableLearnset,
        .eggMoveLearnset = sCarpulusEggMoveLearnset,
    },
    [SPECIES_LUCINUS] =
    {
        .baseHP = 60, .baseAttack = 90, .baseDefense = 60, .baseSpeed = 95, .baseSpAttack = 45, .baseSpDefense = 70,
        .types = MON_TYPES(TYPE_WATER, TYPE_DARK), .catchRate = 90, .expYield = 120, .evYield_Attack = 2, .genderRatio = PERCENT_FEMALE(50), .eggCycles = 25,
        .friendship = 70, .growthRate = GROWTH_MEDIUM_FAST, .eggGroups = MON_EGG_GROUPS(EGG_GROUP_WATER_2), .abilities = { ABILITY_STRONG_JAW, ABILITY_SWIFT_SWIM, ABILITY_SNIPER },
        .bodyColor = BODY_COLOR_GREEN, .speciesName = _("Lucinus"), .cryId = CRY_CARVANHA, .natDexNum = NATIONAL_DEX_LUCINUS, .categoryName = _("AGGUATO"), .height = 14, .weight = 340,
        .description = COMPOUND_STRING(
            "Si nasconde fra i canneti.\n"
            "Scatta sulla preda senza increspare\n"
            "l'acqua."), .pokemonScale = 256,
        .frontPic = gMonFrontPic_Lucinus, .frontPicSize = MON_COORDS_SIZE(64,64), .frontPicYOffset = 4, .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .backPic = gMonBackPic_Lucinus, .backPicSize = MON_COORDS_SIZE(64,64), .backPicYOffset = 4, .palette = gMonPalette_Lucinus, .shinyPalette = gMonShinyPalette_Lucinus, .iconSprite = gMonIcon_Lucinus, .iconPalIndex = 0,
        FOOTPRINT(Carvanha) OVERWORLD(sPicTable_Carvanha, SIZE_32x32, SHADOW_SIZE_M, TRACKS_FOOT, sAnimTable_Following, gOverworldPalette_Carvanha, gShinyOverworldPalette_Carvanha)
        .levelUpLearnset = sLucinusLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sLucinusTeachableLearnset,
        .eggMoveLearnset = sLucinusEggMoveLearnset,
    },
    [SPECIES_NAUFRAGUS] =
    {
        .baseHP = 80, .baseAttack = 95, .baseDefense = 120, .baseSpeed = 35, .baseSpAttack = 55, .baseSpDefense = 90,
        .types = MON_TYPES(TYPE_WATER, TYPE_STEEL), .catchRate = 45, .expYield = 160, .evYield_Defense = 2, .genderRatio = PERCENT_FEMALE(50), .eggCycles = 30,
        .friendship = 70, .growthRate = GROWTH_SLOW, .eggGroups = MON_EGG_GROUPS(EGG_GROUP_WATER_2, EGG_GROUP_MINERAL), .abilities = { ABILITY_BATTLE_ARMOR, ABILITY_STURDY, ABILITY_HEAVY_METAL },
        .bodyColor = BODY_COLOR_BROWN, .speciesName = _("Naufragus"), .cryId = CRY_SKARMORY, .natDexNum = NATIONAL_DEX_NAUFRAGUS, .categoryName = _("RELITTO"), .height = 18, .weight = 1350,
        .description = COMPOUND_STRING(
            "Le placche sembrano prue romane.\n"
            "Nei laghi, le leggende lo scambiano\n"
            "per una nave senza equipaggio."), .pokemonScale = 256,
        .frontPic = gMonFrontPic_Naufragus, .frontPicSize = MON_COORDS_SIZE(64,64), .frontPicYOffset = 4, .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .backPic = gMonBackPic_Naufragus, .backPicSize = MON_COORDS_SIZE(64,64), .backPicYOffset = 4, .palette = gMonPalette_Naufragus, .shinyPalette = gMonShinyPalette_Naufragus, .iconSprite = gMonIcon_Naufragus, .iconPalIndex = 0,
        FOOTPRINT(Skarmory) OVERWORLD(sPicTable_Skarmory, SIZE_32x32, SHADOW_SIZE_L, TRACKS_FOOT, sAnimTable_Following, gOverworldPalette_Skarmory, gShinyOverworldPalette_Skarmory)
        .levelUpLearnset = sNaufragusLevelUpLearnset,
        .teachingType = EXPLICIT_TEACHABLES,
        .teachableLearnset = sNaufragusTeachableLearnset,
        .eggMoveLearnset = sNaufragusEggMoveLearnset,
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
