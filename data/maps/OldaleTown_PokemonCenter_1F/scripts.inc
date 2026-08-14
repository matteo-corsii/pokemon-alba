OldaleTown_PokemonCenter_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, OldaleTown_PokemonCenter_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

OldaleTown_PokemonCenter_1F_OnTransition:
	setrespawn HEAL_LOCATION_OLDALE_TOWN
	call Common_EventScript_UpdateBrineyLocation
	end

OldaleTown_PokemonCenter_1F_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_OLDALE_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

OldaleTown_PokemonCenter_1F_EventScript_Gentleman::
	msgbox OldaleTown_PokemonCenter_1F_Text_TrainersCanUsePC, MSGBOX_NPC
	end

OldaleTown_PokemonCenter_1F_EventScript_Boy::
	msgbox OldaleTown_PokemonCenter_1F_Text_PokemonCentersAreGreat, MSGBOX_NPC
	end

OldaleTown_PokemonCenter_1F_EventScript_Girl::
	lock
	faceplayer
	goto_if_set FLAG_SYS_POKEDEX_GET, OldaleTown_PokemonCenter_1F_EventScript_WirelessClubAvailable
	msgbox OldaleTown_PokemonCenter_1F_Text_WirelessClubNotAvailable, MSGBOX_DEFAULT
	release
	end

OldaleTown_PokemonCenter_1F_EventScript_WirelessClubAvailable::
	msgbox OldaleTown_PokemonCenter_1F_Text_TradedInWirelessClub, MSGBOX_DEFAULT
	release
	end

OldaleTown_PokemonCenter_1F_Text_TrainersCanUsePC:
	.string "Molti viaggiatori si fermano qui prima\n"
	.string "di entrare ad Albèra Storica.$"

OldaleTown_PokemonCenter_1F_Text_PokemonCentersAreGreat:
	.string "Da Via Verdi arrivano sempre scarpe\n"
	.string "infangate e POKéMON stanchi!$"

OldaleTown_PokemonCenter_1F_Text_WirelessClubNotAvailable:
	.string "Il CLUB WIRELESS POKéMON al piano\n"
	.string "superiore è stato costruito da poco.\p"
	.string "Dicono che i lavori non siano ancora\n"
	.string "terminati.$"

OldaleTown_PokemonCenter_1F_Text_TradedInWirelessClub:
	.string "Il CLUB WIRELESS POKéMON al piano\n"
	.string "superiore è stato costruito da poco.\p"
	.string "Ho subito scambiato un POKéMON.$"
