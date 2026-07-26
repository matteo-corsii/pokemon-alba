#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Thousand Arrows does neutral damage to non-grounded Flying types regardless of other typings")
{
    u32 pokemon;
    PARAMETRIZE { pokemon = SPECIES_SKARMORY; }
    PARAMETRIZE { pokemon = SPECIES_SCYTHER; }
    GIVEN {
        ASSUME(MoveIgnoresTypeIfFlyingAndUngrounded(MOVE_THOUSAND_ARROWS) == TRUE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(pokemon);
    } WHEN {
        TURN { MOVE(player, MOVE_THOUSAND_ARROWS); }
        TURN { MOVE(player, MOVE_THOUSAND_ARROWS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THOUSAND_ARROWS, player);
        if (pokemon == SPECIES_SKARMORY) {
            MESSAGE("The opposing Skarmory fell straight down!");
            MESSAGE("Skarmory avversario usa\nCelebrate!");
        } else {
            MESSAGE("The opposing Scyther fell straight down!");
            MESSAGE("Scyther avversario usa\nCelebrate!");
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        MESSAGE("Congratulations, 1!");
        MESSAGE("Wobbuffet usa\nThousand Arrows!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THOUSAND_ARROWS, player);
        if (pokemon == SPECIES_SKARMORY)
        {
            MESSAGE("È superefficace!");
        }
        else
        {
            MESSAGE("Non è molto efficace…");
        }
    }
}
