#include "global.h"

// The object sizes are emitted by the ARM compiler and decoded with nm in CI.
// This deliberately does not link or execute any game code.
#define EMIT_SIZE(name, expression) \
    unsigned char name[(expression)] __attribute__((used));

EMIT_SIZE(diag_sizeof_SaveBlock1, sizeof(struct SaveBlock1));
EMIT_SIZE(diag_offset_SaveBlock1_dexSeen, offsetof(struct SaveBlock1, dexSeen));
EMIT_SIZE(diag_offset_SaveBlock1_dexCaught, offsetof(struct SaveBlock1, dexCaught));
EMIT_SIZE(diag_offset_SaveBlock1_trainerHillTimes, offsetof(struct SaveBlock1, trainerHillTimes));
EMIT_SIZE(diag_offset_SaveBlock1_mysteryGift, offsetof(struct SaveBlock1, mysteryGift));
EMIT_SIZE(diag_sizeof_MysteryGiftSave, sizeof(struct MysteryGiftSave));
EMIT_SIZE(diag_sizeof_WonderCard, sizeof(struct WonderCard));
EMIT_SIZE(diag_sizeof_WonderCardMetadata, sizeof(struct WonderCardMetadata));

EMIT_SIZE(diag_offset_SaveBlock1_bag, offsetof(struct SaveBlock1, bag));
EMIT_SIZE(diag_offset_SaveBlock1_pokeNews, offsetof(struct SaveBlock1, pokeNews));
EMIT_SIZE(diag_offset_SaveBlock1_outbreakPokemonSpecies, offsetof(struct SaveBlock1, outbreakPokemonSpecies));
EMIT_SIZE(diag_offset_SaveBlock1_daycare, offsetof(struct SaveBlock1, daycare));
EMIT_SIZE(diag_offset_SaveBlock1_linkBattleRecords, offsetof(struct SaveBlock1, linkBattleRecords));
EMIT_SIZE(diag_offset_SaveBlock1_giftRibbons, offsetof(struct SaveBlock1, giftRibbons));
EMIT_SIZE(diag_offset_SaveBlock1_externalEventData, offsetof(struct SaveBlock1, externalEventData));
EMIT_SIZE(diag_offset_SaveBlock1_externalEventFlags, offsetof(struct SaveBlock1, externalEventFlags));
EMIT_SIZE(diag_offset_SaveBlock1_roamer, offsetof(struct SaveBlock1, roamer));
EMIT_SIZE(diag_offset_SaveBlock1_enigmaBerry, offsetof(struct SaveBlock1, enigmaBerry));
