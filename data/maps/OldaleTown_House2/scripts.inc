OldaleTown_House2_MapScripts::
	.byte 0

OldaleTown_House2_EventScript_Woman::
	msgbox OldaleTown_House2_Text_PokemonLevelUp, MSGBOX_NPC
	end

OldaleTown_House2_EventScript_Man::
	msgbox OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon, MSGBOX_NPC
	end

OldaleTown_House2_Text_PokemonLevelUp:
	.string "Lottando, i POKéMON salgono di livello\n"
	.string "e diventano più forti.$"

OldaleTown_House2_Text_YoullGoFurtherWithStrongPokemon:
	.string "Con POKéMON più forti al tuo fianco\n"
	.string "potrai affrontare viaggi più lunghi.$"
