#include "global.h"
#include "test/battle.h"

TO_DO_BATTLE_TEST("Conversion 2's type change considers Inverse Battles");

SINGLE_BATTLE_TEST("Conversion 2 randomly changes the type of the user to a type that resists the last move that hit the user (Gen 1-4)")
{
    GIVEN {
        WITH_CONFIG(B_UPDATED_CONVERSION_2, GEN_4);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_OMINOUS_WIND); MOVE(opponent, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Wobbuffet usa\nOminous Wind!");
        // turn 1
        ONE_OF {
            MESSAGE("The opposing Wobbuffet transformed into the Normal type!");
            MESSAGE("The opposing Wobbuffet transformed into the Dark type!");
        }
    }
}

SINGLE_BATTLE_TEST("Conversion 2's type change considers Struggle to be Normal type (Gen 1-4)")
{
    GIVEN {
        WITH_CONFIG(B_UPDATED_CONVERSION_2, GEN_4);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_STRUGGLE); }
        TURN { MOVE(player, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Wobbuffet avversario usa\nStruggle!");
        // turn 2
        ONE_OF {
            MESSAGE("Wobbuffet transformed into the Steel type!");
            MESSAGE("Wobbuffet transformed into the Rock type!");
            MESSAGE("Wobbuffet transformed into the Ghost type!");
        }
    }
}

SINGLE_BATTLE_TEST("Conversion 2 randomly changes the type of the user to a type that resists the last used target's move (Gen 5+)")
{
    GIVEN {
        WITH_CONFIG(B_UPDATED_CONVERSION_2, GEN_5);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_OMINOUS_WIND); MOVE(opponent, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Wobbuffet usa\nOminous Wind!");
        // turn 1
        ONE_OF {
            MESSAGE("The opposing Wobbuffet transformed into the Normal type!");
            MESSAGE("The opposing Wobbuffet transformed into the Dark type!");
        }
    }
}

SINGLE_BATTLE_TEST("Conversion 2's type change considers status moves (Gen 5+)")
{
    GIVEN {
        WITH_CONFIG(B_UPDATED_CONVERSION_2, GEN_5);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_CURSE); }
        TURN { MOVE(player, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Wobbuffet avversario usa\nCurse!");
        // turn 2
        ONE_OF {
            MESSAGE("Wobbuffet transformed into the Normal type!");
            MESSAGE("Wobbuffet transformed into the Dark type!");
        }
    }
}

SINGLE_BATTLE_TEST("Conversion 2's type change considers the type of moves called by other moves")
{
    GIVEN {
        WITH_CONFIG(B_UPDATED_CONVERSION_2, GEN_5);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_OMINOUS_WIND); MOVE(opponent, MOVE_MIRROR_MOVE); }
        TURN { MOVE(player, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Wobbuffet avversario usa\nMirror Move!");
        // turn 2
        ONE_OF {
            MESSAGE("Wobbuffet transformed into the Normal type!");
            MESSAGE("Wobbuffet transformed into the Dark type!");
        }
    }
}

SINGLE_BATTLE_TEST("Conversion 2's type change considers dynamic type moves")
{
    GIVEN {
        WITH_CONFIG(B_UPDATED_CONVERSION_2, GEN_5);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_HAIL); MOVE(opponent, MOVE_WEATHER_BALL); }
        TURN { MOVE(player, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Wobbuffet avversario usa\nWeather Ball!");
        // turn 2
        ONE_OF {
            MESSAGE("Wobbuffet transformed into the Steel type!");
            MESSAGE("Wobbuffet transformed into the Fire type!");
            MESSAGE("Wobbuffet transformed into the Water type!");
            MESSAGE("Wobbuffet transformed into the Ice type!");
        }
    }
}

SINGLE_BATTLE_TEST("Conversion 2's type change considers move types changed by Normalize and Electrify")
{
    GIVEN {
        WITH_CONFIG(B_UPDATED_CONVERSION_2, GEN_5);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_NORMALIZE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ELECTRIFY); MOVE(opponent, MOVE_POUND); }
        TURN { MOVE(player, MOVE_CONVERSION_2); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Wobbuffet usa\nElectrify!");
        MESSAGE("Wobbuffet avversario usa\nBOTTA!");
        // turn 2
        ONE_OF {
            MESSAGE("Wobbuffet transformed into the Ground type!");
            MESSAGE("Wobbuffet transformed into the Dragon type!");
            MESSAGE("Wobbuffet transformed into the Grass type!");
            MESSAGE("Wobbuffet transformed into the Electric type!");
        }
        // turn 3
        MESSAGE("Wobbuffet usa\nPISTOLACQUA!");
        ONE_OF {
            MESSAGE("The opposing Wobbuffet transformed into the Steel type!");
            MESSAGE("The opposing Wobbuffet transformed into the Rock type!");
            MESSAGE("The opposing Wobbuffet transformed into the Ghost type!");
        }
    }
}

SINGLE_BATTLE_TEST("Conversion 2's type change fails targeting Struggle (Gen 5+)")
{
    GIVEN {
        WITH_CONFIG(B_UPDATED_CONVERSION_2, GEN_5);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_STRUGGLE); }
        TURN { MOVE(player, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Wobbuffet avversario usa\nStruggle!");
        // turn 2
        MESSAGE("Wobbuffet usa\nConversion 2!");
        MESSAGE("But it failed!");
    }
}

SINGLE_BATTLE_TEST("Conversion 2 fails if the move used is of typeless damage (Gen 5+)")
{
    GIVEN {
        WITH_CONFIG(B_UPDATED_CONVERSION_2, GEN_5);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ENTEI);
    } WHEN {
        TURN { MOVE(opponent, MOVE_BURN_UP); }
        TURN { MOVE(opponent, MOVE_REVELATION_DANCE); }
        TURN { MOVE(player, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Entei avversario usa\nBurn Up!");
        // turn 2
        MESSAGE("Entei avversario usa\nRevelation Dance!");
        // turn 3
        MESSAGE("Wobbuffet usa\nConversion 2!");
        MESSAGE("But it failed!");
    }
}

SINGLE_BATTLE_TEST("Conversion 2 fails if the targeted move is Stellar Type")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { TeraType(TYPE_STELLAR); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TERA_BLAST, gimmick: GIMMICK_TERA); MOVE(opponent, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Wobbuffet usa\nTera Blast!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TERA_BLAST, player);
        // turn 1
        MESSAGE("Wobbuffet avversario usa\nConversion 2!");
        MESSAGE("But it failed!");
    }
}

SINGLE_BATTLE_TEST("Conversion 2 fails if used by a Terastallized Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { TeraType(TYPE_PSYCHIC); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_CONVERSION_2, gimmick: GIMMICK_TERA); }
    } SCENE {
        MESSAGE("Wobbuffet usa\nConversion 2!");
        MESSAGE("But it failed!");
    }
}

SINGLE_BATTLE_TEST("Conversion 2 fails if last hit by a Stellar-type move (Gen 1-4)")
{
    GIVEN {
        WITH_CONFIG(B_UPDATED_CONVERSION_2, GEN_4);
        PLAYER(SPECIES_WOBBUFFET) { TeraType(TYPE_STELLAR); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TERA_BLAST, gimmick: GIMMICK_TERA); MOVE(opponent, MOVE_CONVERSION_2); }
    } SCENE {
        // turn 1
        MESSAGE("Wobbuffet usa\nTera Blast!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TERA_BLAST, player);
        // turn 2
        MESSAGE("Wobbuffet avversario usa\nConversion 2!");
        MESSAGE("But it failed!");
    }
}
