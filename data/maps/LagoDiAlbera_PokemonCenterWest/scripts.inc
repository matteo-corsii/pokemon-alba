LagoDiAlbera_PokemonCenterWest_MapScripts::
	.byte 0

LagoDiAlbera_PokemonCenterWest_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_LAGO_DI_ALBERA_POKEMON_CENTER_WEST_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

LagoDiAlbera_PokemonCenterWest_EventScript_Hiker::
	msgbox LagoDiAlbera_PokemonCenterWest_Text_Hiker, MSGBOX_NPC
	end

LagoDiAlbera_PokemonCenterWest_EventScript_Youngster::
	msgbox LagoDiAlbera_PokemonCenterWest_Text_Youngster, MSGBOX_NPC
	end

LagoDiAlbera_PokemonCenterWest_Text_Hiker:
	.string "Ho fatto quasi tutto il giro del lago a piedi.\n"
	.string "Da una sponda Albera sembra tranquillo,\n"
	.string "dall'altra completamente selvaggio.$"

LagoDiAlbera_PokemonCenterWest_Text_Youngster:
	.string "I sentieri salgono in fretta da queste parti.\n"
	.string "Non dimenticare mai quanto è profondo\n"
	.string "questo cratere.$"
