LagoDiAlbera_PokemonCenterEast_MapScripts::
	.byte 0

LagoDiAlbera_PokemonCenterEast_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_LAGO_DI_ALBERA_POKEMON_CENTER_EAST_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

LagoDiAlbera_PokemonCenterEast_EventScript_Fisherman::
	msgbox LagoDiAlbera_PokemonCenterEast_Text_Fisherman, MSGBOX_NPC
	end

LagoDiAlbera_PokemonCenterEast_EventScript_Sailor::
	msgbox LagoDiAlbera_PokemonCenterEast_Text_Sailor, MSGBOX_NPC
	end

LagoDiAlbera_PokemonCenterEast_Text_Fisherman:
	.string "Attraversare Albera in barca sembra facile\n"
	.string "quando lo guardi dalla riva. Poi cambia\n"
	.string "il vento.$"

LagoDiAlbera_PokemonCenterEast_Text_Sailor:
	.string "Dall'acqua riesci a vedere posti che dalla\n"
	.string "strada nemmeno immagini.$"
