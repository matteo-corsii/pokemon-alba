#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_RAZOR_WIND) == EFFECT_TWO_TURNS_ATTACK);
    ASSUME(GetMoveEffect(MOVE_SKULL_BASH) == EFFECT_TWO_TURNS_ATTACK);
    ASSUME_MOVE_EFFECT_STAT_CHANGE(MOVE_SKULL_BASH, self: TRUE, defense: 1);
    ASSUME(GetMoveEffect(MOVE_SKY_ATTACK) == EFFECT_TWO_TURNS_ATTACK);

    // Electro shot - check for rain
    ASSUME(GetMoveTwoTurnAttackWeather(MOVE_ELECTRO_SHOT) == B_WEATHER_RAIN);
    ASSUME(GetMoveEffect(MOVE_ELECTRO_SHOT) == EFFECT_TWO_TURNS_ATTACK);
    ASSUME_MOVE_EFFECT_STAT_CHANGE(MOVE_ELECTRO_SHOT, self: TRUE, spAtk: 1);
}

SINGLE_BATTLE_TEST("Razor Wind needs a charging turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_RAZOR_WIND); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        // Charging turn
        if (B_UPDATED_MOVE_DATA >= GEN_5) {
            NOT MESSAGE("Wobbuffet whipped up a whirlwind!");
            MESSAGE("Wobbuffet usa\nRazor Wind!");
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_RAZOR_WIND, player);
        }
        if (B_UPDATED_MOVE_DATA < GEN_5)
            MESSAGE("Wobbuffet whipped up a whirlwind!");
        else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_RAZOR_WIND, player);
        // Attack turn
        MESSAGE("Wobbuffet usa\nRazor Wind!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAZOR_WIND, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Razor Wind doesn't need to charge with Power Herb")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_POWER_HERB); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_RAZOR_WIND); }
    } SCENE {
        if (B_UPDATED_MOVE_DATA >= GEN_5) {
            NOT MESSAGE("Wobbuffet whipped up a whirlwind!");
            MESSAGE("Wobbuffet usa\nRazor Wind!");
        } else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_RAZOR_WIND, player);
        if (B_UPDATED_MOVE_DATA < GEN_5)
            MESSAGE("Wobbuffet whipped up a whirlwind!");
        else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_RAZOR_WIND, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet became fully charged due to its Power Herb!");
        if (B_UPDATED_MOVE_DATA < GEN_5)
            MESSAGE("Wobbuffet usa\nRazor Wind!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAZOR_WIND, player);
        HP_BAR(opponent);
    }
}

DOUBLE_BATTLE_TEST("Razor Wind successfully KOs both opponents")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_POWER_HERB); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WYNAUT) { HP(1); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_RAZOR_WIND); }
    } SCENE {
        if (B_UPDATED_MOVE_DATA >= GEN_5) {
            NOT MESSAGE("Wobbuffet whipped up a whirlwind!");
            MESSAGE("Wobbuffet usa\nRazor Wind!");
        } else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_RAZOR_WIND, playerLeft);
        if (B_UPDATED_MOVE_DATA < GEN_5)
            MESSAGE("Wobbuffet whipped up a whirlwind!");
        else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_RAZOR_WIND, playerLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        MESSAGE("Wobbuffet became fully charged due to its Power Herb!");
        if (B_UPDATED_MOVE_DATA < GEN_5)
            MESSAGE("Wobbuffet usa\nRazor Wind!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAZOR_WIND, playerLeft);
        HP_BAR(opponentLeft);
        MESSAGE("Wobbuffet avversario non ha\npiù energie!");
        MESSAGE("Wynaut avversario non ha\npiù energie!");
    }
}

SINGLE_BATTLE_TEST("Skull Bash needs a charging turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SKULL_BASH); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        // Charging turn
        if (B_UPDATED_MOVE_DATA >= GEN_5) {
            NOT MESSAGE("Wobbuffet lowered its head!");
            MESSAGE("Wobbuffet usa\nSkull Bash!");
        } else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SKULL_BASH, player);
        if (B_UPDATED_MOVE_DATA < GEN_5)
            MESSAGE("Wobbuffet lowered its head!");
        else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SKULL_BASH, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet: DIFESA\naumenta!");
        // Attack turn
        MESSAGE("Wobbuffet usa\nSkull Bash!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKULL_BASH, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Skull Bash doesn't need to charge with Power Herb")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_POWER_HERB); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SKULL_BASH); }
    } SCENE {
        if (B_UPDATED_MOVE_DATA >= GEN_5) {
            NOT MESSAGE("Wobbuffet lowered its head!");
            MESSAGE("Wobbuffet usa\nSkull Bash!");
        } else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SKULL_BASH, player);
        if (B_UPDATED_MOVE_DATA < GEN_5)
            MESSAGE("Wobbuffet lowered its head!");
        else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SKULL_BASH, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet: DIFESA\naumenta!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet became fully charged due to its Power Herb!");
        if (B_UPDATED_MOVE_DATA < GEN_5)
            MESSAGE("Wobbuffet usa\nSkull Bash!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKULL_BASH, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Sky Attack needs a charging turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SKY_ATTACK); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        // Charging turn
        if (B_UPDATED_MOVE_DATA >= GEN_5) {
            NONE_OF {
                MESSAGE("Wobbuffet became cloaked in a harsh light!");
                MESSAGE("Wobbuffet is glowing!");
            }
            MESSAGE("Wobbuffet usa\nSky Attack!");
        } else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SKY_ATTACK, player);
        if (B_UPDATED_MOVE_DATA < GEN_4)
            MESSAGE("Wobbuffet is glowing!");
        else if (B_UPDATED_MOVE_DATA < GEN_5)
            MESSAGE("Wobbuffet became cloaked in a harsh light!");
        else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SKY_ATTACK, player);
        // Attack turn
        MESSAGE("Wobbuffet usa\nSky Attack!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKY_ATTACK, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Sky Attack doesn't need to charge with Power Herb")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_POWER_HERB); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SKY_ATTACK); }
    } SCENE {
        if (B_UPDATED_MOVE_DATA >= GEN_5) {
            NONE_OF {
                MESSAGE("Wobbuffet became cloaked in a harsh light!");
                MESSAGE("Wobbuffet is glowing!");
            }
            MESSAGE("Wobbuffet usa\nSky Attack!");
        } else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SKY_ATTACK, player);
        if (B_UPDATED_MOVE_DATA < GEN_4)
            MESSAGE("Wobbuffet is glowing!");
        else if (B_UPDATED_MOVE_DATA < GEN_5)
            MESSAGE("Wobbuffet became cloaked in a harsh light!");
        else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SKY_ATTACK, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet became fully charged due to its Power Herb!");
        if (B_UPDATED_MOVE_DATA < GEN_5)
            MESSAGE("Wobbuffet usa\nSky Attack!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKY_ATTACK, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Solar Beam and Solar Blade can be used instantly in Sunlight")
{
    enum Move move1, move2;
    PARAMETRIZE { move1 = MOVE_SPLASH; move2 = MOVE_SOLAR_BEAM; }
    PARAMETRIZE { move1 = MOVE_SUNNY_DAY; move2 = MOVE_SOLAR_BEAM; }
    PARAMETRIZE { move1 = MOVE_SPLASH; move2 = MOVE_SOLAR_BLADE; }
    PARAMETRIZE { move1 = MOVE_SUNNY_DAY; move2 = MOVE_SOLAR_BLADE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move1); MOVE(player, move2); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        // Potential visual bug.
        // The script has the B_WAIT_TIME_LONG waitmessage but it does not wait
        if (move2 == MOVE_SOLAR_BEAM) {
            MESSAGE("Wobbuffet usa\nSolar Beam!");
        } else {
            MESSAGE("Wobbuffet usa\nSolar Blade!");
        }
        MESSAGE("Wobbuffet absorbed light!");

        if (move2 == MOVE_SOLAR_BEAM) {
            if (move1 == MOVE_SPLASH) {
                MESSAGE("Wobbuffet usa\nSolar Beam!");
            }
            ANIMATION(ANIM_TYPE_MOVE, move2, player);
        } else {
            if (move1 == MOVE_SPLASH) {
                MESSAGE("Wobbuffet usa\nSolar Blade!");
            }
            ANIMATION(ANIM_TYPE_MOVE, move2, player);
        }
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Solar Beam's power is halved in Rain", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_RAIN_DANCE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_SOLAR_BEAM); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Solar Blade's power is halved in Rain", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_RAIN_DANCE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_SOLAR_BLADE); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Solar Beam's power is halved in a Sandstorm", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_SANDSTORM; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_SAFETY_GOGGLES); }
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_SOLAR_BEAM); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Solar Blade's power is halved in a Sandstorm", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_SANDSTORM; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_SAFETY_GOGGLES); }
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_SOLAR_BLADE); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Solar Beam's power is halved in Hail", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_HAIL; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_SAFETY_GOGGLES); }
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_SOLAR_BEAM); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Solar Blade's power is halved in Hail", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_HAIL; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_SAFETY_GOGGLES); }
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_SOLAR_BLADE); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Solar Beam's power is halved in Snow", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_SNOWSCAPE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_SOLAR_BEAM); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Solar Blade's power is halved in Snow", s16 damage)
{
    enum Move move;
    PARAMETRIZE { move = MOVE_CELEBRATE; }
    PARAMETRIZE { move = MOVE_SNOWSCAPE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_SOLAR_BLADE); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Electro Shot needs a charging Turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ELECTRO_SHOT); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        // Charging turn
        MESSAGE("Wobbuffet usa\nElectro Shot!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTRO_SHOT, player);
        MESSAGE("Wobbuffet absorbed electricity!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet: ATT. SPECIALE\naumenta!");
        // Attack turn
        MESSAGE("Wobbuffet usa\nElectro Shot!");
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Electro Shot doesn't need to charge when it's raining")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_RAIN_DANCE); MOVE(player, MOVE_ELECTRO_SHOT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAIN_DANCE, opponent);
        MESSAGE("Wobbuffet usa\nElectro Shot!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTRO_SHOT, player);
        MESSAGE("Wobbuffet absorbed electricity!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet: ATT. SPECIALE\naumenta!");
        NONE_OF {
            MESSAGE("Wobbuffet usa\nElectro Shot!");
        }
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Electro Shot doesn't need to charge with Power Herb")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_POWER_HERB); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ELECTRO_SHOT); }
    } SCENE {
        MESSAGE("Wobbuffet usa\nElectro Shot!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTRO_SHOT, player);
        MESSAGE("Wobbuffet absorbed electricity!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet: ATT. SPECIALE\naumenta!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet became fully charged due to its Power Herb!");
        NONE_OF {
            MESSAGE("Wobbuffet usa\nElectro Shot!");
        }
        HP_BAR(opponent);
    }
}
