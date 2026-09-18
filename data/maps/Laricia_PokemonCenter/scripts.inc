Laricia_PokemonCenter_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

Laricia_PokemonCenter_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_LARICIA_POKEMON_CENTER_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

Laricia_PokemonCenter_EventScript_Traveler::
	msgbox Laricia_PokemonCenter_Text_Traveler, MSGBOX_NPC
	end

Laricia_PokemonCenter_Text_Traveler:
	.string "La piazza di Laricia e sempre\n"
	.string "piena di gente in viaggio.$"
