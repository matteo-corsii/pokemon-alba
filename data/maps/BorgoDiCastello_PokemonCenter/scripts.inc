BorgoDiCastello_PokemonCenter_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

BorgoDiCastello_PokemonCenter_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_BORGO_DI_CASTELLO_POKEMON_CENTER_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

BorgoDiCastello_PokemonCenter_EventScript_Viaggiatore::
	msgbox BorgoDiCastello_PokemonCenter_Text_Viaggiatore, MSGBOX_NPC
	end

BorgoDiCastello_PokemonCenter_Text_Viaggiatore:
	.string "Camminare su e giu per questi\n"
	.string "vicoli stanca piu del previsto!$"
