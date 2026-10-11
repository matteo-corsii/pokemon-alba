#include "global.h"

// The object sizes are emitted by the ARM compiler and decoded with nm in CI.
// This deliberately does not link or execute any game code.
#define EMIT_SIZE(name, expression) \
    unsigned char name[(expression)] __attribute__((used));

EMIT_SIZE(diag_sizeof_SaveBlock1, sizeof(struct SaveBlock1));
EMIT_SIZE(diag_offset_SaveBlock1_bag, offsetof(struct SaveBlock1, bag));
EMIT_SIZE(diag_offset_SaveBlock1_pokeblocks, offsetof(struct SaveBlock1, pokeblocks));
EMIT_SIZE(diag_offset_SaveBlock1_ausoniaDexSaveSignature, offsetof(struct SaveBlock1, ausoniaDexSaveSignature));
EMIT_SIZE(diag_offset_SaveBlock1_berryBlenderRecords, offsetof(struct SaveBlock1, berryBlenderRecords));
EMIT_SIZE(diag_offset_SaveBlock1_objectEvents, offsetof(struct SaveBlock1, objectEvents));
EMIT_SIZE(diag_offset_SaveBlock1_objectEventTemplates, offsetof(struct SaveBlock1, objectEventTemplates));
EMIT_SIZE(diag_offset_SaveBlock1_flags, offsetof(struct SaveBlock1, flags));
EMIT_SIZE(diag_offset_SaveBlock1_vars, offsetof(struct SaveBlock1, vars));
EMIT_SIZE(diag_offset_SaveBlock1_gameStats, offsetof(struct SaveBlock1, gameStats));
EMIT_SIZE(diag_offset_SaveBlock1_berryTrees, offsetof(struct SaveBlock1, berryTrees));
EMIT_SIZE(diag_offset_SaveBlock1_secretBases, offsetof(struct SaveBlock1, secretBases));
EMIT_SIZE(diag_offset_SaveBlock1_playerRoomDecorations, offsetof(struct SaveBlock1, playerRoomDecorations));
EMIT_SIZE(diag_offset_SaveBlock1_playerRoomDecorationPositions, offsetof(struct SaveBlock1, playerRoomDecorationPositions));
EMIT_SIZE(diag_offset_SaveBlock1_decorationCushions, offsetof(struct SaveBlock1, decorationCushions));
EMIT_SIZE(diag_offset_SaveBlock1_tvShows, offsetof(struct SaveBlock1, tvShows));
EMIT_SIZE(diag_offset_SaveBlock1_pokeNews, offsetof(struct SaveBlock1, pokeNews));
EMIT_SIZE(diag_offset_SaveBlock1_outbreakPokemonSpecies, offsetof(struct SaveBlock1, outbreakPokemonSpecies));
EMIT_SIZE(diag_offset_SaveBlock1_daycare, offsetof(struct SaveBlock1, daycare));
EMIT_SIZE(diag_offset_SaveBlock1_linkBattleRecords, offsetof(struct SaveBlock1, linkBattleRecords));
EMIT_SIZE(diag_offset_SaveBlock1_giftRibbons, offsetof(struct SaveBlock1, giftRibbons));
EMIT_SIZE(diag_offset_SaveBlock1_externalEventData, offsetof(struct SaveBlock1, externalEventData));
EMIT_SIZE(diag_offset_SaveBlock1_externalEventFlags, offsetof(struct SaveBlock1, externalEventFlags));
EMIT_SIZE(diag_offset_SaveBlock1_roamer, offsetof(struct SaveBlock1, roamer));
EMIT_SIZE(diag_offset_SaveBlock1_enigmaBerry, offsetof(struct SaveBlock1, enigmaBerry));
EMIT_SIZE(diag_offset_SaveBlock1_mysteryGift, offsetof(struct SaveBlock1, mysteryGift));
EMIT_SIZE(diag_offset_SaveBlock1_dexSeen, offsetof(struct SaveBlock1, dexSeen));
EMIT_SIZE(diag_offset_SaveBlock1_dexCaught, offsetof(struct SaveBlock1, dexCaught));
EMIT_SIZE(diag_offset_SaveBlock1_trainerHillTimes, offsetof(struct SaveBlock1, trainerHillTimes));

EMIT_SIZE(diag_sizeof_ItemSlot, sizeof(struct ItemSlot));
EMIT_SIZE(diag_sizeof_Bag, sizeof(struct Bag));
EMIT_SIZE(diag_sizeof_Pokeblock, sizeof(struct Pokeblock));
EMIT_SIZE(diag_sizeof_SecretBaseParty, sizeof(struct SecretBaseParty));
EMIT_SIZE(diag_sizeof_SecretBase, sizeof(struct SecretBase));
EMIT_SIZE(diag_sizeof_TVShow, sizeof(TVShow));
EMIT_SIZE(diag_sizeof_PokeNews, sizeof(PokeNews));
EMIT_SIZE(diag_sizeof_MysteryGiftSave, sizeof(struct MysteryGiftSave));
EMIT_SIZE(diag_sizeof_WonderCard, sizeof(struct WonderCard));
EMIT_SIZE(diag_sizeof_WonderCardMetadata, sizeof(struct WonderCardMetadata));
