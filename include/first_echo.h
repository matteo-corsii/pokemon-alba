#ifndef GUARD_FIRST_ECHO_H
#define GUARD_FIRST_ECHO_H

#include "global.h"
#include "pokemon.h"

enum FirstEchoEffect
{
    FIRST_ECHO_EFFECT_NONE,
    FIRST_ECHO_EFFECT_ATK_SPATK,
};

void FirstEcho_RegisterStarterIdentity(enum Species species);
void TryMigrateOriginalStarterIdentity(void);
bool32 FirstEcho_ShouldOfferStarterSwitch(void);
bool32 FirstEcho_TryActivateOnSwitchIn(void);
void FirstEcho_ApplyBoost(void);
void Special_GetFirstEchoActiveMonResult(void);

#endif // GUARD_FIRST_ECHO_H
